#include "robot.h"
#include "utility_print.h"
#include <iostream>

using namespace std;

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

    // start the nmt
    r5_cmd_.rpmsg_type = SEND_R5_CMD_FROM_APU;
    r5_cmd_.nmt_control = kNMTStart;
    state = RPMsgSend(&r5_cmd_, sizeof(r5_cmd));

    // enable pulley1
}

// start robot
void robot::Start()
{
}

// Stop robot
void robot::Stop()
{

    // disable all r5 controllers
    std::cout << "Disable all r5 controllers...\n";

    // stop nmt
    std::cout << "Stop nmt...\n";
    r5_cmd_.rpmsg_type = SEND_R5_CMD_FROM_APU;
    r5_cmd_.nmt_control = kNMTStop;
    RPMsgSend(&r5_cmd_, sizeof(r5_cmd));

    // send shutdown message
    PRINT_MAGENTA("\rstop remote core.\n");
    unsigned int shutdown_msg = SHUTDOWN_MSG;
    RPMsgSend(&shutdown_msg, sizeof(unsigned int));

    sleep(1);

    // stop remote r5-0 core
    std::cout << "Shut down r5-0...\n";
    OpenAMPStop();

    // wait operation
    sleep(2);

    PRINT_BOLDMAGENTA("Goodbye (^_^).");
}
