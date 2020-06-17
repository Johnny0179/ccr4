#pragma once
#include "r5_controllers.h"
#include "inter_core_com.h"
#include "freemodbus_tcp.h"
#include <csignal>
#include <thread>
#include <iostream>
#include <cstdint>
#include <cstring>
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
    // r5_controllers *r5_controllers_ = nullptr;

    // r5_system_state
    r5_state *r5_state_ = nullptr;

    // r5_system_cmd
    r5_cmd *r5_cmd_ = nullptr;

public:
    robot(float a53_control_freq_KHz, float r5_control_freq_KHz , USHORT reg[]);

    ~robot();

    // Initialization
    void Init();

    // Start
    void Start();

    // Stop
    void Stop();

    // Set R5 command
    void SetR5_0Cmd();

    // read r5 state
    void ReadR5_0State();

    // motion control
    void MotionControl(void);

    // motion FSM
    void MotionFSM(enum motion_cmd &motion_cmd);
};
