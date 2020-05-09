#pragma once
#include "inter_core_com.h"

class r5_controllers
{
private:
    /* data */
    // control frequency
    float control_freq_KHz_;

    // controller command
    controllers_cmd controllers_cmd_;

    // state
    // controller_state state_;

public:
    explicit r5_controllers(float control_freq_KHz) : control_freq_KHz_(control_freq_KHz) {}
    ~r5_controllers();

    // periodic control loop
    void ControllersCmdPoll(void);
};
