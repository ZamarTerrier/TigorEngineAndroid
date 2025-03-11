//
// Created by prg6 on 11.03.2025.
//

#ifndef TESTANDROID_PIPELINE_H
#define TESTANDROID_PIPELINE_H

#include "engine_includes.h"

#include "e_pipeline_variables.h"

#include "graphics_object.h"

#include "e_tools.h"
#include "e_descriptor.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    void *GraphicsPipeline;
    void *GraphicsPipelineLayout;
} PipelineStack;

void PipelineSettingSetShader(PipelineSetting *setting, ShaderObject *shader, uint32_t type);

void PipelineDestroyStack(void *pipeline);

void PipelineClearAll();

void PipelineSettingSetDefault(void *setting);

void PipelineCreateGraphics(GraphicsObject* graphObj);

void PipelineDestroy(ShaderPack *pipeline);

#ifdef __cplusplus
}
#endif

#endif //TESTANDROID_PIPELINE_H
