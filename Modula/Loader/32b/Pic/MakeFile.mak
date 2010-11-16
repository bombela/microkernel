# Pilote Pic.

# Bombela
# 30/09/2004
# 07/06/2005

ELF = Pic.elf

include $(MODULA)/Loader/32b/MakeDef.mak

all: $(INCLUDE)/$(ELF:.elf=.h) $(ELF)

$(ELF): $(INCLUDE)/*.h
