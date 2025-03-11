//
// Created by prg6 on 11.03.2025.
//

#include "TigorEngine.h"

#include "engine_includes.h"

#include <iostream>
#include <android/log.h>

#include "e_window.h"
#include "e_device.h"
#include "e_memory.h"
#include "vulkan_android.h"

#include "render_texture.h"

TEngine engine;
bool enableValidationLayers = true;

extern android_app *androidApp;

void EngineCreateSyncobjects() {

    TDevice *device = (TDevice *)engine.device;

    engine.Sync.imageAvailableSemaphores = (VkSemaphore *)AllocateMemoryP(engine.imagesCount, sizeof(VkSemaphore), &engine);
    engine.Sync.renderFinishedSemaphores = (VkSemaphore *)AllocateMemoryP(engine.imagesCount, sizeof(VkSemaphore), &engine);
    engine.Sync.inFlightFences = (VkFence *)AllocateMemoryP(engine.imagesCount, sizeof(VkFence), &engine);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < engine.imagesCount; i++) {
        if (vkCreateSemaphore((VkDevice)device->e_device, &semaphoreInfo, NULL, &engine.Sync.imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore((VkDevice)device->e_device, &semaphoreInfo, NULL, &engine.Sync.renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence((VkDevice)device->e_device, &fenceInfo, NULL, &engine.Sync.inFlightFences[i]) != VK_SUCCESS) {
            printf("failed to create synchronization objects for a frame!");
            exit(1);
        }
    }
}

void EngineInitVulkan(){
    createInstance();

#ifdef __ANDROID__
    TWindow *window = (TWindow *)engine.window;
    AndroidLoadVulkanFunctions((VkInstance)window->instance);
#endif

    if(enableValidationLayers)
        setupDebugMessenger();
    pickPhysicalDevice();
    createLogicalDevice();
    createSurfaceNative(androidApp->window);
    SwapChainCreate();
    SwapChainCreateImageViews();
    BuffersCreateCommandPool();
    ToolsCreateDepthResources();
    BuffersCreateCommand();
    EngineCreateSyncobjects();
}

void EngineClassicInit(){

    EngineInitVulkan();

    char *text = "Null texture";
    engine_buffered_image *images = (engine_buffered_image *)engine.DataR.e_var_images;
    TextureCreateEmptyDefault(&images[engine.DataR.e_var_num_images].texture);
    TextureCreateTextureImageView(&images[engine.DataR.e_var_num_images].texture, VK_IMAGE_VIEW_TYPE_2D);
    TextureCreateSampler(&images[engine.DataR.e_var_num_images].texture.sampler, images[engine.DataR.e_var_num_images].texture.textureType,  images[engine.DataR.e_var_num_images].texture.image_data.mip_levels);

    memcpy(images[engine.DataR.e_var_num_images].path, text, strlen(text));
    engine.DataR.e_var_num_images ++;

    memset(&engine.renders, 0, sizeof(EngineRenderItems));
    memset(&engine.lights, 0, sizeof(EngineLightItems));

    engine.main_render = AllocateMemory(1, sizeof(RenderTexture));

    RenderTextureInit((RenderTexture *)engine.main_render, TIGOR_RENDER_TYPE_WINDOW, 0, 0, 0);

    TEngineSetRender(engine.main_render, 1);

    //GUIManagerInit();
}

void TEngineInitSystem()
{
    memset(&engine, 0, sizeof(TEngine));

    engine.present = true;

    engine.window = (struct TWindow_T *)AllocateMemoryP(1, sizeof(TWindow), &engine);
    engine.device = (struct TDevice_T *)AllocateMemoryP(1, sizeof(TDevice), &engine);
    engine.swapchain = (struct TSwapChain_T *)AllocateMemoryP(1, sizeof(TSwapChain), &engine);

    engine.cache.alloc_buffers_memory_head = (ChildStack *)calloc(1, sizeof(ChildStack));
    engine.cache.alloc_descriptor_head = (ChildStack *)calloc(1, sizeof(ChildStack));
    engine.cache.alloc_pipeline_head = (ChildStack *)calloc(1, sizeof(ChildStack));

    engine.e_var_current_entry = NULL;

    engine.MAX_FRAMES_IN_FLIGHT = 3;

    engine.DataR.e_var_images = AllocateMemoryP(MAX_IMAGES, sizeof(engine_buffered_image), &engine);
    engine.DataR.e_var_num_images = 0;

    engine.DataR.e_var_fonts = (FontCache*)AllocateMemoryP(MAX_FONTS, sizeof(FontCache), &engine);
    engine.DataR.e_var_num_fonts = 0;

#ifdef __ANDROID__
    AndroidLoadVulkanLibrary();
#endif
}

void TEngineHandleAppCommand(android_app * app, int32_t cmd){
    //assert(app->userData != NULL);
    //VulkanExampleBase* vulkanExample = reinterpret_cast<VulkanExampleBase*>(app->userData);
    switch (cmd)
    {
        case APP_CMD_SAVE_STATE:
            std::cout << "APP_CMD_SAVE_STATE" << std::endl;
            /*
            vulkanExample->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)vulkanExample->app->savedState) = vulkanExample->state;
            vulkanExample->app->savedStateSize = sizeof(struct saved_state);
            */
            break;
        case APP_CMD_INIT_WINDOW:
            printf("APP_CMD_INIT_WINDOW");
            androidApp = app;
            EngineClassicInit();
            androidApp->userData = &engine;

            if(engine.func.InitFunc != NULL)
                engine.func.InitFunc();

            break;
        case APP_CMD_LOST_FOCUS:
            printf("APP_CMD_LOST_FOCUS");
            //vulkanExample->focused = false;
            break;
        case APP_CMD_GAINED_FOCUS:
            printf("APP_CMD_GAINED_FOCUS");
            //vulkanExample->focused = true;
            break;
        case APP_CMD_TERM_WINDOW:
            // Window is hidden or closed, clean up resources
            printf("APP_CMD_TERM_WINDOW");
            /*if (vulkanExample->prepared) {
                vulkanExample->swapChain.cleanup();
            }*/
            break;
        default:
            break;
    }
}

int32_t TEngineHandleAppInput(android_app* app, AInputEvent* event){

    return 0;
}

void TEngineSetRender(void *obj, uint32_t count)
{
    if(count == 0)
        return;

    RenderTexture *some_render = (RenderTexture *)obj;

    for (int i=0;i < engine.renders.size;i++){
        if(engine.renders.objects[i] == &some_render[0])
            return;
    }

    for(int i=0;i < count; i++ )
    {
        engine.renders.objects[engine.renders.size] = &some_render[i];

        engine.renders.size ++;
    }
}

void TEngineRender(){

    TDevice *device = (TDevice *)engine.device;
    TSwapChain *swapchain = (TSwapChain *)engine.swapchain;


    for( int i=0;i < engine.gameObjects.size;i++){
        if(!(engine.gameObjects.objects[i]->flags & TIGOR_GAME_OBJECT_FLAG_INIT))
            GameObjectInit(engine.gameObjects.objects[i]);
    }

    vkWaitForFences((VkDevice)device->e_device, 1, &engine.Sync.inFlightFences[engine.imageIndex], VK_TRUE, UINT64_MAX);
    VkResult result = vkAcquireNextImageKHR((VkDevice)device->e_device, swapchain->swapChain, UINT64_MAX, engine.Sync.imageAvailableSemaphores[engine.currentFrame], VK_NULL_HANDLE, &engine.imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChain();

        if(engine.func.RecreateFunc != NULL)
        {
            engine.func.RecreateFunc();
        }

        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        printf("failed to acquire swap chain image!");
        exit(1);
    }

    //vkResetCommandPool(e_device, commandPool, 0);

    /*if(GUIManagerIsInit())
        GUIManagerUpdate();*/

    VkCommandBufferBeginInfo *beginInfo = (VkCommandBufferBeginInfo *)AllocateMemory(1, sizeof(VkCommandBufferBeginInfo));
    beginInfo->sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //beginInfo->flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if(vkBeginCommandBuffer((VkCommandBuffer)device->commandBuffers[engine.imageIndex], beginInfo) != VK_SUCCESS){
        printf("failed begin command buffer/n");
        exit(1);
    }

    FreeMemory(beginInfo);

    for(int i=0;i < engine.renders.size;i++)
    {
        RenderTexture *render;
        engine.current_render = render = engine.renders.objects[i];

        if((render->flags & TIGOR_RENDER_FLAG_ONE_SHOT) && (render->flags & TIGOR_RENDER_FLAG_SHOOTED))
            continue;

        if(render->type & TIGOR_RENDER_TYPE_CUBEMAP)
        {
            for(int k=0;k < 6;k++)
            {
                RenderTextureSetCurrentFrame((RenderTexture *)engine.current_render, k);

                RenderTextureBeginRendering((RenderTexture *)engine.current_render, device->commandBuffers[engine.imageIndex]);

                for( int i=0;i < engine.gameObjects.size;i++)
                    GameObjectDraw(engine.gameObjects.objects[i]);

                RenderTextureEndRendering((RenderTexture *)engine.current_render, device->commandBuffers[engine.imageIndex]);
            }
        }else{

            RenderTextureBeginRendering((RenderTexture *)engine.current_render, device->commandBuffers[engine.imageIndex]);

            for( int i=0;i < engine.gameObjects.size;i++)
                GameObjectDraw(engine.gameObjects.objects[i]);

            /*if(GUIManagerIsInit())
                GUIManagerDraw();*/

            RenderTextureEndRendering((RenderTexture *)engine.current_render, device->commandBuffers[engine.imageIndex]);
        }
    }

    if (vkEndCommandBuffer((VkCommandBuffer)device->commandBuffers[engine.imageIndex]) != VK_SUCCESS) {
        printf("failed to record command buffer!");
        exit(1);
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {engine.Sync.imageAvailableSemaphores[engine.currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = (const VkCommandBuffer *)&device->commandBuffers[engine.imageIndex];

    VkSemaphore signalSemaphores[] = {engine.Sync.renderFinishedSemaphores[engine.currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences((VkDevice)device->e_device, 1, (const VkFence *)&engine.Sync.inFlightFences[engine.currentFrame]);

    if (vkQueueSubmit((VkQueue)device->graphicsQueue, 1, &submitInfo, engine.Sync.inFlightFences[engine.currentFrame]) != VK_SUCCESS) {
        printf("failed to submit draw command buffer!");
        exit(1);
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &engine.imageIndex;

    result = vkQueuePresentKHR((VkQueue)device->presentQueue, &presentInfo);

    vkQueueWaitIdle((VkQueue)device->presentQueue);

    if(engine.framebufferwasResized){

        engine.framebufferwasResized = false;
    }

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || engine.framebufferResized) {
        engine.framebufferResized = false;

        RecreateSwapChain();

        engine.framebufferwasResized = true;

        if(engine.func.RecreateFunc != NULL)
        {
            engine.func.RecreateFunc();
        }

    } else if (result != VK_SUCCESS) {
        printf("failed to present swap chain image!");
        exit(1);
    }

    for(int i=0; i < engine.renders.size;i++)
    {
        RenderTexture *render = engine.renders.objects[i];

        if((render->flags & TIGOR_RENDER_FLAG_ONE_SHOT) && !(render->flags & TIGOR_RENDER_FLAG_SHOOTED))
            render->flags |= TIGOR_RENDER_FLAG_SHOOTED;
    }

    engine.currentFrame = (engine.currentFrame + 1) % engine.MAX_FRAMES_IN_FLIGHT;

    engine.gameObjects.size = 0;

    /*if(GUIManagerIsInit())
        GUIManagerClear();*/
}

void TEngineDraw(GameObject *go){

    for( int i=0;i < engine.gameObjects.size;i++){
        if(engine.gameObjects.objects[i] == go)
            return;
    }

    engine.gameObjects.objects[engine.gameObjects.size] = go;
    engine.gameObjects.size ++;
}

void TEngineSetInitFunc(InitFunc_T func){
    engine.func.InitFunc = func;
}