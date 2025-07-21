# -----------------------------------------------------------------------------------
# @description  Makefile for compiling, linking and flashing code 
#               into microcontroller Atmega328p
#
# @author       Marian Hrinko
# @date         02.06.2025
# @notes        Suffix Replacement within a macro: $(name:string1=string2)
#               For each word in 'name' replace 'string1' with 'string2'
#               For example $(DEPENDENCIES:.c=.o)
# @inspiration  https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
#               https://wiki.hacdc.org/index.php/AVR_Makefile
#
# -----------------------------------------------------------------------------------

# BASIC CONFIGURATION, SETTINGS
# -----------------------------------------------------------------------------------
TARGET        = main
LIBDIR        = lib
DEVICE        = atmega328p
FCPU          = 16000000
OPTIMIZE      = Os
CC            = avr-gcc
CFLAGS        = -g -Wall -DF_CPU=$(FCPU) -mmcu=$(DEVICE) -$(OPTIMIZE)
INCLUDES      = -I.
LIBS          = -L$(LIBDIR)
OBJCOPY       = avr-objcopy

# Objcopy, create hex file flags
# -----------------------------------------------------------------------------------
# -R .eeprom -O ihex or -j .data -j .text -O ihex
OBJFLAGS      = -j .data -j .text -O ihex
AVRSIZE       = avr-size
SFLAGS        = --mcu=$(DEVICE) --format=avr
SOURCES      := $(wildcard *.c $(LIBDIR)/*.c $(LIBDIR)/*/*.c)
OBJECTS	      = $(SOURCES:.c=.o)

# AVRDUDE CONFIGURATION, SETTINGS
# -----------------------------------------------------------------------------------
AVRDUDE       = avrdude
MMCU          = m328p
PORT          = /dev/ttyUSB0
PROGRAMMER    = usbasp
BAUD_RATE     = 19200
AVROBJ_FORMAT = ihex
AVRDUDE_FLAGS = -p $(MMCU) -P $(PORT) -c $(PROGRAMMER) -b $(BAUD_RATE) -u -U

# 
# Create file to programmer
main: $(TARGET).hex
	
# 
# Create hex file
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) $(OBJFLAGS) $(TARGET).elf $(TARGET).hex
	@echo "-----------------------------------------------------------------------" 
	$(AVRSIZE) $(TARGET).elf
# 
# Create .elf file
$(TARGET).elf:$(OBJECTS) 
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET).elf
#
# Create object files
%.o: %.c
	 $(CC) $(CFLAGS) -c $< -o $@
# 
# Program avr - send file to programmer
flash:
	@echo "-----------------------------------------------------------------------"
	$(AVRDUDE) $(AVRDUDE_FLAGS) flash:w:$(TARGET).hex:i
#
# Size
size: 
	@echo "-----------------------------------------------------------------------"
	$(AVRSIZE) -C --mcu=$(DEVICE) $(TARGET).elf
#
# Clean
clean:
	@echo "-----------------------------------------------------------------------"
	rm -f $(OBJECTS) $(TARGET).elf $(TARGET).map
#
# Cleanall
cleanall:
	@echo "-----------------------------------------------------------------------"
	rm -f $(OBJECTS) $(TARGET).hex $(TARGET).elf $(TARGET).map