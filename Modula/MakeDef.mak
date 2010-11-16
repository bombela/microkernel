# Make definitions.

# BOMBELA
# 11/06/2004
# 18/12/2005
# 28/01/2006 - Meilleurs choix d'options de compilation.
# 14/02/2006 - Compilation de .S

# Pour être tranquille.
#---------------------------------------------
.PHONY: all
.PHONY: clean
.PHONY: force
.PHONY: property
.PHONY: help
.PHONY: recurse
.PHONY: subdircompil

# Variable.
#---------------------------------------------
BIN = $(MODULA)/Bin
GCC = gcc-3.4 -m32 -Wall -pipe -O2 -fshort-enums -ggdb -march=i386 -I$(INCLUDE)

# Definition pratique
#---------------------------------------------
MSGCOMPIL = @echo : \ \ Compiling \ $<
MSGCPBIN  = @echo : \ \ Add bin\ \ \ \ $<
MSGCPH  =   @echo : \ \ Add header $<
MSGLINK =   @echo : \ \ Linking\ \ \ \ $@
MSSUBDIR =  @echo : $@
MSGAPI =    @echo : \ \ Gen API \ \ \ $@

define DELETE
	@echo :	R Del.
	@del
endef

# Fonctions standard.
#---------------------------------------------
all:
help:
	@echo .
	@echo all       : Compilation totale.
	@echo force     : Force la generation.
	@echo clean     : Efface les fichiers generes inutile.
	@echo property  : Efface tout les fichiers generes.
	@echo recurse   : Appelle les makefiles en dessous si il y'en a.

force: property all
recurse: $(MKDIR)
property: $(MKPROPERTY) $(MKDIR)
clean: $(MKDIR)

# Definition de commande qui appelle chaque sous dossier.
# $(MKDIR) doit contenir les sous dossier
#---------------------------------------------
.PHONY = $(MKDIR)
$(MKDIR): subdircompil
	$(MSSUBDIR)
	@$(MAKE) --no-print-directory -s -f MakeFile.mak -C $@ $(MAKECMDGOALS)

subdircompil:
	@echo : ------------- $(CURDIR)

# Compilation d'un S en elf.
#---------------------------------------------
%.elf: %.S
	$(MSGCOMPIL)
	@$(GCC) -c $< -o $@

# Compilation d'un asm en elf.
#---------------------------------------------
NASM = nasm
%.elf: %.asm
	$(MSGCOMPIL)
	@$(NASM) -felf $< -o $@

# Compilation d'un c en elf.
#---------------------------------------------
%.elf: %.c
	$(MSGCOMPIL)
	@$(GCC) -c $< -o $@

# Copie dans le dossier Bin.
#---------------------------------------------
$(BIN)/%: %
	$(MSGCPBIN)
	@cp $< $@
