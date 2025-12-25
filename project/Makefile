#---------------------------------------------------------------------------------
# Makefile per StarClash - Configurazione Diretta devkitPro
#---------------------------------------------------------------------------------

# Percorsi interni di devkitPro
export DEVKITPRO := /opt/devkitpro
export DEVKITARM := $(DEVKITPRO)/devkitARM

include $(DEVKITPRO)/libnx/switch_rules

TARGET      := StarClash
ROMFS       := romfs
SOURCES     := .

# Librerie e Inclusioni con percorsi assoluti
LIBS    := -lSDL2_image -lSDL2 -lpng -ljpeg -lz -lnx
LIBDIRS := -L$(DEVKITPRO)/portlibs/switch/lib -L$(DEVKITPRO)/libnx/lib
INCLUDES := -I$(DEVKITPRO)/portlibs/switch/include -I$(DEVKITPRO)/libnx/include

# Impostazioni del compilatore per Nintendo Switch
CXXFLAGS := -g -Wall -O2 -ffunction-sections -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE $(INCLUDES) -D__SWITCH__
CFLAGS   := -g -Wall -O2 -ffunction-sections -fPIE $(INCLUDES) -D__SWITCH__
LDFLAGS  := -specs=$(DEVKITPRO)/libnx/switch.specs -g $(CXXFLAGS) $(LIBDIRS)

# Trova automaticamente tutti i file .cpp nella cartella
CPPFILES := $(wildcard *.cpp)
OFILES   := $(CPPFILES:.cpp=.o)

.PHONY: all clean

all: $(TARGET).nro

$(TARGET).nro: $(TARGET).elf

$(TARGET).elf: $(OFILES)
	$(CXX) $(LDFLAGS) $(OFILES) $(LIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o *.elf *.nro *.map
	@echo "Pulizia completata."