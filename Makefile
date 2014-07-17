TOPDIR := ${shell pwd | sed -e 's/ /\\ /g'}

FLASH_CMD := st-flash --reset

all: compiler vm app app.bin app.hex

# compiler can always be run, raco make will figure the rest out
.PHONY: compiler vm

compiler: vm
	raco make compiler/picobit.rkt

vm:
	cd vm && make
	[ -e vm/picobit-vm ] && cp vm/picobit-vm . || rm -f picobit-vm

app.bin: app.hex
	$(TOPDIR)/hex2bin.py app/app.hex

app.hex: compiler app/app.scm
	racket $(TOPDIR)/compiler/picobit.rkt app/app.scm

clean:
	cd vm && make clean
	rm -f app/app.hex app/app.bin

test: compiler vm
	raco make tests/run-tests.rkt
	racket tests/run-tests.rkt

flash: flash-vm flash-scm

flash-scm:
	$(FLASH_CMD) write app/app.bin 0x08008000

flash-vm:
	$(FLASH_CMD) write vm/picobit.bin 0x08000000
