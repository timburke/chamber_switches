#include <Arduino.h>

uint8_t substrate_map[40] = 
{
0x30,  //1
0x58,  //2
0x28,  //3
0x2A,  //4
0x32,  //5
0x5A,  //6
0x52,  //7
0x50,  //8
0x20,  //9
0x2C,  //10
0x34,  //11
0x5C,  //12
0x54,  //13
0x48,  //14
0x22,  //15
0x2E,  //16
0x36,  //17
0x5E,  //18
0x56,  //19
0x4A,  //20
0x24,  //21
0x38,  //22
0x40,  //23
0xA8,  //24
0xA0,  //25
0x4C,  //26
0x26,  //27
0x3A,  //28
0x42,  //29
0xAA,  //30
0xA2,  //31
0x4E,  //32
0x3E,  //33
0x3C,  //34
0x44,  //35
0xAC,  //36
0xA4,  //37
0xA6,  //38
0x46,  //39
0xAE   //40
};

uint8_t pin_map[14] =
{
255,  //1
0,  //2
1,  //3
2,  //4
3,  //5
4,  //6
255,  //7
255,  //8
5,  //9
6,  //10
7,  //11
8,  //12
9,  //13
255  //14
};

uint8_t map_pin(uint8_t index)
{
  if(index < 1 || index > 14)
    return 255;
    
  return pin_map[index-1];
}

uint8_t map_substrate(uint8_t index)
{
  if (index < 1 || index > 40)
    return 0;
    
  return substrate_map[index-1] >> 1;
}
