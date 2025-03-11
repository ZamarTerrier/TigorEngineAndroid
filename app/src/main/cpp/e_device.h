//
// Created by prg6 on 11.03.2025.
//

#ifndef TESTANDROID_E_DEVICE_H
#define TESTANDROID_E_DEVICE_H


#include "engine_includes.h"

#include "e_device_variables.h"
#include "swapchain.h"
#include "e_tools.h"

typedef struct{

    void* e_physicalDevice;
    void* e_device;

    void* graphicsQueue;
    void* presentQueue;

    void* commandPool;
    void** commandBuffers;

} TDevice;

bool checkDeviceExtensionSupport(void* device);

bool isDeviceSuitable(void* device);

void pickPhysicalDevice();
void createLogicalDevice();

#endif //TESTANDROID_E_DEVICE_H
