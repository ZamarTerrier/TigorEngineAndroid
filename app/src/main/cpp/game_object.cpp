//
// Created by prg6 on 11.03.2025.
//

#include "game_object.h"

#include "e_device.h"

extern TEngine engine;

void GameObjectSetInitFunc(GameObject *go, void *func){
    go->InitPoint = (void (*)(struct GameObject *))func;
}

void GameObjectSetUpdateFunc(GameObject *go, void *func){
    go->UpdatePoint = (void (*)(struct GameObject *))func;
}

void GameObjectSetDrawFunc(GameObject *go, void *func){
    go->DrawPoint = (void (*)(struct GameObject *))func;
}

void GameObjectSetCleanFunc(GameObject *go, void *func){
    go->CleanPoint = (void (*)(struct GameObject *))func;
}

void GameObjectSetRecreateFunc(GameObject *go, void *func){
    go->RecreatePoint = (void (*)(struct GameObject *))func;
}

void GameObjectSetDestroyFunc(GameObject *go, void *func){
    go->DestroyPoint = (void (*)(struct GameObject *))func;
}

void GameObjectSetShaderInitFunc(GameObject *go, void *func){
    go->ShaderInitPoint = (void (*)(struct GameObject *))func;
}

void GameObjectInit(GameObject* go){

    if(go == NULL)
        return;

    if(go->InitPoint != NULL)
        go->InitPoint(go);
}

void GameObjectUpdate(GameObject* go) {

    if(go == NULL)
        return;

    if(go->UpdatePoint == NULL)
        return;

    if(go->UpdatePoint != NULL)
        go->UpdatePoint(go);
}

void GameObjectDraw(GameObject* go) {

    if(go == NULL)
        return;

    if(!(go->flags & TIGOR_GAME_OBJECT_FLAG_INIT))
        return;

    GameObjectUpdate(go);

    if(go->DrawPoint != NULL)
        go->DrawPoint(go);
}

void GameObjectClean(GameObject* go){

    if(go == NULL)
        return;

    if(go->CleanPoint != NULL)
        go->CleanPoint(go);
}

void GameObjectRecreate(GameObject* go){

    if(go == NULL)
        return;

    if(go->RecreatePoint != NULL)
        go->RecreatePoint(go);
}

void GameObjectDestroy(GameObject* go){

    if(go == NULL)
        return;

    //EngineDeviceWaitIdle();

    if(go->DestroyPoint != NULL)
        go->DestroyPoint(go);
}

void GameObjectShaderInit(GameObject* go){

    if(go == NULL)
        return;

    if(go->ShaderInitPoint != NULL)
        go->ShaderInitPoint(go);
}
