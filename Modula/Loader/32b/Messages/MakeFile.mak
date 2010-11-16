# OutT, Sortie de texte.

# Bombela
# 22/04/2005
# 22/04/2005

ELF = Messages.elf

include $(MODULA)/Loader/32b/MakeDef.mak

all: $(INCLUDE)/$(ELF:.elf=.h) $(ELF)

property:
	del "$(ELF)"
	del "$(INCLUDE)/$(ELF:.elf=.h)"
