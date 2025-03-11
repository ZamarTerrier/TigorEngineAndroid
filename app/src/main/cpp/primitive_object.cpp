//
// Created by prg6 on 11.03.2025.
//

#include "primitive_object.h"

#include <vulkan/vulkan.h>

#include "e_math.h"

#include "e_memory.h"
#include "e_camera.h"
#include "e_buffer.h"
#include "pipeline.h"
#include "e_texture.h"
#include "e_blue_print.h"

#include "render_texture.h"
#include "e_shaders.h"

#include "e_resource_shapes.h"
#include "e_resource_descriptors.h"

extern TEngine engine;

void PrimitiveObjectDestroy(PrimitiveObject *po)
{
    GameObject3DDestroy((GameObject3D *)po);

    FreeMemory(po->params);
    po->params = NULL;
}

int PrimitiveObjectInit(PrimitiveObject *po, DrawParam *dParam, char type, void *params){

    GameObject3DInit((GameObject3D *)po, TIGOR_GAME_OBJECT_TYPE_3D);

    GameObjectSetDestroyFunc((GameObject *)po, (void *)PrimitiveObjectDestroy);

    po->type = type;

    PlaneParam *pParam = (PlaneParam *)params;
    SphereParam *sParam = (SphereParam *)params;
    CubeSphereParam *csParam = (CubeSphereParam *)params;
    ConeParam *cParam = (ConeParam *)params;

    int builded = false;

    vertexParam vParam;
    indexParam iParam;

    po->params = NULL;
    switch(type)
    {
        case TIGOR_PRIMITIVE3D_LINE :
            GraphicsObjectSetVertex(&po->go.graphObj, (void *)lineVert, 2, sizeof(Vertex3D), NULL, 0, sizeof(uint32_t));
            break;
        case TIGOR_PRIMITIVE3D_TRIANGLE :
            GraphicsObjectSetVertex(&po->go.graphObj, (void *)triVert, 3, sizeof(Vertex3D), (uint32_t *)triIndx, 3, sizeof(uint32_t));
            break;
        case TIGOR_PRIMITIVE3D_QUAD :
            GraphicsObjectSetVertex(&po->go.graphObj, (void *)quadVert, 4, sizeof(Vertex3D), (uint32_t *)quadIndx, 6, sizeof(uint32_t));
            break;
        case TIGOR_PRIMITIVE3D_PLANE :
            InitPlane3D(&vParam, &iParam, pParam->sectorCount, pParam->stackCount);
            po->params = AllocateMemory(1, sizeof(PlaneParam));
            memcpy(po->params, params, sizeof(PlaneParam));
            builded = true;
            break;
        case TIGOR_PRIMITIVE3D_CUBE :
            GraphicsObjectSetVertex(&po->go.graphObj, (void *)cubeVert, 24, sizeof(Vertex3D), (uint32_t *)cubeIndx, 36, sizeof(uint32_t));
            break;
        case TIGOR_PRIMITIVE3D_CUBESPHERE :
            Cubesphere(&vParam, &iParam, csParam->radius, csParam->verperrow);
            po->params = AllocateMemory(1, sizeof(CubeSphereParam));
            memcpy(po->params, params, sizeof(CubeSphereParam));
            builded = true;
            break;
        case TIGOR_PRIMITIVE3D_ICOSPHERE :
            IcoSphereGenerator(&vParam, &iParam, sParam->radius);
            po->params = AllocateMemory(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            break;
        case TIGOR_PRIMITIVE3D_SPHERE :
            SphereGenerator3D(&vParam, &iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            po->params = AllocateMemory(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            builded = true;
            break;
        case TIGOR_PRIMITIVE3D_CONE :
            ConeGenerator(&vParam, &iParam, cParam->height, cParam->sectorCount, cParam->stackCount);
            po->params = AllocateMemory(1, sizeof(ConeParam));
            memcpy(po->params, params, sizeof(ConeParam));
            builded = true;
            break;
        case TIGOR_PRIMITIVE3D_SKYBOX:
            SphereGenerator3D(&vParam, &iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            po->params = AllocateMemory(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            builded = true;
            break;
    }

    if(builded)
    {
        GraphicsObjectSetVertex(&po->go.graphObj, vParam.vertices, vParam.num_verts, sizeof(Vertex3D), iParam.indices, iParam.indexesSize, sizeof(uint32_t));
        FreeMemory(vParam.vertices);
        FreeMemory(iParam.indices);
    }

    GameObject3DInitTextures((GameObject3D *)po, dParam);

    if(type == TIGOR_PRIMITIVE3D_SKYBOX)
        Transform3DSetScale((struct GameObject3D_T *)po, -500, -500, -500);

    return 1;
}
