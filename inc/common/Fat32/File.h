#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <BlockDevice.h>
#include <Stream.h>

namespace digitalcave {
	class File : Stream {

		private:
			File(File* parent, uint8_t* name, uint8_t attrib, uint32_t cluster, uint32_t size);

			BlockDevice* bd;
			// volume
			uint8_t sectors_per_cluster;
			uint32_t fat_begin_lba;
			uint32_t reserved_sectors;
			uint32_t cluster_begin_lba;

			// file
			uint8_t name[11];
			uint8_t attrib;
			uint32_t start_cluster;
			uint32_t size;

			// stream
			uint32_t current_cluster;
			uint8_t sector;    // current sector in the cluster
			uint32_t position; // current position in the entire file

			/* Uses the Volume ID to determine the block address of a cluster */
			uint32_t lba_addr(uint32_t cluster);
			/* Uses the FAT to determine the next cluster, or 0xffffffff at the end of the chain */
			uint32_t next_cluster(uint32_t cluster);

		public:
			File(BlockDevice* bd);
			~File();

			void filename(uint8_t *b);
			uint8_t isReadOnly();
			uint8_t isSystem();
			uint8_t isVolumeId();
			uint8_t isHidden();
			uint8_t isDirectory();

			File ls( uint8_t (*f)(File*) );

			uint8_t reset();
			uint16_t skip(uint16_t n);
			uint8_t read(uint8_t *b);
			uint16_t read(uint8_t* a, uint16_t len);
			uint8_t write(uint8_t b);

			using Stream::read;
			using Stream::write;
	};
}

#endif
