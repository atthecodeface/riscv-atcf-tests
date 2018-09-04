#Note:
#make LLVM_BUILD=../tools RISCV_TOOLS_PREFIX=../tools/bin/riscv32-none-elf-

ROOT := ${CURDIR}
RISCV_TOOLS_PREFIX := ../tools/bin/riscv32-none-elf-
RISCV_TOOLS_PREFIX := ../riscv-gnu-tools/bin/riscv64-unknown-elf-
LLVM_BUILD := ../llvm_build

LD   := $(RISCV_TOOLS_PREFIX)ld
CC   := $(LLVM_BUILD)/bin/clang
OD   := $(RISCV_TOOLS_PREFIX)objdump
OC   := $(RISCV_TOOLS_PREFIX)objcopy


SRC_DIR     := ${ROOT}/src
TEST_DIR    := ${ROOT}/test
BUILD_DIR   := ${ROOT}/build
SCRIPTS_DIR := ${ROOT}/scripts
OBJ_DIR  := ${BUILD_DIR}/obj
MAP_DIR  := ${BUILD_DIR}/map
BIN_DIR  := ${BUILD_DIR}/bin
DUMP_DIR := ${BUILD_DIR}/dump

TEST_INCLUDES := --include=${TEST_DIR}/include/sim.h

CC_TARG_ARCH := --target=riscv64 -march=rv64i
LD_TARG_ARCH := -melf64lriscv  
OC_TARG_ARCH := --target elf64-littleriscv

CC_TARG_ARCH := --target=riscv32 -march=rv32i
LD_TARG_ARCH := -melf32lriscv  
OC_TARG_ARCH := --target elf32-littleriscv

all: ${DUMP_DIR}/loop.dump ${DUMP_DIR}/logic.dump ${DUMP_DIR}/traps.dump ${DUMP_DIR}/c_arith.dump ${DUMP_DIR}/c_stack.dump ${DUMP_DIR}/c_jump.dump
#${DUMP_DIR}/c_logic.dump
all: ${DUMP_DIR}/c_mv.dump ${DUMP_DIR}/c_branch.dump

all_old:
	${CC} --target=riscv32 -march=rv32i thing.S -c -o thing.o
	${CC} --target=riscv32 -march=rv32i fred.c  -c -o fred.o
	${LD} -melf32lriscv  fred.o thing.o --strip-all --script link.script -Map thing.map -o thing.elf.full -nostdlib
	${OC} --target elf32-littleriscv --remove-section=.comment --remove-section=.note thing.elf.full thing.elf

${OBJ_DIR}/%.o: ${SRC_DIR}/wrappers/%.S
	@echo "Assemble $< to $@"
	@${CC} ${CC_TARG_ARCH} ${TEST_INCLUDES} -I${SRC_DIR} $< -c -o $@

${OBJ_DIR}/%.o: ${SRC_DIR}/simple/%.c
	@echo "Compile $< to $@"
	@${CC} ${CC_TARG_ARCH}  ${TEST_INCLUDES} -I${SRC_DIR} $< -c -o $@

${OBJ_DIR}/%.o: ${SRC_DIR}/compressed/%.c
	@echo "Compile $< to $@"
	@${CC} --target=riscv32 -march=rv32ic  ${TEST_INCLUDES} -I${SRC_DIR} $< -c -o $@

${OBJ_DIR}/%.o: ${SRC_DIR}/compressed/%.S
	@echo "Assemble compressed $< to $@"
	@${CC} --target=riscv32 -march=rv32ic  ${TEST_INCLUDES} -I${SRC_DIR} $< -c -o $@

# --strip-all strips out the symbols, but we want 'tohost'
${BIN_DIR}/%.elf.full: ${OBJ_DIR}/%.o ${SCRIPTS_DIR}/link.script ${OBJ_DIR}/base.o ${OBJ_DIR}/trap.o 
	@echo "Link to $@"
	@${LD} ${LD_TARG_ARCH} ${OBJ_DIR}/base.o ${OBJ_DIR}/trap.o $< --script ${SCRIPTS_DIR}/link.script -Map ${MAP_DIR}/logic.map -o $@ -nostdlib

.PRECIOUS: ${BIN_DIR}/%.elf
.PRECIOUS: ${OBJ_DIR}/%.o

${BIN_DIR}/%.elf: ${BIN_DIR}/%.elf.full
	@echo "Objcopy $< to $@ (remove sections)"
	@${OC} ${OC_TARG_ARCH} --remove-section=.comment --remove-section=.note $< $@

${DUMP_DIR}/%.dump: ${BIN_DIR}/%.elf
	@echo "Disassemble to $@"
	@${OD} --disassemble-all --disassemble-zeroes --insn-width=2 $< >  $@

clean:
	rm -f ${OBJ_DIR}/* ${MAP_DIR}/* ${BIN_DIR}/* ${DUMP_DIR}/*
	mkdir -p ${OBJ_DIR} ${MAP_DIR} ${BIN_DIR} ${DUMP_DIR}

dump:
	${OD} -D thing.elf

map:
	cat thing.map

hdrs:
	${OD} -h thing.elf


hex:
	${OD} -j .start -s thing.elf

dis:
	${OD} -D ${BIN_DIR}/logic.elf.full

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
