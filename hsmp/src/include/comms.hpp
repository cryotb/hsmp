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
			GET_SUMMARY = TranslateCode(0x801),
			READ_MEMORY = TranslateCode(0x802),
			WRITE_MEMORY = TranslateCode(0x803),
		};
	}

	EXTERN BOOLEAN Setup(PDRIVER_OBJECT DriverObject, 
		PDEVICE_OBJECT& pDeviceOut, LPCWSTR szDeviceName, LPCWSTR szSymbolName);
	EXTERN VOID Shutdown(PDEVICE_OBJECT DeviceObject);

	EXTERN NTSTATUS OnMajorFunctions(PDEVICE_OBJECT Object, PIRP Irp);
	EXTERN NTSTATUS OnDeviceControl(PDEVICE_OBJECT Object, PIRP Irp);
}
