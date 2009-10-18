/* Copyright (c) 2008 CSIRO
   Copyright (c) 2008-2009 Xiph.Org Foundation
   Written by Jean-Marc Valin */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include "modes.h"
#include "celt.h"
#include "rate.h"

#define INT16 "%d"
#define INT32 "%d"
#define FLOAT "%f"

#ifdef FIXED_POINT
#define WORD16 INT16
#define WORD32 INT32
#else
#define WORD16 FLOAT
#define WORD32 FLOAT
#endif


void dump_modes(FILE *file, CELTMode **modes, int nb_modes)
{
   int i, j;
   fprintf(file, "/* The contents of this file is automatically generated and contains static\n");
   fprintf(file, "   definitions for some pre-defined modes */\n");
   fprintf(file, "#include \"modes.h\"\n");
   fprintf(file, "#include \"rate.h\"\n");

   fprintf(file, "\n");
   for (i=0;i<nb_modes;i++)
   {
      CELTMode *mode = modes[i];
      fprintf(file, "#ifndef DEF_EBANDS%d_%d\n", mode->Fs, mode->mdctSize);
      fprintf(file, "#define DEF_EBANDS%d_%d\n", mode->Fs, mode->mdctSize);
      fprintf (file, "static const celt_int16 eBands%d_%d[%d] = {\n", mode->Fs, mode->mdctSize, mode->nbEBands+2);
      for (j=0;j<mode->nbEBands+2;j++)
         fprintf (file, "%d, ", mode->eBands[j]);
      fprintf (file, "};\n");
      fprintf(file, "#endif\n");
      fprintf(file, "\n");
      
      
      fprintf(file, "#ifndef DEF_WINDOW%d\n", mode->overlap);
      fprintf(file, "#define DEF_WINDOW%d\n", mode->overlap);
      fprintf (file, "static const celt_word16 window%d[%d] = {\n", mode->overlap, mode->overlap);
      for (j=0;j<mode->overlap;j++)
         fprintf (file, WORD16 ", ", mode->window[j]);
      fprintf (file, "};\n");
      fprintf(file, "#endif\n");
      fprintf(file, "\n");
      
      fprintf(file, "#ifndef DEF_PSY%d\n", mode->Fs);
      fprintf(file, "#define DEF_PSY%d\n", mode->Fs);
      fprintf (file, "static const celt_word16 psy_decayR_%d[%d] = {\n", mode->Fs, MAX_PERIOD/2);
      for (j=0;j<MAX_PERIOD/2;j++)
         fprintf (file, WORD16 ", ", mode->psy.decayR[j]);
      fprintf (file, "};\n");
      fprintf(file, "#endif\n");
      fprintf(file, "\n");

      
      fprintf(file, "#ifndef DEF_ALLOC_VECTORS%d_%d\n", mode->Fs, mode->mdctSize);
      fprintf(file, "#define DEF_ALLOC_VECTORS%d_%d\n", mode->Fs, mode->mdctSize);
      fprintf (file, "static const celt_int16 allocVectors%d_%d[%d] = {\n", mode->Fs, mode->mdctSize, mode->nbEBands*mode->nbAllocVectors);
      for (j=0;j<mode->nbAllocVectors;j++)
      {
         int k;
         for (k=0;k<mode->nbEBands;k++)
            fprintf (file, "%2d, ", mode->allocVectors[j*mode->nbEBands+k]);
         fprintf (file, "\n");
      }
      fprintf (file, "};\n");
      fprintf(file, "#endif\n");
      fprintf(file, "\n");

      fprintf(file, "#ifndef DEF_ALLOC_CACHE%d_%d\n", mode->Fs, mode->mdctSize);
      fprintf(file, "#define DEF_ALLOC_CACHE%d_%d\n", mode->Fs, mode->mdctSize);
      for (j=0;j<mode->nbEBands;j++)
      {
         int k;
         if (j==0 || (mode->bits[j] != mode->bits[j-1]))
         {
            fprintf (file, "static const celt_int16 allocCache_band%d_%d_%d[MAX_PULSES] = {\n", j, mode->Fs, mode->mdctSize);
            for (k=0;k<MAX_PULSES;k++)
               fprintf (file, "%2d, ", mode->bits[j][k]);
            fprintf (file, "};\n");
         } else {
            fprintf (file, "#define allocCache_band%d_%d_%d allocCache_band%d_%d_%d\n", j, mode->Fs, mode->mdctSize, j-1, mode->Fs, mode->mdctSize);
         }
      }
      fprintf (file, "static const celt_int16 *allocCache%d_%d[%d] = {\n", mode->Fs, mode->mdctSize, mode->nbEBands);
      for (j=0;j<mode->nbEBands;j++)
      {
         fprintf (file, "allocCache_band%d_%d_%d, ", j, mode->Fs, mode->mdctSize);
      }
      fprintf (file, "};\n");
      fprintf(file, "#endif\n");
      fprintf(file, "\n");


      fprintf(file, "static const CELTMode mode%d_%d_%d = {\n", mode->Fs, mode->mdctSize, mode->overlap);
      fprintf(file, "0x%x,\t/* marker */\n", 0xa110ca7e);
      fprintf(file, INT32 ",\t/* Fs */\n", mode->Fs);
      fprintf(file, "%d,\t/* overlap */\n", mode->overlap);
      fprintf(file, "%d,\t/* mdctSize */\n", mode->mdctSize);
      fprintf(file, "%d,\t/* nbEBands */\n", mode->nbEBands);
      fprintf(file, "%d,\t/* pitchEnd */\n", mode->pitchEnd);
      fprintf(file, "eBands%d_%d,\t/* eBands */\n", mode->Fs, mode->mdctSize);
      fprintf(file, WORD16 ",\t/* ePredCoef */\n", mode->ePredCoef);
      fprintf(file, "%d,\t/* nbAllocVectors */\n", mode->nbAllocVectors);
      fprintf(file, "allocVectors%d_%d,\t/* allocVectors */\n", mode->Fs, mode->mdctSize);
      fprintf(file, "allocCache%d_%d,\t/* bits */\n", mode->Fs, mode->mdctSize);
      fprintf(file, "{%d, 0, 0},\t/* mdct */\n", 2*mode->mdctSize);
      fprintf(file, "0,\t/* fft */\n");
      fprintf(file, "window%d,\t/* window */\n", mode->overlap);
      fprintf(file, "%d,\t/* nbShortMdcts */\n", mode->nbShortMdcts);
      fprintf(file, "%d,\t/* shortMdctSize */\n", mode->shortMdctSize);
      fprintf(file, "{%d, 0, 0},\t/* shortMdct */\n", 2*mode->mdctSize);
      fprintf(file, "window%d,\t/* shortWindow */\n", mode->overlap);
      fprintf(file, "{psy_decayR_%d},\t/* psy */\n", mode->Fs);
      fprintf(file, "0,\t/* prob */\n");
      fprintf(file, "0x%x,\t/* marker */\n", 0xa110ca7e);
      fprintf(file, "};\n");
   }
   fprintf(file, "\n");
   fprintf(file, "/* List of all the available modes */\n");
   fprintf(file, "#define TOTAL_MODES %d\n", nb_modes);
   fprintf(file, "static const CELTMode * const static_mode_list[TOTAL_MODES] = {\n");
   for (i=0;i<nb_modes;i++)
   {
      CELTMode *mode = modes[i];
      fprintf(file, "&mode%d_%d_%d,\n", mode->Fs, mode->mdctSize, mode->overlap);
   }
   fprintf(file, "};\n");
}

void dump_header(FILE *file, CELTMode **modes, int nb_modes)
{
   int i;
   int channels = 0;
   int frame_size = 0;
   int overlap = 0;
   fprintf (file, "/* This header file is generated automatically*/\n");
   for (i=0;i<nb_modes;i++)
   {
      CELTMode *mode = modes[i];
      if (frame_size==0)
         frame_size = mode->mdctSize;
      else if (frame_size != mode->mdctSize)
         frame_size = -1;
      if (overlap==0)
         overlap = mode->overlap;
      else if (overlap != mode->overlap)
         overlap = -1;
   }
   if (channels>0)
   {
      fprintf (file, "#define CHANNELS(mode) %d\n", channels);
      if (channels==1)
         fprintf (file, "#define DISABLE_STEREO\n");
   }
   if (frame_size>0)
   {
      fprintf (file, "#define FRAMESIZE(mode) %d\n", frame_size);
   }
   if (overlap>0)
   {
      fprintf (file, "#define OVERLAP(mode) %d\n", overlap);
   }
}

int main(int argc, char **argv)
{
   int i, nb;
   FILE *file;
   CELTMode **m;
   if (argc%2 != 1)
   {
      fprintf (stderr, "must have a multiple of 2 arguments\n");
      return 1;
   }
   nb = (argc-1)/2;
   m = malloc(nb*sizeof(CELTMode*));
   for (i=0;i<nb;i++)
   {
      int Fs, frame;
      Fs      = atoi(argv[2*i+1]);
      frame   = atoi(argv[2*i+2]);
      m[i] = celt_mode_create(Fs, frame, NULL);
   }
   file = fopen("static_modes.c", "w");
   dump_modes(file, m, nb);
   fclose(file);
   file = fopen("static_modes.h", "w");
   dump_header(file, m, nb);
   fclose(file);
   for (i=0;i<nb;i++)
      celt_mode_destroy(m[i]);
   free(m);
   return 0;
}
