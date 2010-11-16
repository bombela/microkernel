# Screen Saver.

# BOMBELA
# 22/11/2004
# 03/03/2005
# 27/01/2006

# find /mnt/prog/Modula/Loader/32b -name "*.elf" -exec cp '{}' . ';'

ROOT = ./

# Compilation d'un c en elf.
#---------------------------------------------
GCC = gcc -Wall -pipe -Os -fshort-enums -march=pentium -I"Modules_Modula" -c
%.elf: %.c
	 $(GCC) $< -o $@

ELFS =	SCRSAVER.elf\
	Modules_Modula/*.elf

.PHONY: all
all: SCRSAVER.bin
	
SCRSAVER.bin: $(ELFS)
	ld --oformat binary -Ttext 0x100000 -e ScrSaver -N $+ -o $@
