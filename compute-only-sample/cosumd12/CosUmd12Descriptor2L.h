#pragma once

#include "CosUmd12.h"

class CosUmd12Descriptor
{
public:

    CosUmd12Descriptor(const D3D12DDI_CONSTANT_BUFFER_VIEW_DESC * pDesc)
    {
        m_type = COS_CBV;
        m_cbv = *pDesc;
    }

    CosUmd12Descriptor(const D3D12DDIARG_CREATE_UNORDERED_ACCESS_VIEW_0002 * pDesc)
    {
        m_type = COS_UAV;
        m_uav = *pDesc;
    }

#if GPUVA

    void WriteHWDescriptor(
        GpuHWDescriptor *   pHwDescriptor) const;

#endif

    void WriteHWDescriptor(
        CosUmd12CommandBuffer * pCurCommandBuffer,
        UINT hwDescriptorOffset,
        D3DDDI_PATCHLOCATIONLIST * &pPatchLocations) const;

private:
    friend class CosUmd12RootSignature;
    friend class CosUmd12CommandList;

    GpuHwDescriptorType m_type;
    union
    {
        D3D12DDI_CONSTANT_BUFFER_VIEW_DESC m_cbv;
        D3D12DDIARG_CREATE_SHADER_RESOURCE_VIEW_0002 m_srv;
        D3D12DDIARG_CREATE_UNORDERED_ACCESS_VIEW_0002 m_uav;
    };
};

