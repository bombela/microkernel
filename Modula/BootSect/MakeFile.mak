# BootSector

# BOMBELA
# 12/06/2004
# 07/06/2005

include $(MODULA)/MakeDef.mak

all: $(BIN)/BootSect.bin

$(BIN)/BootSect.bin: BootSect.bin

BootSect.bin: BootSect.asm
	$(MSGCOMPIL)
	$(NASM) -fbin $< -o $@
