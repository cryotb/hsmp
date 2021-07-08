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

	inline auto allocate_physical_pages(SIZE_T length) -> PMDL
	{
		PHYSICAL_ADDRESS pages_alloc{}, pages_max{}, pages_skip{};

		pages_alloc.QuadPart = 0ull;
		pages_max.QuadPart = ~0ull;
		pages_skip.QuadPart = 0ull;

		return MmAllocatePagesForMdl(
			pages_alloc, pages_max,
			pages_skip, length
		);
	}

	inline auto allocate_virtual_pages(SIZE_T length) -> PVOID
	{
		auto* result = PVOID{};
		auto* const mdl = allocate_physical_pages(length);

		if (mdl == nullptr)
			return nullptr;

		__try
		{
			result = MmMapLockedPagesSpecifyCache(mdl, KernelMode,
				MmNonCached, NULL, FALSE, NormalPagePriority);

			if (result &&
				!NT_SUCCESS(MmProtectMdlSystemAddress(
					mdl, PAGE_EXECUTE_READWRITE
				)))
				return nullptr;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return nullptr;
		}

		return result;
	}
}
