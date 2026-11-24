#include "raylib.h"
#include <stdlib.h>

int main() {

    InitWindow(800, 600, "Shader Testing");

    Shader shader = LoadShader(NULL, "./first.fs");
    int resolution_loc = GetShaderLocation(shader, "resolution");
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(shader, resolution_loc, resolution, SHADER_UNIFORM_VEC2);

    while (!WindowShouldClose()) {

        BeginDrawing();
        BeginShaderMode(shader);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
        EndShaderMode();
        EndDrawing();
    }
    UnloadShader(shader);
    CloseWindow();
}
