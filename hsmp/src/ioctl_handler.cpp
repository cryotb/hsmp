#include <include.hpp>

VOID ioctl_handler::GetStatus(PIRP Irp,
	NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused,
	BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer)
{
	// ...
}
