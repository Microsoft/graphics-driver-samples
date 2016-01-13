#ifndef _VC4DISPLAY_HPP_
#define  _VC4DISPLAY_HPP_ 1
//
// Copyright (C) Microsoft.  All rights reserved.
//
//
// Module Name:
//
//    Vc4Display.h
//
// Abstract:
//
//    This module contains code relevant to the VC4 display subsystem including
//      - Hardware Video Scalar (HVS) - responsible for scanning out a frame
//        buffer to the HDMI port. Can do scaling, overlays, rotation, and
//        color space conversion. We cannot register for HVS interrupts
//        because they are handled by the VPU firmware.
//      - PixelValve - sits between the HVS and the HDMI controller and regulates
//        the flow of pixels from the HVS to the HDMI controller. Generates
//        interrupts at various points in the scanout process, including the
//        VfpStart (Vertical Front Porch) interrupt, which is used to know
//        when the current frame buffer is no longer needed
//      - HDMI - receives output from the pixelvalve, participates in mode
//        setting
//      - Hot plug detection (HPD) - a GPIO pin is used to detect when the HDMI
//        cable is plugged and unplugged. A separate driver - gpiohpd - 
//        registers for the interrupt, and calls us at DISPATCH_LEVEL whenever
//        a hotplug event occurs.
//      - I2C Display Data Channel (DDC) - allows us to read the monitor's
//        EDID which contains the list of supported modes. The I2C controller
//        is the same IP as the other BCM2836 I2C controllers, so we leverage
//        bcmi2c to drive the I2C controller, and we interact with it through
//        an SPB connection.
//    
//
// Author:
//
//    Jordan Rhee (jordanrh) November 2015
//
// Environment:
//
//    Kernel mode only.
//

#include "Vc4Hvs.h"
#include "Vc4PixelValve.h"
#include "Vc4Debug.h"

class VC4_DISPLAY {
public: // NONPAGED

    void ResetDevice ();
    
    _Check_return_
    NTSTATUS SystemDisplayEnable (
        _In_ D3DDDI_VIDEO_PRESENT_TARGET_ID TargetId,
        _In_ PDXGKARG_SYSTEM_DISPLAY_ENABLE_FLAGS Flags,
        _Out_ UINT* WidthPtr,
        _Out_ UINT* HeightPtr,
        _Out_ D3DDDIFORMAT* ColorFormatPtr
        );
        
    void SystemDisplayWrite (
        _In_reads_bytes_(SourceHeight * SourceStride) PVOID SourcePtr,
        _In_ UINT SourceWidth,
        _In_ UINT SourceHeight,
        _In_ UINT SourceStride,
        _In_ UINT PositionX,
        _In_ UINT PositionY
        );

    _Check_return_
    _IRQL_requires_(HIGH_LEVEL)
    BOOLEAN InterruptRoutine (IN_ULONG MessageNumber);
    
    _IRQL_requires_(DISPATCH_LEVEL)
    void DpcRoutine ();
    
    static EVT_GPIOHPD_HOTPLUG_NOTIFICATION EvtHotplugNotification;

private: // NONPAGED

    typedef LONG FRAME_BUFFER_ID;

    enum : ULONG { CHILD_COUNT = 1 };

    enum I2C_CHANNEL_INDEX {
        I2C_CHANNEL_INDEX_DDC,      // address 0x50 (for reading EDID blocks)
        I2C_CHANNEL_INDEX_EDDC,     // address 0x30 (for writing segment number)
        I2C_CHANNEL_INDEX_COUNT,
    };

    VC4_DISPLAY (const VC4_DISPLAY&) = delete;
    VC4_DISPLAY& operator= (const VC4_DISPLAY&) = delete;
    
    // store references to objects that are common between all display
    // components to avoid wasting memory
    const DEVICE_OBJECT* const physicalDeviceObjectPtr;
    const DXGKRNL_INTERFACE& dxgkInterface;
    const DXGK_START_INFO& dxgkStartInfo;
    const DXGK_DEVICE_INFO& dxgkDeviceInfo;
    
    VC4_DEBUG dbgHelper;
    DXGK_DISPLAY_INFORMATION dxgkDisplayInfo;
    D3DKMDT_VIDEO_SIGNAL_INFO dxgkVideoSignalInfo;
    D3DKMDT_VIDPN_SOURCE_MODE dxgkCurrentSourceMode;
    
    FILE_OBJECT* hpdFileObjectPtr;
    BOOLEAN hdmiConnected;
    FILE_OBJECT* i2cFileObjectPtrs[I2C_CHANNEL_INDEX_COUNT];
    
    VC4HVS_REGISTERS* hvsRegistersPtr;
    VC4PIXELVALVE_REGISTERS* pvRegistersPtr;

    VC4PIXELVALVE_INTERRUPT pixelValveIntEn;

    SIZE_T frameBufferLength;
    VOID* biosFrameBufferPtr;       // must be freed with MmUnmapIoSpace
    VOID* systemFrameBuffers[1];    // must be freed with MmFreeContiguousMemory

    struct _FRAME_BUFFER_DESCRIPTOR {
        VOID* BufferPtr;
        ULONG PhysicalAddress;
    } frameBuffers[2];

    FRAME_BUFFER_ID activeFrameBufferId;                // written only by the ISR
    volatile FRAME_BUFFER_ID pendingActiveFrameBufferId;   // access must be Interlocked - get from ISR, put from Present
    volatile FRAME_BUFFER_ID backBufferId;              // access must be Interlocked - put from ISR, get from Present

    VC4HVS_DLIST_ENTRY_UNITY* displayListPtr;
    VC4HVS_DLIST_CONTROL_WORD_0 displayListControlWord0;

    __forceinline _FRAME_BUFFER_DESCRIPTOR* getFrameBufferDescriptorFromId (
        FRAME_BUFFER_ID FrameBufferId
        )
    {
        // Id's are 1-based indexes so that 0 can be the "null" value
        NT_ASSERT(FrameBufferId);
        return &this->frameBuffers[FrameBufferId - 1];
    }

public: // PAGED

    _IRQL_requires_(PASSIVE_LEVEL)
    VC4_DISPLAY (
        const DEVICE_OBJECT* PhysicalDeviceObjectPtr,
        const DXGKRNL_INTERFACE& DxgkInterface,
        const DXGK_START_INFO& DxgkStartInfo,
        const DXGK_DEVICE_INFO& DxgkDeviceInfo
        );
    
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS StartDevice (
        ULONG FirstResourceIndex,
        _Out_ ULONG* NumberOfVideoPresentSourcesPtr,
        _Out_ ULONG* NumberOfChildrenPtr
        );
    
    _IRQL_requires_(PASSIVE_LEVEL)
    void StopDevice ();
    
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS DispatchIoRequest (
        IN_ULONG /*VidPnSourceId*/,
        IN_PVIDEO_REQUEST_PACKET VideoRequestPacketPtr
        );

    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS QueryChildRelations (
        _Inout_updates_bytes_(ChildRelationsSizeInBytes) DXGK_CHILD_DESCRIPTOR* ChildRelationsPtr,
        ULONG ChildRelationsSizeInBytes
        );
    
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS QueryChildStatus (
        INOUT_PDXGK_CHILD_STATUS ChildStatusPtr,
        IN_BOOLEAN NonDestructiveOnly
        );
        
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS QueryDeviceDescriptor (
        IN_ULONG ChildUid,
        INOUT_PDXGK_DEVICE_DESCRIPTOR DeviceDescriptorPtr
        );
    
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS SetPowerState (
        IN_ULONG DeviceUid,
        IN_DEVICE_POWER_STATE DevicePowerState,
        IN_POWER_ACTION ActionType
        );

    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS QueryAdapterInfo (
        IN_CONST_PDXGKARG_QUERYADAPTERINFO QueryAdapterInfoPtr
        );
        
    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS SetPointerPosition (
        IN_CONST_PDXGKARG_SETPOINTERPOSITION SetPointerPositionPtr
        );
        
    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS SetPointerShape (
        IN_CONST_PDXGKARG_SETPOINTERSHAPE SetPointerShapePtr
        );

    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS IsSupportedVidPn (
        INOUT_PDXGKARG_ISSUPPORTEDVIDPN IsSupportedVidPnPtr
        );
        
    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS RecommendFunctionalVidPn (
        IN_CONST_PDXGKARG_RECOMMENDFUNCTIONALVIDPN_CONST RecommendFunctionalVidPnPtr
        );
        
    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS EnumVidPnCofuncModality (
        IN_CONST_PDXGKARG_ENUMVIDPNCOFUNCMODALITY_CONST EnumCofuncModalityPtr
        );
        
    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS SetVidPnSourceVisibility (
        IN_CONST_PDXGKARG_SETVIDPNSOURCEVISIBILITY SetVidPnSourceVisibilityPtr
        );
        
    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS CommitVidPn (
        IN_CONST_PDXGKARG_COMMITVIDPN_CONST CommitVidPnPtr
        );
        
    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS UpdateActiveVidPnPresentPath (
        IN_CONST_PDXGKARG_UPDATEACTIVEVIDPNPRESENTPATH_CONST UpdateActiveVidPnPresentPathPtr
        );

    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS RecommendMonitorModes (
        IN_CONST_PDXGKARG_RECOMMENDMONITORMODES_CONST RecommendMonitorModesPtr
        );
        
    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS QueryVidPnHWCapability (
        INOUT_PDXGKARG_QUERYVIDPNHWCAPABILITY VidPnHWCapsPtr
        );
        
    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS PresentDisplayOnly (
        IN_CONST_PDXGKARG_PRESENT_DISPLAYONLY PresentDisplayOnlyPtr
        );
        
    _Check_return_
    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS StopDeviceAndReleasePostDisplayOwnership (
        _In_ D3DDDI_VIDEO_PRESENT_TARGET_ID TargetId,
        _Out_ PDXGK_DISPLAY_INFORMATION DisplayInfo
        );

private: // PAGED

    _IRQL_requires_(PASSIVE_LEVEL)
    static NTSTATUS SourceHasPinnedMode (
        D3DKMDT_HVIDPN VidPnHandle,
        const DXGK_VIDPN_INTERFACE* VidPnInterfacePtr,
        D3DKMDT_VIDEO_PRESENT_SOURCE_MODE_ID SourceId
        );

    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS CreateAndAssignSourceModeSet (
        D3DKMDT_HVIDPN VidPnHandle,
        const DXGK_VIDPN_INTERFACE* VidPnInterfacePtr,
        D3DKMDT_VIDEO_PRESENT_SOURCE_MODE_ID SourceId,
        D3DKMDT_VIDEO_PRESENT_TARGET_MODE_ID TargetId
        ) const;

    _IRQL_requires_(PASSIVE_LEVEL)
    static NTSTATUS TargetHasPinnedMode (
        D3DKMDT_HVIDPN VidPnHandle,
        const DXGK_VIDPN_INTERFACE* VidPnInterfacePtr,
        D3DKMDT_VIDEO_PRESENT_TARGET_MODE_ID TargetId
        );

    _IRQL_requires_(PASSIVE_LEVEL)
    NTSTATUS CreateAndAssignTargetModeSet (
        D3DKMDT_HVIDPN VidPnHandle,
        const DXGK_VIDPN_INTERFACE* VidPnInterfacePtr,
        D3DKMDT_VIDEO_PRESENT_SOURCE_MODE_ID SourceId,
        D3DKMDT_VIDEO_PRESENT_TARGET_MODE_ID TargetId
        ) const;

    _IRQL_requires_(PASSIVE_LEVEL)
    static NTSTATUS IsVidPnSourceModeFieldsValid (
        const D3DKMDT_VIDPN_SOURCE_MODE* SourceModePtr
        );

    _IRQL_requires_(PASSIVE_LEVEL)
    static NTSTATUS IsVidPnPathFieldsValid (
        const D3DKMDT_VIDPN_PRESENT_PATH* PathPtr
        );
};

#endif // _VC4DISPLAY_HPP_