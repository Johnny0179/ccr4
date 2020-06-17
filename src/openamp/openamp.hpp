#pragma once

#define RPMSG_GET_KFIFO_SIZE 1
#define RPMSG_GET_AVAIL_DATA_SIZE 2
#define RPMSG_GET_FREE_SPACE 3

#define RPMSG_HEADER_LEN 16
#define MAX_RPMSG_BUFF_SIZE (512 - RPMSG_HEADER_LEN)
#define PAYLOAD_MIN_SIZE 1
#define PAYLOAD_MAX_SIZE (MAX_RPMSG_BUFF_SIZE - 24)
#define NUM_PAYLOADS (PAYLOAD_MAX_SIZE / PAYLOAD_MIN_SIZE)

#define RPMSG_BUS_SYS "/sys/bus/rpmsg"

class openamp
{
private:
    /* data */
    int charfd = -1, fd = -1, err_cnt;
    	int ret, i, j;
	int size, bytes_rcvd, bytes_sent;
	err_cnt = 0;
	int opt;
	char *rpmsg_dev="virtio0.rpmsg-openamp-demo-channel.-1.0";
	int ntimes = 1;
	char fpath[256];
	char rpmsg_char_name[16];
	struct rpmsg_endpoint_info eptinfo;
	char ept_dev_name[16];
	char ept_dev_path[32];

public:
    openamp(/* args */);
    ~openamp();
};

openamp::openamp(/* args */)
{
}

openamp::~openamp()
{
}
