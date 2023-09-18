// 
// Voxel mesh generator
//
// 2023, Jonathan Tainer
//

#ifndef VX_MESH_H
#define VX_MESH_H

#include <raylib.h>
#include "vx_chunk.h"

Mesh vx_load_chunk_mesh(vx_chunk* chunk);

#endif
