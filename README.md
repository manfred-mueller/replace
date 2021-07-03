WinSed v1.8.0, von LoRd_MuldeR <MuldeR2@GMX.de>.
Umbenannt von Replace auf Grund Konflikt mit dem Windows eigenen
Replace-Befehl zum Ersetzen von Dateien.
Deutsch von Manfred M�ller <info@nass-ek.de>.

Ersetzt jedes Vorkommen von '<Original>' in '<Eingabedatei>' durch '<Ersetzung>'.
Der ge�nderte Inhalt wird dann in '<Ausgabedatei>' geschrieben.

Aufruf:
  replace.exe [Optionen] <Original> <Ersetzung> [<Eingabedatei>] [<Ausgabedatei>]

Optionen:
  -i  Gro�-/Kleinschreibung f�r die Zeichen 'A' bis 'Z' nicht ber�cksichtigen.
  -s  Einzelne Ersetzung; ersetzt nur das *erste* Vorkommen statt alle.
  -a  Eingabe mit ANSI-Codepage (CP-1252) statt UTF-8 verarbeiten.
  -e  Interpretation von Backslash-Escape-Sequenzen in allen Parametern aktivieren.
  -f  Sofortiges Leeren von Dateipuffern erzwingen (kann die Leistung beeintr�chtigen).
  -b  Bin�r-Modus; Parameter '<Original>' und '<Ersetzung>' sind Hex-Strings.
  -n  Normalisieren von CR+LF (Windows) und CR (MacOS) Zeilenumbr�chen zu LF (Unix).
  -g  Globbing aktivieren; der Platzhalter '?' passt auf jedes Zeichen au�er CR/LF.
  -l  Wenn Globbing aktiviert ist, mu� das Platzhalterzeichen auch mit CR und LF �bereinstimmen.
  -j  Versucht, schreibgesch�tzte Dateien zu �berschreiben; d.h. löscht das Nur-Lese-Flag.
  -d  Trockenlauf; Vorkommen von '<Original>' nicht tats�chlich ersetzen.
  -v  Verbose-Modus aktivieren; zus�tzliche Diagnoseinformationen nach STDERR ausgeben.
  -x  Exit-Code entspricht der Anzahl der Ersetzungen; Wert '-1' bedeutet Fehler.
  -t  Selbsttest durchf�hren und beenden.
  -h  Diesen Hilfetext anzeigen und beenden.
ExitCode:
  Gibt standardm�ssig '0' bei Erfolg oder '1' bei Fehler zur�ck.
  Wenn '<Original>' nicht gefunden wurde, wird dies *nicht* als Fehler betrachtet.

Hinweise:
  1. Wenn *nur* eine '<Eingabedatei>' angegeben wird, wird die Datei an Ort und Stelle modifiziert!
  2. Wenn Dateinamen weggelassen werden, wird von STDIN gelesen und nach STDOUT geschrieben.
  3. Der Dateiname kann als "-" angegeben werden, um von STDIN zu lesen oder nach STDOUT zu schreiben.
  4. Die L�nge einer Hex-Zeichenkette mu� *gerade* sein, mit optionalem '0x'-Pr�fix.

Beispiele:
  replace.exe "bla" "blubb" "eingabe.txt" "ausgabe.txt"
  replace.exe -e "bla\nblubb" "qu\tux" "eingabe.txt" "ausgabe.txt"
  replace.exe "bla" "blubb" "test.txt"
  replace.exe -b 0xDEADBEEF 0xCAFEBABE "eingabe.bin" "ausgabe.bin"
  type "quell.txt" | replace.exe "bla" "blubb" > "ziel.txt"