//
// Created by prg6 on 11.03.2025.
//
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "e_texture.h"
#include "e_memory.h"
#include "e_device.h"
#include "e_buffer.h"
#include "e_blue_print.h"

#include "e_direct.h"

#include <vulkan/vulkan.h>
#include "vulkan_android.h"

#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"

#include "e_math.h"
#include "e_tools.h"

#include "e_resource_shapes.h"

extern TEngine engine;

//Не корректно
int ImageWriteFile(uint32_t indx)
{

    /*
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    RenderTexture *render = render_texture;

    uint32_t width = 2048, height = 2048;

    VkDeviceSize bufferSize = width * height * 4;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    ToolsTransitionImageLayout(render->frames[indx].image, render->m_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    ToolsCopyBufferToImage(stagingBuffer, render->frames[indx].image, width, height);
    ToolsTransitionImageLayout(render->frames[indx].image, render->m_format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    char *data;

    if(vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS){
         stbi_write_png("/home/ilia/temp.png", width, height, 4, data, width * 4);
         vkUnmapMemory(e_device, stagingBufferMemory);
    }


    return 0;*/
}

int ImageLoadFile(ImageFileData *data, uint32_t from_file)
{
    char *pixels;

    if(from_file)
        pixels = (char *)stbi_load(data->path, (int *)&data->texWidth, (int *)&data->texHeight, (int *)&data->texChannels, STBI_rgb_alpha);
    else
        pixels = (char *)stbi_load_from_memory((const stbi_uc *)data->buffer, data->buff_size, (int *)&data->texWidth, (int *)&data->texHeight, (int *)&data->texChannels, STBI_rgb_alpha);

    if(!pixels)
        return 1;

    data->data = pixels;

    return 0;
}

int ImageSetTile(const char *path, char *data, uint32_t width, uint32_t height, uint32_t tile_x, uint32_t tile_y, uint32_t tile_size)
{
    ImageFileData f_data;

    f_data.path = (char *)path;

    int res = ImageLoadFile(&f_data, 1);
    if(res)
        printf("Error load image!\n");

    if(f_data.texChannels < 4)
    {
        free(f_data.data);
        return 1;
    }

    res = ImageResize(&f_data, tile_size, tile_size);
    if(res)
        printf("Error when resize!\n");

    uint32_t iter_x = 0, iter_y = 0;

    uint32_t *t_point = (uint32_t *)f_data.data;
    uint32_t *d_point = (uint32_t *)data;
    while(iter_y < f_data.texHeight)
    {
        d_point[((tile_y * tile_size * width) + (tile_x * tile_size)) + ((iter_y * width) + iter_x)] = (t_point[iter_y * f_data.texWidth + iter_x] | 0xFF000000);

        iter_x ++;

        if(iter_x >= f_data.texWidth)
        {
            iter_x = 0;
            iter_y ++;
        }
    }

    free(f_data.data);

    return 0;
}

int ImageResize(ImageFileData *data, uint32_t width, uint32_t height)
{
    char *pixels = (char *)calloc(width * height, sizeof(char) * data->texChannels);

    int result = stbir_resize_uint8((const unsigned char *)data->data, data->texWidth, data->texHeight, 0, (unsigned char *)pixels, width, height, 0, data->texChannels);

    if(!result)
        return 1;

    free(data->data);

    data->data = pixels;
    data->texWidth = width;
    data->texHeight = height;

    return 0;
}

void ImageCreateEmpty(Texture2D *texture, uint32_t usage) {
    TDevice *device = (TDevice *)engine.device;

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = texture->image_data.texWidth;
    imageInfo.extent.height = texture->image_data.texHeight;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = (VkFormat)texture->textureType;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage; //VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage((VkDevice)device->e_device, &imageInfo, NULL, (VkImage *)&texture->image) != VK_SUCCESS) {
        printf("failed to create image!");
        exit(-1);
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements((VkDevice)device->e_device, texture->image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory((VkDevice)device->e_device, &allocInfo, NULL, (VkDeviceMemory *)&texture->memory) != VK_SUCCESS) {
        printf("failed to allocate image memory!");
        exit(-1);
    }

    vkBindImageMemory((VkDevice)device->e_device, texture->image, texture->memory, 0);

}

void TextureCreateEmptyDefault(Texture2D *texture)
{
    TDevice *device = (TDevice *)engine.device;

    BufferObject stagingBuffer;

    texture->textureType = VK_FORMAT_R8G8B8A8_SRGB;
    texture->image_data.texWidth = EMPTY_IMAGE_WIDTH;
    texture->image_data.texHeight = EMPTY_IMAGE_HEIGHT;
    texture->image_data.mip_levels = 1;

    VkDeviceSize bufferSize = EMPTY_IMAGE_HEIGHT * EMPTY_IMAGE_WIDTH * 4;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, TIGOR_BUFFER_ALLOCATE_STAGING);

    uint32_t * data;
    vkMapMemory((VkDevice)device->e_device, stagingBuffer.memory, 0, bufferSize, 0, (void **)&data);
    memcpy(data, tigrib_bin, bufferSize);
    vkUnmapMemory((VkDevice)device->e_device, stagingBuffer.memory);

    ImageCreateEmpty(texture, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    ToolsTransitionImageLayout((void *)texture->image, texture->textureType, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
    ToolsCopyBufferToImage((void *)stagingBuffer.buffer, (void *)texture->image, EMPTY_IMAGE_WIDTH, EMPTY_IMAGE_HEIGHT);
    ToolsTransitionImageLayout((void *)texture->image, texture->textureType, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

    BuffersDestroyBuffer(&stagingBuffer);
}

void TextureCreateEmpty(Texture2D *texture)
{
    TDevice *device = (TDevice *)engine.device;

    BufferObject stagingBuffer;

    VkDeviceSize bufferSize = texture->image_data.texWidth * texture->image_data.texHeight * 4;

    texture->image_data.mip_levels = 1;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, TIGOR_BUFFER_ALLOCATE_STAGING);

    uint32_t *data;
    vkMapMemory((VkDevice)device->e_device, stagingBuffer.memory, 0, bufferSize, 0, (void **)&data);
    memset(data, 0, bufferSize);
    vkUnmapMemory((VkDevice)device->e_device, stagingBuffer.memory);

    ImageCreateEmpty(texture, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    ToolsTransitionImageLayout((void *)texture->image, texture->textureType, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
    ToolsCopyBufferToImage((void *)stagingBuffer.buffer, (void *)texture->image, texture->image_data.texWidth, texture->image_data.texHeight);
    ToolsTransitionImageLayout((void *)texture->image, texture->textureType, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

    BuffersDestroyBuffer(&stagingBuffer);
}

Texture2D *TextureFindTexture(char *path)
{
    if(path == NULL)
        return NULL;

    engine_buffered_image *buff_images = (engine_buffered_image *)engine.DataR.e_var_images;

    char *temp;
    for(int i=0;i < engine.DataR.e_var_num_images;i++)
    {
        temp = buff_images[i].path;
        if(strstr(temp, path))
            return &buff_images[i].texture;
    }

    return NULL;
}

int TextureImageCreate(GameObjectImage *image, uint32_t indx, struct BluePrintDescriptor_T *descriptor, bool from_file) {

    TDevice *device = (TDevice *)engine.device;

    Texture2D *temp_tex;

    BluePrintDescriptor *descr = (BluePrintDescriptor *)descriptor;

    engine_buffered_image *images = (engine_buffered_image *)engine.DataR.e_var_images;

    if(image == NULL)
    {
        descr->textures[indx] = images[0].texture;
        descr->flags |= TIGOR_BLUE_PRINT_FLAG_LINKED_TEXTURE;
        return 0;
    }

    if(image->size == 0){
        if(image->path == NULL)
        {
            descr->textures[indx] = images[0].texture;
            descr->flags |= TIGOR_BLUE_PRINT_FLAG_LINKED_TEXTURE;
            return 0;
        } else if(!DirectIsFileExist((const char*)image->path)){
            descr->textures[indx] = images[0].texture;
            descr->flags |= TIGOR_BLUE_PRINT_FLAG_LINKED_TEXTURE;
            return 0;
        }
    }

    ImageFileData fileData;

    fileData.path = image->path;
    fileData.buffer = image->buffer;
    fileData.buff_size = image->size;

    temp_tex = TextureFindTexture(image->path);

    if(temp_tex != NULL)
    {
        if(fileData.buff_size != temp_tex->image_data.buff_size){
            char buffer[25];

            sprintf(buffer, "_%i", rand() % 10000);

            char *new_path = ToolsMakeString(image->path, buffer);

            FreeMemory(image->path);

            image->path = new_path;
            image->imgWidth = temp_tex->image_data.texWidth;
            image->imgHeight = temp_tex->image_data.texHeight;
        }else{
            descr->textures[indx] = *temp_tex;
            descr->flags |= TIGOR_BLUE_PRINT_FLAG_LINKED_TEXTURE;
            image->imgWidth = 126;
            image->imgHeight = 126;
            return 0;
        }
    }

    if(image->size == 0)
        ImageLoadFile(&fileData, from_file);
    else
        ImageLoadFile(&fileData, false);

    int len = strlen(image->path);
    memset(images[engine.DataR.e_var_num_images].path, 0, 2048);
    memcpy(images[engine.DataR.e_var_num_images].path, image->path, len);
    images[engine.DataR.e_var_num_images].texture.image_data = fileData;
    images[engine.DataR.e_var_num_images].texture.textureType = image->img_type; // VK_FORMAT_R8G8B8A8_SRGB;

    VkDeviceSize imageSize = fileData.texWidth * fileData.texHeight * sizeof(float);

    BufferObject stagingBuffer;

    uint32_t mip_levels = floor(log2(e_max(fileData.texWidth, fileData.texHeight)));

    images[engine.DataR.e_var_num_images].texture.image_data.mip_levels = mip_levels;

    void* data;

    if (!fileData.data) {
        printf("failed to load texture image!");

        descr->textures[indx] = images[0].texture;

        return 0;
    }

    BuffersCreate(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, TIGOR_BUFFER_ALLOCATE_STAGING);

    vkMapMemory((VkDevice)device->e_device, stagingBuffer.memory, 0, imageSize, 0, &data);
    memcpy(data, fileData.data, imageSize);
    vkUnmapMemory((VkDevice)device->e_device, stagingBuffer.memory);

    TextureCreateImage( fileData.texWidth, fileData.texHeight, mip_levels, images[engine.DataR.e_var_num_images].texture.textureType,
                        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        0, &images[engine.DataR.e_var_num_images].texture);

    ToolsTransitionImageLayout((void *)images[engine.DataR.e_var_num_images].texture.image, images[engine.DataR.e_var_num_images].texture.textureType, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mip_levels);
    ToolsCopyBufferToImage((void *)stagingBuffer.buffer, (void *)images[engine.DataR.e_var_num_images].texture.image, fileData.texWidth, fileData.texHeight);
    //ToolsTransitionImageLayout(images[e_var_num_images].texture.textureImage, images[e_var_num_images].texture.textureType, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mip_levels);

    BuffersDestroyBuffer(&stagingBuffer);

    TextureGenerateMipmaps(&images[engine.DataR.e_var_num_images].texture);

    image->imgWidth = fileData.texWidth;
    image->imgHeight = fileData.texHeight;

    return 1;
}

void TextureGenerateMipmaps(Texture2D *texture){

    TDevice *device = (TDevice *)engine.device;

    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties((VkPhysicalDevice)device->e_physicalDevice, (VkFormat)texture->textureType, &formatProperties);

    if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)){
        printf("Error Mipmap\n");
        exit(1);
    }

    VkCommandBuffer commandBuffer = (VkCommandBuffer)beginSingleTimeCommands();

    VkImageMemoryBarrier barrier;
    memset(&barrier, 0, sizeof(VkImageMemoryBarrier));
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = texture->image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int mipWidth = texture->image_data.texWidth;
    int mipHeight = texture->image_data.texHeight;

    for(int i=1;i < texture->image_data.mip_levels;i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

        VkImageBlit blit;
        memset(&blit.srcOffsets[0], 0, sizeof(VkOffset3D));
        blit.srcOffsets[1].x = mipWidth;
        blit.srcOffsets[1].y = mipHeight;
        blit.srcOffsets[1].z = 1;
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        memset(&blit.dstOffsets[0], 0, sizeof(VkOffset3D));
        blit.dstOffsets[1].x = mipWidth > 1 ? mipWidth / 2 : 1;
        blit.dstOffsets[1].y = mipHeight > 1 ? mipHeight / 2 : 1;
        blit.dstOffsets[1].z = 1;
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer, texture->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;


        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

        if(mipWidth > 1) mipWidth /=2;
        if(mipHeight > 1) mipHeight /=2;
    }


    barrier.subresourceRange.baseMipLevel = texture->image_data.mip_levels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

void TextureCreateTextureImageView(Texture2D *texture, uint32_t type) {
    texture->image_view = TextureCreateImageView(texture->image, type, texture->textureType, VK_IMAGE_ASPECT_COLOR_BIT, texture->image_data.mip_levels);
}

void TextureCreateImage(uint32_t width, uint32_t height, uint32_t mip_levels, uint32_t format, uint32_t tiling, uint32_t usage, uint32_t properties, uint32_t flags, Texture2D *texture) {
    TDevice *device = (TDevice *)engine.device;

    VkImageCreateInfo imageInfo = {};
    memset(&imageInfo, 0, sizeof(VkImageCreateInfo));
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mip_levels;

    if(flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
        imageInfo.arrayLayers = 6;
    else
        imageInfo.arrayLayers = 1;

    imageInfo.format = (VkFormat)format;
    imageInfo.tiling = (VkImageTiling)tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = flags;

    if (vkCreateImage((VkDevice)device->e_device, &imageInfo, NULL, &texture->image) != VK_SUCCESS) {
        printf("failed to create image!");
        exit(-1);
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements((VkDevice)device->e_device, texture->image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    memset(&allocInfo, 0, sizeof(VkMemoryAllocateInfo));
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory((VkDevice)device->e_device, &allocInfo, NULL, &texture->memory) != VK_SUCCESS) {
        printf("failed to allocate image memory!");
        exit(-1);
    }

    vkBindImageMemory((VkDevice)device->e_device, texture->image, texture->memory, 0);
}

VkImageView TextureCreateImageView(VkImage image, uint32_t type, uint32_t format, uint32_t aspectFlags, uint32_t mip_levels) {
    TDevice *device = (TDevice *)engine.device;

    VkImageViewCreateInfo viewInfo = {};
    memset(&viewInfo, 0, sizeof(VkImageViewCreateInfo));
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = (VkImageViewType)type; //VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = (VkFormat)format; //VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mip_levels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView((VkDevice)device->e_device, &viewInfo, NULL, &imageView) != VK_SUCCESS) {
        printf("failed to create texture image view!");
        exit(1);
    }

    return imageView;
}

void* TextureCreateImageViewCube(void* image, void **shadowCubeMapFaceImageViews, uint32_t format, uint32_t aspect_mask) {
    TDevice *device = (TDevice *)engine.device;

    VkImageView *some_views = (VkImageView *)shadowCubeMapFaceImageViews;

    VkImageViewCreateInfo *viewInfo = (VkImageViewCreateInfo *)AllocateMemory(1, sizeof(VkImageViewCreateInfo));
    viewInfo->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo->image = (VkImage)image;
    viewInfo->viewType = VK_IMAGE_VIEW_TYPE_CUBE; //VK_IMAGE_VIEW_TYPE_2D;
    viewInfo->format = (VkFormat)format; //VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo->subresourceRange.aspectMask = aspect_mask;
    viewInfo->subresourceRange.baseMipLevel = 0;
    viewInfo->subresourceRange.levelCount = 1;
    viewInfo->subresourceRange.baseArrayLayer = 0;
    viewInfo->subresourceRange.layerCount = 6;

    VkImageView imageView;
    if (vkCreateImageView((VkDevice)device->e_device, viewInfo, NULL, &imageView) != VK_SUCCESS) {
        printf("failed to create texture image view!");
        exit(1);
    }

    viewInfo->viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo->subresourceRange.layerCount = 1;
    viewInfo->image = (VkImage)image;

    for (uint32_t i = 0; i < 6; i++)
    {
        viewInfo->subresourceRange.baseArrayLayer = i;
        vkCreateImageView((VkDevice)device->e_device, viewInfo, NULL, &some_views[i]);
    }

    FreeMemory(viewInfo);

    return (void *)imageView;
}

void TextureCreateSampler(void *sampler, uint32_t texture_type, uint32_t mip_levels) {
    TDevice *device = (TDevice *)engine.device;

    VkSamplerCreateInfo samplerInfo = {};
    memset(&samplerInfo, 0, sizeof(VkSamplerCreateInfo));
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    if(texture_type == VK_FORMAT_R8G8B8A8_UINT || texture_type == VK_FORMAT_R16_UINT || texture_type == VK_FORMAT_R32_UINT)
    {
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_NEAREST;
    }else{
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
    }

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties  ={};
    memset(&properties, 0, sizeof(VkPhysicalDeviceProperties));
    vkGetPhysicalDeviceProperties((VkPhysicalDevice)device->e_physicalDevice, &properties);

    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = mip_levels;

    if (vkCreateSampler((VkDevice)device->e_device, &samplerInfo, NULL, (VkSampler *)sampler) != VK_SUCCESS) {
        printf("failed to create texture sampler!");
        exit(1);
    }
}

void TextureArrayInit(Blueprints *blueprints, uint32_t size)
{
    /*
    if(blueprints->count + 1 > MAX_UNIFORMS)
    {
        printf("Слишком много декрипторов!\n");
        return;
    }

    BluePrintDescriptor *descriptor = &blueprints->descriptors[blueprints->count];

    descriptor->descrType = TIGOR_DESCRIPTOR_TYPE_IMAGE_SAMPLER;
    descriptor->descrCount = 0;
    descriptor->size = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;*/
}

void TextureCreate(struct BluePrintDescriptor_T *descriptor, uint32_t type, GameObjectImage *image, bool from_file){

    BluePrintDescriptor *descr = (BluePrintDescriptor *)descriptor;

    descr->textures = (Texture2D *)AllocateMemory(1, sizeof(Texture2D));

    int res = TextureImageCreate(image, 0, descriptor, from_file);

    if(res)
    {
        engine_buffered_image *images = (engine_buffered_image *)engine.DataR.e_var_images;

        Texture2D *texture = &images[engine.DataR.e_var_num_images].texture;

        texture->flags = (EngineTexture2DFlag)0;

        TextureCreateTextureImageView(texture, type);
        TextureCreateSampler(&texture->sampler, texture->textureType, texture->image_data.mip_levels);

        descr->textures[0] = *texture;

        image->imgHeight = texture->image_data.texHeight;
        image->imgWidth = texture->image_data.texWidth;

        engine.DataR.e_var_num_images ++;
    }
}

void TextureCreateArray(struct BluePrintDescriptor_T *descriptor, uint32_t type, GameObjectImage *images, uint32_t size){

    BluePrintDescriptor *descr = (BluePrintDescriptor *)descriptor;

    descr->textures = (Texture2D *)AllocateMemory(size, sizeof(Texture2D));

    for(int i = 0; i < size;i++){

        int res = TextureImageCreate(&images[i], i, descriptor, false);

        if(res)
        {
            engine_buffered_image *b_images = (engine_buffered_image *)engine.DataR.e_var_images;

            Texture2D *texture = &b_images[engine.DataR.e_var_num_images].texture;

            texture->flags = (EngineTexture2DFlag)0;

            TextureCreateTextureImageView(texture, type);
            TextureCreateSampler(&texture->sampler, texture->textureType, texture->image_data.mip_levels);

            descr->textures[i] = *texture;

            images[i].imgHeight = texture->image_data.texHeight;
            images[i].imgWidth = texture->image_data.texWidth;

            engine.DataR.e_var_num_images ++;
        }
    }
}

void TextureCreateSpecific(struct BluePrintDescriptor_T *descriptor, uint32_t format, uint32_t width, uint32_t height)
{
    BluePrintDescriptor *descr = (BluePrintDescriptor *)descriptor;

    descr->textures = (Texture2D *)AllocateMemory(1, sizeof(Texture2D));

    Texture2D *texture = descr->textures;

    texture->flags = TIGOR_TEXTURE2D_FLAG_GENERATED;
    texture->image_data.texWidth = width;
    texture->image_data.texHeight = height;
    texture->textureType = format;

    TextureCreateEmpty(texture);
    TextureCreateTextureImageView(texture, VK_IMAGE_VIEW_TYPE_2D);
    TextureCreateSampler(&texture->sampler, texture->textureType,  1);

}

void TextureUpdate(struct BluePrintDescriptor_T *descriptor, void *in_data, uint32_t size_data, uint32_t offset)
{
    BluePrintDescriptor *descr = (BluePrintDescriptor *)descriptor;

    TDevice *device = (TDevice *)engine.device;

    Texture2D *texture = &descr->textures[0];

    BufferObject stagingBuffer;

    VkDeviceSize bufferSize = texture->image_data.texWidth * texture->image_data.texHeight * 4;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, TIGOR_BUFFER_ALLOCATE_STAGING);

    uint32_t *data;
    vkMapMemory((VkDevice)device->e_device, stagingBuffer.memory, 0, bufferSize, 0, (void **)&data);
    memset(data, 0, bufferSize);
    memcpy(data + offset, in_data, size_data);
    vkUnmapMemory((VkDevice)device->e_device, stagingBuffer.memory);

    ToolsTransitionImageLayout((void *)texture->image, texture->textureType, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture->image_data.mip_levels);
    ToolsCopyBufferToImage((void *)stagingBuffer.buffer, (void *)texture->image, texture->image_data.texWidth, texture->image_data.texHeight);
    ToolsTransitionImageLayout((void *)texture->image, texture->textureType, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture->image_data.mip_levels);

    BuffersDestroyBuffer(&stagingBuffer);
}

//Не корректно
void TextureSetTexture(struct BluePrintDescriptor_T *descriptor, const char* path){

    /*ImageDestroyTexture(descriptor->textures);

    GameObjectImage g_image;
    memset(&g_image, 0, sizeof(GameObjectImage));

    if(path != NULL)
        TextureCreate(descriptor, &g_image, 1);
    else
        TextureCreateSpecific(descriptor, VK_FORMAT_R8G8B8A8_SINT, 100, 100);*/

}

void ImageDestroyTexture(Texture2D* texture){
    TDevice *device = (TDevice *)engine.device;

    vkFreeMemory((VkDevice)device->e_device, texture->memory, NULL);
    vkDestroyImage((VkDevice)device->e_device, texture->image, NULL);
    vkDestroySampler((VkDevice)device->e_device, texture->sampler, NULL);
    vkDestroyImageView((VkDevice)device->e_device, texture->image_view, NULL);

    if(!(texture->flags & TIGOR_TEXTURE2D_IS_FONT))
        free(texture->image_data.data);
}