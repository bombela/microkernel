# Loader32 de la partie 32 bits du Loader.

# Bombela
# 28/06/2004
# 31/08/2004

include $(MODULA)/Loader/32b/MakeDef.mak

all: Loader32.elf

Loader32.elf: $(INCLUDE)/*.h

property:
	del Loader32.elf