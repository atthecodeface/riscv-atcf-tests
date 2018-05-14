LD   := ../riscv-gnu-tools/bin/riscv64-unknown-elf-ld
CC   := ../llvm_build/bin/clang
OD   := ../riscv-gnu-tools/bin/riscv64-unknown-elf-objdump
OC   := ../riscv-gnu-tools/bin/riscv64-unknown-elf-objcopy


SRC_DIR := src
BUILD_DIR := build
SCRIPTS_DIR := scripts
OBJ_DIR := ${BUILD_DIR}/obj
MAP_DIR := ${BUILD_DIR}/map
BIN_DIR := ${BUILD_DIR}/bin

all: ${BIN_DIR}/loop.elf

all_old:
	${CC} --target=riscv32 -march=rv32i thing.S -c -o thing.o
	${CC} --target=riscv32 -march=rv32i fred.c  -c -o fred.o
	${LD} -melf32lriscv  fred.o thing.o --strip-all --script link.script -Map thing.map -o thing.elf.full -nostdlib
	${OC} --target elf32-littleriscv --remove-section=.comment --remove-section=.note thing.elf.full thing.elf

${OBJ_DIR}/base.o: ${SRC_DIR}/wrappers/base.S
	${CC} --target=riscv32 -march=rv32i $< -c -o $@

${OBJ_DIR}/loop.o: ${SRC_DIR}/simple/loop.c
	${CC} --target=riscv32 -march=rv32i $< -c -o $@

${BIN_DIR}/loop.elf: ${OBJ_DIR}/base.o ${OBJ_DIR}/loop.o ${SCRIPTS_DIR}/link.script
	${LD} -melf32lriscv  ${OBJ_DIR}/base.o ${OBJ_DIR}/loop.o --strip-all --script ${SCRIPTS_DIR}/link.script -Map ${MAP_DIR}/loop.map -o ${BIN_DIR}/loop.elf.full -nostdlib
	${OC} --target elf32-littleriscv --remove-section=.comment --remove-section=.note ${BIN_DIR}/loop.elf.full ${BIN_DIR}/loop.elf

clean:
	rm -f ${OBJ_DIR}/* ${MAP_DIR}/* ${BIN_DIR}/*
	mkdir -p ${OBJ_DIR} ${MAP_DIR} ${BIN_DIR}

dump:
	${OD} -D thing.elf

map:
	cat thing.map

hdrs:
	${OD} -h thing.elf


hex:
	${OD} -j .start -s thing.elf

dis:
	${OD} -D thing.elf

fred:
	${CC} --target=riscv32 -march=rv32i fred.c -S -o fred.S
	${CC} --target=riscv32 -march=rv32i fred.c -c -o fred.o
	${OD} -D fred.o

# supported targets: elf64-littleriscv elf32-littleriscv elf64-little elf64-big elf32-little elf32-big plugin srec symbolsrec verilog tekhex binary ihex
# supported emulations: elf64lriscv elf32lriscv


#GNU_LD      = $(AFPC_TOOLS)/${BINUTILS_ARCH}/bin/ld
#OBJDUMP     = $(AFPC_TOOLS)/${BINUTILS_ARCH}/bin/objdump
#OBJCOPY     = $(AFPC_TOOLS)/${BINUTILS_ARCH}/bin/objcopy
#ELF_TO_MIF  = python $(SCRIPTS_DIR)/elf_to_mif.py --objdump=$(OBJDUMP) --verbose=1
