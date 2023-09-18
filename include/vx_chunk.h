// 
// Voxel chunk data structures
//
// 2023, Jonathan Tainer
//

#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>

// Each chunk is a 16x16 area of blocks
typedef struct vx_chunk {
	
	// Position relative to origin
	int32_t x;
	int32_t z;

	// Array of block IDs
	uint8_t* block;
} vx_chunk;

// Each sector is a 256*256 area of chunks
typedef struct vx_chunk_table {
	
	// Position relative to origin
	int32_t x;
	int32_t z;

	// Array of pointers to chunks
	vx_chunk* chunk;
} vx_chunk_table;

vx_chunk* vx_find_chunk(vx_chunk_table* table, int32_t x, int32_t y);

uint32_t idx(uint32_t x, uint32_t y, uint32_t z);

#endif
