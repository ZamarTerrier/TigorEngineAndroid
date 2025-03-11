//
// Created by prg6 on 11.03.2025.
//

#ifndef TESTANDROID_E_DEVICE_VARIABLES_H
#define TESTANDROID_E_DEVICE_VARIABLES_H

#include "engine_includes.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef struct{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
} QueueFamilyIndices;

typedef struct EDSurfaceFormatKHR {
    uint32_t           format;
    uint32_t           colorSpace;
} EDSurfaceFormatKHR;

typedef struct{
    EISurfaceCapabilitiesKHR capabilities;
    EDSurfaceFormatKHR* formats;
    uint32_t sizeFormats;
    uint32_t* presentModes;
    uint32_t sizeModes;
} SwapChainSupportDetails;

#ifdef __cplusplus
}
#endif

#endif //TESTANDROID_E_DEVICE_VARIABLES_H
