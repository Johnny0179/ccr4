#include "can.h"

can::can(/* args */)
{
    int i, j;
    struct sockaddr_can addr;
    struct ifreq ifr;

    // the stack will overflow if not add 1?
    struct can_filter rfilter[kCanFilterNum + 1];

    // create socket
    s_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    strcpy(ifr.ifr_name, "can0");

    ioctl(s_, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // bind can0
    bind(s_, (struct sockaddr *)&addr, sizeof(addr));

    // receive filter
    for (i = 1; i <= kNodeNum; i++)
    {
        // printf("i:%d\n", i);
        for (j = 1; j <= 4; j++)
        {
            // printf("j:%d\n", j);
            switch (j)
            {
                // TxPDO1
            case 1:
                rfilter[4 * (i - 1) + j].can_id = 0x180 + i;
                rfilter[4 * (i - 1) + j].can_mask = CAN_SFF_MASK;
                // printf("can_id:%x\n", rfilter[4 * (i - 1) + j].can_id);
                break;

                // TxPDO2
            case 2:
                rfilter[4 * (i - 1) + j].can_id = 0x280 + i;
                rfilter[4 * (i - 1) + j].can_mask = CAN_SFF_MASK;
                // printf("can_id:%x\n", rfilter[4 * (i - 1) + j].can_id);
                break;

                // TxPDO3
            case 3:
                rfilter[4 * (i - 1) + j].can_id = 0x380 + i;
                rfilter[4 * (i - 1) + j].can_mask = CAN_SFF_MASK;
                // printf("can_id:%x\n", rfilter[4 * (i - 1) + j].can_id);
                break;

                // TxPDO4
            case 4:
                rfilter[4 * (i - 1) + j].can_id = 0x480 + i;
                rfilter[4 * (i - 1) + j].can_mask = CAN_SFF_MASK;
                // printf("can_id:%x\n", rfilter[4 * (i - 1) + j].can_id);
                break;

            default:
                break;
            }
        }
    }

    // setsockopt(s_, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    // set the socket options
    setsockopt(s_, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
}

can::~can()
{
}

int can::send(const struct can_frame *send_frame)
{
    return write(s_, send_frame, sizeof(*send_frame));
}

int can::receive(can_frame *recv_frame)
{
    return read(s_, recv_frame, sizeof(*recv_frame));
}

void can::close_socketCAN(void)
{
    close(s_);
}
