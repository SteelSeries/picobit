TOPDIR := ${shell pwd | sed -e 's/ /\\ /g'}

all: compiler vm app

# compiler can always be run, raco make will figure the rest out
.PHONY: compiler vm

compiler: vm
	raco make compiler/picobit.rkt

vm:
	cd vm && make
	[ -e vm/picobit-vm ] && cp vm/picobit-vm . || rm -f picobit-vm

app: compiler
	$(TOPDIR)/picobit --size app/app.scm
	$(TOPDIR)/hex2bin.py app/app.hex

clean:
	cd vm && make clean
	rm -f app/app.hex app/app.bin

test: compiler vm
	raco make tests/run-tests.rkt
	racket tests/run-tests.rkt

flash:
	st-flash write vm/picobit.bin 0x08000000 && \
	st-flash write app/app.bin 0x08008000
