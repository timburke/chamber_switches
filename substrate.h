#include <Arduino.h>

#define kNoError            0
#define kCommunicationError 1

byte init_substrate(uint8_t addr, uint8_t no_con);
byte read_register(uint8_t addr, uint8_t reg);
uint8_t set_no_pin(uint8_t index, uint8_t pin);
uint8_t map_substrate(uint8_t index);
uint8_t map_pin(uint8_t index);
uint16_t get_direction(uint8_t address);
uint16_t get_level(uint8_t address);

void pformat(char *fmt, ... );
