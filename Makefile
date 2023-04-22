# Makefile for ex2_q2 spring 2023B

CFLAGS   = -Wall -std=c99
LDFLAGS  = -lm
CC       = gcc
ECHO     = @echo "going to build target $@ (dependent on $^)"

PROG1 = one_student
PROG2 = read_grades
PROG3 = ex2_q1
PROGS = $(PROG1) $(PROG2) $(PROG3)

all: $(PROGS) test


$(PROG1): $(PROG1).c $(PROG3).h
	$(ECHO)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

$(PROG2): $(PROG2).c $(PROG3).h
	$(ECHO)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

$(PROG3): $(PROG3).c $(PROG3).h
	$(ECHO)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -vf *.o ${PROG} all_std.log *.temp

test: $(PROGS)
	@echo going to run test...
	./$(PROG3) readers_msg.txt gr_1.txt gr_2.txt
