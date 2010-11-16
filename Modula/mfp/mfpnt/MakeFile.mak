# BOMBELA
# Created: 11/05/2004
# Updated: 05/05/2005

include ../../MakeDef.mak

all: MFPNT.exe $(BIN)/MFPNT.exe

MFPNT.exe: MFPNT.BIN $(BIN)/BootSect.bin
	cat MFPNT.BIN $(BIN)/BootSect.bin > $@

property: clean
	del MFPNT.exe

clean:
	@del *.dcu
	@del *.~pas
	@del *.~dfm
	@del *.~ddp
	@del *.~dfm
	@del *.~dpr
	@del *.ddp

