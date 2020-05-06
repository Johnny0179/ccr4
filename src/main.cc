// C files

#include "openamp.h"
#include "freemodbus_tcp.h"

// c++11 library
#include <thread>
#include <iostream>
#include "robot.h"
#include "inter_core_com.h"

// extern variables
extern USHORT usRegHoldingBuf[REG_HOLDING_NREGS];
extern r5_cmd R5_cmd;

// thread function declaration
static void modbus_task();
static void echo_task();

int main()
{
  int i;
  for (i = 0; i <= MOTOR_NUM; i++)
  {
    usRegHoldingBuf[i * MAX_RPMSG_SIZE / 2] = i;
  }

  // nmt
  // nmt nmt;

  // init motors with motor id
  // maxon upwheel(2), upclaw(1), pulley1(3), pulley2(4), downclaw1(5), downclaw2(6);

  // robot
  // robot robot(nmt,upwheel, upclaw, pulley1, pulley2, downclaw1, downclaw2);

  // modbus thread
  std::thread modbus_thread(modbus_task);
  modbus_thread.detach();

  // openamp thread
  std::thread openamp_thread(echo_task);
  openamp_thread.detach();

  for (;;)
  {
    // delay 1us
    usleep(1);
  }
  return 0;
}

// modbus polling thread
void modbus_task()
{
  // init modbus tcp
  if (eMBTCPInit(MODBUS_TCP_PORT) != MB_ENOERR)
  {
    fprintf(stderr, "%s: can't initialize modbus stack!\r\n", PROG);
  }
  else
  {
    printf("modbus stack started successfully!\n");
    // enable protocal
    if (eMBEnable() == MB_ENOERR)
    {
      for (;;)
      {
        // openamp communication
        // echo_test();
        if (eMBPoll() == MB_ENOERR)
        {
          // refresh rate 10ms
          usleep(10000);
        }
        else
        {
          printf("modbus poll error!");
          break;
        }
      }
    }
  }

  // disable modbus stack
  (void)eMBDisable();
  // return 0;
}

void echo_task()
{
  int res;
  // openAMP load firmware
  res = OpenAMPLoadFirmware();
  if (0 == res)
  {
    std::cout << "\rOpenAMP load firmware success!\r\n"
              << std::endl;
  }
  else
  {
    std::cout << "\rOpenAMP load firmware Failed!\r\n"
              << std::endl;
  }

  // must wait remote processor init
  sleep(5);

  std::cout << "\rLoad OpenAMP device driver!\r\n"
            << std::endl;
  // load device driver
  res = OpenAMPLoadDriver();
  std::cout << "\rLoad OpenAMP device driver success!\r\n"
            << std::endl;

  // wait remote processor init
  sleep(5);

  for (;;)
  {
    // start nmt
    R5_cmd.nmt_control = 1;

    // echo_test();
    OpenAMPTest();

    sleep(2);

    // stop nmt
    R5_cmd.nmt_control=2;

        // echo_test();
    OpenAMPTest();

    sleep(2);
    // break;
  }
  OpenAMPStop();
}