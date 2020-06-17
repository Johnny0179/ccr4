#include "r5_controllers.h"

r5_controllers::~r5_controllers()
{
}

// // enable controller
// int controller::Enable()
// {
//     int state;
//     cmd.rpmsg_type = SEND_CONTROLLER_CMD_FROM_APU;

//     // enable motor
//     cmd.motor_enable = 1;
//     state = RPMsgSend(&cmd, sizeof(cmd));
//     return state;
// }

// // disable controller
// int controller::Disable()
// {
//     int state;
//     cmd.rpmsg_type = SEND_CONTROLLER_CMD_FROM_APU;

//     // enable motor
//     cmd.motor_enable = 0;
//     state = RPMsgSend(&cmd, sizeof(cmd));
//     return state;
// }

void r5_controllers::ControllersCmdPoll(void)
{
    controllers_cmd_.rpmsg_type = SEND_CONTROLLER_CMD_FROM_APU;
    for (;;)
    {
        // send controllers command
        RPMsgSend(&controllers_cmd_, sizeof(controllers_cmd_));

        // sleep
        usleep(1000 / control_freq_KHz_);
    }
}

// enable controller
void r5_controllers::SetMode(const u8 &controller_id, const u8 &mode)
{
    controllers_cmd_.rpmsg_type = SEND_CONTROLLER_CMD_FROM_APU;

    switch (controller_id)
    {
    case kUpPalm_ID:

        break;

    case kUpWheel_ID:

        break;

    case kPulley1_ID:
        // limit the modes
        if (mode == kIdle || mode == kFollow || mode == kPull)
        {
            controllers_cmd_.pulley1_cmd_.motion_mode = mode;
        }

        break;

    case kPulley2_ID:

        break;

    case kDownClaw1_ID:

        break;

    case kDownClaw2_ID:

        break;

    default:
        break;
    }
}