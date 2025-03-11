//
// Created by prg6 on 11.03.2025.
//

#ifndef TESTANDROID_GRAPHICS_ITEMS_H
#define TESTANDROID_GRAPHICS_ITEMS_H

#include "e_buffer.h"

#include "engine_includes.h"

#define MAX_SHAPES 8

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    TIGOR_SHADER_OBJECT_NONE = 0x1,
    TIGOR_SHADER_OBJECT_READED = 0x2
} ShaderObjectFlags;

typedef struct{
    char* code;
    size_t size;
    ShaderObjectFlags flags;
} ShaderObject;

typedef struct{
    void *pipeline;
    void *layout;
} PipelineStruct;

typedef struct{
    void *descr_pool;
    void *descr_set_layout;
    VkDescriptorSet *descr_sets;
} ShaderDescriptor;

typedef struct{
    ShaderDescriptor descriptor;
    PipelineStruct pipeline;
} ShaderPack;

typedef struct{
    ShaderPack shader_packs[MAX_BLUE_PRINTS];
    uint32_t num_shader_packs;
    bool perspective;
} GraphicItems;

typedef struct{
    BufferObject buffer;
    uint32_t typeSize;
    uint32_t bufferSize;
    bool extend;
    void* vertices;
    uint32_t num_verts;
} vertexParam;

typedef struct{
    BufferObject buffer;
    uint32_t typeSize;
    uint32_t indexesSize;
    uint32_t bufferSize;
    bool extend;
    uint32_t* indices;
} indexParam;

typedef struct{
    char vertShader[256];
    char fragShader[256];
} aboutShader;

typedef struct{
    indexParam iParam;
    vertexParam vParam;

    EIVertexInputBindingDescription* bindingDescription;
    EIVertexInputAttributeDescription* attr;
    uint32_t countBind;
    uint32_t countAttr;

    uint32_t type;

    bool init;
} Shape;

enum EngineDescriptorType{
    TIGOR_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    TIGOR_DESCRIPTOR_TYPE_IMAGE_SAMPLER,
    TIGOR_DESCRIPTOR_TYPE_IMAGE_ARRAY
};

#ifdef __cplusplus
}
#endif

#endif //TESTANDROID_GRAPHICS_ITEMS_H
