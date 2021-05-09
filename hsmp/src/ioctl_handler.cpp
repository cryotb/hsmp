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
