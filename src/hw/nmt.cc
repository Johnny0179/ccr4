#include "nmt.h"
#include "delay.h"

nmt::nmt(/* args */)
{
}

nmt::~nmt()
{
}

void nmt::NMTstart(void)
{
    delay_us(kDelayEpos);
    can_frame nmt_frame;
    // nmt frame init
    nmt_frame.can_id = kNMT;
    nmt_frame.can_dlc = 2;
    nmt_frame.data[0] = kNMT_Start_Node;
    nmt_frame.data[1] = 0;
    if (can0.send(&nmt_frame) != -1)
    {
        printf("Start all nodes!");
    }
    else
    {
        printf("CAN communication error!\n");
    }
}

void nmt::NMTstart(__u8 slave_id)
{
    delay_us(kDelayEpos);
    can_frame nmt_frame;
    // nmt frame init
    nmt_frame.can_id = kNMT;
    nmt_frame.can_dlc = 2;
    nmt_frame.data[0] = kNMT_Start_Node;
    nmt_frame.data[1] = slave_id;
    if (can0.send(&nmt_frame) != -1)
    {
        if (slave_id != 0)
        {
            printf("Start node%d!\n", slave_id);
        }
        else
        {
            printf("Start all nodes!\n");
        }
    }
    else
    {
        printf("CAN communication error!\n");
    }
}

void nmt::NMTPreOperation(__u8 slave_id)
{
    delay_us(kDelayEpos);
    can_frame nmt_frame;
    // nmt frame init
    nmt_frame.can_id = kNMT;
    nmt_frame.can_dlc = 2;
    nmt_frame.data[0] = kNMT_Enter_PreOperational;
    nmt_frame.data[1] = 0;
    if (can0.send(&nmt_frame) != -1)
    {
        printf("Node%d enter pre operation state!\n", slave_id);
    }
    else
    {
        printf("CAN communication error!\n");
    }
}

void nmt::NMTstop(__u8 slave_id)
{
    can_frame nmt_frame;
    // nmt frame init
    nmt_frame.can_id = kNMT;
    nmt_frame.can_dlc = 2;
    nmt_frame.data[0] = kNMT_Stop_Node;
    nmt_frame.data[1] = slave_id;
    if (can0.send(&nmt_frame) != -1)
    {
        if (slave_id != 0)
        {
            printf("Stop node%d!\n", slave_id);
        }
        else
        {
            printf("Stop all nodes!\n");
        }
    }
    else
    {
        printf("CAN communication error!\n");
    }
}

// cmd sync
void nmt::CmdSync()
{
    can_frame nmt_frame;
    // nmt frame init
    nmt_frame.can_id = kSYNC;
    nmt_frame.can_dlc = 0;

    can0.send(&nmt_frame);
}

// txpdo1
ssize_t nmt::TxPdo1(__u8 slave_id, __u16 ctrl_wrd)
{
    can_frame tx_pdo1_frame;

    // tx_pdo1 frame init
    tx_pdo1_frame.can_id = kPDO1rx + slave_id;
    tx_pdo1_frame.can_dlc = 2;
    tx_pdo1_frame.data[0] = ctrl_wrd & 0xff;
    tx_pdo1_frame.data[1] = (ctrl_wrd >> 8) & 0xff;

    return can0.send(&tx_pdo1_frame);
}

// txpdo2
ssize_t nmt::TxPdo2(__u8 slave_id, __u16 ctrl_wrd, __s32 pos_sv,
                    __u16 mode_of_operation)
{
    can_frame tx_pdo2_frame;

    // tx_pdo2 frame init
    tx_pdo2_frame.can_id = kPDO2rx + slave_id;
    tx_pdo2_frame.can_dlc = 7;

    tx_pdo2_frame.data[0] = ctrl_wrd & 0xff;
    tx_pdo2_frame.data[1] = (ctrl_wrd >> 8) & 0xff;

    tx_pdo2_frame.data[2] = pos_sv & 0xff;
    tx_pdo2_frame.data[3] = (pos_sv >> 8) & 0xff;
    tx_pdo2_frame.data[4] = (pos_sv >> 16) & 0xff;
    tx_pdo2_frame.data[5] = (pos_sv >> 24) & 0xff;
    tx_pdo2_frame.data[6] = mode_of_operation;

    return can0.send(&tx_pdo2_frame);
}

// txpdo3
ssize_t nmt::TxPdo3(__u8 slave_id, __s16 target_torque,
                    __u16 mode_of_operation)
{
    can_frame tx_pdo3_frame;

    // tx_pdo4 frame init
    tx_pdo3_frame.can_id = kPDO3rx + slave_id;

    tx_pdo3_frame.can_dlc = 3;
    tx_pdo3_frame.data[0] = target_torque & 0xff;
    tx_pdo3_frame.data[1] = (target_torque >> 8) & 0xff;
    tx_pdo3_frame.data[2] = mode_of_operation;

    return can0.send(&tx_pdo3_frame);
}

// txpdo4
ssize_t nmt::TxPdo4(__u8 slave_id, __s32 speed_set, __u16 mode_of_operation)
{
    can_frame tx_pdo4_frame;

    // tx_pdo4 frame init
    tx_pdo4_frame.can_id = kPDO4rx + slave_id;
    tx_pdo4_frame.can_dlc = 5;
    tx_pdo4_frame.data[0] = speed_set & 0xff;
    tx_pdo4_frame.data[1] = (speed_set >> 8) & 0xff;
    tx_pdo4_frame.data[2] = (speed_set >> 16) & 0xff;
    tx_pdo4_frame.data[3] = (speed_set >> 24) & 0xff;
    tx_pdo4_frame.data[4] = mode_of_operation;

    return can0.send(&tx_pdo4_frame);
}

// rxpdo1
ssize_t nmt::RxPdo1(__u8 id)
{
    can_frame rx_pdo1_frame;

    rx_pdo1_frame.can_id = kPDO1tx + id;
    rx_pdo1_frame.can_dlc = 0;

    return can0.send(&rx_pdo1_frame);
}

void nmt::MotorParaRead(maxon &motor)
{
    can_frame frame;
    can_frame *recv_frame = &frame;

    can0.receive(recv_frame);

    __u16 cob_id = recv_frame->can_id & (~0x007F);
    __u16 SlaveId = (recv_frame->can_id & 0x7F);

    switch (cob_id)
    {
        // 0x180
    case kPDO1tx:
        motor.parameter.status_word_ =
            (__u16)(recv_frame->data[1] << 8) | recv_frame->data[0];
        motor.parameter.torque_pv_ = (__s16)((recv_frame->data[3] << 8) | recv_frame->data[2]);
        motor.parameter.pos_pv_ =
            (__s32)((recv_frame->data[7] << 24) | (recv_frame->data[6] << 16) |
                    (recv_frame->data[5] << 8) | recv_frame->data[4]);
        break;

        // 0x280
    case kPDO2tx:
        motor.parameter.status_word_ = (recv_frame->data[1] << 8) | recv_frame->data[0];
        motor.parameter.torque_pv_ = (__s16)((recv_frame->data[3] << 8) | recv_frame->data[2]);
        motor.parameter.speed_pv_ =
            (__s32)((recv_frame->data[7] << 24) | (recv_frame->data[6] << 16) |
                    (recv_frame->data[5] << 8) | recv_frame->data[4]);
        break;

        // 0x380
    case kPDO3tx:
        motor.parameter.actual_average_vel_ =
            (__s32)((recv_frame->data[3] << 24) | recv_frame->data[2] << 16 |
                    (recv_frame->data[1] << 8) | recv_frame->data[0]);
        motor.parameter.actual_average_torque_ =
            (__s16)((recv_frame->data[5] << 8) | recv_frame->data[4]);
        break;

        // 0x480
    case kPDO4tx:
        motor.parameter.status_word_ =
            (__u16)(recv_frame->data[1] << 8) | recv_frame->data[0];
        motor.parameter.serv_error_ =
            (__u16)((recv_frame->data[3] << 8) | recv_frame->data[2]);
        motor.parameter.torque_pv_ = (__s16)((recv_frame->data[5] << 8) | recv_frame->data[4]);
        motor.parameter.mode_display_ = recv_frame->data[6];
        break;

    default:
        break;
    }
}