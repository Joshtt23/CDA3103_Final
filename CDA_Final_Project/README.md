# MySPIM MIPS Processor Simulator

A single-cycle MIPS processor simulator implemented in C for CDA3103 Computer Logic and Organization.

## Project Overview

This project implements a MIPS processor simulator that demonstrates the functions of the MIPS processor, including datapath operations and control signals. The simulator reads MIPS machine codes and simulates processor operations cycle-by-cycle.

## Features

- **Single-cycle datapath** implementation
- **14 MIPS instructions** support
- **32 general-purpose registers**
- **64kB memory** (0x0000 to 0xFFFF)
- **Big-endian byte order**
- **Word-aligned memory access**
- **Interactive simulation** with step-by-step execution

## Project Structure

```
CDA_Final_Project/
├── .cursor/
│   └── rules.md          # Project-specific guidelines
├── src/
│   ├── spimcore.c        # Main program (DO NOT MODIFY)
│   ├── spimcore.h        # Header file (DO NOT MODIFY)
│   └── project.c         # Implementation file (TO BE FILLED)
├── test/
│   └── *.asc             # Test machine code files
├── Makefile              # Compilation instructions
├── .gitignore            # Git ignore file
└── README.md             # This file
```

## Setup Instructions

1. **Place the provided files:**
   - `spimcore.c` → `src/spimcore.c`
   - `spimcore.h` → `src/spimcore.h`
   - `project.c` → `src/project.c`

2. **Create test directory:**
   ```bash
   make setup
   ```

3. **Place your .asc test files in the `test/` directory**

## Compilation

```bash
# Build the simulator
make

# Or manually:
gcc -Wall -Wextra -std=c99 -g -o spimcore src/spimcore.c src/project.c
```

## Usage

```bash
# Run with a test file
./spimcore test/your_file.asc

# Or use make
make run
```

## Interactive Commands

Once the simulator starts, you can use these commands:

- `r` - **Register**: Display register contents
- `m` - **Memory**: Display memory contents  
- `s` - **Step**: Execute one instruction
- `c` - **Continue**: Run all remaining instructions
- `h` - **Halt**: Check if simulation has halted
- `p` - **Print**: Display the input file
- `g` - **Controls**: Show control signals
- `x` - **Quit**: Exit the simulator

## Implementation Tasks

### Part 1: ALU Function
Implement the `ALU()` function with 8 operations:
- Addition, subtraction, comparison, logical operations
- See `.cursor/rules.md` for detailed specifications

### Part 2: 9 Datapath Functions
1. `instruction_fetch()` - Fetch instruction from memory
2. `instruction_partition()` - Parse instruction components
3. `instruction_decode()` - Decode opcode and set controls
4. `read_register()` - Read register values
5. `sign_extend()` - Sign extend immediate values
6. `ALU_operations()` - Perform ALU operations
7. `rw_memory()` - Memory read/write operations
8. `write_register()` - Write to registers
9. `PC_update()` - Update program counter

## Memory Specifications

- **Size**: 64kB (addresses 0x0000 to 0xFFFF)
- **Program Start**: 0x4000
- **Word Alignment**: All addresses must be multiples of 4
- **Byte Order**: Big-endian
- **Initialization**: All memory zeroed except .asc file contents

## Halt Conditions

The simulator halts when:
- Illegal instruction encountered
- Non-word-aligned jump address
- Non-word-aligned lw/sw address  
- Memory access beyond bounds

## Development Guidelines

- **DO NOT MODIFY** `spimcore.c` and `spimcore.h`
- **ALL CODE** goes in `project.c` only
- **NO I/O** statements in `project.c`
- Follow clean code principles and DRY methodology
- Add comprehensive comments for partial credit

## Testing

Create `.asc` files with hexadecimal machine codes:

```
20010000  # addi $1, $0, 0
200200c8  # addi $2, $0, 200
10220003  # beq $1, $2, 3
00000020  # delay slot
20210001  # addi $1, $1, 1
00000020  # no operation
```

## Make Targets

- `make` - Build the simulator
- `make clean` - Remove build artifacts
- `make run` - Build and run with test.asc
- `make debug` - Build with debug flags
- `make release` - Build optimized version
- `make setup` - Create test directory
- `make help` - Show available targets

## Submission

Submit only `project.c` through WebCourses. No additional report required.
