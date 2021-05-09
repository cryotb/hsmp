#pragma once

namespace ioctl_handler
{
	EXTERN VOID GetStatus(PIRP Irp,
		NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused,
		BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer);
}
