
.phony all:

all: PMan

debug: CXXFLAGS += -DDEBUG -g
debug: CCFLAGS += -DDEBUG -g
debug: PMan

PMan: PMan.c
	gcc PMan.c -lreadline -lhistory -o PMan -ggdb

.PHONY clean:
clean:
	-rm -rf *.o *.exe
