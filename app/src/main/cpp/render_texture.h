//
// Created by prg6 on 11.03.2025.
//

#ifndef TESTANDROID_RENDER_TEXTURE_H
#define TESTANDROID_RENDER_TEXTURE_H


#include "e_texture.h"

#include "engine_includes.h"

#include "game_object.h"

#define MAX_SURFACES 10

typedef enum EngineRenderType{
    TIGOR_RENDER_TYPE_WINDOW = 0x1,
    TIGOR_RENDER_TYPE_IMAGE = 0x2,
    TIGOR_RENDER_TYPE_DEPTH = 0x4,
    TIGOR_RENDER_TYPE_CUBEMAP = 0x8,
    TIGOR_RENDER_TYPE_GEOMETRY = 0x10,
} EngineRenderType;

typedef enum EngineRenderFlag{
    TIGOR_RENDER_FLAG_ONE_SHOT = 0x1,
    TIGOR_RENDER_FLAG_SHOOTED = 0x2,
    TIGOR_RENDER_FLAG_PERSPECTIVE = 0x4,
    TIGOR_RENDER_FLAG_FRUSTRUM = 0x8,
    TIGOR_RENDER_FLAG_ORTHOGRAPHIC = 0x10,
    TIGOR_RENDER_FLAG_DEPTH = 0x20,
} EngineRenderFlag;

typedef struct RenderFrame{
    Texture2D depth_texture;
    Texture2D render_texture;

    void **shadowCubeRenderPass;
    void **shadowCubeMapFaceImageViews;

    void **framebufers;

    uint32_t m_currentLayout;
} RenderFrame;

typedef struct RenderTexture{
    RenderFrame *frames;
    uint32_t num_frames;

    void *render_pass;


    double persp_view_distance;
    double persp_view_near;
    double persp_view_angle;

    double frust_side;
    double frust_near;
    double frust_far;

    double ortg_view_distance;
    double ortg_view_size;

    float cascadeSplit;

    vec3 clear_color;
    vec3 up_vector;

    mat4 view;

    uint32_t m_currentStage;

    uint32_t currFrame;

    uint32_t width;
    uint32_t height;
    uint32_t mip_levels;
    uint32_t m_format;

    EngineRenderType type;

    uint32_t indx_descriptor;
    uint32_t flags;
} RenderTexture;

void RenderTextureInit(RenderTexture *render, uint32_t type, uint32_t width, uint32_t height, uint32_t flags);

void RenderTextureAddSurface(RenderTexture *render);
void RenderTextureReload(RenderTexture *render, uint32_t indx_surf);

void RenderTextureSetCurrentFrame(RenderTexture *render, uint32_t indx_frame);
void RenderTextureBeginRendering(RenderTexture *render, void *cmd_buff);
void RenderTextureRecreate(RenderTexture *render);
void RenderTextureEndRendering(RenderTexture *render, void *cmd_buff);
void RenderTextureDestroy(RenderTexture *render);

#endif //TESTANDROID_RENDER_TEXTURE_H
