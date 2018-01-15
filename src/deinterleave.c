#include <string.h>
#include <pulse/simple.h>

#include "deinterleave.h"

int deinterleaveRawStereo(const void *buf, const int bufLen, size_t dataSize, void *pLeft, void *pRight)
{
  int errval = 0;

  for(uint32_t idx = 0; idx < bufLen; idx += dataSize << 1)
  {
    // Grabs the audio for the left speaker
    memcpy(pLeft+(idx >> 1),buf+idx,dataSize);

    // Grabs the audio for the right speaker
    memcpy(pRight+(idx >> 1),buf+idx+dataSize,dataSize);
  }

  return errval;
}
