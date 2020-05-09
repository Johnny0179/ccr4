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
