

ELF = Elf.elf
HEADER = Elf.h

include $(MODULA)/Loader/32b/MakeDef.mak

all: $(INCLUDE)/$(HEADER) $(ELF)

property:
	-rm "$(ELF)"
	-rm "$(INCLUDE)/$(HEADER)"