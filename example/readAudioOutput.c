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

  uint8_t              errval        = 0;
  uint32_t             sampRateHz    = 44100;
  snd_pcm_t           *pCaptureHandle = NULL;
  snd_pcm_hw_params_t *pHwParams      = NULL;
  snd_pcm_format_t     format        = SND_PCM_FORMAT_UNKNOWN;

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

  errval = snd_pcm_open(&pCaptureHandle,PCM_DEVICE,SND_PCM_STREAM_CAPTURE,0);
  if( errval )
  {
    printf("Failed to open PCM device %s\n", PCM_DEVICE);
    goto exiterror;
  }

  /*--------------------------------------------------------------------------*
   * TODO: Fill stuff in here
   *--------------------------------------------------------------------------*/

  errval = snd_pcm_close(pCaptureHandle);
  if( errval )
  {
    printf("Failed to close PCM device\n");
    goto exiterror;
  }

  exiterror:
    return errval;
}

#ifdef __cplusplus
}
#endif
