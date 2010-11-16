# Loader, linkage de la partie 16 et 32 bits.

# BOMBELA
# 27/06/2004
# 07/06/2005

BINS = 16b/Loader16.bin 32b/Loader32.bin

MKDIR = $(dir $(BINS))

include $(MODULA)/MakeDef.mak

all: $(BIN)/Loader.sys

$(BIN)/Loader.sys: Loader.sys

Loader.sys: $(MKDIR) $(BINS)
	$(MSGLINK)
	@cat $(BINS) > $@