TARGET:= pcapgrep


DEBUG?=no

CC:=gcc
LD:=gcc
TAR:=tar
RM:=rm
CFLAGS:=-Wall -Wunused -Werror -D_GNU_SOURCE
LDFLAGS:=-lpcap


ifeq ($(DEBUG),1)
CFLAGS+=-O0 -g -DDEBUG
else
CFLAGS+=-O3 -DNDEBUG
endif


ifeq ($(VERBOSE),1)
Q=
echo-cmd =
else
Q=@
echo-cmd = @echo $(1)
endif


SRCS:=main.c
SRCS+=grep.c


OBJS:=$(SRCS:%.c=%.o)
HDRS:=$(SRCS:%.c=%.h)


all: $(TARGET)


pcapgrep: $(OBJS)
	$(call echo-cmd,"   LD    $@")
	$(Q)$(LD) $(LDFLAGS) -o $@ $^


%.o: %.c
	$(call echo-cmd,"   CC    $@")
	$(Q)$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean


clean:
	$(call echo-cmd," CLEAN   ")
	$(Q)$(RM) -fr $(TARGET) $(OBJS) *~
