#include "spimcore.h"

// MIPS Opcodes for instruction_decode  
#define OPCODE_RTYPE   0x00  // 000000 - R-type instructions
#define OPCODE_J       0x02  // 000010 - jump (0800100b >> 26 = 0x02)
#define OPCODE_BEQ     0x04  // 000100 - branch on equal (114bfffd >> 26 = 0x04)
#define OPCODE_ADDI    0x08  // 001000 - add immediate (21080001 >> 26 = 0x08)
#define OPCODE_SLTI    0x0A  // 001010 - set less than immediate
#define OPCODE_SLTIU   0x0B  // 001011 - set less than immediate unsigned
#define OPCODE_ANDI    0x0C  // 001100 - and immediate
#define OPCODE_ORI     0x0D  // 001101 - or immediate
#define OPCODE_LUI     0x0F  // 001111 - load upper immediate (3c0e0020 = 0x3C >> 26 = 0x0F)
#define OPCODE_LW      0x23  // 100011 - load word (8fab0000 >> 26 = 0x23)
#define OPCODE_SW      0x2B  // 101011 - store word (afaa0000 >> 26 = 0x2B)

// Function codes for R-type instructions
#define FUNCT_ADD      0x20  // 100000 - add
#define FUNCT_SUB      0x22  // 100010 - subtract
#define FUNCT_AND      0x24  // 100100 - and
#define FUNCT_OR       0x25  // 100101 - or
#define FUNCT_SLT      0x2A  // 101010 - set less than
#define FUNCT_SLTU     0x2B  // 101011 - set less than unsigned

/* ALU */
/* 10 Points - Operations implemented correctly, with control signals consistent with Decode function */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch(ALUControl) {
        case 0x00: // Addition
            *ALUresult = A + B;
            break;
        case 0x01: // Subtraction
            *ALUresult = A - B;
            break;
        case 0x02: // Set less than (signed)
            *ALUresult = ((signed int)A < (signed int)B) ? 1 : 0;
            break;
        case 0x03: // Set less than unsigned
            *ALUresult = (A < B) ? 1 : 0;
            break;
        case 0x04: // AND
            *ALUresult = A & B;
            break;
        case 0x05: // OR
            *ALUresult = A | B;
            break;
        case 0x06: // Shift left by 16
            *ALUresult = B << 16;
            break;
        case 0x07: // NOT A
            *ALUresult = ~A;
            break;
        default:
            *ALUresult = 0;
            break;
    }
    
    // Set Zero flag if result is zero
    *Zero = (*ALUresult == 0) ? 1 : 0;
}

/* instruction fetch */
/* 10 Points - Word Alignment Verification + Fetches Instruction at Correct Address */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // Check word alignment - addresses must be multiples of 4
    if (PC & 0x3) {
        return 1; // Halt condition: non-word-aligned address
    }
    
    // Check if PC is within memory bounds (0x0000 to 0xFFFF)
    if (PC > 0xFFFF) {
        return 1; // Halt condition: address beyond memory
    }
    
    // Use PC >> 2 to get actual location (Mem is array of words, not bytes)
    unsigned memIndex = PC >> 2;
    
    // Check if memory index is within bounds
    // MEMSIZE is 65536 >> 2 = 16384, so valid indices are 0 to 16383
    if (memIndex >= 16384) {
        return 1; // Halt condition: memory access beyond bounds
    }
    
    // Fetch instruction from memory
    *instruction = Mem[memIndex];
    
    return 0; // No halt condition
}


/* instruction partition */
/* 10 Points - Extract all instruction fields regardless of instruction type */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // Extract opcode [31-26]
    *op = (instruction & 0xFC000000) >> 26;
    
    // Extract rs [25-21]
    *r1 = (instruction & 0x03E00000) >> 21;
    
    // Extract rt [20-16]
    *r2 = (instruction & 0x001F0000) >> 16;
    
    // Extract rd [15-11]
    *r3 = (instruction & 0x0000F800) >> 11;
    
    // Extract funct [5-0]
    *funct = instruction & 0x0000003F;
    
    // Extract immediate/offset [15-0]
    *offset = instruction & 0x0000FFFF;
    
    // Extract jump section [25-0] for jump instructions
    *jsec = instruction & 0x03FFFFFF;
}



/* instruction decode */
/* 15 Points - Full Marks: All control signals set correctly + Halt Management */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // Initialize all control signals to default values
    controls->RegDst = 2;    // "don't care"
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 0;
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 0;
    
    switch(op) {
        case OPCODE_RTYPE: // R-type instructions
            controls->RegDst = 1;    // rd field
            controls->RegWrite = 1;  // write to register
            controls->ALUOp = 7;     // 111 - use funct field
            break;
            
        case OPCODE_ADDI: // addi
            controls->RegDst = 0;    // rt field for I-type instructions
            controls->RegWrite = 1;  // write to register
            controls->ALUSrc = 1;    // use immediate value
            controls->ALUOp = 0;     // 000 - addition
            break;
            
        case OPCODE_SLTI: // slti
            controls->RegDst = 0;    // rt field for I-type instructions
            controls->RegWrite = 1;  // write to register
            controls->ALUSrc = 1;    // use immediate value
            controls->ALUOp = 2;     // 010 - set less than
            break;
            
        case OPCODE_SLTIU: // sltiu
            controls->RegDst = 0;    // rt field for I-type instructions
            controls->RegWrite = 1;  // write to register
            controls->ALUSrc = 1;    // use immediate value
            controls->ALUOp = 3;     // 011 - set less than unsigned
            break;
            
        case OPCODE_ANDI: // andi
            controls->RegDst = 0;    // rt field for I-type instructions
            controls->RegWrite = 1;  // write to register
            controls->ALUSrc = 1;    // use immediate value
            controls->ALUOp = 4;     // 100 - AND
            break;
            
        case OPCODE_ORI: // ori
            controls->RegDst = 0;    // rt field for I-type instructions
            controls->RegWrite = 1;  // write to register
            controls->ALUSrc = 1;    // use immediate value
            controls->ALUOp = 5;     // 101 - OR
            break;
            
        case OPCODE_LUI: // lui
            controls->RegDst = 0;    // rt field for I-type instructions
            controls->RegWrite = 1;  // write to register
            controls->ALUSrc = 1;    // use immediate value
            controls->ALUOp = 6;     // 110 - shift left by 16
            break;
            
        case OPCODE_LW: // lw
            controls->RegDst = 0;    // rt field for I-type instructions
            controls->MemRead = 1;   // read from memory
            controls->RegWrite = 1;  // write to register
            controls->MemtoReg = 1;  // data from memory
            controls->ALUSrc = 1;    // use immediate value for address calculation
            controls->ALUOp = 0;     // 000 - addition for address
            break;
            
        case OPCODE_SW: // sw
            controls->MemWrite = 1;  // write to memory
            controls->ALUSrc = 1;    // use immediate value for address calculation
            controls->ALUOp = 0;     // 000 - addition for address
            break;
            
        case OPCODE_BEQ: // beq
            controls->Branch = 1;    // branch instruction
            controls->ALUOp = 1;     // 001 - subtraction for comparison
            controls->ALUSrc = 0;    // use register values for comparison
            break;
            
        case OPCODE_J: // j
            controls->Jump = 1;      // jump instruction
            break;
            
        default:
            // Illegal instruction - halt condition
            return 1;
    }
    
    return 0; // No halt condition
}

/* Read Register */
/* 5 Points - Read both registers correctly */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    // Read data from rs register (r1)
    *data1 = Reg[r1];
    
    // Read data from rt register (r2)
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points - Full Marks: Handle both negative and positive numbers */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    // Check if the 16th bit (sign bit) is set
    if (offset & 0x8000) {
        // Negative number - fill upper 16 bits with 1s
        *extended_value = offset | 0xFFFF0000;
    } else {
        // Non-negative number - fill upper 16 bits with 0s
        *extended_value = offset & 0x0000FFFF;
    }
}

/* ALU operations */
/* 10 Points - Full Marks: Proper ALU control signals, halt conditions, ALUSrc for BEQ */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    unsigned A, B, ALUControl;
    
    // Set ALU input A
    A = data1;
    
    // Set ALU input B based on ALUSrc control signal
    if (ALUSrc == 1) {
        B = extended_value;  // Use sign-extended immediate value
    } else {
        B = data2;           // Use register value
    }
    
    // Determine ALU control signal
    if (ALUOp == 7) {
        // R-type instruction - use funct field for ALU control
        switch(funct) {
            case FUNCT_ADD:  // add
                ALUControl = 0x00; // addition
                break;
            case FUNCT_SUB:  // sub
                ALUControl = 0x01; // subtraction
                break;
            case FUNCT_AND:  // and
                ALUControl = 0x04; // AND
                break;
            case FUNCT_OR:   // or
                ALUControl = 0x05; // OR
                break;
            case FUNCT_SLT:  // slt
                ALUControl = 0x02; // set less than (signed)
                break;
            case FUNCT_SLTU: // sltu
                ALUControl = 0x03; // set less than unsigned
                break;
            default:
                return 1; // Illegal function code - halt
        }
    } else {
        // Use ALUOp directly
        ALUControl = ALUOp;
    }
    
    // Call ALU function
    ALU(A, B, ALUControl, ALUresult, Zero);
    
    return 0; // No halt condition
}

/* Read / Write Memory */
/* 10 Points - Full Marks: Read and write operations with correct halt conditions */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    // Only check memory bounds and alignment if we're actually accessing memory
    if (MemWrite == 1 || MemRead == 1) {
        // Check word alignment for address
        if (ALUresult & 0x3) {
            return 1; // Halt condition: non-word-aligned address
        }
        
        // Check if address is within memory bounds (0x0000 to 0xFFFF)
        if (ALUresult > 0xFFFF) {
            return 1; // Halt condition: address beyond memory
        }
        
        unsigned memIndex = ALUresult >> 2;
        
        // Check if memory index is within bounds
        // MEMSIZE is 65536 >> 2 = 16384, so valid indices are 0 to 16383
        if (memIndex >= 16384) {
            return 1; // Halt condition: memory access beyond bounds
        }
        
        // Memory write operation
        if (MemWrite == 1) {
            Mem[memIndex] = data2;
        }
        
        // Memory read operation
        if (MemRead == 1) {
            *memdata = Mem[memIndex];
        }
    }
    
    return 0; // No halt condition
}


/* Write Register */
/* 10 Points - Full Marks: Correct location, data, and write permission check */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    // Check if register write is enabled
    if (RegWrite == 1) {
        unsigned writeReg, writeData;
        
        // Determine which register to write to (RegDst multiplexer)
        if (RegDst == 1) {
            writeReg = r3; // rd field (for R-type instructions)
        } else {
            writeReg = r2; // rt field (for I-type instructions)
        }
        
        // Determine what data to write (MemtoReg multiplexer)
        if (MemtoReg == 1) {
            writeData = memdata; // Data from memory (for lw instruction)
        } else {
            writeData = ALUresult; // Data from ALU result
        }
        
        // Write to register (but never write to $0 which is always 0)
        if (writeReg != 0) {
            Reg[writeReg] = writeData;
        }
    }
}

/* PC update */
/* 10 Points - Full Marks: Updates for branch, jump, and non-branch instructions */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    unsigned nextPC = *PC + 4; // Default: sequential execution
    
    // Handle jump instruction
    if (Jump == 1) {
        // Jump: Left shift jsec by 2 and use upper 4 bits of current PC+4
        unsigned jumpAddr = jsec << 2;
        jumpAddr |= ((*PC + 4) & 0xF0000000); // Take upper 4 bits from PC+4
        
        nextPC = jumpAddr;
    }
    // Handle branch instruction
    else if (Branch == 1 && Zero == 1) {
        // Branch taken: PC = PC + 4 + (sign-extended offset << 2)
        // The extended_value is already sign-extended from offset
        unsigned branchOffset = extended_value << 2;
        nextPC = *PC + 4 + branchOffset;
    }
    // Otherwise, continue with sequential execution (PC + 4)
    
    *PC = nextPC;
}

