//
// Created by prg6 on 11.03.2025.
//

#include "e_window.h"

#include <vulkan/vulkan.h>

#ifdef __ANDROID__
#include "vulkan_android.h"
#endif

#include "e_memory.h"

#define VK_KHR_SURFACE_EXTENSION_NAME     "VK_KHR_surface"
#define VK_KHR_ANDROID_SURFACE_EXTENSION_NAME "VK_KHR_android_surface"

const char* validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
};
uint32_t num_valid_layers = 1;

const char* deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
uint32_t num_dev_extensions = 1;

extern TEngine engine;
extern bool enableValidationLayers;

bool checkValidationLayerSupport(){
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, ( VkLayerProperties *)NULL);

    VkLayerProperties* availableLayers = (VkLayerProperties*) AllocateMemory(layerCount, sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    bool layerFound;

    for(int i=0; i < num_valid_layers; i++)
    {
        layerFound = false;

        for(int j=0; j < layerCount; j++)
        {
            if(strcmp(validationLayers[i], availableLayers[j].layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
            return false;
    }

    FreeMemory(availableLayers);

    return true;

}

const char** getRequiredExtensions(){

    const char** extensions;

#ifndef __ANDROID__
    extensions = wManagerGetRequiredInstanceExtensions(&engine.wManagerExtensionCount);
#else
    extensions = (const char **)AllocateMemory(3, sizeof(char *));
    extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
    extensions[1] = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;

    engine.wManagerExtensionCount = 2;

#endif

    if(enableValidationLayers)
        engine.wManagerExtensionCount ++;

    const char** wManagerExtensions = (const char **) AllocateMemory(engine.wManagerExtensionCount, sizeof(char *));

    const char *temp;
    for(int i=0; i < engine.wManagerExtensionCount; i++) {
        temp = extensions[i];
        wManagerExtensions[i] = extensions[i];
    }

    FreeMemory(extensions);
    if(enableValidationLayers)
        wManagerExtensions[engine.wManagerExtensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    return wManagerExtensions;

}

void InitWindow(){
    TWindow *window = (TWindow *)engine.window;

#ifndef __ANDROID__
    wManagerInit();
    //wManagerWindowHint(TIGOR_RESIZABLE, false);
    if(!wManagerCreateWindow(window->e_window, engine.width, engine.height, engine.app_name)){
        wManagerTerminate();
        printf("Error when create window!\n");
        exit(0);
    }

    wManagerSetFramebufferSizeCallback(window->e_window, (wManagerFrameBufferSizeFun)framebufferResizeCallback);
#endif
}

static void framebufferResizeCallback(void* window, int width, int height) {

    if(engine.framebufferwasResized)
        engine.framebufferResized = true;

}

void createInstance(){
    TWindow *window = (TWindow *)engine.window;

    if(enableValidationLayers && !checkValidationLayerSupport()){
        printf("validation layers requested, but not available\n");
        enableValidationLayers = false;
    }

    VkApplicationInfo appInfo;
    memset(&appInfo, 0, sizeof(VkApplicationInfo));

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = engine.app_name;
    appInfo.applicationVersion = VK_MAKE_VERSION(0,0,0);
    appInfo.pEngineName = "TigorEngine";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(VkInstanceCreateInfo));

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugInfo;
    memset(&debugInfo, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));

    if(enableValidationLayers){
        createInfo.enabledLayerCount = num_valid_layers;
        createInfo.ppEnabledLayerNames = validationLayers;

        populateDebugMessengerCreateInfo((EdDebugUtilsMessengerCreateInfoEXT *)&debugInfo);

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = NULL;
    }

    const char** wManagerExtensions = getRequiredExtensions();

    createInfo.enabledExtensionCount = engine.wManagerExtensionCount;
    createInfo.ppEnabledExtensionNames = wManagerExtensions;

    if(vkCreateInstance(&createInfo, NULL, (VkInstance *) &window->instance)){
        __android_log_print(ANDROID_LOG_ERROR, "TigorEngine","Failed to create instance\n");
        exit(1);
    }

    FreeMemory(wManagerExtensions);
}

void createSurface() {
    TWindow *window = (TWindow *)engine.window;

#ifndef __ANDROID__
    if (wManagerCreateWindowSurface(window->instance, window->e_window, NULL, (VkSurfaceKHR *) &window->surface) != VK_SUCCESS) {
        printf("failed to create window surface!");
        exit(1);
    }
#endif
}

void createSurfaceNative(ANativeWindow* window){
    TWindow *e_window = (TWindow *)engine.window;

    VkResult err = VK_SUCCESS;

    VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.window = window;
    err = vkCreateAndroidSurfaceKHR((VkInstance)e_window->instance, &surfaceCreateInfo, NULL, (VkSurfaceKHR *)&e_window->surface);
}

vec2 getWindowSize()
{
    vec2 size;

    size.x = engine.width  * engine.diffSize.x;
    size.y = engine.height * engine.diffSize.y;

    return size;
}
