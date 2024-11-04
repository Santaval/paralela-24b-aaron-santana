include ../../../common/Makefile

CC=mpicc
XC=mpic++

INCLUDE += -Iinclude
SRC=hello

# Add new rule to test using mpiexec

TESTMPI=$(TESTINP:$(TESTS)/input%.txt=mpi/output%.txt)

.PHONY: testmpi
testmpi: SHELL:=/bin/bash
testmpi: $(TARGETS) $(TESTMPI)

mpi/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt $(TESTS)/args%.txt
	-$(DIFF) $(word 2,$^) <(mpiexec -np 3 $(EXEARGS) $(shell cat $(word 3,$^)) < $< | sort) ||:
