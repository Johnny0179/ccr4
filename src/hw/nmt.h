#pragma once
#include "can.h"
#include "maxon.h"

class maxon;

class nmt
{
private:
  // NMT Command Specifier, sent by master to change a slave state, Should not be modified.
  static const __u16 kNMT_Start_Node = 0x01;
  static const __u16 kNMT_Stop_Node = 0x02;
  static const __u16 kNMT_Enter_PreOperational = 0x80;
  static const __u16 kNMT_Reset_Node = 0x81;
  static const __u16 kNMT_Reset_Comunication = 0x82;

  /* CANopen Function Codes */
  static const __u16 kNMT = (__u16)0x0 << 7;
  static const __u16 kSYNC = (__u16)0x1 << 7;
  static const __u16 kTIME_STAMP = (__u16)0x2 << 7;

  /* CANopen Function Codes */
  static const __u16 kPDO1tx = (__u16)0x3 << 7;
  static const __u16 kPDO1rx = (__u16)0x4 << 7;
  static const __u16 kPDO2tx = (__u16)0x5 << 7;
  static const __u16 kPDO2rx = (__u16)0x6 << 7;
  static const __u16 kPDO3tx = (__u16)0x7 << 7;
  static const __u16 kPDO3rx = (__u16)0x8 << 7;
  static const __u16 kPDO4tx = (__u16)0x9 << 7;
  static const __u16 kPDO4rx = (__u16)0xA << 7;
  static const __u16 kSDOtx = (__u16)0xB << 7;
  static const __u16 kSDOrx = (__u16)0xC << 7;
  static const __u16 kNODE_GUARD = (__u16)0xE << 7;
  static const __u16 kLSS = (__u16)0xF << 7;

  // can device
  can can0;

  // delay_time wait for epos 1000us
  static const __u32 kDelayEpos = 1000;

public:
  nmt(/* args */);
  ~nmt();

  /* -------------------------NMT functions------------------------------ */
  void NMTstart(void);
  void NMTstart(__u8 slave_id);
  void NMTPreOperation(__u8 slave_id);
  void NMTstop(__u8 slave_id);
  void CmdSync();

  // canopen
  ssize_t TxPdo1(__u8 slave_id, __u16 ctrl_wrd);

  ssize_t TxPdo2(__u8 slave_id, __u16 ctrl_wrd, __s32 pos_sv,
                 __u16 mode_of_operation);

  // for torque set
  ssize_t TxPdo3(__u8 slave_id, __s16 target_torque, __u16 mode_of_operation);

  ssize_t TxPdo4(__u8 slave_id, __s32 speed_set, __u16 mode_of_operation);

  // rxpdo
  ssize_t RxPdo1(__u8 id);
  // ssize_t RxPdo2(__u8 id);
  // ssize_t RxPdo3(__u8 id);
  // ssize_t RxPdo4(__u8 id);

  void MotorParaRead(maxon &motor);
};
