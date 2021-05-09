#include <include.hpp>

extern "C"
{
	VOID DriverUnload(_In_ PDRIVER_OBJECT Object)
	{
		HS_LOG_CALL();

		comms::Shutdown(G::DeviceObject);
	}

	NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT Object, _In_ 
		PUNICODE_STRING RegistryPath)
	{
		HS_LOG_CALL();

		Object->DriverUnload = DriverUnload;

		Object->MajorFunction[IRP_MJ_CREATE] = comms::OnMajorFunctions;
		Object->MajorFunction[IRP_MJ_CLOSE] = comms::OnMajorFunctions;
		Object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = comms::OnDeviceControl;

		UNICODE_STRING sDeviceName{}, sSymbolName{};

		RtlInitUnicodeString(&sDeviceName, _XSW(L"\\Device\\hsmp"));
		RtlInitUnicodeString(&sSymbolName, _XSW(L"\\DosDevices\\hsmp"));

		if(!comms::Setup(Object, G::DeviceObject,
			&sDeviceName, &sSymbolName))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}
}
