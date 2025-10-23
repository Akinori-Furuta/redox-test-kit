# Redox OS test kit (now suspended)

## Introduction

This repository contains Redox OS test kit written
in C and bash codes.
Currently I suspend developing more tests. Due to
there are many critical errors (they may be memory
reference counting or locking, or waiting process
issue in Redox OS kernel).

## Required packages

Add packages `git`, `gcc13`, `gnu-grep`, and `gnu-make`. Type following command at Redox Desktop cosmic-termial window.

```bash
# Required packages
sudo pkg install git gcc13 gnu-grep gnu-make
```

And optional packages `openssh`, and `vim` will help you. You'll find more packages at [Redox OS packages / x86_64-unknown-redox and etc.](https://static.redox-os.org/pkg/)

```bash
# Optional packages
sudo pkg install openssh vim
```

> [!TIP]
> The pkg command won't terminate cleanly. It may stuck and
> print following message. Type __\[Ctrl\]-\[C\]__ to
> terminate command.
>
> ```text
> done
> shutodown(N, M) 
> ```

> [!TIP]
> You may see back trace similar to following output due
>  to page fault (it's Weird, is Rust language free
> from "segmentation fault"?).
> It's better running pkg command at cosmic-terminal
> rather than running pkg command at QEMU text terminal.
>
> ```text
> Page fault: 000000000000000C US
> RFLAG: 0000000000010297
> CS:    000000000000002b
> RIP:   000000000061ad77
> RSP:   0000000000d61e00
> SS:    0000000000000023
> FSBASE  000000000023e000
> GSBASE  0000000000000000
> KGSBASE ffff80007fc64000
> RAX:   0000000000000001
> RCX:   0000000000000000
> RDX:   0000000000000000
> RDI:   0000000000000004
> RSI:   0000000000000000
> R8:    0000000000000000
> R9:    000000000061ad70
> R10:   0000000000000001
> R11:   0000000000000246
> RBX:   0000000000000004
> RBP:   0000000000000010
> R12:   0000000000d61e60
> R13:   0000000000000018
> R14:   0000000000000001
> R15:   0000000000000000
>   FP ffff80000f02fe80: PC ffffffff8007c115
>     FFFFFFFF8007BF40+01D5
>     kernel::arch::x86_shared::interrupt::exception::page::inner
>   FP ffff80000f02ff50: PC ffffffff80078e87
>     FFFFFFFF80078E50+0037
>     kernel::arch::x86_shared::interrupt::exception::page
>   0000000000000010: GUARD PAGE
> ```

## Build

Use QEMU terminal (monitor/console multiplexer). Login as user,

```text
redox login: user
```
> [!TIP]
> If you can't see "`redox login: `" prompt, type enetr to see prompt.

### Clone sources from github

Create and change directory to clone git repository [github https://github.com/Akinori-Furuta/redox-test-kit.git](https://github.com/Akinori-Furuta/redox-test-kit.git). Here we use _/path/to/git-base_ as example. And clone git repository.

```bash
# Create directory to git clone base, if you needed.
mkdir -p /path/to/git-base
# Change directory to git clone base /path/to/git-base
cd /path/to/git-base
# Clone git repository
git clone https://github.com/Akinori-Furuta/redox-test-kit.git
```

### Make binaries

Change directory to cloned files are stored. And make files.

```bash
# Continue from previous type in
# Change directory to redox-test-kit
cd redox-test-kit
# Make files
make
```

## Run

### Simple test: Make files under directories

```bash
# Continue from previous type in
# Change directory to mk-files-tree
pushd mk-files-tree
# Run test
./mk-files-tree.sh
# mk-files-tree.sh creates 4096 binary files under ./test/ab/cd/abcdxxx...xxx
# Here "ab", "cd", and "xxx...xxx" are random string generated from base64 encoded pseudo radom binary strem. 
# Back to redox-test-kit
popd
```
On Redox OS, ./mk-files-tree.sh will fail. You may see following fails,

* Stop script before finish creating 4096 files
  * It seems waiting a event. You can "`PS`" to find process id (PID), "`kill 1` _PID_" from GUI cosmic-terminal.
    Note: Redox OS `kill` command requires signal number
    at first parameter, `1` means SIGHUP in UNIXes signaling system.
    * Some time, QEMU emulator process consumes
      near 100% CPU usage. Especially, run at cosmic-terminal or run other processes simultaneously.
* Kernel panic
  * You may see kernel panic around "src/memory/mod.rs:954:9", you will got following output,

    ```text
    KERNEL PANIC: panicked at src/memory/mod.rs:954:9:
      allocator-owned frames need a PageInfo, but none for [frame at 0x7ffffffffffff000]
    ```

    * It seems reference counting or locking
  failure at file descriptor context or error context.
    * Some memory free failure at system call gate.
    * Some time, QEMU emulator process consumes
      near 100% CPU usage. Especially, run at cosmic-terminal or run other processes simultaneously.
