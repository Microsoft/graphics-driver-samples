////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Compute Command List implementation
//
// Filled up Command Buffers are kept in a m_filledCommandBuffers and submitted to kernel runtime one by one
// 
// The maximal number of Command Buffers is limited by COS_MAX_NUM_COMMAND_BUFFERS
//
// SW and HW commands are kept in separate command buffers so that KMD can emulate or submit to HW directly
//
// Copyright (C) Microsoft Corporation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CosUmd12.h"


HRESULT CosUmd12CommandList::StandUp()
{
    if (IsComputeType())
    {
        m_pDevice->m_pUMCallbacks->pfnSetCommandListDDITableCb(m_rtCommandList, m_pDevice->m_pAdapter->m_hRTTable[CosUmd12Adapter::TableType::Compute]);
    }
    else
    {
        m_pDevice->m_pUMCallbacks->pfnSetCommandListDDITableCb(m_rtCommandList, m_pDevice->m_pAdapter->m_hRTTable[CosUmd12Adapter::TableType::Render]);
    }

    //
    // m_pCommandPool is set with the first Reset()
    //

    return S_OK;
}

void
CosUmd12CommandList::Close()
{
    if (m_pCurCommandBuffer->IsCommandBufferEmpty())
    {
        return;
    }

    m_filledCommandBuffers[m_numFilledCommandBuffers++] = m_pCurCommandBuffer;

    m_pCurCommandBuffer = NULL;
}

void 
CosUmd12CommandList::Reset(
    const D3D12DDIARG_RESETCOMMANDLIST_0040 * pReset)
{
    //
    // Release the command buffers back to the Command Pool
    //

    if (m_pCommandPool)
    {
        for (UINT i = 0; i < m_numFilledCommandBuffers; i++)
        {
            m_pCommandPool->ReleaseCommandBuffer(m_filledCommandBuffers[i]);
        }

        if (m_pCurCommandBuffer)
        {
            m_pCommandPool->ReleaseCommandBuffer(m_pCurCommandBuffer);
        }

        m_numFilledCommandBuffers = 0;
        m_pCurCommandBuffer = 0;
    }

    m_pCommandPool = NULL;

    CosUmd12CommandRecorder * pCommandRecorder = CosUmd12CommandRecorder::CastFrom(pReset->hDrvCommandRecorder);

    m_pCommandPool = pCommandRecorder->GetCommandPool();

    m_pCurCommandBuffer = m_pCommandPool->AcquireCommandBuffer(m_args.QueueFlags);

    if (NULL == m_pCurCommandBuffer)
    {
        m_pDevice->m_pUMCallbacks->pfnSetErrorCb(m_pDevice->m_hRTDevice, E_OUTOFMEMORY);
    }
}

void
CosUmd12CommandList::Execute(CosUmd12CommandQueue * pCommandQueue)
{
    HRESULT hr;

    for (UINT i = 0; i < m_numFilledCommandBuffers; i++)
    {
        hr = m_filledCommandBuffers[i]->Execute(pCommandQueue);
    }
}

void
CosUmd12CommandList::ResourceCopy(
    D3D12DDI_HRESOURCE DstResource,
    D3D12DDI_HRESOURCE SrcResource)
{
    CosUmd12Resource *  pDstResource = CosUmd12Resource::CastFrom(DstResource);
    CosUmd12Resource *  pSrcResource = CosUmd12Resource::CastFrom(SrcResource);

    BYTE *  pCommandBuffer;
    UINT    curCommandOffset;
    D3DDDI_PATCHLOCATIONLIST *  pPatchLocationList;

    GpuCommand * command;

    ReserveCommandBufferSpace(
        true,                           // SW command
        sizeof(*command),
        &pCommandBuffer,
        2,
        2,
        &curCommandOffset,
        &pPatchLocationList);
    if (NULL == pCommandBuffer)
    {
        return;
    }

    assert(pPatchLocationList != NULL);

    command = reinterpret_cast<GpuCommand *>(pCommandBuffer);

    command->m_commandId = GpuCommandId::ResourceCopy;
    command->m_resourceCopy.m_srcGpuAddress.QuadPart = 0;
    command->m_resourceCopy.m_dstGpuAddress.QuadPart = 0;
    command->m_resourceCopy.m_sizeBytes = pDstResource->GetDataSize();

    UINT dstAllocIndex = m_pCurCommandBuffer->UseResource(pDstResource, true);
    UINT srcAllocIndex = m_pCurCommandBuffer->UseResource(pSrcResource, false);

    m_pCurCommandBuffer->SetPatchLocation(
                            pPatchLocationList,
                            dstAllocIndex,
                            curCommandOffset + offsetof(GpuCommand, m_resourceCopy.m_dstGpuAddress),
                            pDstResource->GetHeapOffset());
    m_pCurCommandBuffer->SetPatchLocation(
                            pPatchLocationList,
                            srcAllocIndex,
                            curCommandOffset + offsetof(GpuCommand, m_resourceCopy.m_srcGpuAddress),
                            pSrcResource->GetHeapOffset());

    m_pCurCommandBuffer->CommitCommandBufferSpace(sizeof(*command), 2);
}

void
CosUmd12CommandList::ReserveCommandBufferSpace(
    bool                        bSwCommand,
    UINT                        commandSize,
    BYTE **                     ppCommandBuffer,
    UINT                        allocationListSize,
    UINT                        patchLocationSize,
    UINT *                      pCurCommandOffset,
    D3DDDI_PATCHLOCATIONLIST ** ppPatchLocationList)
{
    m_pCurCommandBuffer->ReserveCommandBufferSpace(
                            bSwCommand,
                            commandSize,
                            ppCommandBuffer,
                            allocationListSize,
                            patchLocationSize,
                            pCurCommandOffset,
                            ppPatchLocationList);

    //
    // The current command buffer has enough space for the new command
    //

    if (*ppCommandBuffer)
    {
        return;
    }

    //
    // New command buffer need to be allocated
    //

    if (m_numFilledCommandBuffers == (COS_MAX_NUM_COMMAND_BUFFERS - 1))
    {
        m_pDevice->m_pUMCallbacks->pfnSetErrorCb(m_pDevice->m_hRTDevice, D3DDDIERR_DEVICEREMOVED);

        return;
    }

    m_filledCommandBuffers[m_numFilledCommandBuffers++] = m_pCurCommandBuffer;

    m_pCurCommandBuffer = m_pCommandPool->AcquireCommandBuffer(m_args.QueueFlags);

    m_pCurCommandBuffer->ReserveCommandBufferSpace(
                            bSwCommand,
                            commandSize,
                            ppCommandBuffer,
                            allocationListSize,
                            patchLocationSize,
                            pCurCommandOffset,
                            ppPatchLocationList);
}

void CosUmd12CommandList::SetDescriptorHeaps(
    UINT numDescriptorHeaps,
    D3D12DDI_HDESCRIPTORHEAP * pDescriptorHeaps)
{
    for (UINT i = 0; i < numDescriptorHeaps; i++)
    {
        ASSERT(pDescriptorHeaps[i].pDrvPrivate);

        CosUmd12DescriptorHeap *pDescriptorHeap = CosUmd12DescriptorHeap::CastFrom(pDescriptorHeaps[i]);

        m_pDescriptorHeaps[pDescriptorHeap->GetHeapType()] = pDescriptorHeap;
    }
}

void
CosUmd12CommandList::SetRootDescriptorTable(
    UINT rootParameterIndex,
    D3D12DDI_GPU_DESCRIPTOR_HANDLE baseDescriptor)
{
    CosUmd12RootSignature * pRootSignature = CosUmd12RootSignature::CastFrom(m_pPipelineState->m_args.hRootSignature);

    pRootSignature->SetRootDescriptorTable(
                        m_rootValues,
                        m_pDescriptorHeaps,
                        rootParameterIndex,
                        baseDescriptor);
}

void
CosUmd12CommandList::SetRoot32BitConstants(
    UINT rootParameterIndex,
    UINT num32BitValuesToSet,
    const void* pSrcData,
    UINT destOffsetIn32BitValues)
{
    CosUmd12RootSignature * pRootSignature = CosUmd12RootSignature::CastFrom(m_pPipelineState->m_args.hRootSignature);

    pRootSignature->SetRoot32BitConstants(m_rootValues, rootParameterIndex, num32BitValuesToSet, pSrcData, destOffsetIn32BitValues);
}

void
CosUmd12CommandList::SetRootView(
    UINT rootParameterIndex,
    D3D12DDI_GPU_VIRTUAL_ADDRESS bufferLocation)
{
    CosUmd12RootSignature * pRootSignature = CosUmd12RootSignature::CastFrom(m_pPipelineState->m_args.hRootSignature);

    pRootSignature->SetRootView(m_rootValues, rootParameterIndex, bufferLocation);
}

void
CosUmd12CommandList::Dispatch(
    UINT ThreadGroupCountX,
    UINT ThreadGroupCountY,
    UINT ThreadGroupCountZ)
{
    CosUmd12RootSignature * pRootSignature = CosUmd12RootSignature::CastFrom(m_pPipelineState->m_args.hRootSignature);
    CosUmd12Shader * pComputeShader = CosUmd12Shader::CastFrom(m_pPipelineState->m_args.hComputeShader);
    UINT commandSize, hwRootSignatureSetCommandSize;
    UINT numPatchLocations;
    BYTE * pCommandBuf;
    UINT curCommandOffset;
    D3DDDI_PATCHLOCATIONLIST * pPatchLocationList;

    //
    // State setup and Dispatch command have to be in the same command buffer, so the space for them
    // in the command buffer is reserved at once
    //

    hwRootSignatureSetCommandSize = commandSize = pRootSignature->GetHwRootSignatureSize(&numPatchLocations);
    commandSize += sizeof(GpuHwComputeShaderDisptch) + pComputeShader->m_args.pShaderCode[1] * sizeof(UINT);

    ReserveCommandBufferSpace(
        false,                          // HW command
        commandSize,
        (BYTE **)&pCommandBuf,
        numPatchLocations,
        numPatchLocations,
        &curCommandOffset,
        &pPatchLocationList);
    if (NULL == pCommandBuf)
    {
        return;
    }

    //
    // Write the Root Signature into the command list
    //

    pRootSignature->WriteHWRootSignature(m_rootValues, m_pDescriptorHeaps, m_pCurCommandBuffer, pCommandBuf, curCommandOffset, pPatchLocationList);

    //
    // Write Dispatch command into the Command List
    //

    GpuHwComputeShaderDisptch * pCSDispath = (GpuHwComputeShaderDisptch *)(pCommandBuf + hwRootSignatureSetCommandSize);

    pCSDispath->m_commandId = ComputeShaderDispatch;
    pCSDispath->m_commandSize = commandSize;

    //
    // TODO: Retrieve num threads per group from shader
    //

    pCSDispath->m_numThreadPerGroup = 4;
    pCSDispath->m_threadGroupCountX = ThreadGroupCountX;
    pCSDispath->m_threadGroupCountY = ThreadGroupCountY;
    pCSDispath->m_threadGroupCountZ = ThreadGroupCountZ;

    memcpy(pCSDispath->m_ShaderHash, pComputeShader->m_args.ShaderCodeHash.Hash, sizeof(pCSDispath->m_ShaderHash));
    memcpy(pCSDispath + 1, pComputeShader->m_args.pShaderCode, pComputeShader->m_args.pShaderCode[1] * sizeof(UINT));

    //
    // Commit both commands into the command buffer
    //

    m_pCurCommandBuffer->CommitCommandBufferSpace(commandSize, numPatchLocations);
}
