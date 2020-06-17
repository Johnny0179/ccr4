#ifndef INTER_CORE_COM_H
#define INTER_CORE_COM_H

#include "openamp.h"
#include "common.h"

// define the rpmsg type
#define SEND_R5_CMD_FROM_APU 1
#define READ_R5_STATE_FROM_APU 2
#define SEND_CONTROLLER_CMD_FROM_APU 3
#define READ_CONTROLLER_CMD_FROM_APU 4

// claw control command
typedef struct claw_control_cmd_type
{
    // motion mode
    u8 motion_mode;

    // hold torque;
    s16 hold_torque;

    // loose distance
    s32 loose_dis;

} claw_control_cmd;

// upwheel control command
typedef struct upwheel_control_cmd_type
{
    // motion mode
    u8 motion_mode;

    // move distance
    s32 move_dis;

} upwheel_control_cmd;

// pulley controll command
typedef struct pulley_control_cmd_type
{
    //choose motion mode
    u8 motion_mode;

    // admittance control factor
    float factor_k;

    // pull master distance
    s32 pull_dis;

} pulley_control_cmd;

// controller command structure
typedef struct controllers_cmd_type
{
    // the rpmsg type
    u8 rpmsg_type;

    // control frequency
    float control_frequency_KHz;

    // controllers cmd
    claw_control_cmd upclaw_cmd_;
    upwheel_control_cmd upwheel_cmd_;
    pulley_control_cmd pulley1_cmd_;
    pulley_control_cmd pulley2_cmd_;
    claw_control_cmd downclaw1_cmd_;
    claw_control_cmd downclaw2_cmd_;

} controllers_cmd;

// maxon parameter
struct maxon_type
{
	/* data */
	u16 motor_id_;
	u16 mode_display_;
	u16 serv_error_;
	s16 status_word_;
	s32 pos_pv_;
	s32 SpdSV_;
	s32 speed_pv_;
	s32 actual_average_vel_;
	s16 torque_pv_;
	s16 actual_average_torque_;
};

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

     // motor state
     struct maxon_type upclaw_parameter;
     struct maxon_type upwheel_parameter;
     struct maxon_type pulley1_parameter;
     struct maxon_type pulley2_parameter;
     struct maxon_type downclaw1_parameter;
     struct maxon_type downclaw2_parameter;

 } r5_state;

 // motion command
enum motion_cmd
{
    IDLE = 0,
    UPCLAW_HOLD,
    UPCLAW_LOOSE,
    UpwheelMoveUp,
    UpwheelMoveDown,
    PulleysTighten,
    PulleysLockPos,
    PulleysAdmittanceControl,
    PulleysPullUp,
    PulleysPullDown,
    DownclawsHold,
    DownclawsLoose
};

// r5 system command state structure
typedef struct r5_cmd_type
{
    // the rpmsg type
    u8 rpmsg_type;

    // the id of the r5 core, 0 or 1
    u8 r5_id;

    // nmt control
    u8 nmt_control;

    /* controllers command */
    // control frequency
    u32 control_frequency_KHz;

    // motion command
    enum motion_cmd cntrlr_motion_cmd;

    // upclaw hold torque
    s16 upclaw_hold_trq;

    // upclaw loose relative position
    s32 upclaw_rltv_pos;

} r5_cmd;

#endif