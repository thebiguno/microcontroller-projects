#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <vector>

#include <CharDisplay.h>

#include "../State.h"

using namespace digitalcave;

/*
 * Copy all files in the named directory to the root of the flash device.  Used to copy 
 * samples to flash prior to playback.  While copying, it will show the status of the
 * current operation on the char display.
 * 
 * Note: this function erases the flash before copying.
 */
void copyFolderToFlash(const char* folderName, CharDisplay display);

std::vector<String> getTopLevelFolders();

#endif