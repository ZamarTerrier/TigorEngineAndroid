//
// Created by prg6 on 11.03.2025.
//

#ifndef TESTANDROID_E_TEXTURE_H
#define TESTANDROID_E_TEXTURE_H

#include "engine_includes.h"
#include "e_texture_variables.h"

#include "e_tools.h"

#define EMPTY_IMAGE_WIDTH 126
#define EMPTY_IMAGE_HEIGHT 126

struct BluePrintDescriptor_T;

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum{
    TIGOR_TEXTURE_FLAG_SPECIFIC = 0x1,
    TIGOR_TEXTURE_FLAG_URGB = 0x2,
    TIGOR_TEXTURE_FLAG_R16 = 0x4,
    TIGOR_TEXTURE_FLAG_R16_UINT = 0x8,
    TIGOR_TEXTURE_FLAG_R16_FLOAT = 0x10,
    TIGOR_TEXTURE_FLAG_R32 = 0x20,
    TIGOR_TEXTURE_FLAG_R32_UINT = 0x40,
    TIGOR_TEXTURE_FLAG_R32_FLOAT = 0x80,
    TIGOR_TEXTURE_FLAG_SRGB = 0x100
} EngineTextureFlags;

int ImageWriteFile(uint32_t indx);
int ImageLoadFile(ImageFileData *data, uint32_t from_file);
int ImageSetTile(const char *path, char *data, uint32_t width, uint32_t height, uint32_t tile_x, uint32_t tile_y, uint32_t tile_size);
int ImageResize(ImageFileData *data, uint32_t width, uint32_t height);

Texture2D *TextureFindTexture(char *image);

void TextureCreateImage(uint32_t width, uint32_t height, uint32_t mip_levels, uint32_t format, uint32_t tiling, uint32_t usage, uint32_t properties, uint32_t flags, Texture2D *texture);

int TextureImageCreate(GameObjectImage *image, uint32_t indx, struct BluePrintDescriptor_T *descriptor, bool from_file);

void TextureGenerateMipmaps(Texture2D *texture);

void ImageCreateEmpty(Texture2D *texture, uint32_t usage) ;
void TextureCreateEmptyDefault(Texture2D *texture);

void TextureCreateTextureImageView(Texture2D *texture, uint32_t type);

void* TextureCreateImageViewCube(void* image, void **shadowCubeMapFaceImageViews, uint32_t format, uint32_t aspect_mask);
VkImageView TextureCreateImageView(VkImage image, uint32_t type, uint32_t format, uint32_t aspectFlags, uint32_t mip_levels) ;

void TextureCreateSampler(void *sampler, uint32_t texture_type, uint32_t mip_levels);

void TextureCreate( struct BluePrintDescriptor_T *descriptor, uint32_t type, GameObjectImage *image, bool from_file);
void TextureCreateArray(struct BluePrintDescriptor_T *descriptor, uint32_t type, GameObjectImage *image, uint32_t size);
void TextureCreateSpecific(struct BluePrintDescriptor_T *descriptor, uint32_t format, uint32_t width, uint32_t height);

void TextureUpdate(struct BluePrintDescriptor_T *descriptor, void *in_data, uint32_t size_data, uint32_t offset);

void TextureSetTexture(struct BluePrintDescriptor_T *descriptor, const char* path);

void ImageDestroyTexture(Texture2D* texture);

#ifdef __cplusplus
}
#endif

#endif //TESTANDROID_E_TEXTURE_H
