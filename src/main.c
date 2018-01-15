#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include <pulse/simple.h>
#include <pulse/error.h>

#include "deinterleave.h"

int main (int argc, char *argv[])
{
  static const pa_sample_spec sampSpec = {
    .format = PA_SAMPLE_S16LE,
    .rate = 44100,
    .channels = 2
  };
  pa_simple *pHandle = NULL;
  int retval = 0;
  int errval = 0;
  void *buf = NULL;
  uint32_t bufSize = 0;
  void *leftBuf = NULL;
  void *rightBuf = NULL;

  static volatile int keepRunning = 1;

  void sigintHandler()
  {
    printf("Catching and handling ctrl-c\n");
    keepRunning = 0;
  }

  signal(SIGINT, sigintHandler);

  // Allocate buffer large enough to hold 
  // 1 second worth of raw pcm data
  bufSize = sampSpec.channels * sampSpec.rate * sizeof(int16_t);
  buf = malloc(bufSize);

  //Allocate buffers for channel data
  leftBuf = malloc(bufSize/sampSpec.channels);
  rightBuf = malloc(bufSize/sampSpec.channels);

  pHandle = pa_simple_new(NULL,
                          argv[0],
                          PA_STREAM_RECORD,
                          NULL,
                          "record",
                          &sampSpec,
                          NULL,
                          NULL,
                          &errval);
  if( !pHandle )
  {
    fprintf(stderr,__FILE__": pa_simple_new() failed: %s\n",pa_strerror(errval));
    goto exiterror;
  }

  while(keepRunning)
  {
    retval = pa_simple_read(pHandle,buf,bufSize,&errval);
    if( retval )
    {
      fprintf(stderr,__FILE__": pa_simple_read() failed: %s\n",pa_strerror(errval));
      goto exiterror;
    }

    deinterleaveRawStereo( buf, bufSize, sizeof(int16_t), leftBuf, rightBuf );
  }

  exiterror:
    if( pHandle )
      pa_simple_free(pHandle);

    if( buf )
      free(buf);

    if( leftBuf )
      free(leftBuf);

    if( rightBuf )
      free(rightBuf);

    return errval;
}
