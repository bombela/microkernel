# IDE, pilote IDE en lecture seule.

# Bombela
# 28/06/2004
# 31/08/2004

ELF = Ide.elf

include $(MODULA)/Loader/32b/MakeDef.mak

all: $(INCLUDE)/$(ELF:.elf=.h) $(ELF)

$(ELF): $(INCLUDE)/*.h

property:
	del "$(ELF)"
	del "$(INCLUDE)/$(ELF:.elf=.h)"