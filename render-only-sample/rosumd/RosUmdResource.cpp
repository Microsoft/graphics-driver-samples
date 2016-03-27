///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Resource implementation
//
// Copyright (C) Microsoft Corporation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "precomp.h"

#include "RosUmdLogging.h"
#include "RosUmdResource.tmh"

#include "RosUmdDevice.h"
#include "RosUmdResource.h"
#include "RosUmdDebug.h"

#include "RosContext.h"

#include "Vc4Hw.h"

RosUmdResource::RosUmdResource() :
    m_signature(_SIGNATURE::CONSTRUCTED),
    m_hKMAllocation(NULL)
{
 
}

RosUmdResource::~RosUmdResource()
{
    assert(
        (m_signature == _SIGNATURE::CONSTRUCTED) ||
        (m_signature == _SIGNATURE::INITIALIZED));
    // do nothing
}

void
RosUmdResource::Standup(
    RosUmdDevice *pUmdDevice,
    const D3D11DDIARG_CREATERESOURCE* pCreateResource,
    D3D10DDI_HRTRESOURCE hRTResource)
{
    UNREFERENCED_PARAMETER(pUmdDevice);
    
    assert(m_signature == _SIGNATURE::CONSTRUCTED);

    m_resourceDimension = pCreateResource->ResourceDimension;
    m_mip0Info = *pCreateResource->pMipInfoList;
    m_usage = pCreateResource->Usage;
    m_bindFlags = pCreateResource->BindFlags;
    m_mapFlags = pCreateResource->MapFlags;
    m_miscFlags = pCreateResource->MiscFlags;
    m_format = pCreateResource->Format;
    m_sampleDesc = pCreateResource->SampleDesc;
    m_mipLevels = pCreateResource->MipLevels;
    m_arraySize = pCreateResource->ArraySize;

    if (pCreateResource->pPrimaryDesc)
    {
        assert(
            (pCreateResource->MiscFlags & D3DWDDM2_0DDI_RESOURCE_MISC_DISPLAYABLE_SURFACE) &&
            (pCreateResource->BindFlags & D3D10_DDI_BIND_PRESENT) &&
            (pCreateResource->pPrimaryDesc->ModeDesc.Width != 0));
        
        m_isPrimary = true;
        m_primaryDesc = *pCreateResource->pPrimaryDesc;
    }
    else
    {
        m_isPrimary = false;
        ZeroMemory(&m_primaryDesc, sizeof(m_primaryDesc));
    }

    memset(&m_TileInfo, 0, sizeof(m_TileInfo));

    CalculateMemoryLayout();

    m_hRTResource = hRTResource;

    // Zero out internal state
    m_hKMResource = 0;
    m_hKMAllocation = 0;

    // Mark that the resource is not referenced by a command buffer (.i.e. null fence value)
    m_mostRecentFence = RosUmdCommandBuffer::s_nullFence;

    m_allocationListIndex = 0;

    m_pData = nullptr;
    m_pSysMemCopy = nullptr;
    m_signature = _SIGNATURE::INITIALIZED;
}

void RosUmdResource::InitSharedResourceFromExistingAllocation (
    const RosAllocationExchange* ExistingAllocationPtr,
    D3D10DDI_HKMRESOURCE hKMResource,
    D3DKMT_HANDLE hKMAllocation,        // can this be a D3D10DDI_HKMALLOCATION?
    D3D10DDI_HRTRESOURCE hRTResource
    )
{
    assert(m_signature == _SIGNATURE::CONSTRUCTED);
    
    ROS_LOG_TRACE(
        "Opening existing resource. "
        "(ExistingAllocationPtr->m_hwWidth/HeightPixels = %u,%u  "
        "ExistingAllocationPtr->m_hwPitchBytes = %u, "
        "ExistingAllocationPtr->m_hwSizeBytes = %u, "
        "ExistingAllocationPtr->m_isPrimary = %d, "
        "hRTResource = 0x%p, "
        "hKMResource= 0x%x, "
        "hKMAllocation = 0x%x)",
        ExistingAllocationPtr->m_hwWidthPixels,
        ExistingAllocationPtr->m_hwHeightPixels,
        ExistingAllocationPtr->m_hwPitchBytes,
        ExistingAllocationPtr->m_hwSizeBytes,
        ExistingAllocationPtr->m_isPrimary,
        hRTResource.handle,
        hKMResource.handle,
        hKMAllocation);
    
    // copy members from the existing allocation into this object
    RosAllocationExchange* basePtr = this;
    *basePtr = *ExistingAllocationPtr;

    // HW specific information calculated based on the fields above
    CalculateMemoryLayout();
    
    NT_ASSERT(
        (m_hwLayout == ExistingAllocationPtr->m_hwLayout) &&
        (m_hwWidthPixels == ExistingAllocationPtr->m_hwWidthPixels) &&
        (m_hwHeightPixels == ExistingAllocationPtr->m_hwHeightPixels) &&
        (m_hwFormat == ExistingAllocationPtr->m_hwFormat) &&
        (m_hwPitchBytes == ExistingAllocationPtr->m_hwPitchBytes) &&
        (m_hwSizeBytes == ExistingAllocationPtr->m_hwSizeBytes));
    
    m_hRTResource = hRTResource;
    m_hKMResource = hKMResource.handle;
    m_hKMAllocation = hKMAllocation;

    m_mostRecentFence = RosUmdCommandBuffer::s_nullFence;
    m_allocationListIndex = 0;

    m_pData = nullptr;
    m_pSysMemCopy = nullptr;
    
    m_signature = _SIGNATURE::INITIALIZED;
}

void
RosUmdResource::Teardown(void)
{
    m_signature = _SIGNATURE::CONSTRUCTED;
    // TODO[indyz]: Implement
}

void
RosUmdResource::ConstantBufferUpdateSubresourceUP(
    UINT DstSubresource,
    _In_opt_ const D3D10_DDI_BOX *pDstBox,
    _In_ const VOID *pSysMemUP,
    UINT RowPitch,
    UINT DepthPitch,
    UINT CopyFlags)
{
    assert(DstSubresource == 0);
    assert(pSysMemUP);

    assert(m_bindFlags & D3D10_DDI_BIND_CONSTANT_BUFFER); // must be constant buffer
    assert(m_resourceDimension == D3D10DDIRESOURCE_BUFFER);

    BYTE *pSysMemCopy = m_pSysMemCopy;
    UINT BytesToCopy = RowPitch;
    if (pDstBox)
    {
        if (pDstBox->left < 0 ||
            pDstBox->left > (INT)m_hwSizeBytes ||
            pDstBox->left > pDstBox->right ||
            pDstBox->right > (INT)m_hwSizeBytes)
        {
            return; // box is outside of buffer size. Nothing to copy.
        }

        pSysMemCopy += pDstBox->left;
        BytesToCopy = (pDstBox->right - pDstBox->left);
    }
    else if (BytesToCopy == 0)
    {
        BytesToCopy = m_hwSizeBytes; // copy whole.
    }
    else
    {
        BytesToCopy = min(BytesToCopy, m_hwSizeBytes);
    }

    CopyMemory(pSysMemCopy, pSysMemUP, BytesToCopy);

    return;

    DepthPitch;
    CopyFlags;
}

void
RosUmdResource::Map(
    RosUmdDevice *pUmdDevice,
    UINT subResource,
    D3D10_DDI_MAP mapType,
    UINT mapFlags,
    D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubRes)
{
    assert(m_mipLevels <= 1);
    assert(m_arraySize == 1);

    UNREFERENCED_PARAMETER(subResource);

    //
    // Constant data is copied into command buffer, so there is no need for flushing
    //

    if (m_bindFlags & D3D10_DDI_BIND_CONSTANT_BUFFER)
    {
        pMappedSubRes->pData = m_pSysMemCopy;

        pMappedSubRes->RowPitch = m_hwPitchBytes;
        pMappedSubRes->DepthPitch = (UINT)m_hwSizeBytes;

        return;
    }

    pUmdDevice->m_commandBuffer.FlushIfMatching(m_mostRecentFence);

    D3DDDICB_LOCK lock;
    memset(&lock, 0, sizeof(lock));

    lock.hAllocation = m_hKMAllocation;

    //
    // TODO[indyz]: Consider how to optimize D3D10_DDI_MAP_WRITE_NOOVERWRITE
    //
    //    D3DDDICB_LOCKFLAGS::IgnoreSync and IgnoreReadSync are used for
    //    D3D10_DDI_MAP_WRITE_NOOVERWRITE optimization and are only allowed
    //    for allocations that can resides in aperture segment.
    //
    //    Currently ROS driver puts all allocations in local video memory.
    //

    SetLockFlags(mapType, mapFlags, &lock.Flags);

    pUmdDevice->Lock(&lock);

    if (lock.Flags.Discard)
    {
        assert(m_hKMAllocation != lock.hAllocation);

        m_hKMAllocation = lock.hAllocation;

        if (pUmdDevice->m_commandBuffer.IsResourceUsed(this))
        {
            //
            // Indicate that the new allocation instance of the resource
            // is not used in the current command batch.
            //

            m_mostRecentFence -= 1;
        }
    }

    pMappedSubRes->pData = lock.pData;
    m_pData = (BYTE*)lock.pData;

    pMappedSubRes->RowPitch = m_hwPitchBytes;
    pMappedSubRes->DepthPitch = (UINT)m_hwSizeBytes;
}

void
RosUmdResource::Unmap(
    RosUmdDevice *pUmdDevice,
    UINT subResource)
{
    UNREFERENCED_PARAMETER(subResource);

    if (m_bindFlags & D3D10_DDI_BIND_CONSTANT_BUFFER)
    {
        return;
    }

    m_pData = NULL;

    D3DDDICB_UNLOCK unlock;
    memset(&unlock, 0, sizeof(unlock));

    unlock.NumAllocations = 1;
    unlock.phAllocations = &m_hKMAllocation;

    pUmdDevice->Unlock(&unlock);
}

VC4TileInfo RosUmdResource::FillTileInfo(UINT bpp)
{
    // Provide detailed information about tile.
    // Partial information about 4kB tiles, 1kB sub-tiles and micro-tiles for
    // given bpp is precalculated.
    // Values are used i.e. during converting bitmap to tiled texture

    VC4TileInfo info = { 0 };

    if (bpp == 8)
    {
        info.VC4_1kBSubTileWidthPixels        = VC4_1KB_SUB_TILE_WIDTH_8BPP;
        info.VC4_1kBSubTileHeightPixels       = VC4_1KB_SUB_TILE_HEIGHT_8BPP;
        info.VC4_MicroTileWidthBytes          = VC4_MICRO_TILE_WIDTH_BYTES_8BPP;
        info.vC4_MicroTileHeight              = VC4_MICRO_TILE_HEIGHT_8BPP;
    }
    else if (bpp == 16)
    {
        info.VC4_1kBSubTileWidthPixels        = VC4_1KB_SUB_TILE_WIDTH_16BPP;
        info.VC4_1kBSubTileHeightPixels       = VC4_1KB_SUB_TILE_HEIGHT_16BPP;
        info.VC4_MicroTileWidthBytes          = VC4_MICRO_TILE_WIDTH_BYTES_16BPP;
        info.vC4_MicroTileHeight              = VC4_MICRO_TILE_HEIGHT_16BPP;
    }
    else if (bpp == 32)
    {
        info.VC4_1kBSubTileWidthPixels        = VC4_1KB_SUB_TILE_WIDTH_32BPP;
        info.VC4_1kBSubTileHeightPixels       = VC4_1KB_SUB_TILE_HEIGHT_32BPP;
        info.VC4_MicroTileWidthBytes          = VC4_MICRO_TILE_WIDTH_BYTES_32BPP;
        info.vC4_MicroTileHeight              = VC4_MICRO_TILE_HEIGHT_32BPP;
    }
    else
    {
        // We expect 8, 16 or 32 bpp only
        assert(false);
    }

    // Calculate sub-tile width in bytes
    info.VC4_1kBSubTileWidthBytes = info.VC4_1kBSubTileWidthPixels * (bpp / 8);
    
    // 4kB tile consists of four 1kB sub-tiles
    info.VC4_4kBTileWidthPixels = info.VC4_1kBSubTileWidthPixels * 2;
    info.VC4_4kBTileHeightPixels = info.VC4_1kBSubTileHeightPixels * 2;
    info.VC4_4kBTileWidthBytes = info.VC4_1kBSubTileWidthBytes * 2;

    return info;
}

void 
RosUmdResource::MapDxgiFormatToInternalFormats(DXGI_FORMAT format, _Out_ UINT &bpp, _Out_ RosHwFormat &rosFormat)
{

    // Number of HW formats is limited, so some of DXGI formats must be emulated.
    // For example, format DXGI_FORMAT_R8_UNORM is emulated with  DXGI_FORMAT_R8G8B8A8_UNORM 
    // where G8, B8 are set to 0
    // 
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    {
        bpp = 32;
        rosFormat = RosHwFormat::X8888;
    }
    break;

    case DXGI_FORMAT_R8G8_UNORM:
    {
        bpp = 32;
        rosFormat = RosHwFormat::X8888;
    }
    break;

    case DXGI_FORMAT_R8_UNORM:
    {
        bpp = 32;
        rosFormat = RosHwFormat::X8888;
    }
    break;

    case DXGI_FORMAT_A8_UNORM:
    {
        bpp = 8;
        rosFormat = RosHwFormat::X8;
    }
    break;

    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    {
        bpp = 8;
        rosFormat = RosHwFormat::X8;
    }
    break;

    case DXGI_FORMAT_D16_UNORM:
    {
        bpp = 8;
        rosFormat = RosHwFormat::X8;
    }
    break;

    default:
    {
        // Formats that are not on the list.
        assert(false);
    }

    }
}

void
RosUmdResource::CalculateTilesInfo()
{
    UINT bpp = 0;

    // Provide information about hardware formats
    MapDxgiFormatToInternalFormats(m_format, bpp, m_hwFormat);

    // Prepare information about tiles
    m_TileInfo = FillTileInfo(bpp);

    m_hwWidthTilePixels = m_TileInfo.VC4_4kBTileWidthPixels;
    m_hwHeightTilePixels = m_TileInfo.VC4_4kBTileHeightPixels;

    m_hwWidthTiles = (m_hwWidthPixels + m_hwWidthTilePixels - 1) / m_hwWidthTilePixels;
    m_hwHeightTiles = (m_hwHeightPixels + m_hwHeightTilePixels - 1) / m_hwHeightTilePixels;
    m_hwWidthPixels = m_hwWidthTiles*m_hwWidthTilePixels;
    m_hwHeightPixels = m_hwHeightTiles*m_hwHeightTilePixels;

    UINT sizeTileBytes = m_hwWidthTilePixels * m_hwHeightTilePixels * (bpp/8);

    m_hwSizeBytes = m_hwWidthTiles * m_hwHeightTiles * sizeTileBytes;
    m_hwPitchBytes = 0;

}

void
RosUmdResource::SetLockFlags(
    D3D10_DDI_MAP mapType,
    UINT mapFlags,
    D3DDDICB_LOCKFLAGS *pLockFlags)
{
    switch (mapType)
    {
    case D3D10_DDI_MAP_READ:
        pLockFlags->ReadOnly = 1;
        break;
    case D3D10_DDI_MAP_WRITE:
        pLockFlags->WriteOnly = 1;
        break;
    case D3D10_DDI_MAP_READWRITE:
        break;
    case D3D10_DDI_MAP_WRITE_DISCARD:
        pLockFlags->Discard = 1;
    case D3D10_DDI_MAP_WRITE_NOOVERWRITE:
        break;
    }

    if (mapFlags & D3D10_DDI_MAP_FLAG_DONOTWAIT)
    {
        pLockFlags->DonotWait = 1;
    }
}

void
RosUmdResource::CalculateMemoryLayout(
    void)
{
    switch (m_resourceDimension)
    {
    case D3D10DDIRESOURCE_BUFFER:
        {
            m_hwLayout = RosHwLayout::Linear;

            // TODO(bhouse) Need mapping code from resource DXGI format to hw format
            m_hwFormat = RosHwFormat::X8;

            m_hwWidthPixels = m_mip0Info.TexelWidth;
            m_hwHeightPixels = m_mip0Info.TexelHeight;

            assert(m_hwFormat == RosHwFormat::X8);
            assert(m_hwHeightPixels == 1);
            m_hwPitchBytes = m_hwSizeBytes = m_hwWidthPixels;
        }
    break;
    case D3D10DDIRESOURCE_TEXTURE2D:
        {
            if (m_usage == D3D10_DDI_USAGE_DEFAULT)
            {
                m_hwLayout = RosHwLayout::Tiled;
            }
            else
            {
                m_hwLayout = RosHwLayout::Linear;
            }

#if VC4

            // TODO[indyz]: Enable tiled render target
            if ((m_bindFlags & D3D10_DDI_BIND_RENDER_TARGET) ||
                (m_bindFlags & D3D10_DDI_BIND_SHADER_RESOURCE))
            {
                m_hwLayout = RosHwLayout::Linear;
            }

#endif

            // TODO(bhouse) Need mapping code from resource DXGI format to hw format
            if (m_bindFlags & D3D10_DDI_BIND_DEPTH_STENCIL)
            {
                m_hwFormat = RosHwFormat::D24S8;
            }
            else
            {
                m_hwFormat = RosHwFormat::X8888;
            }

            // Disable tiled format until issue #48 is fixed.
            //
            // Force tiled layout for given configuration only
            // if ((m_usage == D3D10_DDI_USAGE_DEFAULT) &&
            //    (m_bindFlags == D3D10_DDI_BIND_SHADER_RESOURCE))
            // {
            //    m_hwLayout = RosHwLayout::Tiled;
            // }

            // Using system memory linear MipMap as example
            m_hwWidthPixels = m_mip0Info.TexelWidth;
            m_hwHeightPixels = m_mip0Info.TexelHeight;

#if VC4
            // Align width and height to VC4_BINNING_TILE_PIXELS for binning
#endif

            if (m_hwLayout == RosHwLayout::Linear)
            {
                m_hwWidthTilePixels = VC4_BINNING_TILE_PIXELS;
                m_hwHeightTilePixels = VC4_BINNING_TILE_PIXELS;
                m_hwWidthTiles = (m_hwWidthPixels + m_hwWidthTilePixels - 1) / m_hwWidthTilePixels;
                m_hwHeightTiles = (m_hwHeightPixels + m_hwHeightTilePixels - 1) / m_hwHeightTilePixels;
                m_hwWidthPixels = m_hwWidthTiles*m_hwWidthTilePixels;
                m_hwHeightPixels = m_hwHeightTiles*m_hwHeightTilePixels;

                m_hwSizeBytes = CPixel::ComputeMipMapSize(
                    m_hwWidthPixels,
                    m_hwHeightPixels,
                    m_mipLevels,
                    m_format);

                m_hwPitchBytes = CPixel::ComputeSurfaceStride(
                    m_hwWidthPixels,
                    CPixel::BytesPerPixel(m_format));
            }
            else
            {
                CalculateTilesInfo();
            }
        }
        break;
    case D3D10DDIRESOURCE_TEXTURE1D:
    case D3D10DDIRESOURCE_TEXTURE3D:
    case D3D10DDIRESOURCE_TEXTURECUBE:
        {
            throw RosUmdException(DXGI_DDI_ERR_UNSUPPORTED);
        }
        break;
    }
}

bool RosUmdResource::CanRotateFrom(const RosUmdResource* Other) const
{
    // Make sure we're not rotating from ourself and that the resources
    // are compatible (e.g. size, flags, ...)
    
    return (this != Other) &&
           (!m_pData && !Other->m_pData) &&
           (!m_pSysMemCopy && !Other->m_pSysMemCopy) &&
           (m_hRTResource != Other->m_hRTResource) &&
           ((m_hKMAllocation != Other->m_hKMAllocation) || !m_hKMAllocation) &&
           ((m_hKMResource != Other->m_hKMResource) || !m_hKMResource) &&
           (m_resourceDimension == Other->m_resourceDimension) &&
           (m_mip0Info == Other->m_mip0Info) &&
           (m_usage == Other->m_usage) &&
           (m_bindFlags == Other->m_bindFlags) &&
           (m_bindFlags & D3D10_DDI_BIND_PRESENT) &&
           (m_mapFlags == Other->m_mapFlags) &&
           (m_miscFlags == Other->m_miscFlags) &&
           (m_format == Other->m_format) &&
           (m_sampleDesc == Other->m_sampleDesc) &&
           (m_mipLevels == Other->m_mipLevels) &&
           (m_arraySize == Other->m_arraySize) &&
           (m_isPrimary == Other->m_isPrimary) &&
           ((m_primaryDesc.Flags & ~DXGI_DDI_PRIMARY_OPTIONAL) ==
            (Other->m_primaryDesc.Flags & ~DXGI_DDI_PRIMARY_OPTIONAL)) &&
           (m_primaryDesc.VidPnSourceId == Other->m_primaryDesc.VidPnSourceId) &&
           (m_primaryDesc.ModeDesc == Other->m_primaryDesc.ModeDesc) &&
           (m_primaryDesc.DriverFlags == Other->m_primaryDesc.DriverFlags) &&
           (m_hwLayout == Other->m_hwLayout) &&
           (m_hwWidthPixels == Other->m_hwWidthPixels) &&
           (m_hwHeightPixels == Other->m_hwHeightPixels) &&
           (m_hwFormat == Other->m_hwFormat) &&
           (m_hwPitchBytes == Other->m_hwPitchBytes) &&
           (m_hwSizeBytes == Other->m_hwSizeBytes) &&
           (m_hwWidthTilePixels == Other->m_hwWidthTilePixels) &&
           (m_hwHeightTilePixels == Other->m_hwHeightTilePixels) &&
           (m_hwWidthTiles == Other->m_hwWidthTiles) &&
           (m_hwHeightTiles == Other->m_hwHeightTiles);
}


// Converts R, RG or A buffer to 32 bpp (RGBA) buffer
void  RosUmdResource::ConvertBufferto32Bpp(const BYTE *pSrc, BYTE *pDst, UINT srcBpp, UINT swizzleMask, UINT pSrcStride, UINT pDstStride)
{
    for (UINT i = 0; i < m_mip0Info.TexelHeight; i++)
    {
        UINT32 *dstSwizzled = (UINT32*)pDst;

        UINT dstIndex = 0;
        for (UINT k = 0; k < m_mip0Info.TexelWidth*srcBpp; k += srcBpp)
        {
            UINT32 swizzledRGBA = 0;

            // Gather individual color elements into one DWORD
            for (UINT colorElement = 0; colorElement < srcBpp; colorElement++)
            {
                UINT32 currentColorElement = (UINT32)pSrc[k + colorElement];

                // Move element to the right position
                currentColorElement = currentColorElement << (colorElement << 3);
                swizzledRGBA = swizzledRGBA | currentColorElement;
            }

            swizzledRGBA = swizzledRGBA << swizzleMask;
            dstSwizzled[dstIndex] = swizzledRGBA;
            dstIndex += 1;
        }

        pSrc += pSrcStride;
        pDst += pDstStride;
    }
}

// Converts texture to internal (HW friendly) representation
void RosUmdResource::ConvertInitialTextureFormatToInternal(const BYTE *pSrc, BYTE *pDst, UINT rowStride)
{
    // HW supports only limited set of formats, so most of DXGI formats must
    // be converted at the beginning.    
    UINT swizzleMask = 0;
    UINT srcBpp = 0;
 
    switch (m_format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    {
        // Do nothing
    }
    break;

    case DXGI_FORMAT_R8_UNORM:
    {
        swizzleMask = 0;
        srcBpp = 1;
    }
    break;

    case DXGI_FORMAT_R8G8_UNORM:
    {
        swizzleMask = 0;
        srcBpp = 2;
    }
    break;

    case DXGI_FORMAT_A8_UNORM:
    {
        swizzleMask = 0;
        srcBpp = 1;
    }
    break;

    default:
    {
        assert(false);
    }
    break;
    }
   
    // For DXGI_FORMAT_R8G8B8A8_UNORM and DXGI_FORMAT_A8_UNORM we can do a simple copy or swizzle 
    // texture directly to memory.
    if ((m_format == DXGI_FORMAT_R8G8B8A8_UNORM) || (m_format == DXGI_FORMAT_A8_UNORM))
    {
        if (m_hwLayout == RosHwLayout::Linear)
        {
            for (UINT i = 0; i < m_mip0Info.TexelHeight; i++)
            {
                memcpy(pDst, pSrc, rowStride);

                pSrc += rowStride;
                pDst += m_hwPitchBytes;
            }
        }
        else
        {
            // Swizzle texture to HW format
            ConvertBitmapTo4kTileBlocks(pSrc, pDst, rowStride);
        }
    }
    else
    {
        // We have to convert other formats to internal format         
        if (m_hwLayout == RosHwLayout::Linear)
        {
            // Do a conversion directly to the locked allocation
            ConvertBufferto32Bpp(pSrc, pDst, srcBpp, swizzleMask, rowStride, m_hwPitchBytes);            
        }
        else
        {
            // For tiled layout, additional buffer is allocated. It is a 
            // conversion (temporary) buffer.
            UINT pitch = m_mip0Info.TexelHeight * m_mip0Info.TexelWidth * 4;
            
            auto temporary = std::unique_ptr<BYTE[]>{ new BYTE[pitch] };
            
            UINT dstStride = m_mip0Info.TexelWidth * 4;

            ConvertBufferto32Bpp(pSrc, temporary.get(), srcBpp, swizzleMask, rowStride, dstStride);

            ConvertBitmapTo4kTileBlocks(temporary.get(), pDst, dstStride);

        }
    }
}

// Form 1k sub-tile block
BYTE *RosUmdResource::Form1kSubTileBlock(const BYTE *pInputBuffer, BYTE *pOutBuffer, UINT rowStride)
{    
    // 1k sub-tile block is formed from micro-tiles blocks
    for (UINT h = 0; h < m_TileInfo.VC4_1kBSubTileHeightPixels; h += m_TileInfo.vC4_MicroTileHeight)
    {
        const BYTE *currentBufferPos = pInputBuffer + h*rowStride;

        // Process row of 4 micro-tiles blocks
        for (UINT w = 0; w < m_TileInfo.VC4_1kBSubTileWidthBytes; w+= m_TileInfo.VC4_MicroTileWidthBytes)
        {
            const BYTE *microTileOffset = currentBufferPos + w;

            // Process micro-tile block
            for (UINT t = 0; t < m_TileInfo.vC4_MicroTileHeight; t++)
            {
                memcpy(pOutBuffer, microTileOffset, m_TileInfo.VC4_MicroTileWidthBytes);
                pOutBuffer += m_TileInfo.VC4_MicroTileWidthBytes;
                microTileOffset += rowStride;
            }
        }
    }
    return pOutBuffer;
}

// Form one 4k tile block from pInputBuffer and store in pOutBuffer
BYTE *RosUmdResource::Form4kTileBlock(const BYTE *pInputBuffer, BYTE *pOutBuffer, UINT rowStride, BOOLEAN OddRow)
{
    const BYTE *currentTileOffset = NULL;
   
    UINT subTileHeightPixels        = m_TileInfo.VC4_1kBSubTileHeightPixels;
    UINT subTileWidthBytes          = m_TileInfo.VC4_1kBSubTileWidthBytes;

    if (OddRow)
    {
        // For even rows, process sub-tile blocks in ABCD order, where
        // each sub-tile is stored in memory as follows:
        //
        //  [C  B]   
        //  [D  A]
        //                  

        // Get A block
        currentTileOffset = pInputBuffer + rowStride * subTileHeightPixels + subTileWidthBytes;
        pOutBuffer = Form1kSubTileBlock(currentTileOffset, pOutBuffer, rowStride);

        // Get B block
        currentTileOffset = pInputBuffer + subTileWidthBytes;

        pOutBuffer = Form1kSubTileBlock(currentTileOffset, pOutBuffer, rowStride);

        // Get C block
        pOutBuffer = Form1kSubTileBlock(pInputBuffer, pOutBuffer, rowStride);

        // Get D block
        currentTileOffset = pInputBuffer + rowStride * subTileHeightPixels;
        pOutBuffer = Form1kSubTileBlock(currentTileOffset, pOutBuffer, rowStride);

        // return current position in out buffer
        return pOutBuffer;

    }
    else
    {
        // For even rows, process sub-tile blocks in ABCD order, where
        // each sub-tile is stored in memory as follows:
        // 
        //  [A  D]    
        //  [B  C] 
        //

        // Get A block
        pOutBuffer = Form1kSubTileBlock(pInputBuffer, pOutBuffer, rowStride);

        /// Get B block
        currentTileOffset = pInputBuffer + rowStride * subTileHeightPixels;
        pOutBuffer = Form1kSubTileBlock(currentTileOffset, pOutBuffer, rowStride);

        // Get C Block
        currentTileOffset = pInputBuffer + rowStride * subTileHeightPixels + subTileWidthBytes;
        pOutBuffer = Form1kSubTileBlock(currentTileOffset, pOutBuffer, rowStride);

        // Get D block
        currentTileOffset = pInputBuffer + subTileWidthBytes;
        pOutBuffer = Form1kSubTileBlock(currentTileOffset, pOutBuffer, rowStride);

        // return current position in out buffer
        return pOutBuffer;
    }
}

// Form (CountX * CountY) tile blocks from InputBuffer and store them in OutBuffer
void RosUmdResource::ConvertBitmapTo4kTileBlocks(const BYTE *InputBuffer, BYTE *OutBuffer, UINT rowStride)
{
    UINT CountX = m_hwWidthTiles;
    UINT CountY = m_hwHeightTiles;

    for (UINT k = 0; k < CountY; k++)
    {
        BOOLEAN oddRow = k & 1;
        if (oddRow)
        {
            // Build 4k blocks from right to left for odd rows
            for (int i = CountX - 1; i >= 0; i--)
            {
                const BYTE *blockStartOffset = InputBuffer + k * rowStride * m_TileInfo.VC4_4kBTileHeightPixels + i * m_TileInfo.VC4_4kBTileWidthBytes;
                OutBuffer = Form4kTileBlock(blockStartOffset, OutBuffer, rowStride, oddRow);
            }
        }
        else
        {
            // Build 4k blocks from left to right for even rows
            for (UINT i = 0; i < CountX; i++)
            {
                const BYTE *blockStartOffset = InputBuffer + k * rowStride * m_TileInfo.VC4_4kBTileHeightPixels + i * m_TileInfo.VC4_4kBTileWidthBytes;
                OutBuffer = Form4kTileBlock(blockStartOffset, OutBuffer, rowStride, oddRow);
            }
        }
    }
}
