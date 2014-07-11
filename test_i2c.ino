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
 cmdAdd("status", report_status);
 cmdAdd("substatus", report_substrate);
 
 //Disconnect all substrates from NO pin
 for (i=1; i<=40; i+=1)
 {
   uint8_t addr = map_substrate(i);
   uint8_t err=0;
   
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

void report_substrate(int argc, char **argv)
{
  if (argc != 2)
  {
    pformat("Usage: substatus <substrate #1-40>");
    return;
  }
  
    uint8_t addr = map_substrate(cmdStr2Num(argv[1],10));
    uint16_t dir;
    uint16_t lev;
    uint16_t no_pins;
    
    lev = get_level(addr);
    dir = get_direction(addr);
    
    pformat("Substrate %s status", argv[1]);
    pformat("Address: 0x%X", addr);
    pformat("Direction: 0x%X", dir);
    pformat("Levels: 0x%X", lev);
}

void report_status(int argc, char **argv)
{
  int i, j;
  for (i = 1; i <= 40; i+=1)
  {
    uint8_t addr = map_substrate(i);
    uint8_t err;
    uint16_t dir;
    uint16_t lev;
    uint16_t no_pins;
    
    lev = get_level(addr);
    dir = get_direction(addr);
    
    if (lev == 0xAAAA && dir == 0xAAAA)
    {
      pformat("Substrate %d: IS NOT RESPONDING", i);
      continue;
    }
    else if(lev & 0xFF == 0xAA || lev >> 8 == 0xAA || dir & 0xFF == 0xAA || lev >> 8 == 0xAA)
    {
      pformat("Substrate %d: error reading registers, but substrate did respond", i);
      continue;
    }
    
    //Print out what we got
    //Do not report substrates that are all connected to normally closed as they should be
    if (dir == 0 && lev == 0)
    {
      continue;
    }
    
    if (dir != 0)
    {
      pformat("Substrate %d: not all pins were configured as outputs.  This is an error.", i);
    }
   
    no_pins = ~(lev | dir);
    
    for (j=0; j<10; j+=1)
    {
      uint16_t probe = 1 << j;
      if (no_pins & probe)
        pformat("Substrate %d: pin index %d connected to NO", i, j);
        
        no_pins >>= 1;
    }
  }
  
  pformat("All other substrates have all pins connected to normally closed."); 
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
  
  if (argc != 3)
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
  
  if (argc != 3)
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
    pformat("Setting Pin %d @ Address 0x%x to NO", pin, addr << 1);
  else
    pformat("Invalid Pin or Substrate Indices: arguments were pin '%s', substrate '%s'", argv[2], argv[1]);
}

void loop()
{
  cmdPoll();
}
