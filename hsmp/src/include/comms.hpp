#pragma once

#define SCYTHE_UNUSED_BUFFER 0xCFD11D1023

namespace comms
{
	inline UNICODE_STRING m_SymbolName{};

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
		};
	}

	struct K64GenericRequestResult_t {
		INT ErrorCode;
		DWORD_PTR LastApiErrorCode;
		BOOLEAN HasHandledError;
		BOOLEAN DidComplete;
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

	EXTERN BOOLEAN Setup(PDRIVER_OBJECT DriverObject, 
		PDEVICE_OBJECT& pDeviceOut, PUNICODE_STRING sDeviceName, PUNICODE_STRING sSymbolName);
	EXTERN VOID Shutdown(PDEVICE_OBJECT DeviceObject);

	EXTERN NTSTATUS OnMajorFunctions(PDEVICE_OBJECT Object, PIRP Irp);
	EXTERN NTSTATUS OnDeviceControl(PDEVICE_OBJECT Object, PIRP Irp);
}
