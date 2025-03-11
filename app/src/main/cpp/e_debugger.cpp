//
// Created by prg6 on 11.03.2025.
//
#include "e_window.h"
#include "e_debugger.h"

#include "vulkan_android.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

extern TEngine engine;
extern bool enableValidationLayers;

uint32_t CreateDebugUtilsMessengerEXT(void* arg, const EdDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const EdAllocationCallbacks* pAllocator, void** messenger) {

    VkInstance instance = (VkInstance )arg;
    VkDebugUtilsMessengerEXT* pDebugMessenger = (VkDebugUtilsMessengerEXT *) messenger;

    PFN_vkCreateDebugUtilsMessengerEXT func1 = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func1 != NULL) {
        return func1((VkInstance)instance, (const VkDebugUtilsMessengerCreateInfoEXT *)pCreateInfo, (const VkAllocationCallbacks*)pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(void* arg, void* debugMessenger, const EdAllocationCallbacks* pAllocator) {

    VkInstance instance = (VkInstance )arg;

    PFN_vkDestroyDebugUtilsMessengerEXT func2 = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func2 != NULL) {
        func2((VkInstance)instance, (VkDebugUtilsMessengerEXT)debugMessenger, (const VkAllocationCallbacks*)pAllocator);
    }
}


void *thread_f(const char *text){

    printf("validation layer : %s\n", text);

}

uint32_t debugCallback(
        uint32_t messageSeverity,
        uint32_t messageType,
        const void** CallbackData,
        void* pUserData){

    VkDebugUtilsMessengerCallbackDataEXT *pCallbackData = (struct VkDebugUtilsMessengerCallbackDataEXT *) CallbackData;

    if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        int i =0;
    }

    printf("validation layer : %s\n", pCallbackData->pMessage);

    /*
    pthread_t threader;
    pthread_create(&threader, NULL, thread_f, (void *)pCallbackData->pMessage);
    //переводим в отсоединенное состояние
    pthread_detach(threader);*/

    return VK_FALSE;

}

void populateDebugMessengerCreateInfo(EdDebugUtilsMessengerCreateInfoEXT* createInfo) {
    createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = (void *)debugCallback;
}

void setupDebugMessenger(){

    TWindow *window = (TWindow *)engine.window;

    if(!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT debugInfo;
    memset(&debugInfo, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));

    populateDebugMessengerCreateInfo((EdDebugUtilsMessengerCreateInfoEXT *) &debugInfo);
    if(CreateDebugUtilsMessengerEXT(window->instance, (const EdDebugUtilsMessengerCreateInfoEXT *)&debugInfo, NULL, &engine.debugMessenger) != VK_SUCCESS)
    {
        printf("failed create debug messnger");
        exit(1);
    }

}
