# For bbDesktop

NAME = bbDesktop
CC = g++
CFLAGS = -Os -Wall -s
CPPFLAGS = -I../../blackbox -I. -mwindows -mno-cygwin 
LDFLAGS = $(CFLAGS)
TARGETS = bbDesktop.dll
LIBSEXE = -lgdi32 -lstdc++ 

OBJS = bbPlugin.o DrawIcons.o configfile.o stringutils.o

all: build

build: $(TARGETS) 

bbDesktop.dll: $(OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LIBSEXE) 
	ls -al $@

bbDesktop.exe: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBSEXE)
	ls -al $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

clean:
	rm -f $(TARGETS) *.o