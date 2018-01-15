#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#include <fftw3.h>

#include "deinterleave.h"


int main (int argc, char *argv[])
{
  static volatile int keepRunning = 1;

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

  fftw_complex *pFftwLeftBuf = {NULL}; /* Left Output */
  fftw_complex *pFftwRightBuf = {NULL}; /* Right Output */
  fftw_plan fftwLeftPlan = {0}; /* Left Plan */
  fftw_plan fftwRightPlan = {0}; /* Right Plan */

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

  pFftwLeftBuf = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ((bufSize/sampSpec.channels) + 1));
  pFftwRightBuf = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ((bufSize/sampSpec.channels) + 1));

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

    fftwLeftPlan = fftw_plan_dft_r2c_1d(bufSize/sampSpec.channels, leftBuf, pFftwLeftBuf, FFTW_ESTIMATE);
    fftwRightPlan = fftw_plan_dft_r2c_1d(bufSize/sampSpec.channels, rightBuf, pFftwRightBuf, FFTW_ESTIMATE);

    fftw_execute(fftwLeftPlan);
    fftw_execute(fftwRightPlan);
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

    fftw_destroy_plan(fftwLeftPlan);
    fftw_destroy_plan(fftwRightPlan);

    if( pFftwLeftBuf )
      fftw_free(pFftwLeftBuf);

    if( pFftwRightBuf )
      fftw_free(pFftwRightBuf);

    return errval;
}
