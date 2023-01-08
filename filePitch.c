/*
 *  filePitch.c
 *	 returns the pitch found in the file name into the appropriate AIFF chunk
 *  Created by serge lemouton  2004.
 *  Copyright (c) 2004 ircam. All rights reserved.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "notesymbols.h"
#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include "audiofile.h"
#endif

#define DIAPASON 440.

int findnoteinname ( char *infilename, struct notedemusique *pt,int pts);
int parsename (struct notedemusique pt,char *filename);
void readmarkers ( char *infilename);
void readpitchinfo ( char *infilename);
void writefile (int midipitch,char *infilename, char *outfilename);
void adddcoffset (float offset, char *infilename, char *outfilename);
void usageerror (void);

int testFilePitch (int ac, char **av)
{
  int pitch, pitchTableSize;
  double freq;
  struct notedemusique *pitchTable;

  if (ac != 2)
    usageerror();

  /*lecture des marqueurs*/
  //readmarkers(av[1]);
  /*lecture de l'info de pitch*/
  //readpitchinfo(av[1]);

  /*construction de la table des notes*/
  pitchTable = (struct notedemusique *)malloc(NOCTAVES * 12 * sizeof(struct notedemusique));
  pitchTableSize = initNoteSymbols(&pitchTable,av[1],(int)av[2]);
  /*   for(i=0;i<pitchTableSize;i++) */
  /*     printf(">%s %i\n",pitchTable[i].name,pitchTable[i].midipitch); */
  /*   printf("table construite ...\n");	 */
  /*  recherche dans le nom */
 
 pitch = findnoteinname(av[1],pitchTable,pitchTableSize);
 
  /* ecriture du pitch */
 if(pitch != 0)
   {   
     freq = DIAPASON * exp(log(pow(2.,1./12.)) * (double)(pitch - 69.));
     //  printf ("pitch = %doublefreq = %f\n",pitch, freq);
     fprintf(stdout,"%f",freq);
   }
  else printf("Pas de pitch dans le nom du fichier\n");
 
 return (0);
}

int findnoteinname ( char *infilename, struct notedemusique *pt,int pts)
{
  int i = 0,result;
  while(((result = parsename(pt[i],infilename))==0)& (i<pts))
    i++;
  return (result);
}

int parsename (struct notedemusique pt,char *filename)
{
  char *res;
  char *pitchstring = pt.name;
  
  res = strstr(filename,pitchstring);
  /*    printf("searching %s %s \t",filename,pitchstring); */
  /*    printf( "%s %d\n",res,res==NULL?0:pt.midipitch); */
  return(res==NULL?0:pt.midipitch);
}

void readmarkers ( char *infilename)
{
  AFfilehandle	infile = afOpenFile(infilename, "r", NULL);
  int				markCount, frameCount;
  short			*buffer;
  int *markids;
  int				i;


  frameCount = afGetFrameCount(infile, AF_DEFAULT_TRACK);
  markCount = afGetMarkIDs(infile, AF_DEFAULT_TRACK,markids);


  if (infile == NULL)
    {
      printf("unable to open input file '%s'.\n", infilename);
      return;
    }

  printf("afGetFrameCount: %d\n", frameCount);
  printf("afGetMarkers %d\n", markCount);
  markids = (int *) malloc(markCount * sizeof (int));
  afGetMarkIDs(infile, AF_DEFAULT_TRACK,markids);

  for (i=0; i<markCount; i++)
    {
      printf("mark %d=%s ",markids[i],afGetMarkName(infile, AF_DEFAULT_TRACK,markids[i]));
      printf("time = %d\n",afGetMarkPosition(infile, AF_DEFAULT_TRACK,markids[i]));
    }

}

void readpitchinfo ( char *infilename)
{
  AFfilehandle	infile = afOpenFile(infilename, "r", NULL);
  int				instCount, frameCount;
  int *instids;
  short			*buffer;
  int				i;


  frameCount = afGetFrameCount(infile, AF_DEFAULT_TRACK);
  instCount = afGetInstIDs(infile, instids);


  if (infile == NULL)
    {
      printf("unable to open input file '%s'.\n", infilename);
      return;
    }

  printf("afGetFrameCount: %d\n", (int)afGetFrameCount(infile, AF_DEFAULT_TRACK));
  printf("afGetInst: %d\n", instCount);

}



void adddcoffset (float offset, char *infilename, char *outfilename)
{
  AFfilehandle	infile = afOpenFile(infilename, "r", NULL);
  int				channelCount, frameCount;
  short			*buffer;
  int				i;

  AFfilesetup		outfilesetup = afNewFileSetup();
  AFfilehandle	outfile;

  frameCount = afGetFrameCount(infile, AF_DEFAULT_TRACK);
  channelCount = afGetChannels(infile, AF_DEFAULT_TRACK);

  afInitFileFormat(outfilesetup, AF_FILE_AIFF);
  afInitByteOrder(outfilesetup, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);
  afInitChannels(outfilesetup, AF_DEFAULT_TRACK, channelCount);
  afInitRate(outfilesetup, AF_DEFAULT_TRACK, 44100.0);
  afInitSampleFormat(outfilesetup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);

  outfile = afOpenFile(outfilename, "w", outfilesetup);

  if (infile == NULL)
    {
      printf("unable to open input file '%s'.\n", infilename);
      return;
    }

  printf("afGetFrameCount: %d\n", (int)afGetFrameCount(infile, AF_DEFAULT_TRACK));
  printf("afGetChannels: %d\n", afGetChannels(infile, AF_DEFAULT_TRACK));

  buffer = (short *) malloc(frameCount * channelCount * sizeof (short));
  afReadFrames(infile, AF_DEFAULT_TRACK, (void *) buffer, frameCount);

  for (i=0; i<frameCount; i++)
    {
      buffer[i] += offset;
    }

  afWriteFrames(outfile, AF_DEFAULT_TRACK, (void *) buffer, frameCount);
  afCloseFile(outfile);
}

void usageerror (void)
{
  printf ("Utility to return the pitch of a sound file\n");
  printf("serge lemouton ircam octobre 2004\tversion1.1\n");
  fprintf(stderr, "usage: filePitch infile \n");
  exit(EXIT_FAILURE);
}



