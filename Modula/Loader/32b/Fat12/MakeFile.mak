# Pilote Pic.

# Bombela
# 08/10/2004
# 08/10/2004

ELF = Fat12.elf

include $(MODULA)/Loader/32b/MakeDef.mak

all: $(INCLUDE)/$(ELF:.elf=.h) $(ELF)

$(ELF): $(INCLUDE)/*.h

property:
	del "$(ELF)"
	del "$(INCLUDE)/$(ELF:.elf=.h)"