#include <Arduino.h>
#include <Wire.h>
#include "substrate.h"

/*
 * Send two bytes to the i2c address given in addr.  The first byte is cmd and 
 * the second byte is val.  This is used to communicate with the MAX7318 chip.
 */
 
byte send_command(uint8_t addr, uint8_t cmd, uint8_t val)
{
  Wire.beginTransmission(addr);
  Wire.write(cmd);
  Wire.write(val);
  return Wire.endTransmission();
}

byte read_register(uint8_t addr, uint8_t reg)
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(addr, (uint8_t)1);
  
  if (Wire.available())
    return Wire.read();
   
  return 0xAA;
}

uint16_t get_direction(uint8_t address)
{
  uint16_t dirlo;
  uint16_t dirhi;
  uint16_t dir;
  
  dirlo = read_register(address, 0x06);
  dirhi = read_register(address, 0x07);
  
  dir = dirlo | (dirhi << 8);
  
  return dir;
}

uint16_t get_level(uint8_t address)
{
  uint16_t dirlo;
  uint16_t dirhi;
  uint16_t dir;
  
  dirlo = read_register(address, 0x02);
  dirhi = read_register(address, 0x03);
  
  dir = dirlo | (dirhi << 8);

  return dir;
}

byte set_direction(uint8_t address, uint16_t dir)
{
  uint8_t p1 = dir & 0xFF;
  uint8_t p2 = (dir & 0xFF00) >> 8;
  uint16_t read_dir;
  
  send_command(address, 0x06, p1);
  send_command(address, 0x07, p2);
  
  read_dir = get_direction(address);
  
  if (read_dir == dir)
    return kNoError;
  
  return kCommunicationError;
}

byte set_level(uint8_t addr, uint16_t levels)
{
  uint8_t p1 = levels & 0xFF;
  uint8_t p2 = (levels & 0xFF00) >> 8;
  uint16_t read_level;
  
  send_command(addr, 0x02, p1);
  send_command(addr, 0x03, p2);

  read_level = get_level(addr);
  
  if (read_level == levels)
    return kNoError;
  
  return kCommunicationError;
}

byte init_substrate(uint8_t addr, uint8_t no_con)
{
  uint16_t out = 0;
  uint8_t level_err;
  uint8_t dir_err;
  
  if (no_con)
    out = 0xFFFF;
    
  level_err = set_level(addr, out);
  dir_err = set_direction(addr, 0);
  
  if (level_err || dir_err)
    return kCommunicationError;
    
  return kNoError;
}

uint8_t set_no_pin(uint8_t index, uint8_t pin)
{
  uint8_t addr, pinno;
  uint16_t level;
  
  addr = map_substrate(index);
  pinno = map_pin(pin);
  
  if (addr == 0 || pinno == 255)
    return 1;
  
  level = 1 << pinno;
  
  //Set this substrate to have only this one pin connected to NO
  set_level(addr, level);
  set_direction(addr, 0);
  
  return 0;
}
