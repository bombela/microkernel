# Bombela - 03/11/2005
# 30.06.2006 01:12:07

.PHONY: all clean property doc

FLTKLD = `fltk-config --ldstaticflags`
CCFLAG=-pipe -O3 -v

ifndef OS
all:
	@echo "Préciser OS=<linux32, linux64 ou win32>."
endif

# linux32
ifeq ($(OS),linux32)
 mfpfltk=mfpfltk.32.elf
 objext=linux32.o
 CC=g++ -m32 $(CCFLAG)
endif

# linux64
ifeq ($(OS),linux64)
 mfpfltk=mfpfltk.64.elf
 objext=linux64.o
 CC=g++ -m64 $(CCFLAG)
endif

# win32.
ifeq ($(OS),win32)
 mfpfltk=mfpfltk.32.exe
 objext=win32.o
 CC=g++ $(CCFLAG)
endif

all: $(mfpfltk)

$(mfpfltk): mfpfltk.$(objext) class_part.$(objext) class_drive.$(objext)
	$(CC) -o $@ $^ $(FLTKLD)
	strip $@
	cat ../../Bin/BootSect.bin >> $@

mfpfltk.$(objext): mfpfltk.cpp mfpfltk.h window.cxx window.h util.h largefile.h
	$(CC) -o $@ -c mfpfltk.cpp `fltk-config --cxxflags`

class_part.$(objext): class_part.cpp class_part.h util.h largefile.h
	$(CC) -o $@ -c class_part.cpp

class_drive.$(objext): class_drive.cpp class_drive.h util.h largefile.h
	$(CC) -o $@ -c class_drive.cpp

window.cxx window.h: window.fl
	fluid -c window.fl

clean:
	-rm *.linux*.o
	-rm *.win*.o
	-rm window.cxx
	-rm window.h

property: clean
	-rm mfpfltk.*.exe
	-rm mdpfltk.*.elf

doc:
	doxygen mfpfltk.doxygen
