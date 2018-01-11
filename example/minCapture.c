#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
      
int main (int argc, char *argv[])
{
  int                  errval = 0;
  int                  dir = 0;
  FILE*                fp;
  uint32_t             buffSize = 0;
  char                *buff = NULL;
  uint32_t             sampRateHz = 44100;
  long                 loops = 0;
  snd_pcm_t           *pCaptureHandle = NULL;
  snd_pcm_uframes_t    frames;
  snd_pcm_hw_params_t *pHwParams = NULL;

  errval = snd_pcm_open(&pCaptureHandle, "default",SND_PCM_STREAM_CAPTURE,0);
  if( errval )
  {
    fprintf(stderr,"Unable to open pcm device: %s\n",
            snd_strerror(errval));
    goto exiterror;
  }
  
  snd_pcm_hw_params_alloca(&pHwParams);
  if( !pHwParams )
  {
    fprintf(stderr,"Unable to allocate Hardware Params object\n");
    goto exiterror;
  }

  fp = fopen("/home/anthony/testAudio.pcm","wb");
  if( !fp )
  {
    fprintf(stderr,"Unable open file for writing\n");
    goto exiterror;
  }

  snd_pcm_hw_params_any(pCaptureHandle, pHwParams);

  snd_pcm_hw_params_set_access(pCaptureHandle, pHwParams,
                               SND_PCM_ACCESS_RW_INTERLEAVED);

  snd_pcm_hw_params_set_format(pCaptureHandle, pHwParams,
                               SND_PCM_FORMAT_S16_LE);

  snd_pcm_hw_params_set_channels(pCaptureHandle, pHwParams, 2);

  snd_pcm_hw_params_set_rate_near(pCaptureHandle, pHwParams,
                                  &sampRateHz, &dir);

  frames = 32;
  snd_pcm_hw_params_set_period_size_near(pCaptureHandle,
                                         pHwParams,
                                         &frames,
                                         &dir);

  errval = snd_pcm_hw_params(pCaptureHandle, pHwParams);
  if( errval )
  {
    fprintf(stderr, "unable to set Hardware Parameters: %s\n",
            snd_strerror(errval));
    goto exiterror;
  }

  snd_pcm_hw_params_get_period_size(pHwParams,
                                    &frames,
                                    &dir);
  buffSize = frames * 4;
  buff = (char*)malloc(buffSize);

  snd_pcm_hw_params_get_period_time(pHwParams,&sampRateHz,&dir);

  for(loops = 5000000/sampRateHz; loops > 0; loops--)
  {
    errval = snd_pcm_readi(pCaptureHandle, buff, frames);
    if( errval == -EPIPE )
    {
      fprintf(stderr, "Overrun occurred\n");
      snd_pcm_prepare(pCaptureHandle);
    }
    else if( errval < 0 )
    {
      fprintf(stderr,"Error from read: %s\n",snd_strerror(errval));
    }
    else if( errval != (int)frames)
    {
      fprintf(stderr,"short read, read %d frames\n", errval);
    }
    fwrite(buff, buffSize,1,fp);
  }

  exiterror: /* TODO: Fix the problems that occur while cleaning up */
    snd_pcm_drain(pCaptureHandle);
    snd_pcm_close(pCaptureHandle);

    if( buff )
      free(buff);

    if( &pHwParams )
      snd_pcm_hw_params_free(pHwParams);

    fclose(fp);

    return( errval );
}
