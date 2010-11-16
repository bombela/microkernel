# String.

# Bombela
# 15/04/2005
# Updated: 17/04/2005

ELF = String.elf

include $(MODULA)/Loader/32b/MakeDef.mak

all: $(INCLUDE)/$(ELF:.elf=.h) $(ELF)

$(ELF): $(INCLUDE)/*.h
