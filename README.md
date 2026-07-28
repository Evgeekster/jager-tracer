# jager-trace

A small Linux syscall tracer written in C. A learning project — getting familiar with `ptrace`, `fork`, and the Linux syscalls.

## What it does

Runs any program under `ptrace`, intercepts every system call, and prints its name, arguments, and return code. On exit, displays a summary table — call counts, error counts, total and average time per syscall.

Also able to decode strings from memory of a running process. Currently i couldn't figure out how to parse argements properly without switch-case spamming for every syscall.

## Build

```bash
make
```

Binary will be at `build/jagertrace`.

## Usage

```bash
build/jagertrace [-e] <program> [args]

```
| Flag | Description |
|------|-------------|
| `-e` | Print syscall errors with `strerror` description |
| `-h` | Help |

Examples:
```bash
build/jagertrace ls -la
build/jagertrace -e cat /etc/passwd
```

## Architecture

Tested on **x86_64**. ARM and AArch64 support is started  in the code (`pch.h`) but untested and is not expected to function.

## Project structure

```
src/
├── main.c              — entry point, argument parsing
├── pch.h               — shared includes and arch macros (x86_64/arm/aarch64)
├── trace/              — main ptrace loop, syscall entry/exit handling
├── decode/             — reads string arguments from tracee memory
├── syscall_map/        — syscall number → name table
├── util/               — dynamic list, stats table output
└── net/                — networking (to be implemented)
```

## Dependencies

Python3 & Standard Linux library only. Requires a kernel with `ptrace` support.

## Afterwards

Feel free to use the project however you need or some competent criticism.
Have fun 