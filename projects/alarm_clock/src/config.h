#ifndef CONFIG_H
#define CONFIG_H

typedef struct config {
	uint8_t source;				//Source to play when manually turning on music.  Must be a named folder on the SD card.  Can pick from folders 01 to 08.
	uint8_t music_count[8];		//Number of files in the specified folder.  Folder is index + 1.  Value 0-99 is valid.  0 Means the folder is not there / is empty, and will make music_play into a nop.  (Would not be needed for a real DFPlayerMini, but the clones can't read the file count.)
	uint8_t lamp_brightness;	//Lamp brightness (saved automatically)
	uint8_t volume;				//Volume (saved automatically)
} config_t;

#endif
