#include <include.hpp>

BOOLEAN comms::Setup(PDRIVER_OBJECT DriverObject, PDEVICE_OBJECT& pDeviceOut, 
	PUNICODE_STRING sDeviceName, PUNICODE_STRING sSymbolName)
{
	RtlDuplicateUnicodeString(0, sSymbolName, &m_SymbolName);

	CONST AUTO DeviceCreationResult = util::CreateDevice(DriverObject, &pDeviceOut, sDeviceName,
		FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN);

	if (DeviceCreationResult != STATUS_SUCCESS)
	{
		HS_LOG_LEGACY("failed device creation: %x.", DeviceCreationResult);
		return FALSE;
	}

	pDeviceOut->Flags |= DO_DIRECT_IO;
	pDeviceOut->Flags &= ~DO_DEVICE_INITIALIZING;

	CONST AUTO SymlinkCreationResult = util::CreateSymLink(sDeviceName, sSymbolName);

	if (SymlinkCreationResult != STATUS_SUCCESS)
	{
		HS_LOG_LEGACY("failed symlink creation: %x.", SymlinkCreationResult);
		return FALSE;
	}

	HS_LOG_LEGACY("comms have been set up.");

	return TRUE;
}

VOID comms::Shutdown(PDEVICE_OBJECT DeviceObject)
{
	CONST AUTO SymlinkDeletetionResult = IoDeleteSymbolicLink(&m_SymbolName);

	if (SymlinkDeletetionResult != STATUS_SUCCESS)
		HS_LOG_LEGACY("failed symlink deletion: %x.", SymlinkDeletetionResult);

	IoDeleteDevice(DeviceObject);

	HS_LOG_LEGACY("comms have been shut down.");
}

NTSTATUS comms::OnMajorFunctions(PDEVICE_OBJECT Object, PIRP Irp)
{
	if (Object == nullptr || Irp == nullptr)
		return STATUS_INVALID_PARAMETER;
	
	HS_LOG_LEGACY("onMajorFunctions event was fired.");

	__try
	{
		AUTO* CONST pIoStackLocation = IoGetCurrentIrpStackLocation(Irp);

		if (pIoStackLocation == nullptr)
		{
			return STATUS_INVALID_PARAMETER;
		}

		switch (pIoStackLocation->MajorFunction)
		{
		case IRP_MJ_CREATE:
			HS_LOG_LEGACY("some usermode program has acquired an handle to our comms interface.");
			break;
		case IRP_MJ_CLOSE:
			HS_LOG_LEGACY("some usermode program has released it's handle to our comms interface.");
			break;
		default:
			break;
		}

		AUTO* CONST irpIoSb = &Irp->IoStatus;

		if (irpIoSb == nullptr)
		{
			return STATUS_INVALID_PARAMETER;
		}

		irpIoSb->Information = 0;
		irpIoSb->Status = STATUS_SUCCESS;

		IoCompleteRequest(Irp, IO_NO_INCREMENT);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		HS_LOG_LEGACY("onMajorFunctions -> exception was thrown and handled.");
	}

	return STATUS_SUCCESS;
}

NTSTATUS comms::OnDeviceControl(PDEVICE_OBJECT Object, PIRP Irp)
{
	NTSTATUS ioOpResult = STATUS_SUCCESS;

	HS_LOG_LEGACY("onDeviceControl -> began handling request.");

	if (Object == nullptr || Irp == nullptr)
		return STATUS_INVALID_PARAMETER;

	AUTO* CONST pStackLocation = IoGetCurrentIrpStackLocation(Irp);

	if (pStackLocation == nullptr)
	{
		HS_LOG_LEGACY("onDeviceControl -> failed to determine stack location.");

		ioOpResult = STATUS_INVALID_PARAMETER;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		return STATUS_INVALID_PARAMETER;
	}

	VOID* pInputBuffer = pStackLocation->Parameters.DeviceIoControl.Type3InputBuffer;
	VOID* pOutputBuffer = Irp->UserBuffer;

	if (pInputBuffer == nullptr || pOutputBuffer == nullptr)
	{
		HS_LOG_LEGACY(
			"onDeviceControl -> invalid buffers have been passed, discarding request. (%llx, %llx)",
			Memory::Address(pInputBuffer).Base(), Memory::Address(pOutputBuffer).Base());

		ioOpResult = STATUS_INVALID_PARAMETER;

		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return STATUS_INVALID_PARAMETER;
	}

	CONST BOOLEAN isInputBufferUnused = (Memory::Address(pInputBuffer).Base() == SCYTHE_UNUSED_BUFFER);
	CONST BOOLEAN isOutputBufferUnused = (Memory::Address(pOutputBuffer).Base() == SCYTHE_UNUSED_BUFFER);

	switch (static_cast<IOC::Code>(pStackLocation->Parameters.DeviceIoControl.IoControlCode))
	{
	case IOC::Code::GET_STATUS:
		IOC_CALL_HANDLER(ioctl_handler::GetStatus);
		break;
	default:
		HS_LOG_LEGACY(
			"onDeviceControl -> discarding invalid request, "
			"could not recognize the code it was requested with."
		);

		ioOpResult = STATUS_NOT_SUPPORTED;
		break;
	}

	Irp->IoStatus.Status = ioOpResult;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
