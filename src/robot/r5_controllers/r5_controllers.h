#pragma once
#include "inter_core_com.h"

class r5_controllers
{
private:
    /* data */
    // motor id
    static const u8 kUpPalm_ID = 1;
    static const u8 kUpWheel_ID = 2;
    static const u8 kPulley1_ID = 3;
    static const u8 kPulley2_ID = 4;
    static const u8 kDownClaw1_ID = 5;
    static const u8 kDownClaw2_ID = 6;

    // control frequency
    float control_freq_KHz_;

    // controller command
    controllers_cmd controllers_cmd_;

    // controller modes
    static const u8 kIdle = 0;
    static const u8 kHold = 1;
    static const u8 kLoose = 2;
    static const u8 kMove = 3;
    static const u8 kFollow = 4;
    static const u8 kPull = 5;

public:
    explicit r5_controllers(float control_freq_KHz) : control_freq_KHz_(control_freq_KHz) {}
    ~r5_controllers();

    // periodic control loop
    void ControllersCmdPoll(void);

    // set controller mode
    void SetMode(const u8 &controller_id, const u8 &mode);
};
