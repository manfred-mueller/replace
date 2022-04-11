WinSed v1.8.0, von LoRd_MuldeR <MuldeR2@GMX.de>.
Umbenannt von Replace auf Grund Konflikt mit dem Windows eigenen
Replace-Befehl zum Ersetzen von Dateien.
Deutsch von Manfred Müller <info@nass-ek.de>.

Ersetzt jedes Vorkommen von '\<Original\>' in '\<Eingabedatei\>' durch '\<Ersetzung\>'.
Der geänderte Inhalt wird dann in '\<Ausgabedatei\>' geschrieben.

Aufruf:
  replace.exe [Optionen] \<Original\> \<Ersetzung\> [\<Eingabedatei\>] [\<Ausgabedatei\>]

Optionen:
  -i  Groß-/Kleinschreibung für die Zeichen 'A' bis 'Z' nicht berücksichtigen.
  -s  Einzelne Ersetzung; ersetzt nur das *erste* Vorkommen statt alle.
  -a  Eingabe mit ANSI-Codepage (CP-1252) statt UTF-8 verarbeiten.
  -e  Interpretation von Backslash-Escape-Sequenzen in allen Parametern aktivieren.
  -f  Sofortiges Leeren von Dateipuffern erzwingen (kann die Leistung beeinträchtigen).
  -b  Binär-Modus; Parameter '\<Original\>' und '\<Ersetzung\>' sind Hex-Strings.
  -n  Normalisieren von CR+LF (Windows) und CR (MacOS) Zeilenumbrüchen zu LF (Unix).
  -g  Globbing aktivieren; der Platzhalter '?' passt auf jedes Zeichen außer CR/LF.
  -l  Wenn Globbing aktiviert ist, muß das Platzhalterzeichen auch mit CR und LF übereinstimmen.
  -j  Versucht, schreibgeschützte Dateien zu überschreiben; d.h. löscht das Nur-Lese-Flag.
  -d  Trockenlauf; Vorkommen von '\<Original\>' nicht tatsächlich ersetzen.
  -v  Verbose-Modus aktivieren; zusätzliche Diagnoseinformationen nach STDERR ausgeben.
  -x  Exit-Code entspricht der Anzahl der Ersetzungen; Wert '-1' bedeutet Fehler.
  -t  Selbsttest durchführen und beenden.
  -h  Diesen Hilfetext anzeigen und beenden.
ExitCode:
  Gibt standardmässig '0' bei Erfolg oder '1' bei Fehler zurück.
  Wenn '\<Original\>' nicht gefunden wurde, wird dies *nicht* als Fehler betrachtet.

Hinweise:
  1. Wenn *nur* eine '\<Eingabedatei>\' angegeben wird, wird die Datei an Ort und Stelle modifiziert!
  2. Wenn Dateinamen weggelassen werden, wird von STDIN gelesen und nach STDOUT geschrieben.
  3. Der Dateiname kann als "-" angegeben werden, um von STDIN zu lesen oder nach STDOUT zu schreiben.
  4. Die Länge einer Hex-Zeichenkette muß *gerade* sein, mit optionalem '0x'-Präfix.

Beispiele:
  replace.exe "bla" "blubb" "eingabe.txt" "ausgabe.txt"
  replace.exe -e "bla\nblubb" "qu\tux" "eingabe.txt" "ausgabe.txt"
  replace.exe "bla" "blubb" "test.txt"
  replace.exe -b 0xDEADBEEF 0xCAFEBABE "eingabe.bin" "ausgabe.bin"
  type "quell.txt" | replace.exe "bla" "blubb" > "ziel.txt"
