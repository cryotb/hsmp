#pragma once

namespace ioctl_handler
{
	EXTERN VOID GetStatus(PIRP Irp, NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused, BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer);

	EXTERN VOID MapIoSpace(PIRP Irp, NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused, BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer);
	EXTERN VOID UnmapIoSpace(PIRP Irp, NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused, BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer);

	EXTERN VOID VaToPa(PIRP Irp, NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused, BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer);
	EXTERN VOID PaToVa(PIRP Irp, NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused, BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer);

	EXTERN VOID GetSystemRoutineAddress(PIRP Irp, NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused, BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer);

	EXTERN VOID ReadVirtualMemory(PIRP Irp, NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused, BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer);
	EXTERN VOID WriteVirtualMemory(PIRP Irp, NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused, BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer);

	EXTERN VOID ReadPhysicalMemory(PIRP Irp, NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused, BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer);
	EXTERN VOID WritePhysicalMemory(PIRP Irp, NTSTATUS& IoOpStatus, BOOLEAN IsInputBufferUnused, BOOLEAN IsOutputBufferUnused, PVOID pInputBuffer, PVOID pOutputBuffer);
}
