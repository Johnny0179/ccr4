// C files

#include "openamp.h"
#include "freemodbus_tcp.h"
#include "utility_print.h"

// c++11 library
#include <thread>
#include <iostream>
#include <csignal>
#include "robot.h"
#include "inter_core_com.h"

// thread function declaration
static void modbus_task();
static void echo_task();

robot ccr4(0.001, 0.01);

void signalHandler(int signum)
{
  std::cout << "Interrupt signal (" << signum << ") received.\n";

  ccr4.Stop();

  exit(signum);
}

int main(int argc, char **argv)
{
  PRINT_BOLDMAGENTA("\rAIRS CCR4 Robort START!\n");

  // modbus thread
  std::thread modbus_thread(modbus_task);
  modbus_thread.detach();

  // openamp thread
  std::thread openamp_thread(echo_task);
  openamp_thread.detach();

  signal(SIGINT, signalHandler);

  for (;;)
  {
    // delay 1s
    sleep(1);
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

  // ccr4 init
  ccr4.Init();
}