#include "File.h"

using namespace digitalcave;

File::File(File* parent, uint8_t* name, uint8_t attrib, uint32_t cluster, uint32_t size) :
	bd(parent->bd),
	sectors_per_cluster(parent->sectors_per_cluster),
	fat_begin_lba(parent->fat_begin_lba),
	reserved_sectors(parent->reserved_sectors),
	cluster_begin_lba(parent->cluster_begin_lba),
	name(name),
	attrib(attrib),
 	start_cluster(cluster),
	size(size),
	current_cluster(cluster),
	sector(0),
	position(0)
{}

File::File(BlockDevice* bd) :
	bd(bd)
{
	// read the MBR
	uint8_t a[37];
	this->bd->setBlock(0);
	this->bd->skip(255); this->bd->skip(191); // skip the 446 bytes of boot code
	this->bd->read(a, 16);
	if (a[4] == 0x0b || a[4] == 0x0c) {
		// FAT32; read the Volume ID
		uint32_t lba_begin = (((uint32_t)a[8]) << 24) | (((uint32_t)a[9]) << 16) | (((uint16_t)a[10]) << 8) | a[11];

		this->bd->setBlock(lba_begin);
		this->bd->skip(11);
		this->bd->read(a, 37);
		if (a[0] == 0x02 && a[1] == 0x00) {
			// verified 512 bytes per sector
		}
		this->sectors_per_cluster = a[2];
		uint16_t reserved_sectors = (a[3] << 8) | a[4];
		if (a[5] == 0x02) {
			// verified 2 FATs
		}
		uint32_t sectors_per_fat = (((uint32_t)a[25]) << 24) | (((uint32_t)a[26]) << 16) | (((uint16_t)a[27]) << 8) | a[28];
		this->start_cluster = (((uint32_t)a[33]) << 24) | (((uint32_t)a[34]) << 16) | (((uint16_t)a[35]) << 8) | a[36];
		this->fat_begin_lba = lba_begin + reserved_sectors;
		this->cluster_begin_lba = this->fat_begin_lba + (sectors_per_fat << 1);
	} else {
		// TODO how to deal with unknown filesystem
	}
}

File::~File() {
}

/*
File* File::ls( File* (*f)(File*) ) {
	Stream s = this->open();
	uint8_t[512] b;
	while (true) {
			uint16_t read = s.read(b,512)
			while (read > 0) {
				for (uint16_t i = 0; i < c; i += 32) {
					uint8_t attrib = b[11];
					uint32_t cluster = (b[i+20] << 24) | (b[i+21] << 16) | (b[i+26] << 8) | b[i+27];
					uint32_t size = (b[i+28] << 24) | (b[i+29] << 16) | (b[i+30] << 8) | b[i+31];

					if (attrib & 0xf == 0xf) {
						// long file name; ignore
					} else if (filename[0] = 0xe5) {
						// unused / deleted file; ignore
					} else if (filename[0] == 0x0) {
						// end of directory marker; stop
						return 0;
					} else {
						// normal file / directory
						File file = File(&this->volume, &b[i], b[11], cluster, size);
						File* ret = f(&file);
						if (ret) {
							return ret;
						}
					}
				}
		}
	}
	return 0;
}
*/

void File::filename(uint8_t* name) {
	for (uint8_t i = 0; i < 11; i++) {
		name[i] = this->name[i];
	}
}
uint8_t File::reset() {
	this->current_cluster = this->start_cluster;
	this->sector = 0;
	this->position = 0;
	return 1;
}

uint16_t File::skip(uint16_t len) {
	if (len > this->size || this->size - len < this->position) {
		len = this->size - this->position;
	}
	this->position += len;
	return len;
}

uint8_t File::read(uint8_t* b) {
	return this->read(b,1);
}

uint16_t File::read(uint8_t* a, uint16_t len){
	if (this->current_cluster == 0xffffffff) {
		return 0;
	}
	if (len > this->size || this->size - len < this->position) {
		len = this->size - this->position;
	}

	while (this->position < this->size) {
		this->bd->setBlock(this->lba_addr(this->current_cluster) + this->sector);
		this->bd->skip(this->position % 512);
		uint16_t read = this->bd->read(a,len);
		this->position += read;
		if (read < len) {
			// reached the end of the sector
			if (this->sector == this->sectors_per_cluster) {
				// follow the cluster chain
				this->current_cluster = this->next_cluster(this->current_cluster);
				this->sector = 0;

				if (this->current_cluster == 0xffffffff) {
					// end of the cluster chain
					return read;
				}
			} else {
				// advance to the next sector in the cluster
				this->sector++;
			}
		}
		return read;
	}
	return 0;
}

// TODO for everything is read-only
uint8_t File::write(uint8_t b) {
	return 0;
}

uint32_t File::lba_addr(uint32_t cluster) {
	return this->cluster_begin_lba + (cluster - 2) * this->sectors_per_cluster;
}

uint32_t File::next_cluster(uint32_t cluster) {
	uint32_t fat_sector = cluster >> 7;  // 128 values per sectors_per_cluster
	this->bd->setBlock(this->fat_begin_lba + fat_sector);
	//uint16_t cluster - fat_sector;
	this->bd->skip((cluster - (fat_sector << 7)) * 4);
	uint8_t b[4];
	this->bd->read(b, 4);
	uint32_t result = (((uint32_t)b[3]) << 24) | (((uint32_t)b[2]) << 16) | (((uint16_t)b[1]) << 8) | b[0];
	return (result >= 0xfffffff8) ? 0xffffffff : (result & 0xfffffff);
}
