# Makefile for LeanBar2

NAME = TestPlugin
CC = g++
CFLAGS = -Os -Wall -s
CPPFLAGS = -I../../../blackbox -I. -mwindows -mno-cygwin 
LDFLAGS = $(CFLAGS)
TARGETS = testplugin.dll
LIBSEXE = -lgdi32 -lstdc++ ../../../blackbox/libBlackbox.a

OBJS = ../bbplusplus.cpp test.cpp

all: build

build: $(TARGETS) 

testplugin.dll: $(OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LIBSEXE) 
	ls -al $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

clean:
	rm -f $(TARGETS) *.o