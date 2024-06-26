#pragma once

#include <stdarg.h>         // for va_start, etc.

#pragma region Tracing level definitions

#if !defined(FAILED_NTSTATUS)
#define FAILED_NTSTATUS(status) (((NTSTATUS)(status)) < 0)
#endif

#if !defined(SUCCEEDED_NTSTATUS)
#define SUCCEEDED_NTSTATUS(status) (((NTSTATUS)(status)) >= 0)
#endif

//! Define shorter versions of the ETW trace levels
#define LEVEL_CRITICAL  TRACE_LEVEL_CRITICAL
#define LEVEL_ERROR     TRACE_LEVEL_ERROR
#define LEVEL_WARNING   TRACE_LEVEL_WARNING
#define LEVEL_INFO      TRACE_LEVEL_INFORMATION
#define LEVEL_VERBOSE   TRACE_LEVEL_VERBOSE

//! This is a special LEVEL that changes the trace macro level from ERROR to VERBOSE
//! depending on whether the return value passed to the macro was non-zero or zero,
//! respectively.
#define LEVEL_COND 0xFF
#pragma endregion

//! Logger and Enabled that supports both level and flag.
//! \link https://msdn.microsoft.com/en-us/library/windows/hardware/ff542492(v=vs.85).aspx
#define WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS) WPP_LEVEL_LOGGER(FLAGS)
#define WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS) (WPP_LEVEL_ENABLED(FLAGS) && (WPP_CONTROL(WPP_BIT_ ## FLAGS).Level >= LEVEL))

//! This macro is to be used by the WPP custom macros below that want to do conditional
//! logging based on return value. If LEVEL_VERBOSE is specified when calling a macro that
//! uses this, the level will be set to LEVEL_INFO if return code is 0 or
//! LEVEL_ERROR if the return code is not 0. This can be called in any PRE macro.
//!
//! The "LEVEL == LEVEL_COND" check generates a compiler warning that the "conditional
//! expression is constant" so we explicitly disable that.
#define WPP_CONDITIONAL_LEVEL_FLAGS_OVERRIDE(LEVEL, FLAGS, HR) \
    BOOL bEnabled = WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS); \
    __pragma(warning(push)) \
    __pragma(warning(disable: 4127)) \
    if (LEVEL == LEVEL_COND) \
    { \
        if (SUCCEEDED(HR)) \
        { \
            bEnabled = WPP_LEVEL_FLAGS_ENABLED(LEVEL_VERBOSE, FLAGS); \
        } \
        else \
        { \
            bEnabled = WPP_LEVEL_FLAGS_ENABLED(LEVEL_ERROR, FLAGS); \
        } \
    } \
    __pragma(warning(pop))

#define WPP_CONDITIONAL_LEVEL_FLAGS_OVERRIDE_NTSTATUS(LEVEL, FLAGS, STATUS) \
    BOOLEAN bEnabled = WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS); \
    __pragma(warning(push)) \
    __pragma(warning(disable: 4127)) \
    if (LEVEL == LEVEL_COND) \
    { \
        if (SUCCEEDED_NTSTATUS(STATUS)) \
        { \
            bEnabled = WPP_LEVEL_FLAGS_ENABLED(LEVEL_VERBOSE, FLAGS); \
        } \
        else \
        { \
            bEnabled = WPP_LEVEL_FLAGS_ENABLED(LEVEL_ERROR, FLAGS); \
        } \
    } \
    __pragma(warning(pop))


#define WPP_LEVEL_FLAGS_IFRLOG_ENABLED(LEVEL, FLAGS, IFRLOG) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_FLAGS_IFRLOG_LOGGER(LEVEL, FLAGS, IFRLOG) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)
#define WPP_LEVEL_IFRLOG_FLAGS_ENABLED(LEVEL, IFRLOG, FLAGS) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_IFRLOG_FLAGS_LOGGER(LEVEL, IFRLOG, FLAGS) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_HR_PRE(LEVEL, FLAGS, HR) { WPP_CONDITIONAL_LEVEL_FLAGS_OVERRIDE(LEVEL, FLAGS, HR)
#define WPP_LEVEL_FLAGS_HR_POST(LEVEL, FLAGS, HR) ;}
#define WPP_LEVEL_FLAGS_HR_ENABLED(LEVEL, FLAGS, HR) bEnabled
#define WPP_LEVEL_FLAGS_HR_LOGGER(LEVEL, FLAGS, HR) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_RETVAL_ENABLED(LEVEL, FLAGS, RETVAL) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_FLAGS_RETVAL_LOGGER(LEVEL, FLAGS, RETVAL) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_FI_ENABLED(LEVEL, FLAGS, FI) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_FLAGS_FI_LOGGER(LEVEL, FLAGS, FI) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_STATUS_PRE(LEVEL, FLAGS, STATUS) { WPP_CONDITIONAL_LEVEL_FLAGS_OVERRIDE_NTSTATUS(LEVEL, FLAGS, STATUS)
#define WPP_LEVEL_FLAGS_STATUS_POST(LEVEL, FLAGS, STATUS) ;}
#define WPP_LEVEL_FLAGS_STATUS_ENABLED(LEVEL, FLAGS, STATUS) bEnabled
#define WPP_LEVEL_FLAGS_STATUS_LOGGER(LEVEL, FLAGS, STATUS) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_RETSTATUS_PRE(LEVEL, FLAGS, RETSTATUS) do { NTSTATUS __statusRet = (RETSTATUS); if (FAILED_NTSTATUS(__statusRet)) {
#define WPP_LEVEL_FLAGS_RETSTATUS_POST(LEVEL, FLAGS, RETSTATUS) ; return __statusRet; } } while (0, 0)
#define WPP_LEVEL_FLAGS_RETSTATUS_ENABLED(LEVEL, FLAGS, RETSTATUS) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_FLAGS_RETSTATUS_LOGGER(LEVEL, FLAGS, RETSTATUS) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_IFRLOG_RETSTATUS_PRE(LEVEL, FLAGS, IFRLOG, RETSTATUS) do { NTSTATUS __statusRet = (RETSTATUS); if (FAILED_NTSTATUS(__statusRet)) {
#define WPP_LEVEL_FLAGS_IFRLOG_RETSTATUS_POST(LEVEL, FLAGS, IFRLOG, RETSTATUS) ; return __statusRet; } } while (0, 0)
#define WPP_LEVEL_FLAGS_IFRLOG_RETSTATUS_ENABLED(LEVEL, FLAGS, IFRLOG, RETSTATUS) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_FLAGS_IFRLOG_RETSTATUS_LOGGER(LEVEL, FLAGS, IFRLOG, RETSTATUS) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_IFRLOG_RETSTATUS_ALLOWEDSTATUS_PRE(LEVEL, FLAGS, IFRLOG, RETSTATUS, ALLOWEDSTATUS) do {\
NTSTATUS __statusRet = (RETSTATUS);\
if(__statusRet == ALLOWEDSTATUS)\
{\
    __statusRet = STATUS_SUCCESS;\
}\
if (FAILED_NTSTATUS(__statusRet)) {
#define WPP_LEVEL_FLAGS_IFRLOG_RETSTATUS_ALLOWEDSTATUS_POST(LEVEL, FLAGS, IFRLOG, RETSTATUS, ALLOWEDSTATUS) ; return __statusRet; } } while (0, 0)
#define WPP_LEVEL_FLAGS_IFRLOG_RETSTATUS_ALLOWEDSTATUS_ENABLED(LEVEL, FLAGS, IFRLOG, RETSTATUS, ALLOWEDSTATUS) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_FLAGS_IFRLOG_RETSTATUS_ALLOWEDSTATUS_LOGGER(LEVEL, FLAGS, IFRLOG, RETSTATUS, ALLOWEDSTATUS) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_RETPTR_PRE(LEVEL, FLAGS, RETPTR) do { if ((RETPTR) == nullptr) {
#define WPP_LEVEL_FLAGS_RETPTR_POST(LEVEL, FLAGS, RETPTR) ; return STATUS_INSUFFICIENT_RESOURCES; } } while (0, 0)
#define WPP_LEVEL_FLAGS_RETPTR_ENABLED(LEVEL, FLAGS, RETPTR) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_FLAGS_RETPTR_LOGGER(LEVEL, FLAGS, RETPTR) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_RETSTATUS_RETPTR_PRE(LEVEL, FLAGS, RETSTATUS, RETPTR) do { NTSTATUS __statusRet = (RETSTATUS); if ((RETPTR) == nullptr) {
#define WPP_LEVEL_FLAGS_RETSTATUS_RETPTR_POST(LEVEL, FLAGS, RETSTATUS, RETPTR) ; return __statusRet; } } while (0, 0)
#define WPP_LEVEL_FLAGS_RETSTATUS_RETPTR_ENABLED(LEVEL, FLAGS, RETSTATUS, RETPTR) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_FLAGS_RETSTATUS_RETPTR_LOGGER(LEVEL, FLAGS, RETSTATUS, RETPTR) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_RETSTATUS_POSCOND_PRE(LEVEL, FLAGS, RETSTATUS, POSCOND) do { NTSTATUS __statusRet = (RETSTATUS); if ((POSCOND)) {
#define WPP_LEVEL_FLAGS_RETSTATUS_POSCOND_POST(LEVEL, FLAGS, RETSTATUS, POSCOND) ; return __statusRet; } } while (0, 0)
#define WPP_LEVEL_FLAGS_RETSTATUS_POSCOND_ENABLED(LEVEL, FLAGS, RETSTATUS, POSCOND) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_FLAGS_RETSTATUS_POSCOND_LOGGER(LEVEL, FLAGS, RETSTATUS, POSCOND) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_IFRLOG_POSCOND_RETSTATUS_PRE(LEVEL, FLAGS, IFRLOG, POSCOND, RETSTATUS) do { NTSTATUS __statusRet = (RETSTATUS); if ((POSCOND)) {
#define WPP_LEVEL_FLAGS_IFRLOG_POSCOND_RETSTATUS_POST(LEVEL, FLAGS, IFRLOG, POSCOND, RETSTATUS) ; return __statusRet; } } while (0, 0)
#define WPP_LEVEL_FLAGS_IFRLOG_POSCOND_RETSTATUS_ENABLED(LEVEL, FLAGS, IFRLOG, POSCOND, RETSTATUS) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_FLAGS_IFRLOG_POSCOND_RETSTATUS_LOGGER(LEVEL, FLAGS, IFRLOG, POSCOND, RETSTATUS) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#define WPP_LEVEL_FLAGS_RETSTATUS_NEGCOND_PRE(LEVEL, FLAGS, RETSTATUS, NEGCOND) do { NTSTATUS __statusRet = (RETSTATUS); if (!(NEGCOND)) {
#define WPP_LEVEL_FLAGS_RETSTATUS_NEGCOND_POST(LEVEL, FLAGS, RETSTATUS, NEGCOND) ; return __statusRet; } } while (0, 0)
#define WPP_LEVEL_FLAGS_RETSTATUS_NEGCOND_ENABLED(LEVEL, FLAGS, RETSTATUS, NEGCOND) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)
#define WPP_LEVEL_FLAGS_RETSTATUS_NEGCOND_LOGGER(LEVEL, FLAGS, RETSTATUS, NEGCOND) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)

#pragma region IFR Enablement Macros

// Opt-in to a WPP recorder feature that enables independent evaluation of conditions to decide if a
// message needs to be sent to the recorder, an enabled session, or both.
#define ENABLE_WPP_TRACE_FILTERING_WITH_WPP_RECORDER 1

// Logger/Enabled macros used to decide if a message that is being sent to a custom recorder should
// also go to an enabled session. These do not depend on the custom recorder itself, so just
// delegate to the default.
#define WPP_IFRLOG_LEVEL_FLAGS_LOGGER(IFRLOG, LEVEL, FLAGS) WPP_LEVEL_FLAGS_LOGGER(LEVEL, FLAGS)
#define WPP_IFRLOG_LEVEL_FLAGS_ENABLED(IFRLOG, LEVEL, FLAGS) WPP_LEVEL_FLAGS_ENABLED(LEVEL, FLAGS)

#define WPP_RECORDER_CONDITIONAL_LEVEL_FLAGS_OVERRIDE(LEVEL, FLAGS, HR) \
    ((LEVEL == LEVEL_COND) ? \
        (FAILED(HR) ? \
                WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL_ERROR, FLAGS) : WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL_VERBOSE, FLAGS)) : \
            WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS))

#define WPP_RECORDER_CONDITIONAL_LEVEL_FLAGS_OVERRIDE_NTSTATUS(LEVEL, FLAGS, STATUS) \
    ((LEVEL == LEVEL_COND) ? \
        (FAILED_NTSTATUS(STATUS) ? \
                WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL_ERROR, FLAGS) : WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL_VERBOSE, FLAGS)) : \
            WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS))

#define WPP_RECORDER_LEVEL_FLAGS_HR_ARGS(LEVEL, FLAGS, RETVAL) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_HR_FILTER(LEVEL, FLAGS, RETVAL) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)

#define WPP_RECORDER_LEVEL_FLAGS_RETVAL_ARGS(LEVEL, FLAGS, RETVAL) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_RETVAL_FILTER(LEVEL, FLAGS, RETVAL) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)

#define WPP_RECORDER_LEVEL_FLAGS_FI_ARGS(LEVEL, FLAGS, RETVAL) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_FI_FILTER(LEVEL, FLAGS, RETVAL) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)

#define WPP_RECORDER_LEVEL_FLAGS_STATUS_ARGS(LEVEL, FLAGS, STATUS) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_STATUS_FILTER(LEVEL, FLAGS, STATUS) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)

#define WPP_RECORDER_LEVEL_FLAGS_RETSTATUS_ARGS(LEVEL, FLAGS, RETSTATUS) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_RETSTATUS_FILTER(LEVEL, FLAGS, RETSTATUS) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)

#define WPP_RECORDER_LEVEL_FLAGS_IFRLOG_RETSTATUS_ARGS(LEVEL, FLAGS, IFRLOG, RETSTATUS) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_IFRLOG_RETSTATUS_FILTER(LEVEL, FLAGS, IFRLOG, RETSTATUS) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)

#define WPP_RECORDER_LEVEL_FLAGS_IFRLOG_RETSTATUS_ALLOWEDSTATUS_ARGS(LEVEL, FLAGS, IFRLOG, RETSTATUS, ALLOWEDSTATUS) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_IFRLOG_RETSTATUS_ALLOWEDSTATUS_FILTER(LEVEL, FLAGS, IFRLOG, RETSTATUS, ALLOWEDSTATUS) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)

#define WPP_RECORDER_LEVEL_FLAGS_RETPTR_ARGS(LEVEL, FLAGS, RETPTR) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_RETPTR_FILTER(LEVEL, FLAGS, RETPTR) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)

#define WPP_RECORDER_LEVEL_FLAGS_RETSTATUS_RETPTR_ARGS(LEVEL, FLAGS, RETSTATUS, RETPTR) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_RETSTATUS_RETPTR_FILTER(LEVEL, FLAGS, RETSTATUS, RETPTR) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)

#define WPP_RECORDER_LEVEL_FLAGS_RETSTATUS_POSCOND_ARGS(LEVEL, FLAGS, RETSTATUS, POSCOND) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_RETSTATUS_POSCOND_FILTER(LEVEL, FLAGS, RETSTATUS, POSCOND) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)

#define WPP_RECORDER_LEVEL_FLAGS_IFRLOG_POSCOND_RETSTATUS_ARGS(LEVEL, FLAGS, IFRLOG, RETSTATUS, POSCOND) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_IFRLOG_POSCOND_RETSTATUS_FILTER(LEVEL, FLAGS, IFRLOG, RETSTATUS, POSCOND) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)

#define WPP_RECORDER_LEVEL_FLAGS_RETSTATUS_NEGCOND_ARGS(LEVEL, FLAGS, RETSTATUS, NEGCOND) WPP_RECORDER_LEVEL_FLAGS_ARGS(LEVEL, FLAGS)
#define WPP_RECORDER_LEVEL_FLAGS_RETSTATUS_NEGCOND_FILTER(LEVEL, FLAGS, RETSTATUS, NEGCOND) WPP_RECORDER_LEVEL_FLAGS_FILTER(LEVEL, FLAGS)
#pragma endregion

#pragma region Custom tracing macros

// begin_wpp config
// USEPREFIX(DrvLogCritical,  "%!STDPREFIX!CRIT: ");
// USEPREFIX(DrvLogError,     "%!STDPREFIX!ERROR: ");
// USEPREFIX(DrvLogWarning,   "%!STDPREFIX!WARN: ");
// USEPREFIX(DrvLogInfo,      "%!STDPREFIX!INFO: ");
// USEPREFIX(DrvLogVerbose,   "%!STDPREFIX!VERB: ");
// USEPREFIX(DrvLogEnter,     "%!STDPREFIX!ENTER");
// USEPREFIX(DrvLogExit,      "%!STDPREFIX!EXIT");
// end_wpp

// begin_wpp config
// FUNC DrvLogCritical{LEVEL=TRACE_LEVEL_CRITICAL}(IFRLOG,FLAGS,MSG,...);
// FUNC DrvLogError{LEVEL=TRACE_LEVEL_ERROR}(IFRLOG,FLAGS,MSG,...);
// FUNC DrvLogWarning{LEVEL=TRACE_LEVEL_WARNING}(IFRLOG,FLAGS,MSG,...);
// FUNC DrvLogInfo{LEVEL=TRACE_LEVEL_INFORMATION}(IFRLOG,FLAGS,MSG,...);
// FUNC DrvLogEnter{LEVEL=TRACE_LEVEL_VERBOSE,FLAGS=FLAG_FUNCTION}(IFRLOG,...);
// FUNC DrvLogVerbose{LEVEL=TRACE_LEVEL_VERBOSE}(IFRLOG,FLAGS,MSG,...);
// FUNC DrvLogExit{LEVEL=TRACE_LEVEL_VERBOSE,FLAGS=FLAG_FUNCTION}(IFRLOG,...);
// end_wpp


#ifdef __INTELLISENSE__
#define FLAG_DEVICE_ALL 0x01
#define FLAG_FUNCTION   0x02
#define FLAG_INFO       0x04
#define FLAG_PNP        0x08
#define FLAG_POWER      0x10
#define FLAG_STREAM     0x20
#define FLAG_INIT       0x40
#define FLAG_DDI        0x80
#define FLAG_GENERIC    0x100
void DrvLogCritical(void* log, int flags, const WCHAR* fmt, ...);
void DrvLogError(void* log, int flags, const WCHAR* fmt, ...);
void DrvLogWarning(void* log, int flags, const WCHAR* fmt, ...);
void DrvLogInfo(void* log, int flags, const WCHAR* fmt, ...);
void DrvLogEnter(void* log, ...);
void DrvLogVerbose(void* log, int flags, const WCHAR* fmt, ...);
void DrvLogExit(void* log, ...);

void RETURN_IF_FAILED(NTSTATUS status);
void RETURN_NTSTATUS_IF_FAILED(NTSTATUS status);
void RETURN_NTSTATUS_IF_FAILED_MSG(NTSTATUS status, const WCHAR* fmt, ...);
void RETURN_NTSTATUS_IF_FAILED_UNLESS_ALLOWED(NTSTATUS returnStatus, NTSTATUS allowedStatus);
void RETURN_NTSTATUS_IF_NULL_ALLOC(PVOID ptr);
void RETURN_NTSTATUS_IF_NULL(PVOID ptr);
void RETURN_NTSTATUS_IF_TRUE(BOOL condition, NTSTATUS status);
void RETURN_NTSTATUS_IF_TRUE_MSG(BOOL condition, NTSTATUS status, const WCHAR* fmt, ...);
void RETURN_NTSTATUS_IF_FALSE(BOOL condition, NTSTATUS status);
void RETURN_NTSTATUS(NTSTATUS status);
void RETURN_NTSTATUS_MSG(NTSTATUS status, const WCHAR* fmt, ...);
#endif// __INTELLISENSE__

//*********************************************************
// MACRO: TRACE_METHOD_LINE
//
// begin_wpp config
// FUNC TRACE_METHOD_LINE(LEVEL, FLAGS, MSG, ...);
// USESUFFIX (TRACE_METHOD_LINE, ", this=0x%p", this);
// end_wpp

//*********************************************************
// MACRO: TRACE_METHOD_ENTRY
//
// begin_wpp config
// FUNC TRACE_METHOD_ENTRY(LEVEL, FLAGS);
// USESUFFIX (TRACE_METHOD_ENTRY, "Enter, this=0x%p", this);
// end_wpp

//*********************************************************
// MACRO: TRACE_METHOD_EXIT
//
// begin_wpp config
// FUNC TRACE_METHOD_EXIT(LEVEL, FLAGS);
// USESUFFIX (TRACE_METHOD_EXIT, "Exit, this=0x%p", this);
// end_wpp

//*********************************************************
// MACRO: TRACE_METHOD_EXIT_HR
//
// begin_wpp config
// FUNC TRACE_METHOD_EXIT_HR(LEVEL, FLAGS, HR);
// USESUFFIX (TRACE_METHOD_EXIT_HR, "Exit, this=0x%p, hr=%!HRESULT!", this, HR);
// end_wpp

//*********************************************************
// MACRO: TRACE_METHOD_EXIT_DWORD
//
// begin_wpp config
// FUNC TRACE_METHOD_EXIT_DWORD(LEVEL, FLAGS, RETVAL);
// USESUFFIX (TRACE_METHOD_EXIT_DWORD, "Exit, this=0x%p, ret=0x%08Ix ", this, RETVAL);
// end_wpp

//*********************************************************
// MACRO: TRACE_METHOD_EXIT_PTR
//
// begin_wpp config
// FUNC TRACE_METHOD_EXIT_PTR(LEVEL, FLAGS, RETVAL);
// USESUFFIX (TRACE_METHOD_EXIT_PTR,"Exit, this=0x%p, retptr=0x%p", this, RETVAL);
// end_wpp

//*********************************************************
// MACRO: TRACE_METHOD_EXIT_STATUS
//
// begin_wpp config
// FUNC TRACE_METHOD_EXIT_STATUS(LEVEL, FLAGS, STATUS);
// USESUFFIX (TRACE_METHOD_EXIT_STATUS, "Exit, this=0x%p, status=%!STATUS!", this, STATUS);
// end_wpp

//*********************************************************
// MACRO: TRACE_FUNCTION_ENTRY
//
// begin_wpp config
// FUNC TRACE_FUNCTION_ENTRY(LEVEL, FLAGS);
// USESUFFIX (TRACE_FUNCTION_ENTRY, "Enter");
// end_wpp

//*********************************************************
// MACRO: TRACE_FUNCTION_EXIT
//
// begin_wpp config
// FUNC TRACE_FUNCTION_EXIT(LEVEL, FLAGS);
// USESUFFIX (TRACE_FUNCTION_EXIT, "Exit");
// end_wpp

//*********************************************************
// MACRO: TRACE_FUNCTION_EXIT_HR
//
// begin_wpp config
// FUNC TRACE_FUNCTION_EXIT_HR(LEVEL, FLAGS, HR);
// USESUFFIX (TRACE_FUNCTION_EXIT_HR, "Exit, hr=%!HRESULT!", HR);
// end_wpp

//*********************************************************
// MACRO: TRACE_FUNCTION_EXIT_DWORD
//
// begin_wpp config
// FUNC TRACE_FUNCTION_EXIT_DWORD(LEVEL, FLAGS, RETVAL);
// USESUFFIX (TRACE_FUNCTION_EXIT_DWORD, "Exit, ret=0x%08Ix", RETVAL);
// end_wpp

//*********************************************************
// MACRO: TRACE_FUNCTION_EXIT_PTR
//
// begin_wpp config
// FUNC TRACE_FUNCTION_EXIT_PTR(LEVEL, FLAGS, RETVAL);
// USESUFFIX (TRACE_FUNCTION_EXIT_PTR, "Exit, retptr=0x%p", RETVAL);
// end_wpp

//*********************************************************
// MACRO: TRACE_FUNCTION_EXIT_STATUS
//
// begin_wpp config
// FUNC TRACE_FUNCTION_EXIT_STATUS(LEVEL, FLAGS, STATUS);
// USESUFFIX (TRACE_FUNCTION_EXIT_STATUS, "Exit, status=%!STATUS!", STATUS);
// end_wpp

//*********************************************************
// MACRO: TRACE_LINE
//
// begin_wpp config
// FUNC TRACE_LINE(LEVEL, FLAGS, MSG, ...);
// end_wpp

//*********************************************************
// MACRO: TRACE_HRESULT
//
// begin_wpp config
// FUNC TRACE_HRESULT(LEVEL, FLAGS, HR, MSG, ...);
// USESUFFIX (TRACE_HRESULT, ", ret=%!HRESULT!", HR);
// end_wpp

//*********************************************************
// MACRO: TRACE_FAILURE_INFO (WIL FailureInfo logging)
// see: https://github.com/microsoft/wil/blob/master/include/wil/result_macros.h
//
// begin_wpp config
// FUNC TRACE_FAILURE_INFO(LEVEL, FLAGS, FI);
// USESUFFIX(TRACE_FAILURE_INFO, " [%04X] '%ws', hr=%!HRESULT! ['%s' (%u)]", FI.threadId, FI.pszMessage, FI.hr, FI.pszFile, FI.uLineNumber);
// end_wpp

// MACRO: RETURN_IF_FAILED
//
// begin_wpp config
// FUNC RETURN_IF_FAILED{LEVEL=LEVEL_ERROR,FLAGS=FLAG_DEVICE_ALL,IFRLOG=g_AudioDspLog}(RETSTATUS);
// USEPREFIX(RETURN_IF_FAILED, "%!STDPREFIX!ERROR:");
// USESUFFIX(RETURN_IF_FAILED, " File:%s, Line:%d - status=%!STATUS!", __FILE__, __LINE__, __statusRet);
// end_wpp


// MACRO: RETURN_NTSTATUS_IF_FAILED
//
// begin_wpp config
// FUNC RETURN_NTSTATUS_IF_FAILED{LEVEL=LEVEL_ERROR,FLAGS=FLAG_DEVICE_ALL,IFRLOG=g_AudioDspLog}(RETSTATUS);
// USEPREFIX(RETURN_NTSTATUS_IF_FAILED, "%!STDPREFIX!ERROR:");
// USESUFFIX(RETURN_NTSTATUS_IF_FAILED, " File:%s, Line:%d - status=%!STATUS!", __FILE__, __LINE__, __statusRet);
// end_wpp

// MACRO: RETURN_NTSTATUS_IF_FAILED_MSG
//
// begin_wpp config
// FUNC RETURN_NTSTATUS_IF_FAILED_MSG{LEVEL=LEVEL_ERROR,FLAGS=FLAG_DEVICE_ALL,IFRLOG=g_AudioDspLog}(RETSTATUS, MSG, ...);
// USEPREFIX(RETURN_NTSTATUS_IF_FAILED_MSG, "%!STDPREFIX!ERROR:");
// USESUFFIX(RETURN_NTSTATUS_IF_FAILED_MSG, " - status=%!STATUS!",__statusRet);
// end_wpp

// MACRO: RETURN_NTSTATUS_IF_FAILED_UNLESS_ALLOWED
//
// begin_wpp config
// FUNC RETURN_NTSTATUS_IF_FAILED_UNLESS_ALLOWED{LEVEL=LEVEL_ERROR,FLAGS=FLAG_DEVICE_ALL,IFRLOG=g_AudioDspLog}(RETSTATUS, ALLOWEDSTATUS);
// USEPREFIX(RETURN_NTSTATUS_IF_FAILED_UNLESS_ALLOWED, "%!STDPREFIX!ERROR:");
// USESUFFIX(RETURN_NTSTATUS_IF_FAILED_UNLESS_ALLOWED, " File:%s, Line:%d - status=%!STATUS!", __FILE__, __LINE__, __statusRet);
// end_wpp

// MACRO: RETURN_NTSTATUS_IF_NULL_ALLOC
//
// begin_wpp config
// FUNC RETURN_NTSTATUS_IF_NULL_ALLOC{LEVEL=LEVEL_ERROR,FLAGS=DUMMY}(RETPTR);
// USESUFFIX(RETURN_NTSTATUS_IF_NULL, "status=STATUS_INSUFFICIENT_RESOURCES");
// end_wpp

// MACRO: RETURN_NTSTATUS_IF_NULL
//
// begin_wpp config
// FUNC RETURN_NTSTATUS_IF_NULL{LEVEL=LEVEL_ERROR,FLAGS=DUMMY}(RETSTATUS, RETPTR);
// USESUFFIX(RETURN_NTSTATUS_IF_NULL, "status=%!STATUS!", __statusRet);
// end_wpp

// MACRO: RETURN_NTSTATUS_IF_TRUE
//
// begin_wpp config
// FUNC RETURN_NTSTATUS_IF_TRUE{LEVEL=LEVEL_ERROR,FLAGS=FLAG_DEVICE_ALL,IFRLOG=g_AudioDspLog}(POSCOND, RETSTATUS);
// USESUFFIX(RETURN_NTSTATUS_IF_TRUE, " File:%s, Line:%d - status=%!STATUS!", __FILE__, __LINE__, __statusRet);
// end_wpp

// MACRO: RETURN_NTSTATUS_IF_TRUE_MSG
//
// begin_wpp config
// FUNC RETURN_NTSTATUS_IF_TRUE_MSG{LEVEL=LEVEL_ERROR,FLAGS=FLAG_DEVICE_ALL,IFRLOG=g_AudioDspLog}(POSCOND, RETSTATUS, MSG, ...);
// USESUFFIX(RETURN_NTSTATUS_IF_TRUE_MSG, " - status=%!STATUS!", __statusRet);
// end_wpp

// MACRO: RETURN_NTSTATUS_IF_FALSE
//
// begin_wpp config
// FUNC RETURN_NTSTATUS_IF_FALSE{LEVEL=LEVEL_ERROR,FLAGS=DUMMY}(RETSTATUS, NEGCOND);
// USESUFFIX(RETURN_NTSTATUS_IF_FALSE, " File:%s, Line:%d - status=%!STATUS!", __FILE__, __LINE__, __statusRet);
// end_wpp

// MACRO: RETURN_NTSTATUS
//
// begin_wpp config
// FUNC RETURN_NTSTATUS{LEVEL=LEVEL_ERROR,FLAGS=FLAG_DEVICE_ALL,IFRLOG=g_AudioDspLog}(RETSTATUS);
// USESUFFIX(RETURN_NTSTATUS, " File:%s, Line:%d - status=%!STATUS!", __FILE__, __LINE__, __statusRet);
// end_wpp

// MACRO: RETURN_NTSTATUS_MSG
//
// begin_wpp config
// FUNC RETURN_NTSTATUS_MSG{LEVEL=LEVEL_ERROR,FLAGS=FLAG_DEVICE_ALL,IFRLOG=g_AudioDspLog}(RETSTATUS, MSG, ...);
// USESUFFIX(RETURN_NTSTATUS_MSG, " - status=%!STATUS!", __statusRet);
// end_wpp

#define W32
#define WPP_CHECK_FOR_NULL_STRING  //to prevent exceptions due to NULL strings

#pragma endregion
