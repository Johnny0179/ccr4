#pragma once
#include "r5_controllers.h"
#include "inter_core_com.h"
#include <csignal>
#include <thread>
// enum class cmd
// {
//     idle
// };

class robot
{
private:
    /* data */

    // FSM

    // NMT Start
    static const int kNMTStart = 1;

    // NMT stop
    static const int kNMTStop = 2;

    // control frequency
    float a53_control_freq_KHz_;

    // r5 controllers
    r5_controllers *r5_controllers_ = nullptr;

    // r5_system_state
    r5_state r5_state_;

    // r5_system_cmd
    r5_cmd r5_cmd_;

public:
    explicit robot(float a53_control_freq_KHz, float r5_control_freq_KHz) : a53_control_freq_KHz_(a53_control_freq_KHz)
    {
        r5_controllers_ = new r5_controllers(r5_control_freq_KHz);
    }
    ~robot() { delete r5_controllers_; }

    // Initialization
    void Init();

    // Start
    void Start();

    // Stop
    void Stop();
};
