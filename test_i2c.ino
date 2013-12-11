#include <Wire.h>
#include <Cmd.h>
#include "substrate.h"

uint8_t setup_error = 0;
uint8_t last_address_error = 0;
uint8_t setup_done = 0;

void setup()
{
 int i;
 
 setup_error = 0;
 
 delay(500);
 
 Wire.begin();
 cmdInit(57600);
 cmdAdd("sub2no", sub_to_no);
 cmdAdd("pin2no", pin_to_no);
 cmdAdd("initstatus", setup_status);
 
 //Disconnect all substrates from NO pin
 for (i=1; i<40; i+=1)
 {
   uint8_t addr = map_substrate(i);
   uint8_t err;
   
   err = init_substrate(addr, 0);
   
   if (err)
   {
     setup_error = 1;
     last_address_error = i;
   }
 }
 
 setup_done = 1;
 pformat("Done initializing substrates");
}

void pformat(char *fmt, ... )
{
        char tmp[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(tmp, 128, fmt, args);
        va_end (args);
        Serial.println(tmp);
}

/*
 * Send command to substrate in order to set the direction
 * of the io expander ports 1&2
 */


void sub_to_no(int argc, char **argv)
{
  uint8_t no_con = 0;
  uint32_t addr;
  uint8_t err;
  
  if (argc < 3)
  {
    Serial.println("usage: sub2no <substrate> <yes|no>");
    return;
  }
  
  if (strcmp(argv[2], "yes") == 0)
    no_con = 1;
  else if (strcmp(argv[2], "no") == 0)
    no_con = 0;
  else
  {
    Serial.println("Invalid connection type: either yes or no expected");
    return;
  }
  
  addr = cmdStr2Num(argv[1],10);
  
  err = init_substrate(map_substrate(addr), no_con);
  
  pformat("Talked to (<< 1) Address 0x%x", map_substrate(addr) << 1);
  
  if (err == kNoError)
    pformat("Command Successful");
  else
    pformat("Command FAILED!!");
}

void setup_status(int argc, char **argv)
{
  if (setup_error == 0)
    pformat("All substrates initialized correctly.");
  else
    pformat("Initialization error.  Last error occured on substrate %d", last_address_error);
}

void pin_to_no(int argc, char **argv)
{
  uint8_t pin, result;
  uint32_t addr;
  
  if (argc < 3)
  {
    Serial.println("usage: pin2no <substrate> <pin>");
    return;
  }
  
  addr = cmdStr2Num(argv[1],10);
  pin = cmdStr2Num(argv[2],10);
  
  //set_no_pin expects a substrate index and a pin index
  result = set_no_pin(addr, pin);
  
  addr = map_substrate(cmdStr2Num(argv[1],10));
  
  if (result == 0)
    pformat("Setting Pin %d @ Address 0x%x to NO", pin, addr);
  else
    pformat("Invalid Pin or Substrate Indices: pin %s at substrate %s", argv[2], argv[1]);
}

void loop()
{
  cmdPoll();
}
