// 
// Voxel mesh generator
//
// 2023, Jonathan Tainer
//

#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <string.h>
#include "vx_mesh.h"
#include "config.h"
#include "vx_chunk.h"

#define FACE_PLUS_X	0b00000001
#define FACE_MINUS_X	0b00000010
#define FACE_PLUS_Y	0b00000100
#define FACE_MINUS_Y	0b00001000
#define FACE_PLUS_Z	0b00010000
#define FACE_MINUS_Z	0b00100000

Mesh vx_load_chunk_mesh(vx_chunk* chunk) {
	
	uint8_t* exposed = (uint8_t*)calloc(CHUNK_DIM_X * CHUNK_DIM_Y * CHUNK_DIM_Z, 1);
	
	// Determine which faces should be rendered (not bordered by other blocks)
	for (int y = 0; y < CHUNK_DIM_Y; y++) {
		for (int z = 0; z < CHUNK_DIM_Z; z++) {
			for (int x = 0; x < CHUNK_DIM_X; x++) {
				// +Y face
				if (y == CHUNK_DIM_Y - 1)
					exposed[idx(x,y,z)] |= FACE_PLUS_Y;
				else if (!chunk->block[idx(x,y+1,z)])
					exposed[idx(x,y,z)] |= FACE_PLUS_Y;

				// -Y face
				if (y == 0)
					exposed[idx(x,y,z)] |= FACE_MINUS_Y;
				else if (!chunk->block[idx(x,y-1,z)])
					exposed[idx(x,y,z)] |= FACE_MINUS_Y;
				
				// +X face
				if (x == CHUNK_DIM_X - 1)
					exposed[idx(x,y,z)] |= FACE_PLUS_X;
				else if (!chunk->block[idx(x+1,y,z)])
					exposed[idx(x,y,z)] |= FACE_PLUS_X;

				// -X face
				if (x == 0)
					exposed[idx(x,y,z)] |= FACE_MINUS_X;
				else if (!chunk->block[idx(x-1,y,z)])
					exposed[idx(x,y,z)] |= FACE_MINUS_X;

				// +Z face
				if (z == CHUNK_DIM_Z - 1)
					exposed[idx(x,y,z)] |= FACE_PLUS_Z;
				else if (!chunk->block[idx(x,y,z+1)])
					exposed[idx(x,y,z)] |= FACE_PLUS_Z;

				// -Z face
				if (z == 0)
					exposed[idx(x,y,z)] |= FACE_MINUS_Z;
				else if (!chunk->block[idx(x,y,z-1)])
					exposed[idx(x,y,z)] |= FACE_MINUS_Z;

			}
		}
	}
	
	const int maxTriangles = CHUNK_DIM_X * CHUNK_DIM_Y * CHUNK_DIM_Z * 12;
	const int maxVertices = maxTriangles * 3;
	int vCounter = 0;
	int nCounter = 0;
	int tCounter = 0;

	// Temporary vertex buffers
	Vector3* mapVertices = (Vector3*)malloc(sizeof(Vector3) * maxVertices);
	Vector3* mapNormals = (Vector3*)malloc(sizeof(Vector3) * maxVertices);
	Vector2* mapTexcoords = (Vector2*)malloc(sizeof(Vector2) * maxVertices);

	// Generate mesh for every exposed face
	for (int y = 0; y < CHUNK_DIM_Y; y++) {
		for (int z = 0; z < CHUNK_DIM_Z; z++) {
			for (int x = 0; x < CHUNK_DIM_X; x++) {

				// Compute the corners of the texture within the texture atlas
				char texID = chunk->block[idx(x,y,z)];
				if (texID == 0) continue;

				Vector2 texCorners[4] =	{ { 0.f, 0.f },
							{ 1.f / (float) TEX_ATLAS_COLS, 0.00f },
							{ 1.f / (float) TEX_ATLAS_COLS, 1.f / (float) TEX_ATLAS_ROWS },
							{ 0.00f, 1.f / (float) TEX_ATLAS_ROWS } };

				Vector2 texOffset = { (float) (texID % TEX_ATLAS_COLS) / (float) TEX_ATLAS_COLS, (float) (texID / TEX_ATLAS_COLS) / (float) TEX_ATLAS_ROWS };

				Vector2 texRect[4];
				texRect[0] = Vector2Add(texCorners[0], texOffset);
				texRect[1] = Vector2Add(texCorners[1], texOffset);
				texRect[2] = Vector2Add(texCorners[2], texOffset);
				texRect[3] = Vector2Add(texCorners[3], texOffset);
				
				// +Y face
				if (exposed[idx(x,y,z)] & FACE_PLUS_Y) {
					mapVertices[vCounter + 5] = (Vector3) { x, y+1, z };
					mapVertices[vCounter + 4] = (Vector3) { x+1, y+1, z };
					mapVertices[vCounter + 3] = (Vector3) { x+1, y+1, z+1 };
					mapVertices[vCounter + 2] = (Vector3) { x+1, y+1, z+1 };
					mapVertices[vCounter + 1] = (Vector3) { x, y+1, z+1 };
					mapVertices[vCounter + 0] = (Vector3) { x, y+1, z };
					vCounter += 6;

					mapNormals[nCounter + 0] = (Vector3) { 0.f, 1.f, 0.f };
					mapNormals[nCounter + 1] = (Vector3) { 0.f, 1.f, 0.f };
					mapNormals[nCounter + 2] = (Vector3) { 0.f, 1.f, 0.f };
					mapNormals[nCounter + 3] = (Vector3) { 0.f, 1.f, 0.f };
					mapNormals[nCounter + 4] = (Vector3) { 0.f, 1.f, 0.f };
					mapNormals[nCounter + 5] = (Vector3) { 0.f, 1.f, 0.f };
					nCounter += 6;

					mapTexcoords[tCounter + 5] = texRect[0];
					mapTexcoords[tCounter + 4] = texRect[1];
					mapTexcoords[tCounter + 3] = texRect[2];
					mapTexcoords[tCounter + 2] = texRect[2];
					mapTexcoords[tCounter + 1] = texRect[3];
					mapTexcoords[tCounter + 0] = texRect[0];
					tCounter += 6;
				}

				// -Y face
				if (exposed[idx(x,y,z)] & FACE_MINUS_Y) {
					mapVertices[vCounter + 5] = (Vector3) { x, y, z };
					mapVertices[vCounter + 4] = (Vector3) { x, y, z+1 };
					mapVertices[vCounter + 3] = (Vector3) { x+1, y, z+1 };
					mapVertices[vCounter + 2] = (Vector3) { x+1, y, z+1 };
					mapVertices[vCounter + 1] = (Vector3) { x+1, y, z };
					mapVertices[vCounter + 0] = (Vector3) { x, y, z };
					vCounter += 6;

					mapNormals[nCounter + 0] = (Vector3) { 0.f, -1.f, 0.f };
					mapNormals[nCounter + 1] = (Vector3) { 0.f, -1.f, 0.f };
					mapNormals[nCounter + 2] = (Vector3) { 0.f, -1.f, 0.f };
					mapNormals[nCounter + 3] = (Vector3) { 0.f, -1.f, 0.f };
					mapNormals[nCounter + 4] = (Vector3) { 0.f, -1.f, 0.f };
					mapNormals[nCounter + 5] = (Vector3) { 0.f, -1.f, 0.f };
					nCounter += 6;

					mapTexcoords[tCounter + 5] = texRect[3];
					mapTexcoords[tCounter + 4] = texRect[0];
					mapTexcoords[tCounter + 3] = texRect[1];
					mapTexcoords[tCounter + 2] = texRect[1];
					mapTexcoords[tCounter + 1] = texRect[2];
					mapTexcoords[tCounter + 0] = texRect[3];
					tCounter += 6;
				}

				// +X face
				if (exposed[idx(x,y,z)] & FACE_PLUS_X) {
					mapVertices[vCounter + 5] = (Vector3) { x+1, y+1, z+1 };
					mapVertices[vCounter + 4] = (Vector3) { x+1, y+1, z };
					mapVertices[vCounter + 3] = (Vector3) { x+1, y, z };
					mapVertices[vCounter + 2] = (Vector3) { x+1, y, z };
					mapVertices[vCounter + 1] = (Vector3) { x+1, y, z+1 };
					mapVertices[vCounter + 0] = (Vector3) { x+1, y+1, z+1 };
					vCounter += 6;

					mapNormals[nCounter + 0] = (Vector3) { 1.f, 0.f, 0.f };
					mapNormals[nCounter + 1] = (Vector3) { 1.f, 0.f, 0.f };
					mapNormals[nCounter + 2] = (Vector3) { 1.f, 0.f, 0.f };
					mapNormals[nCounter + 3] = (Vector3) { 1.f, 0.f, 0.f };
					mapNormals[nCounter + 4] = (Vector3) { 1.f, 0.f, 0.f };
					mapNormals[nCounter + 5] = (Vector3) { 1.f, 0.f, 0.f };
					nCounter += 6;

					mapTexcoords[tCounter + 5] = texRect[0];
					mapTexcoords[tCounter + 4] = texRect[1];
					mapTexcoords[tCounter + 3] = texRect[2];
					mapTexcoords[tCounter + 2] = texRect[2];
					mapTexcoords[tCounter + 1] = texRect[3];
					mapTexcoords[tCounter + 0] = texRect[0];
					tCounter += 6;
				}
				
				// -X face
				if (exposed[idx(x,y,z)] & FACE_MINUS_X) {
					mapVertices[vCounter + 5] = (Vector3) { x, y+1, z };
					mapVertices[vCounter + 4] = (Vector3) { x, y+1, z+1 };
					mapVertices[vCounter + 3] = (Vector3) { x, y, z+1 };
					mapVertices[vCounter + 2] = (Vector3) { x, y, z+1 };
					mapVertices[vCounter + 1] = (Vector3) { x, y, z };
					mapVertices[vCounter + 0] = (Vector3) { x, y+1, z };
					vCounter += 6;

					mapNormals[nCounter + 0] = (Vector3) { -1.f, 0.f, 0.f };
					mapNormals[nCounter + 1] = (Vector3) { -1.f, 0.f, 0.f };
					mapNormals[nCounter + 2] = (Vector3) { -1.f, 0.f, 0.f };
					mapNormals[nCounter + 3] = (Vector3) { -1.f, 0.f, 0.f };
					mapNormals[nCounter + 4] = (Vector3) { -1.f, 0.f, 0.f };
					mapNormals[nCounter + 5] = (Vector3) { -1.f, 0.f, 0.f };
					nCounter += 6;

					mapTexcoords[tCounter + 5] = texRect[0];
					mapTexcoords[tCounter + 4] = texRect[1];
					mapTexcoords[tCounter + 3] = texRect[2];
					mapTexcoords[tCounter + 2] = texRect[2];
					mapTexcoords[tCounter + 1] = texRect[3];
					mapTexcoords[tCounter + 0] = texRect[0];
					tCounter += 6;
				}
				
				// +Z face
				if (exposed[idx(x,y,z)] & FACE_PLUS_Z) {
					mapVertices[vCounter + 5] = (Vector3) { x, y+1, z+1 };
					mapVertices[vCounter + 4] = (Vector3) { x+1, y+1, z+1 };
					mapVertices[vCounter + 3] = (Vector3) { x+1, y, z+1 };
					mapVertices[vCounter + 2] = (Vector3) { x+1, y, z+1 };
					mapVertices[vCounter + 1] = (Vector3) { x, y, z+1 };
					mapVertices[vCounter + 0] = (Vector3) { x, y+1, z+1 };
					vCounter += 6;

					mapNormals[nCounter + 0] = (Vector3) { 0.f, 0.f, 1.f };
					mapNormals[nCounter + 1] = (Vector3) { 0.f, 0.f, 1.f };
					mapNormals[nCounter + 2] = (Vector3) { 0.f, 0.f, 1.f };
					mapNormals[nCounter + 3] = (Vector3) { 0.f, 0.f, 1.f };
					mapNormals[nCounter + 4] = (Vector3) { 0.f, 0.f, 1.f };
					mapNormals[nCounter + 5] = (Vector3) { 0.f, 0.f, 1.f };
					nCounter += 6;

					mapTexcoords[tCounter + 5] = texRect[0];
					mapTexcoords[tCounter + 4] = texRect[1];
					mapTexcoords[tCounter + 3] = texRect[2];
					mapTexcoords[tCounter + 2] = texRect[2];
					mapTexcoords[tCounter + 1] = texRect[3];
					mapTexcoords[tCounter + 0] = texRect[0];
					tCounter += 6;
				}
				
				// -Z face
				if (exposed[idx(x,y,z)] & FACE_MINUS_Z) {
					mapVertices[vCounter + 5] = (Vector3) { x+1, y+1, z };
					mapVertices[vCounter + 4] = (Vector3) { x, y+1, z };
					mapVertices[vCounter + 3] = (Vector3) { x, y, z };
					mapVertices[vCounter + 2] = (Vector3) { x, y, z };
					mapVertices[vCounter + 1] = (Vector3) { x+1, y, z };
					mapVertices[vCounter + 0] = (Vector3) { x+1, y+1, z };
					vCounter += 6;

					mapNormals[nCounter + 0] = (Vector3) { 0.f, 0.f, -1.f };
					mapNormals[nCounter + 1] = (Vector3) { 0.f, 0.f, -1.f };
					mapNormals[nCounter + 2] = (Vector3) { 0.f, 0.f, -1.f };
					mapNormals[nCounter + 3] = (Vector3) { 0.f, 0.f, -1.f };
					mapNormals[nCounter + 4] = (Vector3) { 0.f, 0.f, -1.f };
					mapNormals[nCounter + 5] = (Vector3) { 0.f, 0.f, -1.f };
					nCounter += 6;

					mapTexcoords[tCounter + 5] = texRect[0];
					mapTexcoords[tCounter + 4] = texRect[1];
					mapTexcoords[tCounter + 3] = texRect[2];
					mapTexcoords[tCounter + 2] = texRect[2];
					mapTexcoords[tCounter + 1] = texRect[3];
					mapTexcoords[tCounter + 0] = texRect[0];
					tCounter += 6;
				}
			}
		}
	}
	
	free(exposed);
	
	Mesh mesh = { 0 };
	mesh.vertexCount = vCounter;
	mesh.triangleCount = vCounter / 3;
	
	mesh.vertices = (float*)malloc(sizeof(float) * mesh.vertexCount * 3);
	mesh.normals = (float*)malloc(sizeof(float) * mesh.vertexCount * 3);
	mesh.texcoords = (float*)malloc(sizeof(float) * mesh.vertexCount * 2);
	mesh.colors = NULL;
	
	memcpy(mesh.vertices, mapVertices, sizeof(float) * mesh.vertexCount * 3);
	memcpy(mesh.normals, mapNormals, sizeof(float) * mesh.vertexCount * 3);
	memcpy(mesh.texcoords, mapTexcoords, sizeof(float) * mesh.vertexCount * 2);

	free(mapVertices);
	free(mapNormals);
	free(mapTexcoords);
	
	UploadMesh(&mesh, false);

	return mesh;
}





