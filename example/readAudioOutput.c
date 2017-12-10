/*
 * Reading Output Audio example program
 *   Author: Anthony Lannutti <lannuttia@gmail.com>
 *   
 * This small program is intended to be a sanity check for me to verify that I
 * understand how to use the alsa C api.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "alsa/asoundlib.h"

#define PCM_DEVICE "default"

/* flag set by verbose */
static int verbose = 0;

static void help(void)
{
  printf(
    "Usage: readAudioOutput [OPTION]... \n"
    "-h, --help       Help\n"
    "-v, --verbose    Enables verbose output\n"
    "-r, --samplerate Sets sample rate. This defaults to 44100.\n"
    "-f, --format     Sets audio format\n"
    "                    0 = Signed 8 bit\n"
    "                    1 = Unsigned 8 bit\n"
    "                    2 = Signed 16 bit Little Endian\n"
    "                    3 = Signed 16 bit Big Endian\n"
    "                    4 = Unsigned 16 bit Little Endian\n"
    "                    5 = Unsigned 16 bit Big Endian\n"
    "                    6 = Signed 24 bit Little Endian using low three bytes in 32-bit word\n"
    "                    7 = Signed 24 bit Big Endian using low three bytes in 32-bit word\n"
    "                    8 = Unsigned 24 bit Little Endian using low three bytes in 32-bit word\n"
    "                    9 = Unsigned 24 bit Big Endian using low three bytes in 32-bit word\n"
    "                   10 = Signed 32 bit Little Endian\n"
    "                   11 = Signed 32 bit Big Endian\n"
    "                   12 = Unsigned 32 bit Little Endian\n"
    "                   13 = Unsigned 32 bit Big Endian\n"
    "                   14 = Float 32 bit Little Endian, Range -1.0 to 1.0\n"
    "                   15 = Float 32 bit Big Endian, Range -1.0 to 1.0\n"
    "                   16 = Float 64 bit Little Endian, Range -1.0 to 1.0\n"
    "                   17 = Float 64 bit Big Endian, Range -1.0 to 1.0\n"
    "                   18 = IEC-958 Little Endian\n"
    "                   19 = IEC-958 Big Endian\n"
    "                   20 = Mu-Law\n"
    "                   21 = A-Law\n"
    "                   22 = Ima-ADPCM\n"
    "                   23 = MPEG\n"
    "                   24 = GSM\n"
    "                   31 = Special\n"
    "                   32 = Signed 24bit Little Endian in 3 bytes format\n"
    "                   33 = Signed 24bit Big Endian in 3 bytes format\n"
    "                   34 = Unsigned 24bit Little Endian in 3 bytes format\n"
    "                   35 = Unsigned 24bit Big Endian in 3 bytes format\n"
    "                   36 = Signed 20bit Little Endian in 3 bytes format\n"
    "                   37 = Signed 20bit Big Endian in 3 bytes format\n"
    "                   38 = Unsigned 20bit Little Endian in 3 bytes format\n"
    "                   39 = Unsigned 20bit Big Endian in 3 bytes format\n"
    "                   40 = Signed 18bit Little Endian in 3 bytes format\n"
    "                   41 = Signed 18bit Big Endian in 3 bytes format\n"
    "                   42 = Unsigned 18bit Little Endian in 3 bytes format\n"
    "                   43 = Unsigned 18bit Big Endian in 3 bytes format\n"
    "                   44 = G.723 (ADPCM) 24 kbit/s, 8 samples in 3 bytes\n"
    "                   45 = G.723 (ADPCM) 24 kbit/s, 1 sample in 1 byte\n"
    "                   46 = G.723 (ADPCM) 40 kbit/s, 8 samples in 3 bytes\n"
    "                   47 = G.723 (ADPCM) 40 kbit/s, 1 sample in 1 byte\n"
    "                   48 = Direct Stream Digital (DSD) in 1-byte samples (x8)\n"
    "                   49 = Direct Stream Digital (DSD) in 2-byte samples (x16)\n"
    "                   50 = Direct Stream Digital (DSD) in 4-byte samples (x32)\n"
    "                   51 = Direct Stream Digital (DSD) in 2-byte samples (x16)\n"
    "                   52 = Direct Stream Digital (DSD) in 4-byte samples (x32)\n"
    );
};

int main(int argc, char *argv[])
{
  static struct option long_option[] =
  {
    {"help",             no_argument, NULL, 'h'},
    {"verbose",          no_argument, NULL, 'v'},
    {"samplerate", required_argument, NULL, 'r'},
    {"format",     required_argument, NULL, 'f'},
    {0, 0, 0, 0}
  };

  uint8_t              errval         = 0;
  uint32_t             sampRateHz     = 44100;
  uint32_t             channels       = 2;
  unsigned int         numChannels    = 0;
  uint32_t             currSampRate   = 0;
  uint8_t              sampSize       = 0;
  char                *pBuff          = NULL;
  uint32_t             buffSize       = 0;
  snd_pcm_t           *pCaptureHandle = NULL;
  snd_pcm_hw_params_t *pHwParams      = NULL;
  snd_pcm_format_t     format         = SND_PCM_FORMAT_UNKNOWN;
  snd_pcm_uframes_t    frames;

  while(1)
  {
    int c;

    if ((c = getopt_long(argc, argv, "hvr:f:", long_option, NULL)) < 0)
      break;

    switch (c)
    {
      case 'h':
        help();
        return errval;

      case 'v':
        verbose = 1;
        break;

      case 'f':
        format = atoi(optarg);
        break;
      
      case 'r':
        sampRateHz = atoi(optarg);
        break;

      case ':':
      case '?':
      default:
        help();
        errval++;
        return errval;
    }
  }

  if( format == SND_PCM_FORMAT_UNKNOWN )
  {
    printf("FATAL ERR: %s:%d "
           "Data format was not set\n",
           __FILE__,__LINE__);
    help();
    goto exiterror;
  }
  else
  {
    switch(format)
    {
      case SND_PCM_FORMAT_S8:
      case SND_PCM_FORMAT_U8:
        sampSize = 1;
        break;

      case SND_PCM_FORMAT_S16_LE:
      case SND_PCM_FORMAT_S16_BE:
      case SND_PCM_FORMAT_U16_LE:
      case SND_PCM_FORMAT_U16_BE:
        sampSize = 2;
        break;

      case SND_PCM_FORMAT_S24_LE:
      case SND_PCM_FORMAT_S24_BE:
      case SND_PCM_FORMAT_U24_LE:
      case SND_PCM_FORMAT_U24_BE:
        sampSize = 3;
        break;

      case SND_PCM_FORMAT_S32_LE:
      case SND_PCM_FORMAT_S32_BE:
      case SND_PCM_FORMAT_U32_LE:
      case SND_PCM_FORMAT_U32_BE:
        sampSize = 4;
        break;

      case SND_PCM_FORMAT_FLOAT_LE:
      case SND_PCM_FORMAT_FLOAT_BE:
        sampSize = sizeof(float);
        break;

      case SND_PCM_FORMAT_FLOAT64_LE:
      case SND_PCM_FORMAT_FLOAT64_BE:
        sampSize = 8;
        break;

      case SND_PCM_FORMAT_IEC958_SUBFRAME_LE:
      case SND_PCM_FORMAT_IEC958_SUBFRAME_BE:
        sampSize = 4;
        break;

      default:
        printf("Unsupported format: %i\n", format);
        errval = 1;
        goto exiterror;
    }
  }

  errval = snd_pcm_open(&pCaptureHandle,PCM_DEVICE,SND_PCM_STREAM_CAPTURE,0);
  if( errval )
  {
    printf("FATAL ERR: %s:%d "
           "Failed to open PCM device: %s\n",
           __FILE__,__LINE__,snd_strerror(errval));
    goto exiterror;
  }

  /* Allocating a hardware parameters object */
  errval = snd_pcm_hw_params_malloc(&pHwParams);
  if( errval )
  {
    printf("FATAL ERR: %s:%d "
           "Failed to allocate hardware parameters object: %s\n",
           __FILE__,__LINE__,snd_strerror(errval));
    goto exiterror;
  }

  /* Filling the hardware parameters object with default values */
  errval = snd_pcm_hw_params_any(pCaptureHandle, pHwParams);
  if( errval )
  {
    printf("ADVISORY: %s:%d "
           "Failed to fill hardware parameters object with default values: %s\n",
           __FILE__,__LINE__,snd_strerror(errval));
  }

  errval = snd_pcm_hw_params_set_access(pCaptureHandle,pHwParams,
                                        SND_PCM_ACCESS_RW_INTERLEAVED);
  if( errval )
  {
    printf("ADVISORY: %s:%d "
           "Failed to set interleaved mode: %s\n",
           __FILE__,__LINE__,snd_strerror(errval));
  }

  errval = snd_pcm_hw_params_set_format(pCaptureHandle,pHwParams,
                                        format);
  if( errval )
  {
    printf("ADVISORY: %s:%d "
           "Failed to set format: %s\n",
           __FILE__,__LINE__,snd_strerror(errval));
  }

  errval = snd_pcm_hw_params_set_channels(pCaptureHandle,pHwParams,
                                          channels);
  if( errval )
  {
    printf("ADVISORY: %s:%d "
           "Failed to set channels: %s\n",
           __FILE__,__LINE__,snd_strerror(errval));
  }

  errval = snd_pcm_hw_params_set_rate(pCaptureHandle,pHwParams,
                                      sampRateHz,0);
  if( errval )
  {
    printf("ADVISORY: %s:%d "
           "Failed to set channels: %s\n",
           __FILE__,__LINE__,snd_strerror(errval));
  }

  errval = snd_pcm_hw_params_get_channels(pHwParams, &numChannels);
  if( errval )
  {
    printf("ADVISORY: %s:%d "
           "Failed to get the number of channels: %s\n",
           __FILE__,__LINE__,snd_strerror(errval));
  }

  errval = snd_pcm_hw_params_get_rate(pHwParams,&currSampRate,0);
  if( errval )
  {
    printf("ADVISORY: %s:%d "
           "Failed get the sample rate: %s\n",
           __FILE__,__LINE__,snd_strerror(errval));
  }

  if( verbose )
  {
    printf("PCM name: '%s'\n", snd_pcm_name(pCaptureHandle));
    printf("PCM state: '%s'\n", snd_pcm_state_name(snd_pcm_state(pCaptureHandle)));
    switch(numChannels)
    {
      case 1:
        printf("Channel: mono\n");
        break;
      case 2:
        printf("Channel: stereo\n");
        break;
      default:
        printf("Channel: unknown\n");
        break;
    }
    printf("rate: %d bps\n",currSampRate);
  }

  errval = snd_pcm_hw_params_get_rate(pHwParams,&currSampRate,0);
  if( errval )
  {
    printf("ADVISORY: %s:%d "
           "Failed get the sample rate: %s\n",
           __FILE__,__LINE__,snd_strerror(errval));
  }

  errval = snd_pcm_hw_params_get_period_size(pHwParams, &frames, 0);

  buffSize = frames * channels * sampSize;
  pBuff = (char*)malloc(buffSize);

  printf("frames: %d\n", frames);

  for( uint64_t idx = 0; idx < 5000000; idx++ )
  {
    errval = snd_pcm_readi(pCaptureHandle, pBuff, frames);
    if( errval < 0 )
    {
      if( errval == EBADFD )
      {
        printf("PCM is not in the right state\n");
      }
      else if( errval == EPIPE )
      {
        printf("an overrun occurred\n");
      }
      else if( errval == ESTRPIPE )
      {
        printf("a suspend event occurred\n");
      }
      else
      {
        printf("an unexpected error code was recieved\n");
      }
    }
    else
    {
      printf("Read %ld bytes\n", errval);
    }
  }

  errval = snd_pcm_close(pCaptureHandle);
  if( errval )
  {
    printf("FATAL ERR: %s:%d "
           "Failed to close PCM device: %s\n",
           __FILE__,__LINE__,snd_strerror(errval));
    goto exiterror;
  }

  exiterror:
    if( pHwParams )
      snd_pcm_hw_params_free( pHwParams );

    if( pBuff )
      free(pBuff);

    return errval;
}

#ifdef __cplusplus
}
#endif
