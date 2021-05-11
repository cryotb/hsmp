#include <include.hpp>

VOID ioctl_handler::GetStatus(IOCTL_ARGS)
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

VOID ioctl_handler::MapIoSpace(IOCTL_ARGS)
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

VOID ioctl_handler::UnmapIoSpace(IOCTL_ARGS)
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

VOID ioctl_handler::AllocatePool(IOCTL_ARGS)
{
	if (IsInputBufferUnused || IsOutputBufferUnused)
	{
		IoOpStatus = STATUS_INVALID_PARAMETER;
		return;
	}

	AUTO* CONST pArguments = static_cast<comms::K64AllocatePool_t*>(pInputBuffer);
	AUTO* CONST pResult = static_cast<comms::K64AddressExpression_t*>(pOutputBuffer);

	AUTO* CONST pPool = 
		ExAllocatePool( static_cast<POOL_TYPE>(pArguments->Type), pArguments->Length );

	if (pPool == nullptr)
		return;

	pResult->Result = Memory::Address(pPool).Base();

	pResult->DidComplete = TRUE;
}

VOID ioctl_handler::FreePool(IOCTL_ARGS)
{
	if (IsInputBufferUnused || IsOutputBufferUnused)
	{
		IoOpStatus = STATUS_INVALID_PARAMETER;
		return;
	}

	AUTO* CONST pArguments = static_cast<comms::K64AddressExpression_t*>(pInputBuffer);
	AUTO* CONST pResult = static_cast<comms::K64GenericRequestResult_t*>(pOutputBuffer);

	ExFreePool( reinterpret_cast<void*>(pArguments->Result) );

	pResult->DidComplete = TRUE;
}

VOID ioctl_handler::VaToPa(IOCTL_ARGS)
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

VOID ioctl_handler::PaToVa(IOCTL_ARGS)
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

VOID ioctl_handler::GetSystemRoutineAddress(IOCTL_ARGS)
{
	if (IsInputBufferUnused || IsOutputBufferUnused)
	{
		IoOpStatus = STATUS_INVALID_PARAMETER;
		return;
	}

	AUTO* CONST pArguments = static_cast<comms::K64WideString_t*>(pInputBuffer);
	AUTO* CONST pResult = static_cast<comms::K64AddressExpression_t*>(pOutputBuffer);

	UNICODE_STRING sSystemRoutineName{};
	RtlInitUnicodeString(&sSystemRoutineName, pArguments->Buffer);

	pResult->Result = 
		Memory::Address(
			MmGetSystemRoutineAddress(&sSystemRoutineName)
		).Base();

	if (!pResult->Result)
		return;

	pResult->DidComplete = TRUE;
}

VOID ioctl_handler::ReadVirtualMemory(IOCTL_ARGS)
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

VOID ioctl_handler::WriteVirtualMemory(IOCTL_ARGS)
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

VOID ioctl_handler::ReadPhysicalMemory(IOCTL_ARGS)
{

}

VOID ioctl_handler::WritePhysicalMemory(IOCTL_ARGS)
{

}

VOID ioctl_handler::CallEntryPoint(IOCTL_ARGS)
{
	if (IsInputBufferUnused || IsOutputBufferUnused)
	{
		IoOpStatus = STATUS_INVALID_PARAMETER;
		return;
	}

	using fnEntryPoint = NTSTATUS(PVOID, PVOID);

	AUTO* CONST pArguments = static_cast<comms::K64CallEntryPoint_t*>(pInputBuffer);
	AUTO* CONST pResult = static_cast<comms::K64CallEntryPointResult_t*>(pOutputBuffer);

	auto* const pfnEntryPoint = Memory::Address(pArguments->Address)
		.As<fnEntryPoint*>();

	__try
	{
		pResult->ReturnValue = pfnEntryPoint(pArguments->Rcx, pArguments->Rdx);
	} __except(EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}

	pResult->DidComplete = TRUE;
}
