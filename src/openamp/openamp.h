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

// motor number of robot
#define MOTOR_NUM 6

// max rpmsg bytes
#define MAX_RPMSG_SIZE (512 - 16)

int echo_test(void);

// init the openamp 
int OpenAMPInit(void);

// stop the openamp 
int OpenAMPStop(void);

// exit openamp

#endif