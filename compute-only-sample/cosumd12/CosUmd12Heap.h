#pragma once

#include "CosUmd12.h"

class CosUmd12Device;

class CosUmd12Heap : public CosAllocationExchange
{
public:
    explicit CosUmd12Heap(CosUmd12Device* pDevice)
    {
        m_pDevice = pDevice;
        memset(&m_desc, 0, sizeof(m_desc));
    }

    explicit CosUmd12Heap(CosUmd12Device * pDevice, D3D12DDI_HRTRESOURCE hRTHeap, const D3D12DDIARG_CREATEHEAP_0001 * pDesc)
    {
        m_pDevice = pDevice;
        m_hRTHeap = hRTHeap;
        m_desc = *pDesc;

        m_hKMAllocation = 0;
        m_pCpuAddress = NULL;
    }

    ~CosUmd12Heap()
    {
        Teardown();
    }

    HRESULT Standup();
    void Teardown();

    static int CalculateSize()
    {
        return sizeof(CosUmd12Heap);
    }

    static CosUmd12Heap* CastFrom(D3D12DDI_HHEAP);
    D3D12DDI_HHEAP CastTo() const;

    HRESULT Map(void** pHeapData);
    void Unmap();

    D3DKMT_HANDLE GetAllocationHandle()
    {
        return m_hKMAllocation;
    }

private:

    friend class CosUmd12Resource;

    CosUmd12Device * m_pDevice;
    D3D12DDIARG_CREATEHEAP_0001 m_desc;
    D3D12DDI_HRTRESOURCE m_hRTHeap;

    D3DKMT_HANDLE m_hKMAllocation;
    BYTE * m_pCpuAddress;

    D3D12DDI_GPU_VIRTUAL_ADDRESS m_uniqueAddress;
};

inline CosUmd12Heap* CosUmd12Heap::CastFrom(D3D12DDI_HHEAP hHeap)
{
    return static_cast< CosUmd12Heap* >(hHeap.pDrvPrivate);
}

inline D3D12DDI_HHEAP CosUmd12Heap::CastTo() const
{
    return MAKE_D3D12DDI_HHEAP(const_cast< CosUmd12Heap* >(this));
}

