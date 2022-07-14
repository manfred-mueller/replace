/*********************************************************/
/* WinSed, von LoRd_MuldeR <MuldeR2@GMX.de>.             */
/* Umbenannt von Replace auf Grund des Windows eigenen   */
/* Replace-Befehls zum Ersetzen von Dateien.             */
/* Deutsche Version von Manfred Müller <info@nass-ek.de> */
/* Veröffentlicht unter der CC0 1.0 Universal-Lizenz!    */
/*********************************************************/

#include "libreplace/replace.h"
#include "utils.h"
#include "selftest.h"

#include <ShellAPI.h> /*CommandLineToArgvW*/

#include <Windows.h>

#ifdef _DEBUG
#define REPLACE_MAIN(X, ...) X wmain(__VA_ARGS__)
#define _tmain wmain
#else
#define REPLACE_MAIN(X, ...) static X replace_main(__VA_ARGS__)
#define _tmain replace_main
#endif

#define CHECK_ABORT_REQUEST() do \
{ \
	if(g_abort_requested) \
	{ \
		if(!options.return_replace_count) \
		{ \
			result = EXIT_ABORTED; \
		} \
		print_text(std_err, ABORTED_MESSAGE); \
		goto cleanup; \
	} \
} \
while(0)

static const CHAR *const ABORTED_MESSAGE = "Prozess wurde abgebrochen.\n";

/* ======================================================================= */
/* Manpage                                                                 */
/* ======================================================================= */

#define __VERSION_STR__(X, Y, Z) #X "." #Y "." #Z
#define _VERSION_STR_(X, Y, Z) __VERSION_STR__(X, Y, Z)
#define VERSION_STR _VERSION_STR_(LIBREPLACE_VERSION_MAJOR, LIBREPLACE_VERSION_MINOR, LIBREPLACE_VERSION_PATCH)

static void print_manpage(const HANDLE std_err)
{
	print_text(std_err, "WinSed v" VERSION_STR ", von LoRd_MuldeR <MuldeR2@GMX.de>.\nDeutsch von Manfred M""\xc3\xbc""ller <info@nass-ek.de>.\n\n");
	print_text(std_err, "Ersetzt jedes Vorkommen von '<Original>' in '<Eingabedatei>' durch '<Ersetzung>'.\n");
	print_text(std_err, "Der ge""\xc3\xa4""nderte Inhalt wird dann in '<Ausgabedatei>' geschrieben.\n\n");
	print_text(std_err, "Aufruf:\n");
	print_text(std_err, "  winsed.exe [Optionen] <Original> <Ersetzung> [<Eingabedatei>] [<Ausgabedatei>]\n\n");
	print_text(std_err, "Optionen:\n");
	print_text(std_err, "  -i  Gro""\xc3\x9f""-/Kleinschreibung f""\xc3\xbc""r die Zeichen 'A' bis 'Z' nicht ber""\xc3\xbc""cksichtigen. \n");
	print_text(std_err, "  -s  Einzelne Ersetzung; ersetzt nur das *erste* Vorkommen statt alle.\n");
	print_text(std_err, "  -a  Eingabe mit ANSI-Codepage (CP-1252) statt UTF-8 verarbeiten.\n");
	print_text(std_err, "  -e  Interpretation von Backslash-Escape-Sequenzen in allen Parametern aktivieren.\n");
	print_text(std_err, "  -f  Sofortiges Leeren von Dateipuffern erzwingen (kann die Leistung beeintr""\xc3\xa4""chtigen).\n");
	print_text(std_err, "  -b  Bin""\xc3\xa4""r-Modus; Parameter '<Original>' und '<Ersetzung>' sind Hex-Strings.\n");
	print_text(std_err, "  -n  Normalisieren von CR+LF (Windows) und CR (MacOS) Zeilenumbr""\xc3\xbc""chen zu LF (Unix).\n");
	print_text(std_err, "  -g  Globbing aktivieren; der Platzhalter '?' passt auf jedes Zeichen au""\xc3\x9f""er CR/LF.\n");
	print_text(std_err, "  -l  Wenn Globbing aktiviert ist, mu""\xc3\x9f"" das Platzhalterzeichen auch mit CR und LF ""\xc3\xbc""bereinstimmen.\n");
	print_text(std_err, "  -j  Versucht, schreibgesch""\xc3\xbc""tzte Dateien zu ""\xc3\xbc""berschreiben; d.h. l""\xc3\xb6""scht das Nur-Lese-Flag.\n");
	print_text(std_err, "  -d  Trockenlauf; Vorkommen von '<Original>' nicht tats""\xc3\xa4""chlich ersetzen.\n");
	print_text(std_err, "  -v  Verbose-Modus aktivieren; zus""\xc3\xa4""tzliche Diagnoseinformationen nach STDERR ausgeben.\n");
	print_text(std_err, "  -x  Exit-Code entspricht der Anzahl der Ersetzungen; Wert '-1' bedeutet Fehler.\n");
	print_text(std_err, "  -t  Selbsttest durchf""\xc3\xbc""hren und beenden.\n");
	print_text(std_err, "  -h  Diesen Hilfetext anzeigen und beenden.\n");
	print_text(std_err, "ExitCode:\n");
	print_text(std_err, "  Gibt standardm""\xc3\xa4""ssig '0' bei Erfolg oder '1' bei Fehler zur""\xc3\xbc""ck.\n");
	print_text(std_err, "  Wenn '<Original>' nicht gefunden wurde, wird dies *nicht* als Fehler betrachtet.\n\n");
	print_text(std_err, "Hinweise:\n");
	print_text(std_err, "  1. Wenn *nur* eine '<Eingabedatei>' angegeben wird, wird die Datei an Ort und Stelle modifiziert!\n");
	print_text(std_err, "  2. Wenn Dateinamen weggelassen werden, wird von STDIN gelesen und nach STDOUT geschrieben.\n");
	print_text(std_err, "  3. Der Dateiname kann als \"-\" angegeben werden, um von STDIN zu lesen oder nach STDOUT zu schreiben.\n");
	print_text(std_err, "  4. Die L""\xc3\xa4""nge einer Hex-Zeichenkette mu""\xc3\x9f"" *gerade* sein, mit optionalem '0x'-Pr""\xc3\xa4""fix.\n\n");
	print_text(std_err, "Beispiele:\n");
	print_text(std_err, "  winsed.exe \"bla\" \"blubb\" \"eingabe.txt\" \"ausgabe.txt\"\n");
	print_text(std_err, "  winsed.exe -e \"bla\\nblubb\" \"qu\\tux\" \"eingabe.txt\" \"ausgabe.txt\"\n");
	print_text(std_err, "  winsed.exe \"bla\" \"blubb\" \"test.txt\"\n");
	print_text(std_err, "  winsed.exe -b 0xDEADBEEF 0xCAFEBABE \"eingabe.bin\" \"ausgabe.bin\"\n");
	print_text(std_err, "  type \"quell.txt\" | winsed.exe \"bla\" \"blubb\" > \"ziel.txt\"\n\n");
}

/* ======================================================================= */
/* Ctrl+C handler routine                                                  */
/* ======================================================================= */

BOOL WINAPI ctrl_handler_routine(const DWORD type)
{
	switch(type)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		g_abort_requested = TRUE;
		return TRUE;
	}
	return FALSE;
}

/* ======================================================================= */
/* Command-line Options                                                    */
/* ======================================================================= */

static int parse_options(const HANDLE std_err, const int argc, const LPCWSTR *const argv, int *const index, options_t *const options)
{
	DWORD flag_pos;
	SecureZeroMemory(options, sizeof(options_t));
	while(*index < argc)
	{
		const WCHAR *const value = argv[*index];
		if((value[0U] == L'-') && (value[1U] != L'\0'))
		{
			*index += 1U;
			if((value[1U] == L'-') && (value[2U] == L'\0'))
			{
				return TRUE; /*stop here!*/
			}
			for(flag_pos = 1U; value[flag_pos]; ++flag_pos)
			{
				switch(value[flag_pos])
				{
				case L'?':
				case L'a':
					options->ansi_cp = TRUE;
					break;
				case L'b':
					options->binary_mode = TRUE;
					break;
				case L'd':
					options->flags.dry_run = TRUE;
					break;
				case L'e':
					options->escpae_chars = TRUE;
					break;
				case L'f':
					options->force_sync = TRUE;
					break;
				case L'g':
					options->globbing = TRUE;
					break;
				case L'h':
					options->show_help = TRUE;
					break;
				case L'i':
					options->flags.case_insensitive = TRUE;
					break;
				case L'l':
					options->flags.match_crlf = TRUE;
					break;
				case L'n':
					options->flags.normalize = TRUE;
					break;
				case L's':
					options->flags.replace_once = TRUE;
					break;
				case L't':
					options->self_test = TRUE;
					break;
				case L'v':
					options->flags.verbose = TRUE;
					break;
				case L'x':
					options->return_replace_count = TRUE;
					break;
				case L'j':
					options->force_overwrite = TRUE;
					break;
				default:
					print_text(std_err, "Fehler: Ung""\xc3\xbc""ltige Kommandozeilenoption gefunden!\n");
					return FALSE;
				}
			}
		}
		else
		{
			break; /*no more options*/
		}
	}
	return TRUE;
}

/* ======================================================================= */
/* Main                                                                    */
/* ======================================================================= */

REPLACE_MAIN(UINT, const int argc, LPCWSTR *const argv)
{
	UINT result = EXIT_FAILURE, previous_output_cp = 0U;
	int param_offset = 1;
	BYTE *needle = NULL, *replacement = NULL;
	WORD *needle_expanded = NULL;
	DWORD needle_len = 0U, replacement_len = 0U, replacement_count = 0U;
	options_t options;
	HANDLE input = INVALID_HANDLE_VALUE, output = INVALID_HANDLE_VALUE;
	libreplace_logger_t logger;
	libreplace_io_t io_functions;
	file_input_t *file_input_context = NULL;
	file_output_t *file_output_context = NULL;
	const WCHAR *source_file = NULL, *output_file = NULL, *temp_path = NULL, *temp_file = NULL;

	SetConsoleOutputCP(CP_UTF8);

	/* -------------------------------------------------------- */
	/* Init standard handles                                    */
	/* -------------------------------------------------------- */

	const HANDLE std_inp = GetStdHandle(STD_INPUT_HANDLE);
	const HANDLE std_out = GetStdHandle(STD_OUTPUT_HANDLE);
	const HANDLE std_err = GetStdHandle(STD_ERROR_HANDLE);

	/* -------------------------------------------------------- */
	/* Parse options                                            */
	/* -------------------------------------------------------- */

	if(!parse_options(std_err, argc, argv, &param_offset, &options))
	{
		goto cleanup;
	}

	if (options.return_replace_count)
	{
		result = (UINT)(-1);
	}

	if(options.show_help)
	{
		print_manpage(std_err);
		result = options.return_replace_count ? 0U : EXIT_SUCCESS;
		goto cleanup;
	}

	/* -------------------------------------------------------- */
	/* Parameter validation                                     */
	/* -------------------------------------------------------- */

	if(options.binary_mode && (options.ansi_cp || options.escpae_chars || options.globbing || options.flags.case_insensitive))
	{
		print_text(std_err, "Fehler: Die Optionen '-a', '-e', '-g' und '-i' sind nicht kompatibel mit dem Bin""\xc3\xa4""rmodus!");
		goto cleanup;
	}

	if(options.flags.match_crlf && (!options.globbing))
	{
		print_text(std_err, "Fehler: Die Option '-l' macht nur Sinn, wenn Globbing aktiviert ist!");
		goto cleanup;
	}

	if((!options.self_test) && (argc - param_offset < 2U))
	{
		print_manpage(std_err);
	}

	if((!options.self_test) && (!argv[param_offset][0U]))
	{
		print_text(std_err, "Fehler: Suchstring (Eingabe) darf nicht leer sein!\n");
		goto cleanup;
	}

	if((argc - param_offset > 2) && (!argv[param_offset + 2L][0U]))
	{
		print_text(std_err, "Fehler: Wenn eine Eingabedatei angegeben wird, darf es sich nicht um eine leere Zeichenkette handeln!\n");
		goto cleanup;
	}

	if((argc - param_offset > 3) && (!argv[param_offset + 3L][0U]))
	{
		print_text(std_err, "Fehler: Wenn eine Ausgabedatei angegeben wird, darf es sich nicht um eine leere Zeichenkette handeln!\n");
		goto cleanup;
	}

	/* -------------------------------------------------------- */
	/* Self-test mode */
	/* -------------------------------------------------------- */

	if(options.self_test)
	{
		print_text(std_err, "Starte Selbsttest...\n");
		if(!self_test(std_err))
		{
			CHECK_ABORT_REQUEST();
			print_text(std_err, "Fehler: Selbsttest gescheitert !!!\n");
			goto cleanup;
		}
		print_text(std_err, "Selbsttest abgeschlossen.\n");
		result = EXIT_SUCCESS;
		goto cleanup;
	}

	/* -------------------------------------------------------- */
	/* Initialize search parameters and file names              */
	/* -------------------------------------------------------- */

	needle = options.binary_mode ? decode_hex_string(argv[param_offset], &needle_len) : utf16_to_bytes(argv[param_offset], &needle_len, SELECTED_CP);
	if(!needle)
	{
		print_text(std_err, "Fehler: Dekodierung des Strings 'Original' fehlgeschlagen!\n");
		goto cleanup;
	}

	if(needle_len > LIBREPLACE_MAXLEN)
	{
		print_text_fmt(std_err, "Fehler:Suchstring 'Original' darf %ld Zeichen nicht ""\xc3\xbc""berschreiten!\n", LIBREPLACE_MAXLEN);
		goto cleanup;
	}

	replacement = options.binary_mode ? decode_hex_string(argv[param_offset + 1U], &replacement_len) : utf16_to_bytes(argv[param_offset + 1U], &replacement_len, SELECTED_CP);
	if(!replacement)
	{
		print_text(std_err, "Fehler: Dekodierung der Zeichenkette 'Ersetzung' fehlgeschlagen!\n");
		goto cleanup;
	}
	
	if(replacement_len > LIBREPLACE_MAXLEN)
	{
		print_text_fmt(std_err, "Fehler: Ersetzungsstring darf %ld Zeichen nicht ""\xc3\xbc""berschreiten!\n", LIBREPLACE_MAXLEN);
		goto cleanup;
	}

	if(options.escpae_chars)
	{
		if(!(expand_escape_chars(needle, &needle_len) && expand_escape_chars(replacement, &replacement_len)))
		{
			print_text(std_err, "Fehler: Parameter enth""\xc3\xa4""lt eine ung""\xc3\xbc""ltige Escape-Sequenz!\n");
			goto cleanup;
		}
	}

	needle_expanded = expand_wildcards(needle, needle_len, options.globbing ? &MY_WILDCARD : NULL);
	if(!needle_expanded)
	{
		print_text(std_err, "Fehler: Expandieren von Platzhalterzeichen fehlgeschlagen!");
		goto cleanup;
	}

	source_file = (argc - param_offset > 2U) ? argv[param_offset + 2U] : NULL;
	output_file = (argc - param_offset > 3U) ? argv[param_offset + 3U] : NULL;

	if(NOT_EMPTY(source_file) && NOT_EMPTY(output_file) && (lstrcmpiW(source_file, L"-") != 0) && (lstrcmpiW(output_file, L"-") != 0))
	{
		if(lstrcmpW(source_file, output_file) == 0)
		{
			print_text(std_err, "Fehler: Eingabe- und Ausgabedatei d""\xc3\xbc""rfen nicht identisch sein!\n");
			goto cleanup;
		}
	}

	/* -------------------------------------------------------- */
	/* Open input and output files                              */
	/* -------------------------------------------------------- */

	if(NOT_EMPTY(source_file) && (lstrcmpiW(source_file, L"-") != 0))
	{
		input = open_file(source_file, FALSE);
	}
	else
	{
		if(options.flags.verbose)
		{
			print_text(std_err, "Lese Eingabe von STDIN-Stream.\n");
		}
		input = std_inp;
	}

	if(input == INVALID_HANDLE_VALUE)
	{
		CHECK_ABORT_REQUEST();
		print_text(std_err, "Fehler: Die Eingabedatei konnte nicht zum Lesen ge""\xc3\xb6""ffnet werden!\n");
		goto cleanup;
	}

	if(!(file_input_context = alloc_file_input(input)))
	{
		print_text(std_err, "Fehler: Fehler beim Zuweisen des Datei-Eingabekontextes!\n");
		goto cleanup;
	}

	if(EMPTY(output_file) && NOT_EMPTY(source_file) && (lstrcmpiW(source_file, L"-") != 0))
	{
		if(options.flags.verbose)
		{
			print_text(std_err, "Diesmal wird der Modus 'Verarbeitung an Ort und Stelle' verwendet.\n");
		}
		if((!options.force_overwrite) && has_readonly_attribute(input))
		{
			print_text(std_err, "Fehler: Die schreibgesch""\xc3\xbc""tzte Datei kann nicht an Ort und Stelle geaendert werden!\n");
			goto cleanup;
		}
		temp_file = generate_temp_file(temp_path = get_directory_part(source_file), &output);
		if(EMPTY(temp_file))
		{
			CHECK_ABORT_REQUEST();
			print_text(std_err, "Fehler: Tempor""\xc3\xa4""re Datei konnte nicht erstellt werden!");
			goto cleanup;
		}
	}

	if(EMPTY(temp_file))
	{
		if(NOT_EMPTY(output_file) && (lstrcmpiW(output_file, L"-") != 0))
		{
			if(options.force_overwrite)
			{
				clear_readonly_attribute(output_file);
			}
			output = open_file(output_file, TRUE);
		}
		else
		{
			if(options.flags.verbose)
			{
				print_text(std_err, "Ausgabe auf STDOUT schreiben.\n");
			}
			output = std_out;
		}
	}

	if(output == INVALID_HANDLE_VALUE)
	{
		CHECK_ABORT_REQUEST();
		print_text(std_err, "Fehler: Die Ausgabedatei konnte nicht zum Schreiben ge""\xc3\xb6""ffnet werden!");
		goto cleanup;
	}

	if(!(file_output_context = alloc_file_output(output, options.force_sync)))
	{
		print_text(std_err, "Fehler: Fehler beim Zuweisen des Dateiausgabekontextes!\n");
		goto cleanup;
	}

	/* -------------------------------------------------------- */
	/* Set up terminal                                          */
	/* -------------------------------------------------------- */

	if(GetFileType(output) == FILE_TYPE_CHAR)
	{
		const UINT current_cp = GetConsoleOutputCP();
		if((current_cp != 0U) && (current_cp != SELECTED_CP))
		{
			if(SetConsoleOutputCP(SELECTED_CP))
			{
				previous_output_cp = current_cp;
			}
		}
	}

	/* -------------------------------------------------------- */
	/* Search & replace!                                        */
	/* -------------------------------------------------------- */

	init_logging_functions(&logger, print_text_ptr, (DWORD_PTR)std_err);
	init_io_functions(&io_functions, file_read_byte, file_write_byte, (DWORD_PTR)file_input_context, (DWORD_PTR)file_output_context);

	CHECK_ABORT_REQUEST();

	if(!libreplace_search_and_replace(&io_functions, &logger, needle_expanded, needle_len, replacement, replacement_len, &options.flags, &replacement_count, &g_abort_requested))
	{
		CHECK_ABORT_REQUEST();
		print_text(std_err, "Fehler: Ein unbekannter Fehler ist aufgetreten. Die Ausgabe ist wahrscheinlich unvollst""\xc3\xa4""ndig!");
		goto cleanup;
	}

	CHECK_ABORT_REQUEST();

	/* -------------------------------------------------------- */
	/* Finishing touch                                          */
	/* -------------------------------------------------------- */

	if(input != std_inp)
	{
		CloseHandle(input);
		input = INVALID_HANDLE_VALUE;
	}

	if(output != std_out)
	{
		CloseHandle(output);
		output = INVALID_HANDLE_VALUE;
	}

	if(NOT_EMPTY(temp_file) && (!options.flags.dry_run))
	{
		if(options.force_overwrite)
		{
			clear_readonly_attribute(source_file);
		}
		if(!move_file(temp_file, source_file))
		{
			CHECK_ABORT_REQUEST();
			print_text(std_err, "Fehler: Die Originaldatei konnte nicht durch die ge""\xc3\xa4""nderte Datei ersetzt werden!");
			goto cleanup;
		}
	}

	result = options.return_replace_count ? ((replacement_count <= ((DWORD)MAXINT32)) ? replacement_count : MAXINT32) : EXIT_SUCCESS;

	/* -------------------------------------------------------- */
	/* Final clean-up                                           */
	/* -------------------------------------------------------- */

cleanup:

	if((input != INVALID_HANDLE_VALUE) && (input != std_inp))
	{
		CloseHandle(input);
	}

	if((output != INVALID_HANDLE_VALUE) && (output != std_out))
	{
		CloseHandle(output);
	}

	if(NOT_EMPTY(temp_file) && file_exists(temp_file))
	{
		delete_file(temp_file);
	}

	if(needle)
	{
		LocalFree((HLOCAL)needle);
	}

	if(needle_expanded)
	{
		LocalFree((HLOCAL)needle_expanded);
	}

	if(replacement)
	{
		LocalFree((HLOCAL)replacement);
	}

	if(temp_file)
	{
		LocalFree((HLOCAL)temp_file);
	}

	if(temp_path)
	{
		LocalFree((HLOCAL)temp_path);
	}

	if(file_input_context)
	{
		LocalFree((HLOCAL)file_input_context);
	}

	if(file_output_context)
	{
		LocalFree((HLOCAL)file_output_context);
	}

	if(previous_output_cp)
	{
		SetConsoleOutputCP(previous_output_cp);
	}

	return result;
}

/* ======================================================================= */
/* Entry point                                                             */
/* ======================================================================= */

void _entryPoint(void)
{
	int argc;
	UINT result = (UINT)(-1);
	LPWSTR *argv;

	SetErrorMode(SetErrorMode(0x3) | 0x3);
	SetConsoleCtrlHandler(ctrl_handler_routine, TRUE);

	if(argv = CommandLineToArgvW(GetCommandLineW(), &argc))
	{
		result = _tmain(argc, argv);
		LocalFree(argv);
	}

	ExitProcess(result);
}
