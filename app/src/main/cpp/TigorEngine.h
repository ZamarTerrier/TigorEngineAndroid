//
// Created by prg6 on 11.03.2025.
//

#ifndef TESTANDROID_TIGORENGINE_H
#define TESTANDROID_TIGORENGINE_H

#include <cstdint>

#include <game-activity/native_app_glue/android_native_app_glue.h>

#include "game_object.h"

typedef void (*InitFunc_T)();

void TEngineInitSystem();

void TEngineHandleAppCommand(android_app * app, int32_t cmd);
int32_t TEngineHandleAppInput(android_app* app, AInputEvent* event);

void TEngineRender();
void TEngineSetRender(void *obj, uint32_t count);

void TEngineDraw(GameObject *go);

void TEngineSetInitFunc(InitFunc_T func);

#endif //TESTANDROID_TIGORENGINE_H
