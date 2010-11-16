# Loader, partie 16 bits

# BOMBELA
# 12/06/2004
# 31/08/2004

include $(MODULA)/MakeDef.mak

all: Loader16.bin

Loader16.bin: Loader16.asm
	$(MSGCOMPIL)
	@nasm -fbin $< -o $@
