/*to test notesymbols.h*/

#include "notesymbols.h"

testNoteSymbols()
{
  int pitchTableSize, i;
  struct notedemusique *pitchTable;

  pitchTable = (struct notedemusique *)malloc(NOCTAVES * 12 * sizeof(struct notedemusique));
  pitchTableSize = initNoteSymbols(&pitchTable,"la",3);
  for(i=0;i<pitchTableSize;i++)
    printf(">%s %i\n",pitchTable[i].name,pitchTable[i].midipitch);
}
