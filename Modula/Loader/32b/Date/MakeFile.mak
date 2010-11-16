# Pilote Pic.

# Bombela
# 01/02/2005
# 01/02/2005

ELF = Date.elf

include $(MODULA)/Loader/32b/MakeDef.mak

all: $(INCLUDE)/$(ELF:.elf=.h) $(ELF)

$(ELF): *.h $(INCLUDE)/*.h

property:
	del "$(ELF)"
	del "$(INCLUDE)/$(ELF:.elf=.h)"
