#include "checksum.hpp"
#include <cstdint>

uint16_t checksum(const uint16_t *addr, int len)
{

  uint32_t sum = 0;

  while (len > 1)
  {
    sum += *addr++;
    len -= 2;
  }

  if(len == 1)
  {
    sum += *(const uint8_t*)addr;
  }
  
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);

  return (uint16_t)~sum;
}
