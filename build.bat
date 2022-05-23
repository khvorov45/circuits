@echo off

cl -Od -Zi ^
	-DCIRCUITS_WINDOWS ^
	-Icode/freetype_include ^
	-nologo -Zl -TC -GS- -std:c17 ^
	-Fdbuild/circuits.pdb -Febuild/circuits.exe -Fobuild/circuits.obj ^
	-Wall -wd4100 -wd4255 -wd4820 -wd4065 -wd5045 ^
	-diagnostics:column ^
	code/circuits.c ^
	-link -incremental:no -subsystem:windows -NODEFAULTLIB Kernel32.lib User32.lib Gdi32.lib build/freetype.lib
