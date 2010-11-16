# PM3DCD16.exe

# Bombela
# 11/06/2004
# 29/09/2004

DEPENDS = PutPix.obj Trad.pas Img.pas SCROLBOX.pas TEXTLIST.pas ..\..\bin\BootSect.bin

#Compilation normale.
#-----------------------------------------------------------------------------
.PHONY: all
all: ..\..\bin\MFP16.exe

..\..\bin\MFP16.exe: MFP16.exe
	copy MFP16.EXE ..\..\bin

MFP16.exe: MFP16.pas $(DEPENDS)
	tpclib -B MFP16.pas
	copy /b /y MFP16.EXE+Modula.BMP MFP16.exe
	copy /b /y MFP16.EXE+..\..\bin\BootSect.bin MFP16.EXE

PutPix.obj: PutPix.asm
	nasmw -fobj PutPix.asm

.PHONY: property	
property: clean
	del MFP16.EXE
	del *.obj

.PHONY: clean	
clean:
	del *.tpu
	
	
.PHONY: force
force: property all