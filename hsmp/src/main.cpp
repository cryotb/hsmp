#include <include.hpp>

extern "C"
{
	VOID DriverUnload(_In_ PDRIVER_OBJECT Object)
	{
		HS_LOG_CALL();
	}

	NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT Object, _In_ 
		PUNICODE_STRING RegistryPath)
	{
		HS_LOG_CALL();

		Object->DriverUnload = DriverUnload;

		return STATUS_SUCCESS;
	}
}
