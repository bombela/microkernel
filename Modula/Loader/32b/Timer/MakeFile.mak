# Pilote timer.

# Bombela
# 03/07/2004
# 29/09/2004
# 26/11/2005

include $(MODULA)/Loader/32b/MakeDef.mak

all: $(INCLUDE)/Timer.h Timer.elf

Timer.elf: Timer_Main.elf
	$(MSGLINK)
	@ld -melf_i386 -r INT_Timer.elf Timer_Main.elf CPU_Speed.elf -o Timer.elf

Timer_Main.elf: CPU_Speed.elf INT_Timer.elf $(INCLUDE)/*.h

INT_Timer.elf: INT_Timer.S
