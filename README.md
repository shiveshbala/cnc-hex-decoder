# CNC Hex Decoder

A lightweight C tool for decoding hex-encoded IEEE 754 doubles from Fanuc CNC macro files (G10 L85/L86 params) into readable floats. Parses dumps like MACRO.TXT, extracts P numbers and values, and spits out clean CSVs for Excel analysis—perfect for reverse-engineering machine offsets, toolpaths, or tolerances without the hassle. Compile once, run interactively, and tweak as needed.

## Features
- **Interactive input**: Just type your .txt file name, and it auto-suggests a .csv output. Override if you want.
- **Robust parsing**: Handles G10 lines with P(hex) format, skips junk like % comments or empty lines.
- **Precise decoding**: Converts 16-digit big-endian hex to doubles—NaNs show as "nan" for those FFs.
- **CSV export**: Columns for P_Number, Hex_Value, and Decoded_Double. Opens straight in Excel.
- **Progress tracking**: Dots in the console for long files like PCODE.TXT.

## Installation
1. Dowload the `hexconversion.exe` file or compile the `hexconversion.c` file using gcc or other C compilers
2. Have the `.txt` file in the same directory where the `hexconversion.exe` file is located.
3. Use `./hexconversion` command in the terminal or execute the `hexconversion.exe` on Windows.

No dependencies beyond standard C libs.

## Usage
Run the file, and it'll prompt you:
```
Enter the input filename (e.g., macro.txt): macro.txt
Suggested output: macro.csv
Want a different output filename? (Press Enter for suggested, or type one): 
```
- Hit Enter for the default (strips .txt, adds .csv).
- Type something custom like "my-params.csv" to override.
- Watch the dots for progress, then open the CSV in Excel.

Example run on a Fanuc dump:
- Input: `macro.txt` with lines like `G10L85P506(407F400000000000)`
- Output: `macro.csv` row: `506,407F400000000000,500.0000000000`

## Example Output
Your CSV will look like this (snippet):
| P_Number | Hex_Value          | Decoded_Double |
|----------|--------------------|----------------|
| 506     | 407F400000000000  | 500.0000000000 |
| 507     | 406B800000000000  | 220.0000000000 |
| 509     | 403E000000000000  | 30.0000000000 |

## License
MIT License—use it, tweak it, sell widgets with it. Just keep the notice. See [LICENSE](LICENSE) for details.

## Contributing
Got ideas? Fork it, tweak, and PR. Or hit me up—always curious about CNC quirks.
Questions? Open an issue.
