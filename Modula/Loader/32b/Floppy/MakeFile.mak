# Floppy.

# Bombela
# 01/10/2004
# 24/10/2004

include $(MODULA)/Loader/32b/MakeDef.mak

all: $(INCLUDE)/Floppy.h Floppy.elf

Floppy.elf: Floppy_Main.elf Floppy_Int.elf
	$(MSGLINK)
	@ld -melf_i386 -r Floppy_Main.elf Floppy_Int.elf -o $@

Floppy_Main.elf: $(INCLUDE)/*.h
