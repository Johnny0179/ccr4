// C files
extern "C"
{
#include "echo.h"
#include "freemodbus_tcp.h"
}

// c++11 library
#include <thread>

// extern variables
extern USHORT usRegHoldingBuf[REG_HOLDING_NREGS];

// thread function declaration
static void modbus_task();
static void echo_task();

int main()
{
  int res;

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
        if (eMBPoll() == MB_ENOERR)
        {
          // refresh rate 1ms
          usleep(1000);
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
  // char *dev="/dev/rpmsg0";
  echo_test();
}