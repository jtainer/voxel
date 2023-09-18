// 
// Voxel chunk data structures
//
// 2023, Jonathan Tainer
//

#include <stdint.h>
#include <stdlib.h>
#include "vx_chunk.h"
#include "config.h"

vx_chunk* vx_find_chunk(vx_chunk_table* table, int32_t x, int32_t y) {
	return NULL;
}

uint32_t idx(uint32_t x, uint32_t y, uint32_t z) {
	return x + (CHUNK_DIM_X * z) + (CHUNK_DIM_X * CHUNK_DIM_Z * y);
}
