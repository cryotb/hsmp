#pragma once

#define SCYTHE_UNUSED_BUFFER 0xCFD11D1023

namespace Util
{
	template < typename _BUFFER_TYPE_IN, typename _BUFFER_TYPE_OUT >
	inline BOOLEAN DeviceControl(HANDLE hDevice, DWORD dwCode, _BUFFER_TYPE_IN pInBuffer, _BUFFER_TYPE_OUT pOutBuffer)
	{
		auto dwBytesReturned = DWORD{};

		return DeviceIoControl(hDevice, dwCode, reinterpret_cast<void*>(pInBuffer), sizeof(_BUFFER_TYPE_IN),
			reinterpret_cast<void*>(pOutBuffer), sizeof(_BUFFER_TYPE_OUT), &dwBytesReturned, nullptr);
	}
}

namespace comms
{
	namespace IOC
	{
		constexpr auto TranslateCode(UINT32 dwIn) -> ULONG
		{
			return CTL_CODE(FILE_DEVICE_UNKNOWN, dwIn, METHOD_NEITHER, FILE_ANY_ACCESS);
		}

		enum class Code : ULONG
		{
			GET_STATUS = TranslateCode(0x800),

			MAP_IO_SPACE = TranslateCode(0x801),
			UNMAP_IO_SPACE = TranslateCode(0x802),

			VA_TO_PA = TranslateCode(0x803),
			PA_TO_VA = TranslateCode(0x804),

			ALLOCATE_POOL = TranslateCode(0x805),
			FREE_POOL = TranslateCode(0x806),

			READ_VIRTUAL_MEMORY = TranslateCode(0x807),
			WRITE_VIRTUAL_MEMORY = TranslateCode(0x808),

			READ_PHYSICAL_MEMORY = TranslateCode(0x809),
			WRITE_PHYSICAL_MEMORY = TranslateCode(0x810),

			GET_SYSTEM_ROUTINE_ADDRESS = TranslateCode(0x811),

			CALL_ENTRY_POINT = TranslateCode(0x812),
		};
	}

	struct K64GenericRequestResult_t {
		INT ErrorCode;
		DWORD_PTR LastApiErrorCode;
		BOOLEAN HasHandledError;
		BOOLEAN DidComplete;
	};

	struct K64CallEntryPoint_t
	{
		DWORD_PTR Address;

		PVOID Rcx;
		PVOID Rdx;
		PVOID R8;
		PVOID R9;
	};

	struct K64MmpSessionBeginData_t
	{
		DWORD32 ProcessId;
	};

	struct K64AllocatePool_t
	{
		INT Type;
		UINT64 Length;
	};

	struct K64CopyVMem_t
	{
		DWORD_PTR StartAddress;
		DWORD_PTR Size;
		PVOID Buffer;
	};

	struct K64ReadVirtualMemory_t
	{
		BOOLEAN InKernel;
		DWORD32 ProcessId;
		DWORD_PTR Source;
		PVOID Destination;
		UINT64 Length;
	};

	struct K64WriteVirtualMemory_t
	{
		BOOLEAN InKernel;
		DWORD32 ProcessId;
		PVOID Source;
		DWORD_PTR Destination;
		UINT64 Length;
	};

	struct K64MemBuf_t
	{
		PVOID Pointer;
		SIZE_T Size;
	};

	struct K64WideString_t
	{
		WCHAR Buffer[512];
	};

	struct K64StatusInfo_t {
		INT CurrentState;
		BOOLEAN ManuallyMapped;
	};

	struct K64MemBufRequestResult_t : K64GenericRequestResult_t
	{
		BOOLEAN RequestSize;
		SIZE_T BufferLength;
		PVOID BufferPointer;
	};

	struct K64CallEntryPointResult_t : K64GenericRequestResult_t
	{
		NTSTATUS ReturnValue;
	};

	struct K64KernelModule_t
	{
		BOOLEAN WasAssigned;
		DWORD_PTR ImageBase;
		DWORD_PTR ImageSize;
		UCHAR Name[256];
	};

	struct K64AddressRange_t
	{
		DWORD_PTR dwMin;
		DWORD_PTR dwMax;
	};

	struct K64ModuleInfo_t
	{
		DWORD_PTR dwBase;
		DWORD_PTR dwSize;
	};

	struct K64KernelModules_t : K64GenericRequestResult_t
	{
		SIZE_T WriteIndex;
		K64KernelModule_t Modules[1024];
	};

	struct K64MmpTargetModule_t
	{
		BOOLEAN WasAssigned;
		DWORD_PTR ImageBase;
		DWORD_PTR ImageSize;
		PCWCH BaseName[256];
	};

	struct K64MmpTargetModules_t : K64GenericRequestResult_t
	{
		SIZE_T WriteIndex;
		DWORD_PTR HostBase;
		K64MmpTargetModule_t Modules[1024];
	};

	struct K64BooleanExpression_t : K64GenericRequestResult_t
	{
		BOOLEAN Result;
	};

	struct K64AddressExpression_t : K64GenericRequestResult_t
	{
		DWORD_PTR Result;
	};

	struct K64AddressRangeExpression_t : K64GenericRequestResult_t
	{
		DWORD_PTR dwMin;
		DWORD_PTR dwMax;
	};

	struct K64ModuleInfoExpression_t : K64GenericRequestResult_t
	{
		DWORD_PTR dwBase;
		DWORD_PTR dwSize;
	};

	enum K64GenericErrors {
		KGE_NONE,
		KGE_OBJECT_NOT_FOUND,

		KGE_KNOWN_API_FAILURE, // usually an NT STATUS will be passed.
		KGE_UNKNOWN_API_FAILURE, // the error is not known, so we have thing to diagnose with.
		KGE_INVALID_PARAMETERS,
		KGE_INVALID_REQUEST,

		KGE_ACCESS_DENIED,
		KGE_REQUEST_REJECTED,
		KGE_COUNT,
	};
}

class C_HSCTL
{
public:
	C_HSCTL() = default;
	~C_HSCTL() = default;

	auto IsActive() CONST { return m_bActive; }

	C_HSCTL(const std::wstring& deviceName)
	{
		m_hDevice = CreateFileW(fmt::format(L"\\\\.\\{}", deviceName).c_str(), GENERIC_ALL, 0, nullptr, OPEN_EXISTING,
			FILE_ATTRIBUTE_SYSTEM, nullptr);

		if (m_hDevice == nullptr || m_hDevice == INVALID_HANDLE_VALUE)
			return;

		m_bActive = TRUE;
	}

	[[nodiscard]] BOOLEAN GetStatus(comms::K64StatusInfo_t& sData) CONST
	{
		assert(m_bActive);

		if (!Util::DeviceControl(m_hDevice, static_cast<DWORD>(comms::IOC::Code::GET_STATUS), 
			SCYTHE_UNUSED_BUFFER, &sData))
			return FALSE;

		return TRUE;
	}

	[[nodiscard]] BOOLEAN GetVirtualForPhysical(DWORD_PTR dwPhysical, DWORD_PTR* pdwPhysical)
	{
		assert(m_bActive);

		comms::K64AddressExpression_t sInput{}, sOutput{};

		sInput.Result = dwPhysical;

		if (!Util::DeviceControl(m_hDevice, static_cast<DWORD>(comms::IOC::Code::VA_TO_PA),
			&sInput, &sOutput))
			return FALSE;

		*pdwPhysical = sOutput.Result;
		
		return sOutput.DidComplete;
	}

	[[nodiscard]] BOOLEAN GetPhysicalForVirtual(DWORD_PTR dwVirtual, DWORD_PTR* pdwPhysical)
	{
		assert(m_bActive);

		comms::K64AddressExpression_t sInput{}, sOutput{};

		sInput.Result = dwVirtual;

		if (!Util::DeviceControl(m_hDevice, static_cast<DWORD>(comms::IOC::Code::PA_TO_VA),
			&sInput, &sOutput))
			return FALSE;

		*pdwPhysical = sOutput.Result;

		return sOutput.DidComplete;
	}

	[[nodiscard]] PVOID AllocatePool(SIZE_T uLength)
	{
		assert(m_bActive);

		comms::K64AllocatePool_t sInput{};
		comms::K64AddressExpression_t sOutput{};

		sInput.Type = 0; /* NonPagedPool */
		sInput.Length = uLength;

		if (!Util::DeviceControl(m_hDevice, static_cast<DWORD>(comms::IOC::Code::ALLOCATE_POOL),
			&sInput, &sOutput))
			return nullptr;

		if (!sOutput.DidComplete)
			return nullptr;
		
		return reinterpret_cast<PVOID>(sOutput.Result);
	}

	[[nodiscard]] BOOLEAN FreePool(PVOID pPool)
	{
		comms::K64AddressExpression_t sInput{};
		comms::K64GenericRequestResult_t sOutput{};

		sInput.Result = reinterpret_cast<DWORD_PTR>(pPool);

		if (!Util::DeviceControl(m_hDevice, static_cast<DWORD>(comms::IOC::Code::FREE_POOL),
			&sInput, &sOutput))
			return FALSE;

		return sOutput.DidComplete;
	}
	
	[[nodiscard]] BOOLEAN UReadVirtualMemory(DWORD32 dwProcessId, 
		DWORD_PTR dwSource, PVOID pBuffer, SIZE_T uLength)
	{
		assert(m_bActive);

		comms::K64ReadVirtualMemory_t sInput{};
		comms::K64GenericRequestResult_t sOutput{};

		sInput.InKernel = FALSE;
		sInput.ProcessId = dwProcessId;

		sInput.Source = dwSource;
		sInput.Destination = pBuffer;
		sInput.Length = uLength;

		if (!Util::DeviceControl(m_hDevice, static_cast<DWORD>(comms::IOC::Code::READ_VIRTUAL_MEMORY),
			&sInput, &sOutput))
			return FALSE;

		return sOutput.DidComplete;
	}

	[[nodiscard]] BOOLEAN UWriteVirtualMemory(DWORD32 dwProcessId,
		DWORD_PTR dwDest, PVOID pSource, SIZE_T uLength)
	{
		assert(m_bActive);

		comms::K64WriteVirtualMemory_t sInput{};
		comms::K64GenericRequestResult_t sOutput{};

		sInput.InKernel = FALSE;
		sInput.ProcessId = dwProcessId;

		sInput.Source = pSource;
		sInput.Destination = dwDest;
		sInput.Length = uLength;

		if (!Util::DeviceControl(m_hDevice, static_cast<DWORD>(comms::IOC::Code::WRITE_VIRTUAL_MEMORY),
			&sInput, &sOutput))
			return FALSE;

		return sOutput.DidComplete;
	}

	[[nodiscard]] BOOLEAN KReadVirtualMemory(DWORD_PTR dwSource, PVOID pBuffer, SIZE_T uLength)
	{
		assert(m_bActive);

		comms::K64ReadVirtualMemory_t sInput{};
		comms::K64GenericRequestResult_t sOutput{};

		sInput.InKernel = TRUE;
		sInput.ProcessId = 0;

		sInput.Source = dwSource;
		sInput.Destination = pBuffer;
		sInput.Length = uLength;

		if (!Util::DeviceControl(m_hDevice, static_cast<DWORD>(comms::IOC::Code::READ_VIRTUAL_MEMORY),
			&sInput, &sOutput))
			return FALSE;

		return sOutput.DidComplete;
	}

	[[nodiscard]] BOOLEAN KWriteVirtualMemory(DWORD_PTR dwDest, PVOID pSource, SIZE_T uLength)
	{
		assert(m_bActive);

		comms::K64WriteVirtualMemory_t sInput{};
		comms::K64GenericRequestResult_t sOutput{};

		sInput.InKernel = TRUE;
		sInput.ProcessId = 0;

		sInput.Source = pSource;
		sInput.Destination = dwDest;
		sInput.Length = uLength;

		if (!Util::DeviceControl(m_hDevice, static_cast<DWORD>(comms::IOC::Code::WRITE_VIRTUAL_MEMORY),
			&sInput, &sOutput))
			return FALSE;

		return sOutput.DidComplete;
	}

	[[nodiscard]] BOOLEAN UFillVirtualMemory(DWORD32 dwProcessId, 
		DWORD_PTR dwDest, UINT8 uData, SIZE_T uLength)
	{
		auto* const pBuffer = malloc(uLength);

		if (pBuffer == nullptr)
			return FALSE;

		RtlFillMemory(pBuffer, uLength, uData);

		const auto bResult = UWriteVirtualMemory(dwProcessId, dwDest, pBuffer, uLength);

		free(pBuffer);

		return bResult;
	}

	[[nodiscard]] BOOLEAN KFillVirtualMemory(DWORD_PTR dwDest, UINT8 uData, SIZE_T uLength)
	{
		auto* const pBuffer = malloc(uLength);

		if (pBuffer == nullptr)
			return FALSE;

		RtlFillMemory(pBuffer, uLength, uData);

		const auto bResult = KWriteVirtualMemory(dwDest, pBuffer, uLength);

		free(pBuffer);

		return bResult;
	}

	[[nodiscard]] BOOLEAN GetSystemRoutineAddress(const std::wstring& pszName, DWORD_PTR* pdwOut)
	{
		assert(m_bActive);

		comms::K64WideString_t sInput{};
		comms::K64AddressExpression_t sOutput{};

		wcscpy_s(sInput.Buffer, pszName.c_str());
		
		if (!Util::DeviceControl(m_hDevice, static_cast<DWORD>(comms::IOC::Code::GET_SYSTEM_ROUTINE_ADDRESS),
			&sInput, &sOutput))
			return FALSE;

		*pdwOut = sOutput.Result;

		return sOutput.DidComplete;
	}

	VOID Release()
	{
		if (!m_bActive)
			return;

		m_bActive = FALSE;

		CloseHandle(m_hDevice);
	}
private:
	BOOLEAN m_bActive{};
	HANDLE m_hDevice{};
};
