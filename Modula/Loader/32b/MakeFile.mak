# Loader, partie 32 bits.

# BOMBELA
# 27/06/2004
# 07/06/2005

ELFS =	Loader32/Loader32.elf OutText/OutText.elf String/String.elf Pic/Pic.elf Int/Int.elf \
	Timer/Timer.elf Date/Date.elf Dma/Dma.elf Floppy/Floppy.elf  \
	Fat12/Fat12.elf Ide/Ide.elf Fat32/Fat32.elf Mem/Mem.elf Elf/Elf.elf

#Ide/Ide.elf
	
MKDIR = $(dir $(ELFS))

include $(MODULA)/Loader/32b/MakeDef.mak

all: Loader32.bin

Loader32.bin: $(MKDIR) $(ELFS) $(MODULA)/Loader/32b/Include/*.h loader32.api
	$(MSGLINK)
	@ld -melf_i386 -Ttext 0x9000 -e Loader32 -N $(ELFS) -o Loader32.elf
	@ld -melf_i386 --oformat binary -Ttext 0x9000 -e Loader32 -N $(ELFS) -o $@


loader32.api: $(MODULA)/Loader/32b/Include/*.h
	$(MSGAPI)
	@find Include -name "*.h" -exec pars_scite {} ';' > loader32.api

propety:
	find -name "*.elf" -delete
