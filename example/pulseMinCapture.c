#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <pulse/simple.h>
#include <pulse/error.h>

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
  FILE *pFile = NULL;

  pFile = fopen("/home/anthony/testPulseCapture.raw", "wb");
  if( !pFile )
  {
    fprintf(stderr,__FILE__": fopen() failed\n");
    goto exiterror;
  }

  // Allocate buffer large enough to hold 
  // 1 second worth of raw pcm data
  bufSize = sampSpec.channels * sampSpec.rate * sizeof(int16_t);
  buf = malloc(bufSize);

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

  for( int idx = 0; idx < 5; idx++ )
  {
    retval = pa_simple_read(pHandle,buf,bufSize,&errval);
    if( retval )
    {
      fprintf(stderr,__FILE__": pa_simple_read() failed: %s\n",pa_strerror(errval));
      goto exiterror;
    }

    retval = fwrite(buf,bufSize,1,pFile);
    if( retval != bufSize )
    {
      fprintf(stdout,"Failed to write entire buffer to file\n");
    }
  }

  exiterror:
    if( pHandle )
      pa_simple_free(pHandle);

    if( pFile )
      fclose(pFile);

    if( buf )
      free(buf);

    return errval;
}
