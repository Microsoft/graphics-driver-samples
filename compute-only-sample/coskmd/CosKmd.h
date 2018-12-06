#pragma once

//
//    Compute Only Sample (Cos) Kernel Mode Driver (Kmd) Header
//
//    Copyright (C) Microsoft.  All rights reserved.
//

#include <initguid.h>
#include <ntddk.h>
#include <ntintsafe.h>
#include <ntstrsafe.h>

#define ENABLE_DXGK_SAL

extern "C" {
#include <dispmprt.h>
#include <dxgiformat.h>
#include <WppRecorder.h>
} // extern "C"

#include "CosAllocation.h"
#include "CosContext.h"
#include "CosAdapter.h"

#define COSD_SEGMENT_VIDEO_MEMORY 1

// TODO: Clean up later
#define COSD_SEGMENT_APERTURE 2
#define COSD_SEGMENT_APERTURE_BASE_ADDRESS  0xC0000000

#define COSD_PAGING_BUFFER_SIZE     PAGE_SIZE

typedef struct _COSPRIVATEINFO2 : public _COSADAPTERINFO
{
    UINT             m_Dummy;
} COSPRIVATEINFO2;

const UINT C_COSD_DMAPRIVATEDATASIZE_KMD = 0x100;

typedef struct _COSUMDDMAPRIVATEDATA
{
    BYTE              m_KmdPrivateData[C_COSD_DMAPRIVATEDATASIZE_KMD];
    UINT              m_DmaPacketIndex;
    UINT              m_Dummy;
    UINT              m_DmaBufferSize;
} COSUMDDMAPRIVATEDATA;

typedef struct _COSDUMDMAPRIVATEDATA2 : public COSUMDDMAPRIVATEDATA
{
    UINT  m_Dummy;
} COSUMDDMAPRIVATEDATA2;

#define COSD_VERSION 2

const int C_COSD_GPU_ENGINE_COUNT = 1;

#if DBG

// #define BINNER_DBG  1

#endif

#if GPUVA

//
// The DDI_N indicates DDI call sequence during adapter initialization
//

#define GPUVA_INIT_DDI_1    1
#define GPUVA_INIT_DDI_2    1
#define GPUVA_INIT_DDI_3    1
#define GPUVA_INIT_DDI_4    1
#define GPUVA_INIT_DDI_5    1
#define GPUVA_INIT_DDI_6    1
#define GPUVA_INIT_DDI_7    1
#define GPUVA_INIT_DDI_8    1

//
// The current GPUVA implementation declares 1 aperture segment
// so the implicit system memory segment (created by OS) is 2.
//

#define IMPLICIT_SYSTEM_MEMORY_SEGMENT_ID   2

#endif

// #define GPU_CACHE_WORKAROUND 1

