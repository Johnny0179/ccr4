#include "maxon.h"

// init list
maxon::maxon(__u16 motor_id)
{
    parameter.motor_id_ = motor_id;
}

maxon::~maxon()
{
}

ssize_t maxon::SetCtrlWrd(nmt &nmt, __u16 ctrl_wrd)
{
    return nmt.TxPdo1(parameter.motor_id_, ctrl_wrd);
}

// __s8 maxon::SetSpeed(nmt nmt, __s32 speed)
// {
//     // nmt.TxPdo4(motor_id_, speed, );
// }

// disable motor
__s8 maxon::Disable(nmt &nmt)
{
    SetCtrlWrd(nmt, 0x0000);
    delay_us(kDelayEpos);
    // wait disable cmd success
    while (parameter.status_word_ != 0x0240)
    {
        delay_us(kDelayEpos);
    }

    return kCfgSuccess;
}

// enable motor
__s8 maxon::Enable(nmt &nmt)
{
    printf("motor id: %d\n", parameter.status_word_);
    SetCtrlWrd(nmt, 0x0006);
    delay_us(kDelayEpos);
    // bit5: quick stop
    while ((parameter.status_word_ >> 5 & 1) != 1)
    {
        // must delay!
        delay_us(kDelayEpos);
    }

    SetCtrlWrd(nmt, 0x000F);
    delay_us(kDelayEpos);
    // wait enable cmd success
    while ((parameter.status_word_ & 0x00ff) != 0x37)
    {
        // must delay!
        delay_us(kDelayEpos);
    }
    return kCfgSuccess;
}