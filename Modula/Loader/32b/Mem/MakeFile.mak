

ELF = Mem.elf
HEADER = Mem.h

include $(MODULA)/Loader/32b/MakeDef.mak

all: $(INCLUDE)/$(HEADER) $(ELF)

property:
	-rm "$(ELF)"
	-rm "$(INCLUDE)/$(HEADER)"