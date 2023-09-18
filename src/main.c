// 
// Currently just running test cases for the various components
//

#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include "vx_chunk.h"
#include "vx_mesh.h"

int main() {
	InitWindow(1920, 1080, "Voxel test");
	SetTargetFPS(60);
	DisableCursor();

	Camera camera = { 0 };
	camera.position = (Vector3) { 20.f, 10.f, 20.f };
	camera.target = (Vector3) { 0.f, 0.f, 0.f };
	camera.up = (Vector3) { 0.f, 1.f, 0.f };
	camera.fovy = 90.f;
	camera.projection = CAMERA_PERSPECTIVE;


	vx_chunk chunk = { 0, 0, malloc(16*16*256) };

	Image image = GenImageColor(16, 16, WHITE);
	Color* pixel = (Color*) image.data;
	for (int i = 0; i < 256; i++) {
		pixel[i] = (Color) { GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
	}
	Texture texture = LoadTextureFromImage(image);
	UnloadImage(image);

	for (int i = 0; i < 16*16*256; i++) {
		chunk.block[i] = GetRandomValue(0, 10);
	}

	Model model = LoadModelFromMesh(vx_load_chunk_mesh(&chunk));
	model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = texture;

	while (!WindowShouldClose()) {

		UpdateCamera(&camera, CAMERA_FIRST_PERSON);

		BeginDrawing();
		ClearBackground(WHITE);
		BeginMode3D(camera);
		DrawModel(model, Vector3Zero(), 1.f, WHITE);
		EndMode3D();
		DrawFPS(10, 10);
		EndDrawing();
	}

	UnloadTexture(texture);
	UnloadModel(model);
	CloseWindow();

	free(chunk.block);

	return 0;
}
