# MySPIM MIPS Processor Simulator - Project Rules

## Project Overview
This is a CDA3103 Computer Logic and Organization project to implement a MIPS processor simulator called MySPIM using C programming language.

## Critical Requirements
- **DO NOT MODIFY** `spimcore.c` and `spimcore.h` - these are provided files
- **ALL IMPLEMENTATION** must be in `project.c` only
- **NO INPUT/OUTPUT** statements in `project.c` (handled by spimcore)
- **SINGLE-CYCLE DATAPATH** implementation required

## Implementation Tasks

### Part 1: ALU Function
Implement `ALU()` function with 8 operations:
- 000: Z = A + B (addition)
- 001: Z = A - B (subtraction) 
- 010: Z = 1 if A < B, else 0 (signed comparison)
- 011: Z = 1 if A < B, else 0 (unsigned comparison)
- 100: Z = A AND B (bitwise AND)
- 101: Z = A OR B (bitwise OR)
- 110: Z = Shift B left by 16 bits
- 111: Z = NOT A (bitwise NOT)

### Part 2: 9 Datapath Functions
Refer to `image.png` (Figure 2: Single-cycle datapath) for component interactions and data flow.

1. **`instruction_fetch()`** - Fetch instruction from memory
   - Inputs: PC (Program Counter value)
   - Outputs: Instruction [31-0] (fetched instruction)
   - Component: Instruction Memory

2. **`instruction_partition()`** - Parse instruction into components
   - Inputs: Instruction [31-0]
   - Outputs: Instruction [25-21] (rs), [20-16] (rt), [15-11] (rd), [31-26] (opcode), [15-0] (immediate), [5-0] (funct)

3. **`instruction_decode()`** - Decode opcode and set control signals
   - Inputs: Instruction [31-26] (opcode)
   - Outputs: RegDst, Jump, Branch, MemRead, MemtoReg, ALUOp, MemWrite, ALUSrc, RegWrite
   - Component: Control Unit

4. **`read_register()`** - Read register values
   - Inputs: Instruction [25-21] (rs), [20-16] (rt), RegDst control signal
   - Outputs: Read data 1, Read data 2, Write register (selected by RegDst)
   - Component: Register File

5. **`sign_extend()`** - Sign extend immediate values
   - Inputs: Instruction [15-0] (16-bit immediate)
   - Outputs: 32-bit sign-extended value
   - Component: Sign Extend Unit

6. **`ALU_operations()`** - Perform ALU operations
   - Inputs: Read data 1, Read data 2, sign-extended value, ALUSrc, ALUOp, funct field, PC+4
   - Outputs: ALU result, Zero flag, branch target address
   - Components: ALU Control, ALU, Branch Target Adder

7. **`rw_memory()`** - Memory read/write operations
   - Inputs: ALU result (address), Read data 2 (write data), MemRead, MemWrite
   - Outputs: Read data (from memory)
   - Component: Data Memory

8. **`write_register()`** - Write to registers
   - Inputs: Write register, ALU result, Read data (from memory), MemtoReg, RegWrite
   - Outputs: Updates Register File
   - Component: Write-back multiplexer

9. **`PC_update()`** - Update program counter
   - Inputs: PC, Zero flag, Branch, Jump, Instruction [25-0], branch target
   - Outputs: New PC value
   - Components: PC+4 Adder, Jump Address Calculation, PC multiplexers

## 14 MIPS Instructions to Implement
Refer to `1.png` (Figure 1: Instructions table) for complete instruction specifications.

### Arithmetic Instructions
- **add**: `add $s1,$s2,$s3` → `$s1 = $s2 + $s3` (3 operands, overflow detected)
- **sub**: `sub $s1,$s2,$s3` → `$s1 = $s2 - $s3` (3 operands, overflow detected)
- **addi**: `addi $s1,$s2,100` → `$s1 = $s2 + 100` (+ constant, overflow detected)

### Logic Instructions
- **and**: `and $s1,$s2,$s3` → `$s1 = $s2 & $s3` (3 operands, logical AND)
- **or**: `or $s1,$s2,$s3` → `$s1 = $s2 | $s3` (3 operands, logical OR)

### Data Transfer Instructions
- **lw**: `lw $s1,100($s2)` → `$s1 = Memory[$s2 + 100]` (word from memory to register)
- **sw**: `sw $s1,100($s2)` → `Memory[$s2 + 100] = $s1` (word from register to memory)
- **lui**: `lui $s1,100` → `$s1 = 100 * 2^16` (loads constant in upper 16 bits)

### Conditional Branch Instructions
- **beq**: `beq $s1,$s2,25` → `if ($s1 == $s2) goto PC + 4 + 100` (equal test, PC relative branch)
- **slt**: `slt $s1,$s2,$s3` → `if ($s2 < $s3) $s1 = 1 else $s1 = 0` (compare less than, two's complement)
- **slti**: `slti $s1,$s2,100` → `if ($s2 < 100) $s1 = 1 else $s1 = 0` (compare < constant, two's complement)
- **sltu**: `sltu $s1,$s2,$s3` → `if ($s2 < $s3) $s1 = 1 else $s1 = 0` (compare less than, natural number)
- **sltiu**: `sltiu $s1,$s2,100` → `if ($s2 < 100) $s1 = 1 else $s1 = 0` (compare < constant, natural number)

### Unconditional Jump Instruction
- **j**: `j 2500` → `goto 10000` (Jump to target address)

## Memory Specifications
- **Size**: 64kB (0x0000 to 0xFFFF)
- **Program Start**: 0x4000
- **Word-aligned**: All addresses must be multiples of 4
- **Big-endian**: Most significant byte first
- **Initialization**: All memory zeroed except .asc file contents

## Halt Conditions
- Illegal instruction encountered
- Non-word-aligned jump address
- Non-word-aligned lw/sw address
- Memory access beyond bounds

## Code Quality Standards
- **Clean, concise code** following best practices
- **DRY principles** - avoid code duplication
- **Maintainable structure** with clear function organization
- **Comprehensive comments** for partial credit
- **No input/output** in project.c
- **Cross-platform compatibility** where possible

## Compilation
```bash
gcc -o spimcore spimcore.c project.c
./spimcore <filename>.asc
```

## Testing Commands
- `r` - Display registers
- `m` - Display memory
- `s` - Step through instruction
- `c` - Continue execution
- `h` - Check halt status
- `p` - Print input file
- `g` - Display control signals
- `x` - Quit simulation
