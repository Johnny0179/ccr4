#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

class can
{
private:
    /* data */
    int s_;

    //CAN filter number, 8 canopen devices, each has 4 TxPDO.
    static const __u8 kNodeNum = 6;

    static const int kCanFilterNum = 24;

public:
    can(/* args */);
    ~can();

    void close_socketCAN(void);
    int send(const struct can_frame *send_frame);
    int receive(can_frame *recv_frame);
};


