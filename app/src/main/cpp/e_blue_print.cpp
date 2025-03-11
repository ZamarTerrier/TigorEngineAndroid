//
// Created by prg6 on 11.03.2025.
//

#include "e_blue_print.h"

#include "e_device.h"
#include "e_buffer.h"
#include "e_memory.h"
#include "swapchain.h"
#include "e_texture.h"
#include "pipeline.h"
#include "graphics_object.h."

#include <vulkan/vulkan.h>

#include "vulkan_android.h"

extern TEngine engine;

int BluePrintFindBluePrintBind(Blueprints *blueprints, uint32_t num_pack, uint32_t binding, uint32_t shader_bit){
    for(int i=0;i < blueprints->blue_print_packs[num_pack].num_descriptors;i++)
        if(blueprints->blue_print_packs[num_pack].descriptors[i].binding == binding){

            if(!(blueprints->blue_print_packs[num_pack].descriptors[i].stageflag & shader_bit))
                blueprints->blue_print_packs[num_pack].descriptors[i].stageflag |= shader_bit;

            return true;
        }

    return false;
}

uint32_t BluePrintInit(Blueprints *blueprints)
{
    uint32_t nums = blueprints->num_blue_print_packs;

    PipelineSetting* settings = (PipelineSetting *)&blueprints->blue_print_packs[nums].setting;
    PipelineSettingSetDefault(settings);


    blueprints->num_blue_print_packs++;
    return nums;
}

void BluePrintAddSomeUpdater(Blueprints *blueprints, uint32_t indx_pack, uint32_t bind_indx, UpdateDescriptor Updater){
    BluePrintPack *pack = &blueprints->blue_print_packs[indx_pack];

    BluePrintDescriptor *descriptor = NULL;

    for(int i=0;i < pack->num_descriptors;i++){
        BluePrintDescriptor *temp = &pack->descriptors[i];
        if( temp->descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){
            if(temp->binding == bind_indx){
                descriptor = temp;
            }
        }
    }

    if(descriptor != NULL)
        descriptor->update = (void *)Updater;
    else
        printf("Blue Print : Uniform bind %i is not found!\n", bind_indx);
}

void BluePrintSetSettingsValue(Blueprints *blueprints, uint32_t indx_pack, uint32_t type, uint32_t value){

    PipelineSetting *settings = &blueprints->blue_print_packs[indx_pack].setting;

    switch(type){
        case 0:
            settings->poligonMode = value;
            break;
        case 1:
            settings->topology = value;
            break;
        case 2:
            settings->cull_mode = value;
            break;
        case 3:
            settings->flags = value;
            break;
        default:
            break;
    }
}

uint32_t BluePrintGetSettingsValue(Blueprints *blueprints, uint32_t indx_pack, uint32_t type){

    PipelineSetting *settings = &blueprints->blue_print_packs[indx_pack].setting;

    switch(type){
        case 0:
            return settings->poligonMode;
        case 1:
            return settings->topology;
        case 2:
            return settings->cull_mode;
        case 3:
            return settings->flags;
        default:
            break;
    }

    return 0;
}

BluePrintDescriptor *BluePrintAddExistUniformStorage(Blueprints *blueprints, uint32_t indx_pack, uint32_t flags, BufferContainer uniform, void *update_func, uint32_t layer_indx)
{

    BluePrintPack *pack = &blueprints->blue_print_packs[indx_pack];

    if(blueprints->blue_print_packs[indx_pack].num_descriptors + 1 >= MAX_UNIFORMS)
    {
        printf("Too much descriptors!\n");
        return NULL;
    }
    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[blueprints->blue_print_packs[indx_pack].num_descriptors];

    descriptor->uniform = uniform;
    descriptor->descrType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptor->count = 1;
    descriptor->stageflag = flags;
    descriptor->buffsize = uniform.type_size;
    descriptor->image = NULL;
    descriptor->update = update_func;
    descriptor->indx_layer = layer_indx;
    descriptor->flags = TIGOR_BLUE_PRINT_FLAG_LINKED_UNIFORM;

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;

    return &pack->descriptors[pack->num_descriptors - 1];
}

BluePrintDescriptor *BluePrintAddUniformStorage(Blueprints *blueprints, uint32_t indx_pack, uint64_t size, uint32_t flags, void *update_func, uint32_t layer_indx){

    BluePrintPack *pack = &blueprints->blue_print_packs[indx_pack];

    if(blueprints->blue_print_packs[indx_pack].num_descriptors + 1 >= MAX_UNIFORMS)
    {
        printf("Too much descriptors!\n");
        return NULL;
    }

    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[blueprints->blue_print_packs[indx_pack].num_descriptors];

    descriptor->uniform.count = engine.imagesCount;
    descriptor->descrType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptor->count = 1;
    descriptor->stageflag = flags;
    descriptor->buffsize = size;
    descriptor->image = NULL;
    descriptor->update = update_func;
    descriptor->indx_layer = layer_indx;

    BuffersCreateStorage(&descriptor->uniform);

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;

    return &pack->descriptors[pack->num_descriptors - 1];
}

void BluePrintAddPushConstant(Blueprints *blueprints, uint32_t indx_pack, uint64_t size, uint32_t stage, uint32_t offset){

    BluePrintPushConstant *push_constant = &blueprints->blue_print_packs[indx_pack].push_constants[blueprints->blue_print_packs[indx_pack].num_push_constants];

    push_constant->offset = offset;
    push_constant->size = size;
    push_constant->stageFlags = stage;

    blueprints->blue_print_packs[indx_pack].num_push_constants ++;
}

void BluePrintAddUniformObjectC(Blueprints *blueprints, uint32_t indx_pack, uint64_t size, uint32_t flags, uint32_t binding){

    BluePrintPack *pack = &blueprints->blue_print_packs[indx_pack];

    if(pack->num_descriptors + 1 >= MAX_UNIFORMS)
    {
        printf("Too much descriptors!\n");
        return;
    }

    BluePrintDescriptor *descriptor = &pack->descriptors[pack->num_descriptors];

    descriptor->uniform.type_size = size;
    descriptor->uniform.count = engine.imagesCount;
    descriptor->descrType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor->count = 1;
    descriptor->stageflag = flags;
    descriptor->binding = binding;
    descriptor->buffsize = size;
    descriptor->image = NULL;
    descriptor->update = NULL;
    descriptor->indx_layer = 0;

    BuffersCreateUniform(&descriptor->uniform);

    pack->num_descriptors ++;
}

void BluePrintAddUniformObject(Blueprints *blueprints, uint32_t indx_pack, uint64_t size, uint32_t flags, void *update_func, uint32_t layer_indx){

    if(blueprints->blue_print_packs[indx_pack].num_descriptors + 1 >= MAX_UNIFORMS)
    {
        printf("Too much descriptors!\n");
        return;
    }

    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[blueprints->blue_print_packs[indx_pack].num_descriptors];

    descriptor->uniform.type_size = size;
    descriptor->uniform.count = engine.imagesCount;
    descriptor->descrType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor->count = 1;
    descriptor->stageflag = flags;
    descriptor->binding = blueprints->blue_print_packs[indx_pack].curr_bind;
    descriptor->buffsize = size;
    descriptor->image = NULL;
    descriptor->update = update_func;
    descriptor->indx_layer = layer_indx;

#ifndef NDEBUG
    printf("Uniform buffer size is : %i\n", size);
#endif


    BuffersCreateUniform(&descriptor->uniform);

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;
    blueprints->blue_print_packs[indx_pack].curr_bind ++;
}

void BluePrintAddRenderImageArray(Blueprints *blueprints, uint32_t indx_pack, void *obj, uint32_t size)
{
    /*RenderTexture **renders = (RenderTexture **)obj;

    uint32_t nums = blueprints->blue_print_packs[indx_pack].num_descriptors;
    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[nums];

    descriptor->textures = (Texture2D *)AllocateMemoryP(engine.imagesCount, sizeof(Texture2D*), blueprints);
    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->count = size;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = TIGOR_BLUE_PRINT_FLAG_LINKED_TEXTURE;

    Texture2D **textures = (Texture2D **)descriptor->textures;

    for(int i=0;i < engine.imagesCount;i++)
    {
        textures[i] = (Texture2D *)AllocateMemoryP(size, sizeof(Texture2D), blueprints);

        for(int j=0;j < size; j++)
        {
            if(renders[j]->flags & TIGOR_RENDER_FLAG_ONE_SHOT)
            {
                textures[i][j].image_view = (VkImageView)renders[j]->frames[0].render_texture.image_view;
                textures[i][j].sampler = (VkSampler)renders[j]->frames[0].render_texture.sampler;
            }else{
                textures[i][j].image_view = (VkImageView)renders[j]->frames[i].render_texture.image_view;
                textures[i][j].sampler = (VkSampler)renders[j]->frames[i].render_texture.sampler;
            }
        }
    }

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;*/
}

void BluePrintAddRenderImageVector(Blueprints *blueprints, uint32_t indx_pack, void *obj, uint32_t size)
{
    /*TSwapChain *swapchain = (TSwapChain *)engine.swapchain;

    RenderTexture **renders = obj;

    uint32_t nums = blueprints->blue_print_packs[indx_pack].num_descriptors;
    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[nums];

    descriptor->textures = AllocateMemoryP(engine.imagesCount, sizeof(Texture2D*), blueprints);
    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->count = size;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = TIGOR_BLUE_PRINT_FLAG_LINKED_TEXTURE;

    Texture2D *textures = descriptor->textures;

    for(int i=0;i < engine.imagesCount;i++)
    {
        textures[i] = AllocateMemoryP(size, sizeof(Texture2D), blueprints);

        for(int j=0;j < size; j++)
        {
            if(renders[j]->flags & TIGOR_RENDER_FLAG_ONE_SHOT)
            {
                textures[i][j].image_view = TextureCreateImageView(renders[j]->frames[0].render_texture.image, VK_IMAGE_VIEW_TYPE_2D_ARRAY, renders[j]->type == TIGOR_RENDER_TYPE_DEPTH ? findDepthFormat() : swapchain->swapChainImageFormat, renders[j]->type == TIGOR_RENDER_TYPE_DEPTH ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT, 1);
                textures[i][j].sampler = renders[j]->frames[0].render_texture.sampler;
            }else{
                textures[i][j].image_view = TextureCreateImageView(renders[j]->frames[i].render_texture.image, VK_IMAGE_VIEW_TYPE_2D_ARRAY, renders[j]->type == TIGOR_RENDER_TYPE_DEPTH ? findDepthFormat() : swapchain->swapChainImageFormat, renders[j]->type == TIGOR_RENDER_TYPE_DEPTH ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT, 1);
                textures[i][j].sampler = renders[j]->frames[i].render_texture.sampler;
            }
            textures[i][j].flags |= TIGOR_TEXTURE2D_FLAG_VIEW;
        }
    }

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;*/
}

void BluePrintAddRenderImageCube(Blueprints *blueprints, uint32_t indx_pack, uint32_t indx_cube, void *obj)
{
    /*RenderTexture *render = obj;

    uint32_t nums = blueprints->blue_print_packs[indx_pack].num_descriptors;
    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[nums];

    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->count = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = 0;

    Texture2D **textures = (Texture2D **)descriptor->textures;

    for(int i=0;i < engine.imagesCount;i++)
    {
        textures[i] = AllocateMemoryP(1, sizeof(Texture2D), blueprints);

        if(render->flags & TIGOR_RENDER_FLAG_ONE_SHOT)
        {
            textures[i][0].image_view = (VkImageView)render->frames[0].shadowCubeMapFaceImageViews[indx_cube];
            textures[i][0].sampler = (VkSampler)render->frames[0].render_texture.sampler;
        }else{
            textures[i][0].image_view = (VkImageView)render->frames[i].shadowCubeMapFaceImageViews[indx_cube];
            textures[i][0].sampler = (VkSampler)render->frames[i].render_texture.sampler;
        }
        textures[i][0].flags = 0;

    }

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;*/
}

void BluePrintAddRenderImage(Blueprints *blueprints, uint32_t indx_pack, void *obj)
{
    /*RenderTexture *render = obj;

    uint32_t nums = blueprints->blue_print_packs[indx_pack].num_descriptors;
    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[nums];

    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->count = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = 0;
    descriptor->textures = AllocateMemory(engine.imagesCount, sizeof(Texture2D));

    for(int i=0;i < engine.imagesCount;i++)
    {
        if(render->flags & TIGOR_RENDER_FLAG_ONE_SHOT)
        {
            descriptor->textures[i].image_view = (VkImageView)render->frames[0].render_texture.image_view;
            descriptor->textures[i].sampler = (VkSampler)render->frames[0].render_texture.sampler;
        }else{
            descriptor->textures[i].image_view = (VkImageView)render->frames[i].render_texture.image_view;
            descriptor->textures[i].sampler = (VkSampler)render->frames[i].render_texture.sampler;
        }
        descriptor->textures[i].flags = 0;

    }

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;*/
}

BluePrintDescriptor *BluePrintAddTextureC(Blueprints *blueprints, uint32_t indx_pack, uint32_t stage_bit, uint32_t binding){

    BluePrintPack *pack = &blueprints->blue_print_packs[indx_pack];

    if(pack->num_descriptors + 1 > MAX_UNIFORMS)
    {
        printf("Too much descriptors!\n");
        return NULL;
    }

    BluePrintDescriptor *descriptor = &pack->descriptors[pack->num_descriptors];

    descriptor->image = NULL;
    descriptor->count = 1;
    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->binding = binding;
    descriptor->stageflag = stage_bit;// VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = TIGOR_BLUE_PRINT_FLAG_SINGLE_IMAGE;

    pack->num_descriptors ++;

    return &pack->descriptors[pack->num_descriptors - 1];
}

void BluePrintSetTextureImage(Blueprints *blueprints, uint32_t indx_pack, Texture2D *texture, uint32_t bind_indx){

    BluePrintPack *pack = &blueprints->blue_print_packs[indx_pack];

    BluePrintDescriptor *descriptor = NULL;

    for(int i=0;i < pack->num_descriptors;i++){
        BluePrintDescriptor *temp = &pack->descriptors[i];
        if( temp->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            if(temp->binding == bind_indx){
                descriptor = temp;
                break;
            }
        }
    }

    if(descriptor != NULL){
        descriptor->textures = (Texture2D *)AllocateMemory(1, sizeof(Texture2D));

        descriptor->flags = TIGOR_BLUE_PRINT_FLAG_SINGLE_IMAGE | TIGOR_BLUE_PRINT_FLAG_LINKED_TEXTURE;
        descriptor->textures[0] = *texture;
        descriptor->count = 1;
    }
}

BluePrintDescriptor *BluePrintSetTextureImageCreate(Blueprints *blueprints, uint32_t indx_pack, GameObjectImage *image, uint32_t bind_indx){

    BluePrintPack *pack = &blueprints->blue_print_packs[indx_pack];

    if(image != NULL)
        if(image->img_type == 0)
            image->img_type = VK_FORMAT_R8G8B8A8_SRGB;

    BluePrintDescriptor *descriptor = NULL;

    for(int i=0;i < pack->num_descriptors;i++){
        BluePrintDescriptor *temp = &pack->descriptors[i];
        if( temp->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            if(temp->binding == bind_indx){
                descriptor = temp;
                break;
            }
        }
    }

    if(descriptor != NULL){
        descriptor->image = image;

        if(image == NULL)
            TextureCreate((struct BluePrintDescriptor_T *)descriptor, VK_IMAGE_VIEW_TYPE_2D, NULL, 0);
        else if(!(image->flags & TIGOR_TEXTURE_FLAG_SPECIFIC))
        {
            if(descriptor->image->size > 0)
                TextureCreate((struct BluePrintDescriptor_T *)descriptor, VK_IMAGE_VIEW_TYPE_2D, descriptor->image, 0);
            else
                TextureCreate((struct BluePrintDescriptor_T *)descriptor, VK_IMAGE_VIEW_TYPE_2D, descriptor->image, 1);

        }else
        {
            if(image->flags & TIGOR_TEXTURE_FLAG_URGB)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R8G8B8A8_UINT, image->imgWidth, image->imgHeight);
            else if(image->flags & TIGOR_TEXTURE_FLAG_R16)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R16_UNORM, image->imgWidth, image->imgHeight);
            else if(image->flags & TIGOR_TEXTURE_FLAG_R16_UINT)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R16_UINT, image->imgWidth, image->imgHeight);
            else if(image->flags & TIGOR_TEXTURE_FLAG_R32)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R32_SINT, image->imgWidth, image->imgHeight);
            else if(image->flags & TIGOR_TEXTURE_FLAG_R32_UINT)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R32_UINT, image->imgWidth, image->imgHeight);
            else if(image->flags & TIGOR_TEXTURE_FLAG_R32_FLOAT)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R32_SFLOAT, image->imgWidth, image->imgHeight);
            else if(image->flags & TIGOR_TEXTURE_FLAG_SRGB)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R8G8B8A8_SRGB, image->imgWidth, image->imgHeight);
            else
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R8G8B8A8_SINT, image->imgWidth, image->imgHeight);
        }
    }else
        printf("Error : Texture is not found!\n");

    return descriptor;
}

BluePrintDescriptor *BluePrintAddTextureImage(Blueprints *blueprints, uint32_t indx_pack, GameObjectImage *image, uint32_t stage_bit)
{
    BluePrintPack *pack = &blueprints->blue_print_packs[indx_pack];


    if(pack->num_descriptors + 1 > MAX_UNIFORMS)
    {
        printf("Too much descriptors!\n");
        return NULL;
    }

    BluePrintDescriptor *descriptor = &pack->descriptors[pack->num_descriptors];

    descriptor->image = image;
    descriptor->count = 1;
    descriptor->binding = blueprints->blue_print_packs[indx_pack].curr_bind;

    if(image == NULL){
        TextureCreate((struct BluePrintDescriptor_T *)descriptor, VK_IMAGE_VIEW_TYPE_2D, descriptor->image, 1);
    }else{

        if(image->img_type == 0)
            image->img_type = VK_FORMAT_R8G8B8A8_SRGB;

        if(!(image->flags & TIGOR_TEXTURE_FLAG_SPECIFIC))
        {
            if(descriptor->image->size > 0)
                TextureCreate((struct BluePrintDescriptor_T *)descriptor, VK_IMAGE_VIEW_TYPE_2D, descriptor->image, 0);
            else
                TextureCreate((struct BluePrintDescriptor_T *)descriptor, VK_IMAGE_VIEW_TYPE_2D, descriptor->image, 1);

        }else
        {
            if(image->flags & TIGOR_TEXTURE_FLAG_URGB)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R8G8B8A8_UINT, image->imgWidth, image->imgHeight);
            else if(image->flags & TIGOR_TEXTURE_FLAG_R16)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R16_UNORM, image->imgWidth, image->imgHeight);
            else if(image->flags & TIGOR_TEXTURE_FLAG_R16_UINT)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R16_UINT, image->imgWidth, image->imgHeight);
            else if(image->flags & TIGOR_TEXTURE_FLAG_R32)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R32_SINT, image->imgWidth, image->imgHeight);
            else if(image->flags & TIGOR_TEXTURE_FLAG_R32_UINT)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R32_UINT, image->imgWidth, image->imgHeight);
            else if(image->flags & TIGOR_TEXTURE_FLAG_SRGB)
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R8G8B8A8_SRGB, image->imgWidth, image->imgHeight);
            else
                TextureCreateSpecific((struct BluePrintDescriptor_T *)descriptor, VK_FORMAT_R8G8B8A8_SINT, image->imgWidth, image->imgHeight);
        }
    }

    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->stageflag = stage_bit;// VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = TIGOR_BLUE_PRINT_FLAG_SINGLE_IMAGE;

    pack->num_descriptors ++;

    blueprints->blue_print_packs[indx_pack].curr_bind++;

    return &pack->descriptors[pack->num_descriptors - 1];
}

void BluePrintAddTextureImageArrayCreate(Blueprints *blueprints, uint32_t indx_pack, GameObjectImage *images, uint32_t size, uint32_t bind_indx)
{
    BluePrintPack *pack = &blueprints->blue_print_packs[indx_pack];

    BluePrintDescriptor *descriptor = NULL;

    for(int i=0;i < pack->num_descriptors;i++){
        BluePrintDescriptor *temp = &pack->descriptors[i];
        if( temp->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            if(temp->binding == bind_indx){
                descriptor = temp;
                break;
            }
        }
    }

    if(descriptor != NULL){
        descriptor->image = images;
        descriptor->count = size;

        for(int i=0;i < size;i++)
        {
            if(images[i].img_type == 0)
                images[i].img_type = VK_FORMAT_R8G8B8A8_SRGB;
        }

        TextureCreateArray((struct BluePrintDescriptor_T *)descriptor, VK_IMAGE_VIEW_TYPE_2D, images, size);

        descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
        descriptor->flags = TIGOR_BLUE_PRINT_FLAG_SINGLE_IMAGE;
    }
}


void BluePrintClearTextures(BluePrintDescriptor *descriptor){

    TDevice *device = (TDevice *)engine.device;

    for(int i=0;i < descriptor->count;i++){
        Texture2D *texture = (Texture2D *)&descriptor->textures[i];

        if(texture->flags & TIGOR_TEXTURE2D_FLAG_GENERATED)
        {
            ImageDestroyTexture(texture);
        }else if(texture->flags & TIGOR_TEXTURE2D_FLAG_VIEW){
            vkDestroyImageView((VkDevice)device->e_device, texture->image_view, NULL);
        }
    }
}

void BluePrintClearShaders(PipelineSetting *settings){
    for(int i=0;i < settings->num_stages;i++){
        if(settings->stages[i].flags & TIGOR_SHADER_OBJECT_READED)
            FreeMemory(settings->stages[i].code_shader);
    }
}

void BluePrintClearAll(Blueprints *blueprints){

    TDevice *device = (TDevice *)engine.device;

    for(int i=0;i < blueprints->num_blue_print_packs;i++)
    {
        for(int j=0;j < blueprints->blue_print_packs[i].num_descriptors;j++)
        {
            BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[i].descriptors[j];

            if(descriptor->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER || descriptor->descrType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE){

                if(!(descriptor->flags & TIGOR_BLUE_PRINT_FLAG_LINKED_TEXTURE)){
                    BluePrintClearTextures(descriptor);
                }

                FreeMemory(descriptor->textures);
            }else{
                if(descriptor->flags & TIGOR_BLUE_PRINT_FLAG_LINKED_UNIFORM)
                    continue;

                BuffersDestroyContainer(&descriptor->uniform);
            }
        }

        BluePrintClearShaders(&blueprints->blue_print_packs[i].setting);
    }

}