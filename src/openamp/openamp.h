#ifndef ECHO_H
#define ECHO_H

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include "rpmsg.h"
#include "common.h"

#include <pthread.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C"
{
#endif

// shutdown mesage
#define SHUTDOWN_MSG	0xEF56A55A

// motor number of robot
#define MOTOR_NUM 6

// max rpmsg bytes
#define MAX_RPMSG_SIZE (512 - 16)

    int echo_test(void);

    // load firmware
    int OpenAMPLoadFirmware(void);

    int OpenAMPLoadDriver(void);

    // stop the openamp
    int OpenAMPStop(void);

    int OpenAMPTest(void);

    // init the openamp
    int OpenAMPInit(void);

    // openamp send message
    int RPMsgSend(void *msg,u8 msg_size);

    // openamp read
    int RPMsgRead(void *msg,u8 msg_size);
     
    

// exit openamp
#ifdef __cplusplus
}
#endif

#endif