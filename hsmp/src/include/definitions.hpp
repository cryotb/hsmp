#pragma once

#define AUTO auto
#define INLINE inline
#define EXTERN extern

#if defined(_DEBUG)
#define HS_LOG_LEGACY(FMT, ...) DbgPrint(FMT, __VA_ARGS__); DbgPrint(_XS("\n"))
#define HS_LOG_CALL() DbgPrint(_XS("CALL -> %s(...). \n"), _XS(__FUNCTION__))
#else
#define HS_LOG_LEGACY(FMT, ...) __nop()
#define HS_LOG_CALL() __nop()
#endif

#define IOC_CALL_HANDLER(X) X(Irp, ioOpResult, isInputBufferUnused, isOutputBufferUnused, pInputBuffer, pOutputBuffer)
