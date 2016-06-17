#ifndef MARPAEBNF_INTERNAL_LOGGING_H
#define MARPAEBNF_INTERNAL_LOGGING_H

#ifndef MARPAEBNF_NTRACE
#define MARPAEBNF_TRACEF(genericLoggerp, funcs, fmts, ...) do { if ((genericLoggerp) != NULL) { GENERICLOGGER_TRACEF(genericLoggerp, "[%s] " fmts, funcs, __VA_ARGS__); } } while (0)
#define MARPAEBNF_TRACE(genericLoggerp, funcs, msgs)       do { if ((genericLoggerp) != NULL) { GENERICLOGGER_TRACEF(genericLoggerp, "[%s] %s", funcs, msgs); } } while (0)
#else
#define MARPAEBNF_TRACEF(genericLoggerp, funcs, fmts, ...)
#define MARPAEBNF_TRACE(genericLoggerp, funcs, msgs)
#endif

#define MARPAEBNF_DEBUGF(genericLoggerp, fmts, ...)     do { if ((genericLoggerp) != NULL) { GENERICLOGGER_DEBUGF    ((genericLoggerp), (fmts), __VA_ARGS__); } } while (0)
#define MARPAEBNF_DEBUG(genericLoggerp, ...)            do { if ((genericLoggerp) != NULL) { GENERICLOGGER_DEBUG     ((genericLoggerp),         __VA_ARGS__); } } while (0)
#define MARPAEBNF_INFOF(genericLoggerp, fmts, ...)      do { if ((genericLoggerp) != NULL) { GENERICLOGGER_INFOF     ((genericLoggerp), (fmts), __VA_ARGS__); } } while (0)
#define MARPAEBNF_INFO(genericLoggerp, ...)             do { if ((genericLoggerp) != NULL) { GENERICLOGGER_INFO      ((genericLoggerp),         __VA_ARGS__); } } while (0)
#define MARPAEBNF_NOTICEF(genericLoggerp, fmts, ...)    do { if ((genericLoggerp) != NULL) { GENERICLOGGER_NOTICEF   ((genericLoggerp), (fmts), __VA_ARGS__); } } while (0)
#define MARPAEBNF_NOTICE(genericLoggerp, ...)           do { if ((genericLoggerp) != NULL) { GENERICLOGGER_NOTICE    ((genericLoggerp),         __VA_ARGS__); } } while (0)
#define MARPAEBNF_WARNF(genericLoggerp, fmts, ...)      do { if ((genericLoggerp) != NULL) { GENERICLOGGER_WARNF     ((genericLoggerp), (fmts), __VA_ARGS__); } } while (0)
#define MARPAEBNF_WARN(genericLoggerp, ...)             do { if ((genericLoggerp) != NULL) { GENERICLOGGER_WARN      ((genericLoggerp),         __VA_ARGS__); } } while (0)
#define MARPAEBNF_ERRORF(genericLoggerp, fmts, ...)     do { if ((genericLoggerp) != NULL) { GENERICLOGGER_ERRORF    ((genericLoggerp), (fmts), __VA_ARGS__); } } while (0)
#define MARPAEBNF_ERROR(genericLoggerp, ...)            do { if ((genericLoggerp) != NULL) { GENERICLOGGER_ERROR     ((genericLoggerp),         __VA_ARGS__); } } while (0)
#define MARPAEBNF_CRITICALF(genericLoggerp, fmts, ...)  do { if ((genericLoggerp) != NULL) { GENERICLOGGER_CRITICALF ((genericLoggerp), (fmts), __VA_ARGS__); } } while (0)
#define MARPAEBNF_CRITICAL(genericLoggerp, ...)         do { if ((genericLoggerp) != NULL) { GENERICLOGGER_CRITICAL  ((genericLoggerp),         __VA_ARGS__); } } while (0)
#define MARPAEBNF_ALERTF(genericLoggerp, fmts, ...)     do { if ((genericLoggerp) != NULL) { GENERICLOGGER_ALERTF    ((genericLoggerp), (fmts), __VA_ARGS__); } } while (0)
#define MARPAEBNF_ALERT(genericLoggerp, ...)            do { if ((genericLoggerp) != NULL) { GENERICLOGGER_ALERT     ((genericLoggerp),         __VA_ARGS__); } } while (0)
#define MARPAEBNF_EMERGENCYF(genericLoggerp, fmts, ...) do { if ((genericLoggerp) != NULL) { GENERICLOGGER_EMERGENCYF((genericLoggerp), (fmts), __VA_ARGS__); } } while (0)
#define MARPAEBNF_EMERGENCY(genericLoggerp, ...)        do { if ((genericLoggerp) != NULL) { GENERICLOGGER_EMERGENCY ((genericLoggerp),         __VA_ARGS__); } } while (0)

#endif /* MARPAEBNF_INTERNAL_LOGGING_H */
