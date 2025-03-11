//
// Created by prg6 on 11.03.2025.
//

#ifndef TESTANDROID_E_WINDOW_H
#define TESTANDROID_E_WINDOW_H

#include "engine_includes.h"

#include "e_debugger.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    void* instance;
    void* surface;

    void *e_window;

} TWindow;

bool checkValidationLayerSupport();

const char** getRequiredExtensions();

void InitWindow();

static void framebufferResizeCallback(void* window, int width, int height);

void createInstance();

void createSurface();
void createSurfaceNative(ANativeWindow* window);

vec2 getWindowSize();

#ifdef __cplusplus
}
#endif

#endif //TESTANDROID_E_WINDOW_H
