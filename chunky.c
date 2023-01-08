/*
 *  chunky.c
 *	AIFF FILE UTILITY
 *	 to write the pitch found in the file name into the appropriate AIFF chunk
 *  Created by serge lemouton on Sun Jun 08 2004.
 *  Copyright (c) 2004 ircam. All rights reserved.
 *
 */


/*version 0.23 (2007):
* change WORDS_BIGENDIAN to 0 in ../config.h because Intel is not BigEndian 
* add markers support
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "notesymbols.h"
#include "velocitysymbols.h"
#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include "audiofile.h"
#endif

#define TEST 0
#define MAXMARKERS 300

typedef struct markerStruct {
    int marknumber;
    char name[64];
    AFframecount pos;
}  markers_t;


/*Prototypes*/

int findnoteinname ( char *infilename, struct notedemusique *pt,int pts);
int parsename (struct notedemusique pt,char *filename);
int *findvelinname ( char *infilename, struct velItem *vt,int pts);
struct velItem parsevelname (struct velItem pt,char *filename);
void readmarkers ( char *infilename);
void readpitchinfo ( char *infilename);
void writefile (int midipitch,int v1,int v2,char *infilename, char *outfilename, char *labelFile);
void adddcoffset (float offset, char *infilename, char *outfilename);
void usage (void);
void chunky_readReaperLabelFile(char *labelFileName,markers_t *markerData);
void chunky_readLabelFile(char *labelFileName,markers_t *markerData);
void chunky_readCollFile(char *labelFileName,markers_t *markerData);
static int markers = 0;
static int reapermarkers = 0;


/*MAIN*/
int main (int ac, char **av)
{
	int pitch, pitchTableSize, i;
	struct notedemusique *pitchTable;
	int *vel;
	/* arguments with default values */   
	char	*infile   = NULL,
		*outfile = NULL,
		*types	  = NULL,
		*labelFile = NULL;
	char *octave  = "3";
	
	int dynamics = 1,
		verbose = 0,
        loop_mode = 0,
		oct = 3;
	if(ac == 0)usage();  
	/* Argument parsing */
	for (i = 1; i < ac; i++)
    {
		if (av [i][0] == '-')
		{
			char *arg = av [i] + 1;
			
			while (*arg)
				switch (*arg++)
				{
					case 'v': verbose = 1;  break;
                    case 'l': loop_mode = 1;  break;
					case 'h': usage(); break;
					case 'm': markers = 1; labelFile = av[++i];break;
                    case 'r': reapermarkers = 1; labelFile = av[++i];break;
					case 'd': dynamics = 1;  break;
					case 'o': if (i == ac - 1) fprintf(stderr, "octave ??");
						octave = av [++i]; break;
					case 'n': /* no arg after last option, complain */
						if (i == ac - 1)   
							usage ();
						types = av [++i];	       break;
					default :  usage();		       break;
				}
		}
			else if (!infile)
				infile = av [i];
			else if (!outfile)
				outfile = av [i];
			else
				usage();
    }
		switch(*octave)
		{
			case '3' : oct = 3;break;
			case '4' : oct = 4;break;
			default : fprintf (stderr,"Unknown octave");
		}
		
		/*print options*/
		if(verbose)
		{
			printf ("octave = %s %d\n",octave,oct);
			printf ("types = %s\n",types);
			printf ("infile = %s\n",infile);
			printf ("outfile = %s\n",outfile);
		}
		
		/*Lecture des marqueurs*/
		//  readmarkers(av[1]);
		/*lecture de l'info de pitch*/
		//readpitchinfo(av[1]);
		
		/*construction de la table des notes*/
		pitchTable = (struct notedemusique *)malloc(NTYPES * NOCTAVES * 12 * sizeof(struct notedemusique));
		pitchTableSize = initNoteSymbols(&pitchTable,types,oct);
		//   if(verbose)
		//   {
		//for(i=0;i<pitchTableSize;i++) 
		//   printf(">%s %i\n",pitchTable[i].name,pitchTable[i].midipitch);
		//    printf("table construite ...size = %d\n",pitchTableSize); 
		// }
		/*recherche dans le nom */
		pitch = findnoteinname(infile,pitchTable,pitchTableSize);
		
		/*recherche de la dynamique*/
		if (dynamics)
		{
			vel = findvelinname(infile,velocitytable,NVELITEMS);
			printf ("chunky vel = %d %d\n",vel[0],vel[1]);
		}
		
		/*ecriture du fichier de sortie */
		if(pitch != 0)
		{   
			printf ("chunky pitch= %d\n",pitch);
		}
		else printf("Pas de pitch dans le nom du fichier\n");
        writefile(pitch,vel[0],vel[1],infile,outfile,labelFile);
		return 0;
}

int findnoteinname ( char *infilename, struct notedemusique *pt,int pts)
{
	int i = 0,result;
	while(((result = parsename(pt[i],infilename))==0)& (i<pts))
		i++;
	return (result);
}

int *findvelinname ( char *infilename, struct velItem *vt,int pts)
{
	int i = 0;
	char *res = NULL;
	int result[]={0,127};
	
	
	while (((res = strstr(infilename,vt[i].name)) == NULL) & (i < pts))
    {
		i++;
    }
	if (i != pts)
		if (res != NULL)
		{result[0]=vt[i].lowvel;
			result[1]=vt[i].highvel;
		}
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

void writefile (int midipitch,int lowvelocity,int highvelocity,char *infilename, char *outfilename,char *labelFile)
{
	AFfilehandle	infile = afOpenFile(infilename, "r", NULL);
	int	       		channelCount, frameCount;
	int i = 0;
	short			*buffer;
	int sampleformat,sampleWidth;
	double rate;
//	int nmarks=8;
	int markids[MAXMARKERS];
	AFframecount pos,pos_t[100];
	markers_t markerData[MAXMARKERS];
    markers_t *markerP;
    markerP = markerData;
	
	AFfilesetup  	outfilesetup = afNewFileSetup();
	AFfilehandle	outfile;
	frameCount = afGetFrameCount(infile, AF_DEFAULT_TRACK);
	channelCount = afGetChannels(infile, AF_DEFAULT_TRACK);
	afGetVirtualSampleFormat(infile,AF_DEFAULT_TRACK,&sampleformat,&sampleWidth);
	rate = afGetRate(infile,AF_DEFAULT_TRACK);
	/* printf("afGetFrameCount: %d\n", frameCount); */
	/* printf("afGetChannels: %d\n", channelCount); */
	 printf("sampleWidth and Rate: %d %f\n", sampleWidth,rate);
    /* printf("sustain loop %f %f\n");*/
    printf(" file '%s'.\n", infilename);
	
	afInitFileFormat(outfilesetup, AF_FILE_AIFF);
	afInitByteOrder(outfilesetup, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);
	afInitChannels(outfilesetup, AF_DEFAULT_TRACK, channelCount);
	afInitRate(outfilesetup, AF_DEFAULT_TRACK, rate);
	afInitSampleFormat(outfilesetup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, sampleWidth);
	if (markers)
	{
//		chunky_readLabelFile(labelFile,markerData);
        chunky_readCollFile(labelFile,markerData);
    }
    if (reapermarkers)
    {
        
        chunky_readReaperLabelFile(labelFile,markerData);
        
    }
    if (markers || reapermarkers)
    {
        
        for(i=0;i<markers;i++)
            markids[i]=markerData[i].marknumber;
        afInitMarkIDs ( outfilesetup, AF_DEFAULT_TRACK, markids, markers );
        for(i=0;i<markers;i++)
        {
            printf ("adding marker %d %f\n",markerData[i].marknumber,rate * markerData[i].pos / 1000. );
            afInitMarkName ( outfilesetup, AF_DEFAULT_TRACK, markerData[i].marknumber,markerData[i].name );
        }
    }
	
	outfile = afOpenFile(outfilename, "w", outfilesetup);
	
	if (infile == NULL)
	{
		printf("unable to open input file '%s'.\n", infilename);
		return;
	}
    if (outfile == NULL)
    {
        printf("unable to open output file '%s'.\n", outfilename);
        return;
    }
	if (sampleWidth <= 16)
		buffer = (short *) malloc(frameCount * channelCount * sizeof (short));
	else
    {
		buffer = (short *) malloc(frameCount * channelCount * 4);
        printf("un petit doute sur les fichiers son pas 16 bits");
    }
	if (buffer == NULL)fprintf(stderr,"probleme d_allocation");
	afReadFrames(infile, AF_DEFAULT_TRACK, (void *) buffer, frameCount);
	afWriteFrames(outfile, AF_DEFAULT_TRACK, (void *) buffer, frameCount);
	afSetInstParamLong (outfile,AF_DEFAULT_INST,AF_INST_MIDI_BASENOTE, midipitch);
	afSetInstParamLong (outfile,AF_DEFAULT_INST,AF_INST_MIDI_LOVELOCITY, lowvelocity);
	afSetInstParamLong (outfile,AF_DEFAULT_INST,AF_INST_MIDI_HIVELOCITY, highvelocity);
	
	if (markers)
	{
		//	afSetMarkPosition ( outfile, AF_DEFAULT_TRACK, 1 ,1000 );
		for(i=0;i<markers;i++)
        {
            printf ("adding marker %d %f\n",markerData[i].marknumber,rate * markerData[i].pos / 1000. );
			afSetMarkPosition ( outfile, AF_DEFAULT_TRACK, markerData[i].marknumber,rate * markerData[i].pos / 1000. );
        }

	}

	afCloseFile(outfile);
}

void chunky_readLabelFile(char *labelFileName,markers_t *markerData)
{
    float marknumber = 0.;
    int i = 0;
	char inputstring[256],name[10];
	//AFframecount pos;
	float pos;
	FILE * labelFile;
	markers_t *markerP;
	int tmpNC = 0;
	
	markerP = markerData;
	markers = 0;
	if ((labelFile=fopen(labelFileName,"r")))
	{while (fgets(inputstring,256,labelFile) != NULL)
	{
		if((tmpNC = sscanf(inputstring,"%f %f %s",&marknumber,&pos,markerP->name))>0)
		{
		i++;
		markers++;
		markerP->marknumber = marknumber;
		markerP->pos=(AFframecount)pos;
		//markerData->name = &name;
		printf("marker : %d %d %s - %s\n",i,markerP->marknumber,markerP->name,inputstring);
		markerP++;
		}
	}}
	else printf("no file %s",labelFileName);
}

void chunky_readCollFile(char *labelFileName,markers_t *markerData)
{
    float marknumber = 0.;
    int i = 0;
    char inputstring[256],name[10];
        char posstr[64];
    //AFframecount pos;
    float pos;
    FILE * labelFile;
    markers_t *markerP;
    int tmpNC = 0;
    
    markerP = markerData;
    markers = 0;
    if ((labelFile=fopen(labelFileName,"r")))
    {
        for (int i = 1;
             i != MAXMARKERS // Make sure we don't overflow the array
             && fscanf(labelFile, "%d, %s %f;", &marknumber,markerP->name,&pos ) != EOF;
             i++,markers++
             )
        {  markerP->marknumber = i;
            markerP->pos=(AFframecount)pos;
            printf("marker : _%d_ _%d_ _%s_ /%f/ \n",i,markerP->marknumber,markerP->name,pos);
            markerP++;
        }
        fclose (labelFile);
    }
    else printf("no file %s",labelFileName);
}

void chunky_readReaperLabelFile(char *labelFileName,markers_t *markerData)
{
    int jjj = 0;
    AFframecount pos;
    char marklabel[64];
    char inputstring[256];
    char posstr[64];
    FILE * labelFile;
    markers_t *markerP;
    int tmpNC = 0;
    
    markerP = markerData;
    markers = 0;
    jjj = 0;
    if ((labelFile=fopen(labelFileName,"r")))
    {while ((fgets(inputstring,1024,labelFile) != NULL))
    {
        if((tmpNC = sscanf(inputstring,"%[^,], %[^,], %[^,^\n]",&marklabel,markerP->name,&posstr))>0)
        {
                    printf("string -> %s\n",inputstring);
            jjj++;
            markers++;
            markerP->marknumber = markers;
            pos = 1*atof(posstr);
            markerP->pos = pos;
            //markerData->name = &name;
            printf("marker -> %d \n%s %s %s \n",markers,marklabel,markerP->name,posstr);
            markerP++;
        }
    }}
    else printf("no file %s",labelFileName);
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

void usage (void)
{
	printf ("Utility to write the pitch of a sound file in the appropriate chunk\n");
	printf("serge lemouton ircam novembre 2007,2013 \t version 2.1\n");
	fprintf(stderr, "usage: chunky [options] infile outfile\n"
			"Options:\n"
			"        -v : verbose\n"
            "        -l : loop\n"
			"        -a : all possible notations (default)\n"
			"        -n : note name for 440Hz : \n"
			"\t      possible types la, A- a A Ab etc.\n"
			"        -o : octave pour 440 : \n"
			"\t      en Europe -o 3(default)\n"
			"        -d : dynamics (-ff, -mf, -pp) \n"
			"        -m label_file : write markers from file (audacity) \n"
            "        -r label_file : write markers from file (reaper) \n"
			);
	exit(EXIT_FAILURE);
}



