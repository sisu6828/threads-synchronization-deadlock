DEBUG   := y
CC      := gcc
OS      := $(shell uname)
CFLAGS  := -std=gnu99 -g -Wall  -Wno-deprecated-declarations -I ../psem
LDFLAGS :=
C_LINK_OPTIONS = -lm
CUNIT_LINK = -lcunit

ifeq ($(DEBUG), y	)
	CFLAGS += -DDEBUG -g
endif

ifeq ($(OS), Linux)
    CFLAGS += -pthread
    LDLIBS += -pthread -lrt
endif

.PHONY: all clean

all: bin/n_barrier_test bin/sthreads_test bin/bank_test

../psem/psem.o:
	cd ../psem; make 

bin/bank_test:  obj/bank_test.o obj/bank.o src/bank.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(filter %.o, $^) -o $@
	
bin/n_barrier_test: ../psem/psem.o obj/n_barrier.o obj/n_barrier_test.o src/n_barrier.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(filter %.o, $^) -o $@

bin/sthreads_test: obj/sthreads_test.o obj/sthreads.o src/sthreads.h 
	$(CC) $(CFLAGS) $(LDLIBS) -lcunit $(filter-out src/sthreads.h, $^) -o $@ $(CUNIT_LINK)

obj/sthreads.o: src/sthreads.c src/sthreads.h
	$(CC) $(CFLAGS) -c  $(filter-out src/sthreads.h, $^) -o $@ 

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c  $< -o $@

clean:
	$(RM) *~ src/*~ src/#* obj/*.o bin/*
	$(RM) -rf bin/*.dSYM
	cd ../psem; make clean

