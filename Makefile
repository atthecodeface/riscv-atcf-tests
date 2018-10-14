#a Variables
# Note:
# make LLVM_BUILD=../tools RISCV_TOOLS_PREFIX=../tools/bin/riscv32-none-elf-
# make LLVM_BUILD=/home/gavin/Git/riscv_tools RISCV_TOOLS_PREFIX=/home/gavin/Git/riscv_tools/bin/riscv64-elf-

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

.PHONY: all all_binaries
all: all_binaries

#a Templates
# rv.add_single_obj
#
# Add an object descriptor (list file) to a binary
#
# @param $1	binary name
# @param $2 object descriptor
#
define rv.add_single_obj
.PRECIOUS: ${OBJ_DIR}/$2.o
$2__OBJ := ${OBJ_DIR}/$2.o
$1__LINK_OBJECTS += $$($2__OBJ)
endef

# rv.add_obj
#
# Add a list of  object descriptors to a binary
#
# @param $1	binary name
# @param $2 object descriptors
#
define rv.add_obj
$(foreach obj,$2,$(eval $(call rv.add_single_obj,$1,$(obj))))
endef

# rv.link
#
# Create link targets to build a binary
#
# @param $1	binary name (for .dump/elf etc)
# @param $2	link script name
# @param $3	objects
#
define rv.link

$1__SCRIPT   := ${SCRIPTS_DIR}/$2.script
$1__DUMP     := ${DUMP_DIR}/$1.dump
$1__MAP      := ${MAP_DIR}/$1.map
$1__ELF_FULL := ${BIN_DIR}/$1.elf.full
$1__ELF      := ${BIN_DIR}/$1.elf

#$1__LINK_OBJECTS ?= 


# --strip-all strips out the symbols, but we want 'tohost'
$$($1__ELF_FULL): $$($1__LINK_OBJECTS) $$($1__SCRIPT)
	@echo "Link to $$($1__ELF_FULL)"
	@${LD} ${LD_TARG_ARCH} $$($1__LINK_OBJECTS) --script $$($1__SCRIPT) -Map $$($1__MAP) -o $$($1__ELF_FULL) -nostdlib

.PRECIOUS: $$($1__ELF)
$$($1__ELF): $$($1__ELF_FULL)
	@echo "Objcopy $$($1__ELF_FULL) to $$($1__ELF) (remove sections)"
	@${OC} ${OC_TARG_ARCH} --remove-section=.comment --remove-section=.note $$($1__ELF_FULL) $$($1__ELF)

$$($1__DUMP): $$($1__ELF)
	@echo "Disassemble to $$($1__DUMP)"
	@${OD} --disassemble-all --disassemble-zeroes --insn-width=2 $$($1__ELF) > $$($1__DUMP)

.PHONY: $1__DUMP
$1__DUMP: $$($1__DUMP)

$1__CLEAN:
	rm -f $$($1__ELF_FULL) $$($1__ELF) $$($1__DUMP) $$($1__MAP)

clean: $1__CLEAN

all_binaries: $1__DUMP

$(eval $(call rv.add_obj,$1,$3))

endef

#a Define binaries
help:
	@echo "Help:"
	@echo ""
	@echo "make all"
	@echo "  to make all the RISC-V tests for the atthecodeface RISC-V cdl_hardware regression"
	@echo ""
	@echo "make hps"
	@echo "  to make the HPS RISC-V images"

$(eval $(call rv.link,loop,link,base loop))
$(eval $(call rv.link,logic,link,base logic))
$(eval $(call rv.link,traps,link,base trap traps))
$(eval $(call rv.link,timer_irqs,link,base trap_int timer timer_irqs))
$(eval $(call rv.link,data,link,base trap data))
$(eval $(call rv.link,c_dprintf,link,base c_dprintf))
$(eval $(call rv.link,c_arith,link,base c_arith))
$(eval $(call rv.link,c_stack,link,base c_stack))
$(eval $(call rv.link,c_jump,link,base c_jump))
$(eval $(call rv.link,c_logic,link,base c_logic))
$(eval $(call rv.link,c_mv,link,base c_mv))
$(eval $(call rv.link,c_branch,link,base c_branch))

#a Default rules for building objects
${OBJ_DIR}/%.o: ${SRC_DIR}/wrappers/%.S
	@echo "Assemble $< to $@"
	@${CC} ${CC_TARG_ARCH} ${TEST_INCLUDES} -I${SRC_DIR} $< -c -o $@

${OBJ_DIR}/%.o: ${SRC_DIR}/simple/%.c
	@echo "Compile $< to $@"
	@${CC} ${CC_TARG_ARCH}  ${TEST_INCLUDES} -I${SRC_DIR} $< -c -o $@

${OBJ_DIR}/%.o: ${SRC_DIR}/lib/%.c
	@echo "Compile $< to $@"
	@${CC} ${CC_TARG_ARCH}  ${TEST_INCLUDES} -I${SRC_DIR} $< -c -o $@

${OBJ_DIR}/c_%.o: ${SRC_DIR}/simple/%.c
	@echo "Compile $< to $@"
	@${CC} --target=riscv32 -march=rv32ic  ${TEST_INCLUDES} -I${SRC_DIR} $< -c -o $@

${OBJ_DIR}/%.o: ${SRC_DIR}/compressed/%.c
	@echo "Compile $< to $@"
	@${CC} --target=riscv32 -march=rv32ic  ${TEST_INCLUDES} -I${SRC_DIR} $< -c -o $@

${OBJ_DIR}/%.o: ${SRC_DIR}/compressed/%.S
	@echo "Assemble compressed $< to $@"
	@${CC} --target=riscv32 -march=rv32ic  ${TEST_INCLUDES} -I${SRC_DIR} $< -c -o $@

.PRECIOUS: ${OBJ_DIR}/%.o


clean:
	rm -f ${OBJ_DIR}/*.o
	mkdir -p ${OBJ_DIR} ${MAP_DIR} ${BIN_DIR} ${DUMP_DIR}

#a Old stuff
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

include Makefile.hps
