ifneq ($(RESEND),1)
include $(DEVKITPRO)/libnx/switch_rules
endif

TARGET      := StarClash
SOURCES     := .
LIBS        := -lsdl2 -lpng -lz -lnx

# Impostazioni per il compilatore
CXXFLAGS    := -g -Wall -O2 -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE -I$(DEVKITPRO)/libnx/include -I$(DEVKITPRO)/portlibs/switch/include
LDFLAGS     := -specs=$(DEVKITPRO)/libnx/switch.specs -g $(CXXFLAGS) -L$(DEVKITPRO)/libnx/lib -L$(DEVKITPRO)/portlibs/switch/lib

# Trova tutti i file .cpp
CPPFILES    := $(wildcard *.cpp)
OFILES      := $(CPPFILES:.cpp=.o)

all: $(TARGET).nro

$(TARGET).nro: $(TARGET).elf

$(TARGET).elf: $(OFILES)
	$(CXX) $(LDFLAGS) $(OFILES) $(LIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
