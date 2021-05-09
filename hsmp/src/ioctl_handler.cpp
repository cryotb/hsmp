#include <include.hpp>

VOID ioctl_handler::GetStatus(PIRP Irp,
	NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused,
	BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer)
{
	if (IsOutputBufferUnused)
	{
		IoOpStatus = STATUS_INVALID_PARAMETER;
		return;
	}

	AUTO* CONST pStatusInfo = static_cast<comms::K64StatusInfo_t*>(pOutputBuffer);

	if (pStatusInfo)
	{
		pStatusInfo->CurrentState = 37;
		pStatusInfo->ManuallyMapped = FALSE;

		Irp->IoStatus.Information = sizeof(*pStatusInfo);
	}
}

VOID ioctl_handler::MapIoSpace(PIRP Irp,
	NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused,
	BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer)
{
	if (IsInputBufferUnused || IsOutputBufferUnused)
	{
		IoOpStatus = STATUS_INVALID_PARAMETER;
		return;
	}

	AUTO* CONST pArguments = static_cast<comms::K64MemBuf_t*>(pInputBuffer);
	AUTO* CONST pResult = static_cast<comms::K64AddressExpression_t*>(pOutputBuffer);

	PHYSICAL_ADDRESS sPhysicalAddress{};

	sPhysicalAddress.QuadPart = Memory::Address(pArguments->Pointer).Base();

	AUTO* CONST pMappedSpace = MmMapIoSpace(sPhysicalAddress, pArguments->Size, MmNonCached);

	pResult->Result = Memory::Address(pMappedSpace).Base();
	pResult->DidComplete = TRUE;
}

VOID ioctl_handler::UnmapIoSpace(PIRP Irp,
	NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused,
	BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer)
{
	if (IsInputBufferUnused || IsOutputBufferUnused)
	{
		IoOpStatus = STATUS_INVALID_PARAMETER;
		return;
	}

	AUTO* CONST pArguments = static_cast<comms::K64MemBuf_t*>(pInputBuffer);
	AUTO* CONST pResult = static_cast<comms::K64GenericRequestResult_t*>(pOutputBuffer);

	MmUnmapIoSpace(pArguments->Pointer, pArguments->Size);

	pResult->DidComplete = TRUE;
}

VOID ioctl_handler::VaToPa(PIRP Irp,
	NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused,
	BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer)
{
	if (IsInputBufferUnused || IsOutputBufferUnused)
	{
		IoOpStatus = STATUS_INVALID_PARAMETER;
		return;
	}

	AUTO* CONST pArguments = static_cast<comms::K64AddressExpression_t*>(pInputBuffer);
	AUTO* CONST pResult = static_cast<comms::K64AddressExpression_t*>(pOutputBuffer);

	AUTO sPhysicalAddress = MmGetPhysicalAddress( Memory::Address(pArguments->Result).Ptr() );

	pResult->Result = sPhysicalAddress.QuadPart;

	pResult->DidComplete = TRUE;
}

VOID ioctl_handler::PaToVa(PIRP Irp,
	NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused,
	BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer)
{
	if (IsInputBufferUnused || IsOutputBufferUnused)
	{
		IoOpStatus = STATUS_INVALID_PARAMETER;
		return;
	}

	AUTO* CONST pArguments = static_cast<comms::K64AddressExpression_t*>(pInputBuffer);
	AUTO* CONST pResult = static_cast<comms::K64AddressExpression_t*>(pOutputBuffer);

	PHYSICAL_ADDRESS sPhysicalAddress{};

	sPhysicalAddress.QuadPart = pArguments->Result;

	AUTO* CONST pVirtualAddress = MmGetVirtualForPhysical(sPhysicalAddress);

	pResult->Result = Memory::Address(pVirtualAddress).Base();

	pResult->DidComplete = TRUE;
}

VOID ioctl_handler::ReadVirtualMemory(PIRP Irp,
	NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused,
	BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer)
{
	if (IsInputBufferUnused || IsOutputBufferUnused)
	{
		IoOpStatus = STATUS_INVALID_PARAMETER;
		return;
	}

	AUTO* CONST pArguments = static_cast<comms::K64ReadVirtualMemory_t*>(pInputBuffer);
	AUTO* CONST pResult = static_cast<comms::K64GenericRequestResult_t*>(pOutputBuffer);

	if (pArguments->InKernel)
	{
		if (!util::KCopyVirtualMemory(pArguments->Destination,
			Memory::Address(pArguments->Source).Ptr(), pArguments->Length))
			return;

		pResult->DidComplete = TRUE;
	} else
	{
		PEPROCESS pTargetProcess = nullptr;

		if (!util::LookupProcessById(pArguments->ProcessId, &pTargetProcess))
			return;

		if (pTargetProcess == nullptr)
			return;

		if (!util::UCopyVirtualMemory(pTargetProcess, PsGetCurrentProcess(),
			pArguments->Destination, Memory::Address(pArguments->Source)
			.Ptr(), pArguments->Length))
			return;

		ObDereferenceObject(pTargetProcess);

		pResult->DidComplete = TRUE;
	}
}

VOID ioctl_handler::WriteVirtualMemory(PIRP Irp,
	NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused,
	BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer)
{
	if (IsInputBufferUnused || IsOutputBufferUnused)
	{
		IoOpStatus = STATUS_INVALID_PARAMETER;
		return;
	}

	AUTO* CONST pArguments = static_cast<comms::K64WriteVirtualMemory_t*>(pInputBuffer);
	AUTO* CONST pResult = static_cast<comms::K64GenericRequestResult_t*>(pOutputBuffer);

	if (pArguments->InKernel)
	{
		if (!util::KCopyVirtualMemory(Memory::Address(pArguments->Destination)
			.Ptr(), pArguments->Source, pArguments->Length))
			return;

		pResult->DidComplete = TRUE;
	}
	else
	{
		PEPROCESS pTargetProcess = nullptr;

		if (!util::LookupProcessById(pArguments->ProcessId, &pTargetProcess))
			return;

		if (pTargetProcess == nullptr)
			return;

		if (!util::UCopyVirtualMemory(PsGetCurrentProcess(), pTargetProcess,
			Memory::Address(pArguments->Destination).Ptr(),
			pArguments->Source, pArguments->Length))
			return;

		ObDereferenceObject(pTargetProcess);

		pResult->DidComplete = TRUE;
	}
}

VOID ioctl_handler::ReadPhysicalMemory(PIRP Irp,
	NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused,
	BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer)
{

}

VOID ioctl_handler::WritePhysicalMemory(PIRP Irp,
	NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused,
	BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer)
{

}
