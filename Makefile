PROGRAM=libuunicode.a
CPP=gcc
LIBS=-lm SOURCE=.
OBJS=uunicode.o
LINKFLAGS=-shared
CPPFLAGS=-D_REENTRANT -D_LINUX 
all : CPPFLAGS += -O3 -DNDEBUG 
all : libultraunicode 
all : tests

tests: libultraunicode
	$(CPP) $(CPPFLAGS) -o tests tests.cpp libuunicode.a
	./tests

libultraunicode: $(OBJS)
	ar rcs ./$(PROGRAM) $(OBJS)

%.o: $(SOURCE)/%.c
	$(CPP) $(CPPFLAGS) -fpic -c $< -o ./$@ 

clean:
	rm -rf *.o
	rm -rf $(PROGRAM)
	rm -rf tests

