#pragma once
#include "nmt.h"
#include "delay.h"

// Forward Declaration
class nmt;

struct maxon_type
{
  /* data */
  __u16 motor_id_;

  __u16 mode_display_;

  __u16 serv_error_;

  __s16 status_word_;

  __s32 pos_pv_;

  __s32 SpdSV;

  __s32 speed_pv_;

  __s32 actual_average_vel_;

  __s16 torque_pv_;

  __s16 actual_average_torque_;
};

class maxon
{
private:
  /* data */
  maxon_type parameter;

  // delay_time wait for epos 1000us
  static const __u32 kDelayEpos = 1000;

  // Config parameters
  static const __s8 kCfgSuccess = 0;
  static const __s8 kCfgFail = -1;

public:
  maxon(/* args */);
  ~maxon();

  friend class nmt;

  ssize_t SetCtrlWrd(nmt &nmt, __u16 ctrl_wrd);
  // disable motor
  __s8 Disable(nmt &nmt);
  __s8 Enable(nmt &nmt);

  // __s8 SetSpeed(nmt nmt, __s32 speed);

  // void SetTorque(nmt nmt, __s16 torque);
};


