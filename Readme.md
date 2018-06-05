RISC-V tests
============

This repository contains some larger scale RISC-V tests for use with
the LLVM compiler.

The purpose is to build on the very simple tests that are part of
riscv-tools, and not to replace them.

In particular, these tests are used to test my small RISC-V
implementations that are on github
(https://github.com/atthecodeface/cdl_hardware.git)

Making
------

To make the tests:

make clean
make all

To do this, it expects to find:

RISC-V GNU binutils (for ld, objdump, objcopy)
../riscv-gnu-tools/bin/riscv64-unknown-elf-*

RISC-V LLVM
../llvm_build/bin/clang

