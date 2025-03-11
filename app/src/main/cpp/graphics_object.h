//
// Created by prg6 on 11.03.2025.
//

#ifndef TESTANDROID_GRAPHICS_OBJECT_H
#define TESTANDROID_GRAPHICS_OBJECT_H


#include "engine_includes.h"
#include "graphics_items.h"

#include "e_blue_print.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    Blueprints blueprints;
    GraphicItems gItems;
    aboutShader aShader;
    bool lock;
    Shape shapes[MAX_SHAPES];
    uint32_t num_shapes;
} GraphicsObject;

void GraphicsObjectInit(GraphicsObject* graphObj, uint32_t type);

void GraphicsObjectSetShaderWithUniform(GraphicsObject* graphObj, ShaderObject *shader, uint32_t pack_indx);
void GraphicsObjectSetShader(GraphicsObject* graphObj, ShaderObject *shader, uint32_t pack_ind, uint32_t shader_type);

void GraphicsObjectSetVertex(GraphicsObject* graphObj, void *vert, int vertCount, uint32_t type_v_size, uint32_t *inx, int indxCount, uint32_t type_i_size);

void GraphicsObjectCreateDrawItems(GraphicsObject* graphObj);
void GraphicsObjectCleanPipelines(GraphicsObject *graphObj);
void GraphicsObjectClean(GraphicsObject* graphObj);
void GraphicsObjectDestroy(GraphicsObject* graphObj);

void GraphicsObjectSetShadersPath(GraphicsObject* graphOb, const char* vert, const char* frag);

#ifdef __cplusplus
}
#endif

#endif //TESTANDROID_GRAPHICS_OBJECT_H
