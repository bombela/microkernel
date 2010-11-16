# Make definitions pour Loader32.

# BOMBELA
# 11/06/2004
# 05/06/2005

include $(MODULA)/MakeDef.mak

# Macro utile.
#---------------------------------------------
INCLUDE = $(MODULA)/Loader/32b/Include

# Mise à jours des .h dans INCLUDE.
#---------------------------------------------
$(INCLUDE)/%.h:%.h
	$(MSGCPH)
	@cat $(INCLUDE)/AutoInclude.txt $< > $(INCLUDE)/$<
