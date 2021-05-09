#pragma once

namespace util
{
	INLINE NTSTATUS CreateDevice(DRIVER_OBJECT* driverObject, DEVICE_OBJECT** outCreatedDeviceObject, UNICODE_STRING* deviceName, const ULONG deviceType, const ULONG deviceCrs) {
		return IoCreateDevice(driverObject, NULL, deviceName, deviceType, deviceCrs, FALSE, outCreatedDeviceObject);
	}

	INLINE NTSTATUS CreateSymLink(UNICODE_STRING* deviceName, UNICODE_STRING* symbolName) {
		return IoCreateSymbolicLink(symbolName, deviceName);
	}

	INLINE BOOLEAN KCopyVirtualMemory(PVOID pDst, PVOID pSrc, UINT64 uLength)
	{
		SIZE_T nBytesTransferred = 0;
		MM_COPY_ADDRESS sCopyAddress{};

		sCopyAddress.VirtualAddress = pSrc;

		return NT_SUCCESS(MmCopyMemory(pDst, sCopyAddress, 
			uLength, MM_COPY_MEMORY_VIRTUAL, &nBytesTransferred));
	}

	INLINE BOOLEAN UCopyVirtualMemory(PEPROCESS pSrcProc, PEPROCESS pDstProc, 
		PVOID pDst, PVOID pSrc, UINT64 uLength)
	{
		SIZE_T uReturnLength{};
		return NT_SUCCESS(MmCopyVirtualMemory(pSrcProc, pSrc, pDstProc,
			pDst, uLength, KernelMode, &uReturnLength));
	}

	INLINE BOOLEAN LookupProcessById(DWORD32 dwId, PEPROCESS* peOut)
	{
		return NT_SUCCESS(PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(dwId), peOut));
	}
}
