
#include "assembler.h"
static Bool shouldNotCreateFile = false;
void updateShouldNotCreateFile(Bool newValue, Bool isMainFunction)
{
    static Bool alreadyModified = false; // Tracks if modified outside main
    // Reset the tracking flag when called from main
    if (isMainFunction)
    {
        alreadyModified = false;
        shouldNotCreateFile = newValue;
        return;
    }
    // Update only if not already modified outside main
   // printf("if (!alreadyModified) out :\n  \n");

    if (!alreadyModified)
    {
     //   printf("if (!alreadyModified) in:\n  \n");
        shouldNotCreateFile = newValue;
        alreadyModified = true;
    }
}
// Getter function to retrieve the value
Bool getShouldNotCreateFile()
{
    return shouldNotCreateFile;
}
