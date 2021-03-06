/*********************************************************/
/* WinSed, von LoRd_MuldeR <MuldeR2@GMX.de>.             */
/* Umbenannt von Replace auf Grund des Windows eigenen   */
/* Replace-Befehls zum Ersetzen von Dateien.             */
/* Deutsche Version von Manfred Müller <info@nass-ek.de> */
/* Veröffentlicht unter der CC0 1.0 Universal-Lizenz!    */
/*********************************************************/

#ifndef INC_LIBREPLACE_H
#define INC_LIBREPLACE_H

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#endif

#define LIBREPLACE_VERSION_MAJOR 1
#define LIBREPLACE_VERSION_MINOR 8
#define LIBREPLACE_VERSION_PATCH 0

#define LIBREPLACE_FLUSH    ((WORD)-1)
#define LIBREPLACE_WILDCARD ((WORD)MAXWORD)
#define LIBREPLACE_MAXLEN   ((DWORD)(MAXDWORD >> 1))

typedef BOOL (*libreplace_rd_func_t)(BYTE *const data, const DWORD_PTR context, BOOL *const error_flag);
typedef BOOL (*libreplace_wr_func_t)(const WORD data, const DWORD_PTR context);

typedef struct libreplace_io_t
{
	libreplace_rd_func_t func_rd;
	libreplace_wr_func_t func_wr;
	DWORD_PTR context_rd;
	DWORD_PTR context_wr;
}
libreplace_io_t;

typedef BOOL (*libreplace_logging_func_t)(const DWORD_PTR context, const CHAR *const text);

typedef struct libreplace_logger_t
{
	libreplace_logging_func_t logging_func;
	DWORD_PTR context;
}
libreplace_logger_t;

typedef struct libreplace_flags_t
{ 
	BOOL case_insensitive;
	BOOL normalize;
	BOOL replace_once;
	BOOL dry_run;
	BOOL match_crlf;
	BOOL verbose;
}
libreplace_flags_t;

BOOL libreplace_search_and_replace(const libreplace_io_t *const io_functions, const libreplace_logger_t *const logger, const WORD *const needle, const DWORD needle_len, const BYTE *const replacement, const DWORD replacement_len, const libreplace_flags_t *const options, DWORD *const replacement_count, volatile BOOL *const abort_flag);

#endif /*INC_LIBREPLACE_H*/
