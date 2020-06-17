#include "openamp.h"
#include "inter_core_com.h"
#include "freemodbus_tcp.h"

#define RPMSG_GET_KFIFO_SIZE 1
#define RPMSG_GET_AVAIL_DATA_SIZE 2
#define RPMSG_GET_FREE_SPACE 3

#define RPMSG_HEADER_LEN 16
#define MAX_RPMSG_BUFF_SIZE (512 - RPMSG_HEADER_LEN)
#define PAYLOAD_MIN_SIZE 1
#define PAYLOAD_MAX_SIZE (MAX_RPMSG_BUFF_SIZE - 24)
#define NUM_PAYLOADS (PAYLOAD_MAX_SIZE / PAYLOAD_MIN_SIZE)

#define RPMSG_BUS_SYS "/sys/bus/rpmsg"

// mutex
static pthread_mutex_t rpmsg_mutex;

// extern variables
extern USHORT usRegHoldingBuf[REG_HOLDING_NREGS];

char *rpmsg_dev = "virtio0.rpmsg-openamp-demo-channel.-1.0";
char rpmsg_char_name[16];
char fpath[256];
struct rpmsg_endpoint_info eptinfo;
char ept_dev_name[16];
char ept_dev_path[32];

r5_cmd R5_cmd;
r5_state R5_state;

static int charfd = -1, fd = -1;
char sbuf[512];
int r5_id = 0;

/* write a string to an existing and writtable file */
static int file_write(char *path, char *str)
{
	int fd;
	ssize_t bytes_written;
	size_t str_sz;

	fd = open(path, O_WRONLY);
	if (fd == -1)
	{
		perror("Error");
		return -1;
	}
	str_sz = strlen(str);
	bytes_written = write(fd, str, str_sz);
	if (bytes_written != str_sz)
	{
		if (bytes_written == -1)
		{
			perror("Error");
		}
		close(fd);
		return -1;
	}

	if (-1 == close(fd))
	{
		perror("Error");
		return -1;
	}
	return 0;
}

static int rpmsg_create_ept(int rpfd, struct rpmsg_endpoint_info *eptinfo)
{
	int ret;

	ret = ioctl(rpfd, RPMSG_CREATE_EPT_IOCTL, eptinfo);
	if (ret)
		perror("Failed to create endpoint.\n");
	return ret;
}

static char *get_rpmsg_ept_dev_name(const char *rpmsg_char_name,
									const char *ept_name,
									char *ept_dev_name)
{
	char sys_rpmsg_ept_name_path[64];
	char svc_name[64];
	char *sys_rpmsg_path = "/sys/class/rpmsg";
	FILE *fp;
	int i;
	int ept_name_len;

	for (i = 0; i < 128; i++)
	{
		sprintf(sys_rpmsg_ept_name_path, "%s/%s/rpmsg%d/name",
				sys_rpmsg_path, rpmsg_char_name, i);
		printf("checking %s\n", sys_rpmsg_ept_name_path);
		if (access(sys_rpmsg_ept_name_path, F_OK) < 0)
			continue;
		fp = fopen(sys_rpmsg_ept_name_path, "r");
		if (!fp)
		{
			printf("failed to open %s\n", sys_rpmsg_ept_name_path);
			break;
		}
		fgets(svc_name, sizeof(svc_name), fp);
		fclose(fp);
		printf("svc_name: %s.\n", svc_name);
		ept_name_len = strlen(ept_name);
		if (ept_name_len > sizeof(svc_name))
			ept_name_len = sizeof(svc_name);
		if (!strncmp(svc_name, ept_name, ept_name_len))
		{
			sprintf(ept_dev_name, "rpmsg%d", i);
			return ept_dev_name;
		}
	}

	printf("Not able to RPMsg endpoint file for %s:%s.\n",
		   rpmsg_char_name, ept_name);
	return NULL;
}

static int bind_rpmsg_chrdev(const char *rpmsg_dev_name)
{
	char fpath[256];
	char *rpmsg_chdrv = "rpmsg_chrdev";
	int fd;
	int ret;

	/* rpmsg dev overrides path */
	sprintf(fpath, "%s/devices/%s/driver_override",
			RPMSG_BUS_SYS, rpmsg_dev_name);
	fd = open(fpath, O_WRONLY);
	if (fd < 0)
	{
		fprintf(stderr, "Failed to open %s, %s\n",
				fpath, strerror(errno));
		return -EINVAL;
	}
	ret = write(fd, rpmsg_chdrv, strlen(rpmsg_chdrv) + 1);
	if (ret < 0)
	{
		fprintf(stderr, "Failed to write %s to %s, %s\n",
				rpmsg_chdrv, fpath, strerror(errno));
		return -EINVAL;
	}
	close(fd);

	/* bind the rpmsg device to rpmsg char driver */
	sprintf(fpath, "%s/drivers/%s/bind", RPMSG_BUS_SYS, rpmsg_chdrv);
	fd = open(fpath, O_WRONLY);
	if (fd < 0)
	{
		fprintf(stderr, "Failed to open %s, %s\n",
				fpath, strerror(errno));
		return -EINVAL;
	}
	ret = write(fd, rpmsg_dev_name, strlen(rpmsg_dev_name) + 1);
	if (ret < 0)
	{
		fprintf(stderr, "Failed to write %s to %s, %s\n",
				rpmsg_dev_name, fpath, strerror(errno));
		return -EINVAL;
	}
	close(fd);
	return 0;
}

static int get_rpmsg_chrdev_fd(const char *rpmsg_dev_name,
							   char *rpmsg_ctrl_name)
{
	char dpath[256];
	char fpath[256];
	char *rpmsg_ctrl_prefix = "rpmsg_ctrl";
	DIR *dir;
	struct dirent *ent;
	int fd;

	sprintf(dpath, "%s/devices/%s/rpmsg", RPMSG_BUS_SYS, rpmsg_dev_name);
	dir = opendir(dpath);
	if (dir == NULL)
	{
		fprintf(stderr, "Failed to open dir %s\n", dpath);
		return -EINVAL;
	}
	while ((ent = readdir(dir)) != NULL)
	{
		if (!strncmp(ent->d_name, rpmsg_ctrl_prefix,
					 strlen(rpmsg_ctrl_prefix)))
		{
			printf("Opening file %s.\n", ent->d_name);
			sprintf(fpath, "/dev/%s", ent->d_name);
			fd = open(fpath, O_RDWR | O_NONBLOCK);
			if (fd < 0)
			{
				fprintf(stderr,
						"Failed to open rpmsg char dev %s,%s\n",
						fpath, strerror(errno));
				return fd;
			}
			sprintf(rpmsg_ctrl_name, "%s", ent->d_name);
			return fd;
		}
	}

	fprintf(stderr, "No rpmsg char dev file is found\n");
	return -EINVAL;
}

int OpenAMPLoadFirmware(void)
{
	/* Bring up remote firmware */
	printf("\r\nMaster>Loading remote firmware\r\n");

	/* Write firmware name to remoteproc sysfs interface */
	sprintf(sbuf,
			"/sys/class/remoteproc/remoteproc%u/firmware",
			r5_id);

	if (0 != file_write(sbuf, "r5-0-freertos.elf"))
	{
		return -EINVAL;
	}

	/* Tell remoteproc to load and start remote cpu */
	sprintf(sbuf,
			"/sys/class/remoteproc/remoteproc%u/state",
			r5_id);
	if (0 != file_write(sbuf, "start"))
	{
		return -EINVAL;
	}
	else
	{
		return 0;
	}
}

int OpenAMPLoadDriver(void)
{
	int ret;

	/* Load rpmsg_char driver */
	printf("\r\nMaster>probe rpmsg_char\r\n");
	ret = system("modprobe rpmsg_char");
	if (ret < 0)
	{
		// perror("Failed to load rpmsg_char driver.\n");
		printf("\033[31m"
			   "Failed to load rpmsg_char driver.\n"
			   "\033[0m");
		return -EINVAL;
	}

	printf("\r\n Open rpmsg dev %s! \r\n", rpmsg_dev);
	sprintf(fpath, "%s/devices/%s", RPMSG_BUS_SYS, rpmsg_dev);
	if (access(fpath, F_OK))
	{
		fprintf(stderr, "Not able to access rpmsg device %s, %s\n",
				fpath, strerror(errno));
		return -EINVAL;
	}
	ret = bind_rpmsg_chrdev(rpmsg_dev);
	if (ret < 0)
		return ret;
	charfd = get_rpmsg_chrdev_fd(rpmsg_dev, rpmsg_char_name);
	if (charfd < 0)
		return charfd;

	/* Create endpoint from rpmsg char driver */
	strcpy(eptinfo.name, "rpmsg-openamp-demo-channel");
	eptinfo.src = 0;
	eptinfo.dst = 0xFFFFFFFF;
	ret = rpmsg_create_ept(charfd, &eptinfo);
	if (ret)
	{
		printf("failed to create RPMsg endpoint.\n");
		return -EINVAL;
	}
	if (!get_rpmsg_ept_dev_name(rpmsg_char_name, eptinfo.name,
								ept_dev_name))
		return -EINVAL;
	sprintf(ept_dev_path, "/dev/%s", ept_dev_name);

	// open file
	fd = open(ept_dev_path, O_RDWR | O_NONBLOCK);
	if (fd < 0)
	{
		perror("Failed to open rpmsg device.");
		close(charfd);
		return -1;
	}
	return 0;
}

int echo_test()
{
	int i;
	int bytes_rcvd, bytes_sent;

	// messege transfer
	for (i = 0; i <= MOTOR_NUM; i++)
	{
		printf("\r\n **********************************");
		printf("****\r\n");
		printf("\r\n  Openamp coummunication round: %d \r\n", i);
		printf("\r\n **********************************");
		printf("****\r\n");

		printf("usRegHoldingBuf[%d]:%d\r\n", i * MAX_RPMSG_SIZE / 2, usRegHoldingBuf[i * MAX_RPMSG_SIZE / 2]);

		bytes_sent = write(fd, &usRegHoldingBuf[i * MAX_RPMSG_SIZE / 2], MAX_RPMSG_SIZE);

		if (bytes_sent <= 0)
		{
			printf("\r\n Error sending data");
			printf(" .. \r\n");
			break;
		}
		else
		{
			printf("\r\n Send %d bytes", bytes_sent);
		}

		// read the data
		do
		{
			bytes_rcvd = read(fd, &usRegHoldingBuf[i * MAX_RPMSG_SIZE / 2], MAX_RPMSG_SIZE);
		} while ((bytes_rcvd < MAX_RPMSG_SIZE) || (bytes_rcvd < 0));
		printf("\r\n Read %d bytes", bytes_rcvd);
		printf("usRegHoldingBuf[%d]:%d\r\n", i * MAX_RPMSG_SIZE / 2, usRegHoldingBuf[i * MAX_RPMSG_SIZE / 2]);

		printf("\r\n **********************************");
		printf("****\r\n");
		printf("\r\n Openamp communication round %d done!\r\n", i);
		printf("\r\n **********************************");
		printf("****\r\n");
	}

	return 0;
}

// stop the openamp
int OpenAMPStop(void)
{
	// destroy the mutex
	pthread_mutex_destroy(&rpmsg_mutex);

	close(fd);
	if (charfd >= 0)
		close(charfd);

	// // remove driver
	// system("modprobe -r rpmsg_char");

	// stop processor
	sprintf(sbuf,
			"/sys/class/remoteproc/remoteproc%u/state",
			r5_id);
	(void)file_write(sbuf, "stop");

	return 0;
}

int OpenAMPTest(void)
{
	int bytes_rcvd, bytes_sent;
	u8 temp_buff[MAX_RPMSG_SIZE] = {0};

	R5_state.rpmsg_type = READ_R5_STATE_FROM_APU;
	R5_cmd.rpmsg_type = SEND_R5_CMD_FROM_APU;

	printf("\r\n **********************************");
	printf("\r\n  Openamp test\r\n");

	// copy the date
	memcpy(temp_buff, &R5_cmd, sizeof(r5_cmd));

	// read the r5 core state
	bytes_sent = write(fd, temp_buff, MAX_RPMSG_SIZE);

	if (bytes_sent <= 0)
	{
		printf("\r\n Error sending data\n");
		printf(" .. \r\n");
	}
	else
	{
		printf("\r\n Send %d bytes\n", bytes_sent);
	}

	// read the data
	do
	{
		bytes_rcvd = read(fd, temp_buff, MAX_RPMSG_SIZE);
	} while ((bytes_rcvd < MAX_RPMSG_SIZE) || (bytes_rcvd < 0));
	printf("\rRead %d bytes\n", bytes_rcvd);

	// copy the r5 state
	memcpy(&R5_state, temp_buff, sizeof(r5_state));

	printf("\rps core tmp:%d\n", R5_state.ps_core_temp);
	printf("\rpl core tmp:%d\n", R5_state.pl_core_temp);
	return 0;
}

// init openamp
int OpenAMPInit(void)
{
	int state;
	state = OpenAMPLoadFirmware();

	if (0 == state)
	{
		printf("\rOpenAMP load firmware success!\r\n");
	}
	else
	{
		perror("\rOpenAMP load firmware Failed!\r\n");
	}

	// must wait remote processor init
	sleep(3);

	printf("\rLoad OpenAMP device driver!\r\n");

	if (0 == OpenAMPLoadDriver())
	{
		printf("\rOpenAMP load driver success!\r\n");
	}
	else
	{
		perror("\rOpenAMP load driver Failed!\r\n");
	}

	// wait for the driver load to complete
	sleep(3);

	// init mutex
	state = pthread_mutex_init(&rpmsg_mutex, NULL);
	if (state != 0)
	{
		perror("\rrpmsg mutex initialization failed!\n");
		exit(EXIT_FAILURE);
	}

	return state;
}

// send a rpmsg
int RPMsgSend(void *msg, u8 msg_size)
{
	int bytes_sent, state;
	u8 temp_buff[MAX_RPMSG_SIZE] = {0};
	if (msg_size > MAX_RPMSG_SIZE)
	{
		perror("\rrpmsg size exceed!\n");
	}

	// copy the date
	memcpy(temp_buff, msg, msg_size);

	// pthread_mutex_lock(&rpmsg_mutex);
	// send the rpmsg
	bytes_sent = write(fd, temp_buff, MAX_RPMSG_SIZE);
	// pthread_mutex_unlock(&rpmsg_mutex);

	if (bytes_sent <= 0)
	{
		perror("\r\n rpmsg send error!\n");
		// printf(" .. \r\n");
		state = -1;
	}
	else
	{
		state = 0;
	}
	return state;
}

// read a rpmsg
int RPMsgRead(void *msg, u8 msg_size)
{
	int bytes_rcvd, state;
	u8 temp_buff[MAX_RPMSG_SIZE] = {0};
	if (msg_size > MAX_RPMSG_SIZE)
	{
		perror("\rrpmsg size exceed!\n");
	}

	// pthread_mutex_lock(&rpmsg_mutex);
	// read the msg
	do
	{
		bytes_rcvd = read(fd, temp_buff, MAX_RPMSG_SIZE);
	} while ((bytes_rcvd < MAX_RPMSG_SIZE) || (bytes_rcvd < 0));
	// pthread_mutex_unlock(&rpmsg_mutex);

	state = 0;

	// copy the data
	memcpy(msg, temp_buff, msg_size);

	return state;
}