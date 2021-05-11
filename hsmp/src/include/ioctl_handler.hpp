#pragma once

#define IOCTL_ARGS PIRP Irp, NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused, BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer

namespace ioctl_handler
{
	EXTERN VOID GetStatus(IOCTL_ARGS);

	EXTERN VOID AllocatePool(IOCTL_ARGS);
	EXTERN VOID FreePool(IOCTL_ARGS);

	EXTERN VOID MapIoSpace(IOCTL_ARGS);
	EXTERN VOID UnmapIoSpace(IOCTL_ARGS);

	EXTERN VOID VaToPa(IOCTL_ARGS);
	EXTERN VOID PaToVa(IOCTL_ARGS);

	EXTERN VOID GetSystemRoutineAddress(IOCTL_ARGS);

	EXTERN VOID ReadVirtualMemory(IOCTL_ARGS);
	EXTERN VOID WriteVirtualMemory(IOCTL_ARGS);

	EXTERN VOID ReadPhysicalMemory(IOCTL_ARGS);
	EXTERN VOID WritePhysicalMemory(IOCTL_ARGS);
	
	EXTERN VOID CallEntryPoint(IOCTL_ARGS);
}
