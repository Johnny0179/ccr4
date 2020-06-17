#include "robot.h"
#include "utility_print.h"
#include <iostream>

using namespace std;

robot::robot(float a53_control_freq_KHz, float r5_control_freq_KHz , USHORT reg[])
{
    a53_control_freq_KHz_ = a53_control_freq_KHz;
    // r5_controllers_ = new r5_controllers(r5_control_freq_KHz);
    // r5_cmd_->control_frequency_KHz = r5_control_freq_KHz;
    r5_cmd_=(r5_cmd *)&reg[0];
    // r5_cmd_ = new r5_cmd;
    r5_state_=(r5_state *)&reg[REG_HOLDING_NREGS/2];
    // r5_state_ = new r5_state;
}

robot::~robot()
{
    // delete r5_controllers_;
    // delete r5_cmd_;
    // delete r5_state_;
}

// Init robot
void robot::Init()
{
    int state;

    usleep(100000);

    // Init the r5 core
    if (0 == OpenAMPInit())
    {
        // cout << "\r r5-0 init success.\n";
        PRINT_GREEN("\rR5@0 INIT SUCCESS.\n");
    }
    else
    {
        // cout << "\r r5-0 init failed.\n";
        PRINT_RED("\rR5@0 init failed.\n");
        //  remove the openamp driver
        OpenAMPStop();
        abort();
    }

    // set the control frequency of r5-0 controllers
    r5_cmd_->control_frequency_KHz = 0.1;

    // start the nmt
    // r5_cmd_->rpmsg_type = SEND_R5_CMD_FROM_APU;
    r5_cmd_->nmt_control = kNMTStart;
    // state = RPMsgSend(r5_cmd_, sizeof(r5_cmd));
    SetR5_0Cmd();

    if (state == 0)
    {
        PRINT_GREEN("\r rpmsg send.\n");
    }
}

// start robot
void robot::Start()
{
    MotionControl();
}

// Stop robot
void robot::Stop()
{

    // disable all r5 controllers
    std::cout << "Disable all r5 controllers...\n";

    // stop nmt
    std::cout << "Stop nmt...\n";
    // r5_cmd_->rpmsg_type = SEND_R5_CMD_FROM_APU;
    r5_cmd_->nmt_control = kNMTStop;
    // RPMsgSend(r5_cmd_, sizeof(r5_cmd));
    SetR5_0Cmd();

    // send shutdown message
    // PRINT_MAGENTA("\rstop remote core.\n");
    // unsigned int shutdown_msg = SHUTDOWN_MSG;
    // RPMsgSend(&shutdown_msg, sizeof(unsigned int));

    sleep(1);

    // stop remote r5-0 core
    std::cout << "Shut down r5-0...\n";
    OpenAMPStop();

    // wait operation
    sleep(2);

    PRINT_BOLDMAGENTA("Goodbye (^_^).");
}

// send r5 command
void robot::SetR5_0Cmd()
{
    r5_cmd_->rpmsg_type = SEND_R5_CMD_FROM_APU;
    RPMsgSend(r5_cmd_, sizeof(r5_cmd_));
}

// read r5 state
void robot::ReadR5_0State()
{
    r5_state_->rpmsg_type = READ_R5_STATE_FROM_APU;
    RPMsgSend(r5_state_, sizeof(r5_state_));
    RPMsgRead(r5_state_, sizeof(r5_state_));
}

void robot::MotionFSM(enum motion_cmd &motion_cmd)
{
    switch (motion_cmd)
    {
    case IDLE:
        /* code */
        break;

        // upclaw hold
    case UPCLAW_HOLD:
        // set the hold torque,30%
        r5_cmd_->upclaw_hold_trq = 300;

        // check if the hold motion complete
        if (r5_state_->upclaw_parameter.actual_average_vel_ != 0 || abs(r5_state_->upclaw_parameter.actual_average_torque_ - r5_cmd_->upclaw_hold_trq) > 10)
        {
            // set the motion command
            motion_cmd = UPCLAW_HOLD;
        }
        else
        {
            // motion complete, go back to IDLE state
            motion_cmd = IDLE;
        }

        break;

    default:
        break;
    }
}

void robot::MotionControl(void)
{
    for (;;)
    {
        MotionFSM(r5_cmd_->cntrlr_motion_cmd);
        // send command
        r5_state_->rpmsg_type = READ_R5_STATE_FROM_APU;
        RPMsgSend(r5_cmd_, sizeof(r5_cmd));
        /* controller loop */
        usleep(1000 / a53_control_freq_KHz_);
    }
}
