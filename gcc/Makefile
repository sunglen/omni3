#Makefile for arm cortex tools chain

TARGET := omni3v0.elf
CORE := cortex-m3
MCU := STM32F10X_HD
LDS := STM32F103ZE.ld

CC = arm-none-eabi-gcc
LIBS := 
INCLUDE := Include
INCLUDE += system board user ntshell
#OBJDIR= obj

CFLAGS :=  -Wall
CFLAGS :=  -mcpu=$(CORE) -D$(MCU)
CFLAGS += -mlittle-endian -mthumb
LDFLAGS := -T$(LDS)
LDFLAGS +=  -Wl,--gc-sections

# Compiler flags to generate dependency files.
#GENDEPFLAGS = -MMD -MP -MF .dep/$(@F).d
#use = , not :=
#DEPFLAGS = -MM -MT $@ -MF  .dep/$(patsubst %.o,%.o.d,$@)
DEPFLAGS = -MM -MT $@ -MF  $(patsubst %.o,%.o.d,$@)

OPENOCD = openocd
OPENOCD_FLAGS := -f /usr/share/openocd/scripts/board/mcs-1-32.cfg

#SOURCE := $(wildcard system/*.c board/*.c user/*.c ntshell/*.c *.s)
OBJS := $(sort \
$(patsubst %.s,%.o,$(wildcard system/*.s)) \
$(patsubst %.c,%.o,$(wildcard system/*.c)) \
$(patsubst %.c,%.o,$(wildcard board/*.c)) \
$(patsubst %.c,%.o,$(wildcard user/*.c)) \
$(patsubst %.c,%.o,$(wildcard ntshell/*.c)) \
)

#DEPS := $(patsubst %.c,%.o.d,$(wildcard *.c))
#the following is also OK
DEPS := $(OBJS:.o=.o.d)

.PHONY: target clean rebuild program

target: $(TARGET)
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -f $(DEPS)
rebuild: clean target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS) $(addprefix -l,$(LIBS))

# Include the dependency files.
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
-include $(DEPS)

%.o: %.c
	$(CC) $(CFLAGS)  $(addprefix -l,$(LIBS)) $(addprefix -I,$(INCLUDE))  $(DEPFLAGS) $<
	$(CC) $(CFLAGS)  $(addprefix -l,$(LIBS)) $(addprefix -I,$(INCLUDE))  -o $@ -c $<
%.o: %.s
	$(CC) $(CFLAGS)  $(addprefix -l,$(LIBS)) $(addprefix -I,$(INCLUDE)) -o $@ -c $<

program: target
	$(OPENOCD) $(OPENOCD_FLAGS) -c "program $(TARGET)"
