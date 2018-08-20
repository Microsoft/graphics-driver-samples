#include "CosUmd12.h"

void APIENTRY CosUmd12CommandList_Ddi_CloseCommandList(
    D3D12DDI_HCOMMANDLIST CommandList)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ResetCommandList_0040(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ const D3D12DDIARG_RESETCOMMANDLIST_0040* pReset)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_DrawInstanced(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT VertexCountPerInstance,
    UINT InstanceCount,
    UINT StartVertexLocation,
    UINT StartInstanceLocation)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_DrawIndexedInstanced(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT IndexCountPerInstance,
    UINT InstanceCount,
    UINT StartIndexLocation,
    INT BaseVertexLocation,
    UINT StartInstanceLocation)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_Dispatch(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT ThreadGroupCountX,
    UINT ThreadGroupCountY,
    UINT ThreadGroupCountZ)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ClearUnorderedAccessViewUint_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_GPU_DESCRIPTOR_HANDLE ViewGPUHandleInCurrentHeap,
    D3D12DDI_CPU_DESCRIPTOR_HANDLE ViewCPUHandle,
    D3D12DDI_HRESOURCE Resource,
    const UINT Values[4],
    UINT NumRects,
    _In_reads_(NumRects) const D3D12DDI_RECT* pRects)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ClearUnorderedAccessViewFloat_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_GPU_DESCRIPTOR_HANDLE ViewGPUHandleInCurrentHeap,
    D3D12DDI_CPU_DESCRIPTOR_HANDLE ViewCPUHandle,
    D3D12DDI_HRESOURCE Resource,
    const FLOAT Values[4],
    UINT NumRects,
    _In_reads_(NumRects) const D3D12DDI_RECT* pRects)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ClearRenderTargetView_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_CPU_DESCRIPTOR_HANDLE ViewCPUHandle,
    const FLOAT ColorRGBA[4],
    UINT NumRects,
    _In_reads_(NumRects) const D3D12DDI_RECT* pRects)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ClearDepthStencilView_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_CPU_DESCRIPTOR_HANDLE ViewCPUHandle,
    UINT Flags,
    FLOAT Depth,
    UINT8 Stencil,
    UINT NumRects,
    _In_reads_(NumRects) const D3D12DDI_RECT* pRects)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_DiscardResource_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HRESOURCE Resource,
    _In_opt_ const D3D12DDIARG_DISCARD_RESOURCE_0003* pDiscard)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_CopyTextureRegion_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ const D3D12DDIARG_BUFFER_PLACEMENT* pBufferPlacementDst,
    D3D12DDIARG_PLACED_RESOURCE PlacedResourceDst,
    UINT DstX,
    UINT DstY,
    UINT DstZ,
    _In_ const D3D12DDIARG_BUFFER_PLACEMENT* pBufferPlacementSrc,
    D3D12DDIARG_PLACED_RESOURCE PlacedResourceSrc,
    _In_opt_ const D3D12DDI_BOX* pSrcBox)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ResourceCopy(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HRESOURCE DstResource,
    D3D12DDI_HRESOURCE SrcResource)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_CopyTiles(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HRESOURCE Resource,
    _In_ const D3D12DDI_TILED_RESOURCE_COORDINATE* pRegionStartCoord,
    _In_ const D3D12DDI_TILE_REGION_SIZE* pRegionSize,
    D3D12DDI_HRESOURCE Buffer, // buffer
    UINT64 BufferStartOffsetInBytes,
    D3D12DDI_TILE_COPY_FLAGS CopyFlags)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_CopyBufferRegion_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDIARG_BUFFER_PLACEMENT Dst,
    D3D12DDIARG_BUFFER_PLACEMENT Src,
    UINT64 SrcBytes)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ResourceResolveSubresource(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HRESOURCE DstResource,
    UINT DstSubresource,
    D3D12DDI_HRESOURCE SrcResource,
    UINT SrcSubresource,
    DXGI_FORMAT Format)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ExecuteBundle(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HCOMMANDLIST Bundle)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ExecuteIndirect(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HCOMMANDSIGNATURE CommandSignature,
    UINT MaxCommandCount,
    D3D12DDIARG_BUFFER_PLACEMENT ArgumentBuffer,
    D3D12DDIARG_BUFFER_PLACEMENT CountBuffer)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ResourceBarrier_0022(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT Count,
    _In_reads_(Count) const D3D12DDIARG_RESOURCE_BARRIER_0022* pResourceBarrierDesc)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_Blt(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ const D3D12DDIARG_BLT*)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_Present_0051(
    D3D12DDI_HCOMMANDLIST CommandList, 
    D3D12DDI_HCOMMANDQUEUE CommandQueue, 
    _In_ const D3D12DDIARG_PRESENT_0001* pDesc, 
    _Out_ D3D12DDI_PRESENT_0051* pPresent,
    _Out_opt_ D3D12DDI_PRESENT_CONTEXTS_0051*,
    _Out_opt_ D3D12DDI_PRESENT_HWQUEUES_0051*)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_BeginQuery_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HQUERYHEAP QueryHeap,
    D3D12DDI_QUERY_TYPE QueryType,
    UINT Index)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_EndQuery_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HQUERYHEAP QueryHeap,
    D3D12DDI_QUERY_TYPE QueryType,
    UINT Index)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ResolveQueryData(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HQUERYHEAP QueryHeap,
    D3D12DDI_QUERY_TYPE QueryType,
    UINT StartElement,
    UINT ElementCount,
    D3D12DDI_HRESOURCE DestinationBuffer,
    UINT64 DestinationOffset)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetPredication(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HRESOURCE Buffer,
    UINT64 AlignedBufferOffset,
    D3D12DDI_PREDICATION_OP Operation)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_IaSetTopology_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_PRIMITIVE_TOPOLOGY Topology)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_RsSetViewports_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT Count,
    _In_reads_(Count) const D3D12DDI_VIEWPORT* pViewports)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_RsSetScissorRects_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT Count,
    _In_reads_(Count) const D3D12DDI_RECT* pRects)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_OmSetBlendFactor(
    D3D12DDI_HCOMMANDLIST CommandList,
    const FLOAT BlendFactor[4])
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_OmSetStencilRef(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT StencilRef)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetPipelineState(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HPIPELINESTATE PipelineState)
{
    CosUmd12CommandList * pCommandList = CosUmd12CommandList::CastFrom(CommandList);
    ASSERT(pCommandList->IsRenderType());

    CosUmd12PipelineState * pPipelineState = CosUmd12PipelineState::CastFrom(PipelineState);

    pCommandList->SetPipelineState(pPipelineState);
}

void APIENTRY CosUmd12CommandList_Ddi_SetDescriptorHeaps_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ UINT NumDescriptorHeaps,
    _In_reads_(NumDescriptorHeaps) D3D12DDI_HDESCRIPTORHEAP* pDescriptorHeaps)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetComputeRootSignature(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ D3D12DDI_HROOTSIGNATURE RootSignature)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetGraphicsRootSignature(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ D3D12DDI_HROOTSIGNATURE RootSignature)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetComputeRootDescriptorTable(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ UINT RootParameterIndex,
    _In_ D3D12DDI_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetGraphicsRootDescriptorTable(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ UINT RootParameterIndex,
    _In_ D3D12DDI_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetComputeRoot32BitConstant(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT RootParameterIndex,
    UINT SrcData,
    UINT DestOffsetIn32BitValues)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetGraphicsRoot32BitConstant(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT RootParameterIndex,
    UINT SrcData,
    UINT DestOffsetIn32BitValues)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetComputeRoot32BitConstants_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT RootParameterIndex,
    UINT Num32BitValuesToSet,
    const void* pSrcData,
    UINT DestOffsetIn32BitValues)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetGraphicsRoot32BitConstants_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT RootParameterIndex,
    UINT Num32BitValuesToSet,
    const void* pSrcData,
    UINT DestOffsetIn32BitValues)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetComputeRootConstantBufferView(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT RootParameterIndex,
    _In_ D3D12DDI_GPU_VIRTUAL_ADDRESS BufferLocation)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetGraphicsRootConstantBufferView(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT RootParameterIndex,
    _In_ D3D12DDI_GPU_VIRTUAL_ADDRESS BufferLocation)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetComputeRootShaderResourceView(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT RootParameterIndex,
    _In_ D3D12DDI_GPU_VIRTUAL_ADDRESS BufferLocation)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetGraphicsRootShaderResourceView(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT RootParameterIndex,
    _In_ D3D12DDI_GPU_VIRTUAL_ADDRESS BufferLocation)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetComputeRootUnorderedAccessView(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT RootParameterIndex,
    _In_ D3D12DDI_GPU_VIRTUAL_ADDRESS BufferLocation)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetGraphicsRootUnorderedAccessView(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT RootParameterIndex,
    _In_ D3D12DDI_GPU_VIRTUAL_ADDRESS BufferLocation)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_IaSetIndexBuffer(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ const D3D12DDI_INDEX_BUFFER_VIEW* pDesc)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_IaSetVertexBuffers_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ UINT StartSlot,
    _In_ UINT NumViews,
    _In_reads_opt_(NumViews) const D3D12DDI_VERTEX_BUFFER_VIEW * pViews)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SoSetTargets_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ UINT StartSlot,
    _In_ UINT NumViews,
    _In_reads_opt_(NumViews) const D3D12DDI_STREAM_OUTPUT_BUFFER_VIEW* pViews)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_OmSetRenderTargets_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    _In_ UINT NumRenderTargetDescriptors,
    _In_ const D3D12DDI_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors,
    _In_ BOOL RTsSingleHandleToDescriptorRange,
    _In_opt_ const D3D12DDI_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetMarker(
    D3D12DDI_HCOMMANDLIST CommandList, 
    UINT64 Marker)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ClearRootArguments(
    D3D12DDI_HCOMMANDLIST CommandList)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_AtomicCopyBufferRegion_0003(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDIARG_BUFFER_PLACEMENT Dst,
    D3D12DDIARG_BUFFER_PLACEMENT Src,
    UINT64 SrcBytes)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_OmSetDepthBounds_0025(
    D3D12DDI_HCOMMANDLIST CommandList,
    FLOAT Min,
    FLOAT Max)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetSamplePositions_0027(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT NumSamplesPerPixel,
    UINT NumPixels,
    __in_ecount(NumSamplesPerPixel*NumPixels) D3D12DDI_SAMPLE_POSITION* pSamplePositions)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_ResourceResolveSubresourceRegion(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HRESOURCE DstResource,
    UINT DstSubresource,
    UINT DstX,
    UINT DstY,
    D3D12DDI_HRESOURCE SrcResource,
    UINT SrcSubresource,
    __in_opt D3D12DDI_RECT* pSrcRect,
    DXGI_FORMAT Format,
    D3D12DDI_RESOLVE_MODE ResolveMode)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetProtectedResourceSession_0030(
    D3D12DDI_HCOMMANDLIST CommandList,
    D3D12DDI_HPROTECTEDRESOURCESESSION_0030 ProtectedSession)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_WriteBufferImmediate_0032(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT Count,
    _In_reads_(Count) CONST D3D12DDI_WRITEBUFFERIMMEDIATE_PARAMETER_0032 *pParams,
    _In_reads_opt_(Count) CONST D3D12DDI_WRITEBUFFERIMMEDIATE_MODE_0032 *pModes)
{
    STOP_IN_FUNCTION();
}

void APIENTRY CosUmd12CommandList_Ddi_SetViewInstanceMask_0033(
    D3D12DDI_HCOMMANDLIST CommandList,
    UINT Mask)
{
    STOP_IN_FUNCTION();
}

D3D12DDI_COMMAND_LIST_FUNCS_3D_0052 g_CosUmd12CommandList_Ddi_0052 =
{
    CosUmd12CommandList_Ddi_CloseCommandList,                          // pfnCloseCommandList
    CosUmd12CommandList_Ddi_ResetCommandList_0040,                     // pfnResetCommandList
    CosUmd12CommandList_Ddi_DrawInstanced,                                    // pfnDrawInstanced
    CosUmd12CommandList_Ddi_DrawIndexedInstanced,                             // pfnDrawIndexedInstanced
    CosUmd12CommandList_Ddi_Dispatch,                                  // pfnDispatch
    CosUmd12CommandList_Ddi_ClearUnorderedAccessViewUint_0003,         // pfnClearUnorderedAccessViewUint
    CosUmd12CommandList_Ddi_ClearUnorderedAccessViewFloat_0003,        // pfnClearUnorderedAccessViewFloat
    CosUmd12CommandList_Ddi_ClearRenderTargetView_0003,                // pfnClearRenderTargetView
    CosUmd12CommandList_Ddi_ClearDepthStencilView_0003,                // pfnClearDepthStencilView
    CosUmd12CommandList_Ddi_DiscardResource_0003,                      // pfnDiscardResource
    CosUmd12CommandList_Ddi_CopyTextureRegion_0003,                    // pfnCopyTextureRegion
    CosUmd12CommandList_Ddi_ResourceCopy,                              // pfnResourceCopy
    CosUmd12CommandList_Ddi_CopyTiles,                                 // pfnCopyTiles
    CosUmd12CommandList_Ddi_CopyBufferRegion_0003,                     // pfnCopyBufferRegion
    CosUmd12CommandList_Ddi_ResourceResolveSubresource,                // pfnResourceResolveSubresource
    CosUmd12CommandList_Ddi_ExecuteBundle,                             // pfnExecuteBundle
    CosUmd12CommandList_Ddi_ExecuteIndirect,                           // pfnExecuteIndirect
    CosUmd12CommandList_Ddi_ResourceBarrier_0022,                      // pfnResourceBarrier
    CosUmd12CommandList_Ddi_Blt,                                              // pfnBlt
    CosUmd12CommandList_Ddi_Present_0051,                              // pfnPresent
    CosUmd12CommandList_Ddi_BeginQuery_0003,                           // pfnBeginQuery
    CosUmd12CommandList_Ddi_EndQuery_0003,                             // pfnEndQuery
    CosUmd12CommandList_Ddi_ResolveQueryData,                          // pfnResolveQueryData
    CosUmd12CommandList_Ddi_SetPredication,                            // pfnSetPredication
    CosUmd12CommandList_Ddi_IaSetTopology_0003,                               // pfnIaSetTopology    
    CosUmd12CommandList_Ddi_RsSetViewports_0003,                       // pfnRsSetViewports
    CosUmd12CommandList_Ddi_RsSetScissorRects_0003,                    // pfnRsSetScissorRects
    CosUmd12CommandList_Ddi_OmSetBlendFactor,                                 // pfnOmSetBlendFactor
    CosUmd12CommandList_Ddi_OmSetStencilRef,                                  // pfnOmSetStencilRef
    CosUmd12CommandList_Ddi_SetPipelineState,                          // pfnSetPipelineState
    CosUmd12CommandList_Ddi_SetDescriptorHeaps_0003,                   // pfnSetDescriptorHeaps
    CosUmd12CommandList_Ddi_SetComputeRootSignature,                   // pfnSetComputeRootSignature
    CosUmd12CommandList_Ddi_SetGraphicsRootSignature,                  // pfnSetGraphicsRootSignature
    CosUmd12CommandList_Ddi_SetComputeRootDescriptorTable,             // pfnSetComputeRootDescriptorTable
    CosUmd12CommandList_Ddi_SetGraphicsRootDescriptorTable,                   // pfnSetGraphicsRootDescriptorTable
    CosUmd12CommandList_Ddi_SetComputeRoot32BitConstant,               // pfnSetComputeRoot32BitConstant
    CosUmd12CommandList_Ddi_SetGraphicsRoot32BitConstant,                     // pfnSetGraphicsRoot32BitConstant
    CosUmd12CommandList_Ddi_SetComputeRoot32BitConstants_0003,         // pfnSetComputeRoot32BitConstants
    CosUmd12CommandList_Ddi_SetGraphicsRoot32BitConstants_0003,               // pfnSetGraphicsRoot32BitConstants
    CosUmd12CommandList_Ddi_SetComputeRootConstantBufferView,          // pfnSetComputeRootConstantBufferView
    CosUmd12CommandList_Ddi_SetGraphicsRootConstantBufferView,                // pfnSetGraphicsRootConstantBufferView
    CosUmd12CommandList_Ddi_SetComputeRootShaderResourceView,          // pfnSetComputeRootShaderResourceView
    CosUmd12CommandList_Ddi_SetGraphicsRootShaderResourceView,                // pfnSetGraphicsRootShaderResourceView
    CosUmd12CommandList_Ddi_SetComputeRootUnorderedAccessView,         // pfnSetComputeRootUnorderedAccessView
    CosUmd12CommandList_Ddi_SetGraphicsRootUnorderedAccessView,               // pfnSetGraphicsRootUnorderedAccessView
    CosUmd12CommandList_Ddi_IaSetIndexBuffer,                                 // pfnIASetIndexBuffer
    CosUmd12CommandList_Ddi_IaSetVertexBuffers_0003,                          // pfnIASetVertexBuffers
    CosUmd12CommandList_Ddi_SoSetTargets_0003,                         // pfnSOSetTargets
    CosUmd12CommandList_Ddi_OmSetRenderTargets_0003,                   // pfnOMSetRenderTargets
    CosUmd12CommandList_Ddi_SetMarker,                                 // pfnSetMarker
    CosUmd12CommandList_Ddi_ClearRootArguments,                        // pfnClearRootArguments
    CosUmd12CommandList_Ddi_AtomicCopyBufferRegion_0003,               // pfnAtomicCopyBufferRegion
    CosUmd12CommandList_Ddi_OmSetDepthBounds_0025,                     // pfnOMSetDepthBounds
    CosUmd12CommandList_Ddi_SetSamplePositions_0027,                   // pfnSetSamplePositions
    CosUmd12CommandList_Ddi_ResourceResolveSubresourceRegion,          // pfnResourceResolveSubresourceRegion
    CosUmd12CommandList_Ddi_SetProtectedResourceSession_0030,          // pfnSetProtectedResourceSession
    CosUmd12CommandList_Ddi_WriteBufferImmediate_0032,                 // pfnWriteBufferImmediate
    CosUmd12CommandList_Ddi_SetViewInstanceMask_0033,                  // pfnSetViewInstanceMask
};