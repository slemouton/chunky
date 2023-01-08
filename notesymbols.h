#include <stdio.h>

#define MAXCHAR 6
#define NOCTAVES 8
#define NTYPES 6

struct notedemusique {
  char name[MAXCHAR];
  int midipitch;};
 
struct notedemusique chromaticScaleF[] = {
  "do", 24,
  "do#", 25,
  "re", 26,
  "re#", 27,
  "mi", 28,
  "fa", 29,
  "fa#" , 30,
  "sol", 31,
  "sol#",32,
  "la", 33,
  "la#", 34,
  "si", 35};

struct notedemusique chromaticScaleUS[] = {
  "C-", 24,
  "C#", 25,
  "D-", 26,
  "D#", 27,
  "E-", 28,
  "F-", 29,
  "F#" , 30,
  "G-", 31,
  "G#",32,
  "A-", 33,
  "A#", 34,
  "B-", 35};

struct notedemusique chromaticScale3[] = {
  "c", 24,
  "c#", 25,
  "d", 26,
  "d#", 27,
  "e", 28,
  "f", 29,
  "f#" , 30,
  "g", 31,
  "g#",32,
  "a", 33,
  "a#", 34,
  "b", 35};

struct notedemusique chromaticScale4[] = {
  "C", 24,
  "C#", 25,
  "D", 26,
  "Eb", 27,
  "E", 28,
  "F", 29,
  "F#" , 30,
  "G", 31,
  "G#",32,
  "A", 33,
  "A#", 34,
  "B", 35};
struct notedemusique chromaticScale4b[] = {
  "C", 24,
  "Db", 25,
  "D", 26,
  "Eb", 27,
  "E", 28,
  "F", 29,
  "Gb" , 30,
  "G", 31,
  "Ab",32,
  "A", 33,
  "Bb", 34,
  "B", 35};
 
int initNoteSymbols(struct notedemusique **p, char *types, int octave){
  int i,j,k;
  int index = 0;
  char temp[MAXCHAR];
  struct notedemusique *tableauDeNotes ;

  tableauDeNotes = *p; 
  
  for(j= 0;j<NOCTAVES;j++)
    {
      k = j - octave + 3;
         if ((types == NULL) || (!strcmp(types, "la")))
      for(i = 0;i<12;i++)
	{
	  sprintf(temp,"%s%d",chromaticScaleF[i].name,j);
	  strcpy(tableauDeNotes[index].name,temp);
	  tableauDeNotes[index].midipitch = chromaticScaleF[i].midipitch + 12*k;
	  index++;
	} 
       if ((types == NULL) || (!strcmp(types, "A-")))
      for(i = 0;i<12;i++)
	{
	  sprintf(temp,"%s%d",chromaticScaleUS[i].name,j);
	  strcpy(tableauDeNotes[index].name,temp);
	  tableauDeNotes[index].midipitch = chromaticScaleUS[i].midipitch + 12*k;
	  index++;
	}
       if ((types == NULL) || (!strcmp(types, "a")))
       for(i = 0;i<12;i++)
	{
	  sprintf(temp,"%s%d",chromaticScale3[i].name,j);
	  strcpy(tableauDeNotes[index].name,temp);
	  tableauDeNotes[index].midipitch = chromaticScale3[i].midipitch + 12*k;
	  index++;
	}
       if ((types == NULL) || (!strcmp(types, "A")))
      for(i = 0;i<12;i++)
	{
	  sprintf(temp,"%s%d",chromaticScale4[i].name,j);
	  strcpy(tableauDeNotes[index].name,temp);
	  tableauDeNotes[index].midipitch = chromaticScale4[i].midipitch + 12*k;
	  index++;
	}
       if ((types == NULL) || (!strcmp(types, "Ab")))
      for(i = 0;i<12;i++)
	{
	  sprintf(temp,"%s%d",chromaticScale4b[i].name,j);
	  strcpy(tableauDeNotes[index].name,temp);
	  tableauDeNotes[index].midipitch = chromaticScale4b[i].midipitch + 12*k;
	  index++;
	}

    } 
  return(index - 1);
}
