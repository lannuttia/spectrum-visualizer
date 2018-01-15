#include <stdio.h>
#include <stdlib.h>
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
  FILE *pFileStereo = NULL;
  FILE *pFileLeft = NULL;
  FILE *pFileRight = NULL;

  pFileStereo = fopen("/home/anthony/pulseCapture_Stereo.raw", "wb");
  if( !pFileStereo )
  {
    fprintf(stderr,__FILE__": fopen() for stereo failed: %s\n",strerror(errno));
    goto exiterror;
  }

  pFileLeft = fopen("/home/anthony/pulseCapture_Left.raw", "wb");
  if( !pFileLeft )
  {
    fprintf(stderr,__FILE__": fopen() for left failed: %s\n",strerror(errno));
    goto exiterror;
  }

  pFileRight = fopen("/home/anthony/pulseCapture_Right.raw", "wb");
  if( !pFileRight )
  {
    fprintf(stderr,__FILE__": fopen() for right failed: %s\n",strerror(errno));
    goto exiterror;
  }

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

  for( int idx = 0; idx < 1 ; idx++ )
  {
    retval = pa_simple_read(pHandle,buf,bufSize,&errval);
    if( retval )
    {
      fprintf(stderr,__FILE__": pa_simple_read() failed: %s\n",pa_strerror(errval));
      goto exiterror;
    }

    deinterleaveRawStereo( buf, bufSize, sizeof(int16_t), leftBuf, rightBuf );

    retval = fwrite(buf,bufSize,1,pFileStereo);
    if( retval != bufSize )
    {
      fprintf(stdout,"Failed to write entire buffer to file\n");
    }

    retval = fwrite(leftBuf,(bufSize >> 1),1,pFileLeft);
    if( retval != (bufSize >> 1) )
    {
      fprintf(stdout,"Failed to write entire buffer to file\n");
    }

    retval = fwrite(rightBuf,(bufSize >> 1),1,pFileRight);
    if( retval != (bufSize >> 1) )
    {
      fprintf(stdout,"Failed to write entire buffer to file\n");
    }
  }

  exiterror:
    if( pHandle )
      pa_simple_free(pHandle);

    if( pFileStereo )
      fclose(pFileStereo);

    if( pFileLeft )
      fclose(pFileLeft);

    if( pFileRight )
      fclose(pFileRight);

    if( buf )
      free(buf);

    if( leftBuf )
      free(leftBuf);

    if( rightBuf )
      free(rightBuf);

    return errval;
}
