#ifndef INTER_CORE_COM_H
#define INTER_CORE_COM_H

#include "openamp.h"
#include "common.h"


// define the rpmsg type
#define SEND_R5_CMD_FROM_APU 1
#define READ_R5_STATE_FROM_APU 2
#define SEND_CONTROLLER_CMD_FROM_APU 3
#define READ_CONTROLLER_CMD_FROM_APU 4

// motor command
typedef struct controller_cmd_type
{
    // controller enable control
    u8 controller_state;

    // mode of operation
    u16 mode_of_operation;

    // target relative position
    s32 target_rela_pos;

    // target speed
    s32 target_speed;

    // target torque
    s16 target_torque;
} controller_cmd;

// r5 system command structure
typedef struct controllers_cmd_type
{
    // the rpmsg type
    u8 rpmsg_type;

    // control frequency
    float control_frequency_KHz;

    // controllers cmd
    controller_cmd controller1_cmd;
    controller_cmd controller2_cmd;
    controller_cmd controller3_cmd;
    controller_cmd controller4_cmd;
    controller_cmd controller5_cmd;
    controller_cmd controller6_cmd;

} controllers_cmd;

// controller states
// enum class controller_state
// {
//     disable,
//     enable,
//     error
// };

// r5 system state structure
typedef struct r5_state_type
{
    // the rpmsg type
    u8 rpmsg_type;

    // the id of the r5 core, 0 or 1
    u8 r5_id;

    // ps core temperature
    u8 ps_core_temp;

    // pl core temperature
    u8 pl_core_temp;

    // nmt state
    u8 nmt_state;

} r5_state;

// r5 system command state structure
typedef struct r5_cmd_type
{
    // the rpmsg type
    u8 rpmsg_type;

    // the id of the r5 core, 0 or 1
    u8 r5_id;

    // nmt control
    u8 nmt_control;

} r5_cmd;

#endif