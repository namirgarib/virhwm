//
// Created by Namir Garib on 2024/09/08.
//

#include "virhwm.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>


/* ============== MICROPROGRAM INSTRUCTIONS ================= */
/* ====================== CHANGE HERE ======================== */
MIP mip[] = {
    {0,1,1,1,0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,0,1,0,0,0,0,0,-1},
    {0,1,1,1,0,0,0,1,1,0,0,1,0,0,1,0,1,0,0,0,1,1,1,1,0,1,0,0,0,0,0,7},
    {0,1,1,1,0,0,0,1,1,0,0,1,0,0,0,0,1,1,0,0,1,1,1,1,1,0,0,0,0,0,0,8},
    {0,1,1,0,0,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,1,1,1,0,0,1,1,0,0,1,1,0,1,0,1,0,0,1,1,0,1,1,1,1,1,0,0,0,0,0,0},
    {0,1,1,1,0,0,0,1,1,2,1,1,0,0,0,0,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0},
    {0,1,1,1,0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,0,1,0,0,0,0,0,63},
    {0,1,1,1,0,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {0,1,1,1,0,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,1,1,0,0,0,0,1,0}
};
/* ==================================================================== */


// Global variables
short memory[nmax] = {0}; // MEMORY ARRAY
short pc = 0;
short a_reg = 0;
short ma1_reg = 0;
short ma2_reg = 0;
short md1_reg = 0;
short md2_reg = 0;
short loop_reg = 0;
short A = 0;
short B = 0;
short C = 0;
short D = 0;

bool ovf = false;
bool zf = false;
bool mf = false;
bool pre_ovf = false;
bool pre_mf = false;
bool pre_zf = false;
bool jump = false;

short mem[nmax] = {0}; // Hardware memory
short a_regi[8] = {0};
short b_regi[8] = {0};

std::vector<short> a_vector(8);
std::vector<short> b_vector(8);

//memset(mem, 0, sizeof(mem));
//short a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
//short b[8] = {1, 1, 1 ,1 ,1, 1, 1, 1};
//memcpy(mem, a, sizeof(a));
//memcpy(mem + 8, b, sizeof(b));

/* ========== FUNCTION DECLARATIONS ========== */
void initializeMemory() {
    std::copy(memory, memory + nmax, mem);
    std::copy(mem, mem + 8, a_regi);
    std::copy(mem + 8, mem + 16, b_regi);

    a_vector = std::vector<short>(a_regi, a_regi + 8);
    b_vector = std::vector<short>(b_regi, b_regi + 8);
}

void updateMemory() {
    // Copy the contents of a_vector to the first 8 elements of memory
    std::copy(a_vector.begin(), a_vector.end(), mem);

    // Copy the contents of b_vector to the next 8 elements of memory
    std::copy(b_vector.begin(), b_vector.end(), mem + 8);
}

// ALU OPERATIONS
Result falu(short mode, short a, short b) {
    Result result{};
    result.C = 0;
    result.ovf = false;
    result.z = false;
    result.m = false;

    // ALU LOGIC
    switch (mode) {
        case 1:  // Addition
            result.C = a + b;
            result.z = result.C == 0;
            result.m = result.C < 0;
            result.ovf = ((a > 0 && b > 0 && result.C < 0) || (a < 0 && b < 0 && result.C > 0));
        break;

        case 2:  // Subtraction
            result.C = a - b;
            result.z = result.C == 0;
            result.m = result.C < 0;
            result.ovf = ((a > 0 && b < 0 && result.C < 0) || (a < 0 && b > 0 && result.C > 0));
        break;

        case 3:  // Bitwise OR
            result.C = a | b;
            result.z = result.C == 0;
            result.m = result.C < 0;
        break;

        case 4:  // Bitwise AND
            result.C = a & b;
            result.z = result.C == 0;
            result.m = result.C < 0;
        break;

        case 5:  // Bitwise XOR
            result.C = a ^ b;
            result.z = result.C == 0;
            result.m = result.C < 0;
        break;

        case 6:  // Right shift
            result.C = a >> 1;
            result.C |= (a & 0x8000);  // Keep the sign bit
            result.m = (result.C & 0x8000) != 0;
            result.z = result.C == 0;
        break;

        case 7:  // Left shift
            result.C = a << 1;
            result.C &= 0xFFFF;  // Ensure it stays 16-bit
            result.m = (result.C & 0x8000) != 0;
            result.z = result.C == 0;
        break;

        default:
            std::cout << "Invalid mode" << std::endl;
            exit(EXIT_FAILURE);
    }

    return result;
}

short mpy(int m1, int m2) {
    return m1 * m2;
}

void microprogramLoop() {
    while (1) {
        MIP now = mip[pc];  // Fetch microinstruction

        if (now.stp != 0) {
            std::cout << "Program end" << std::endl;
            break;
        }

        // Handle MA1 register
        if (now.MA1l == 1) {
            if (now.MA1i == 1) {
                ma1_reg = C;
            } else if (now.MA1i == 0 && now.Deo == 0) {
                ma1_reg = now.emit;
            }
        }
        if (now.A1I == 1) {
            ma1_reg++;
        }

        // Handle MA2 register
        if (now.MA2l == 1 && now.Deo == 0) {
            ma2_reg = now.emit;
        }
        if (now.A2I == 1) {
            ma2_reg++;
        }

        // Memory read
        if (now.MD1l == 1) {
            if (now.MD1i == 0 && now.r_w == 1 && now.memo == 0) {
                if (ma1_reg < nmax && ma1_reg >= 0) {
                    md1_reg = mem[ma1_reg];
                } else {
                    std::cerr << "Memory read error: out of memory range" << std::endl;
                    exit(0);
                }
            } else if (now.MD1i == 1 && now.Deo == 0) {
                md1_reg = now.emit;
            } else if (now.MD1i == 2) {
                md1_reg = C;
            }
        }

        // Memory write
        if (now.r_w == 0 && now.memo == 0) {
            if (now.MMD1o == 0 && ma1_reg < nmax && ma1_reg >= 0) {
                mem[ma1_reg] = md1_reg;
            }
            if (now.MMD2o == 0 && ma2_reg < nmax && ma2_reg >= 0) {
                mem[ma2_reg] = md2_reg;
            }
        }

        // A-bus control
        if ((now.Ao + now.MA1o + now.A1o + now.A0o + now.Byo) < 4) {
            std::cerr << "A-bus conflict error" << std::endl;
            exit(0);
        }

        if (now.Ao == 0) {
            A = a_reg;
        } else if (now.MA1o == 0) {
            A = ma1_reg;
        } else if (now.A1o == 0) {
            A = 1;
        } else if (now.A0o == 0) {
            A = 0;
        } else if (now.Byo == 0) {
            A = C;
        }

        // MPY
        D = mpy(md1_reg, md2_reg);

        // B-bus control
        if ((now.BMD1o + now.BMPYo + now.Beo) < 2) {
            std::cerr << "B-bus conflict error" << std::endl;
            exit(0);
        }

        if (now.BMD1o == 0) {
            B = md1_reg;
        } else if (now.BMPYo == 0) {
            B = D;
        } else if (now.Beo == 0) {
            B = now.emit;
        }

        // Latch control
        if (now.Al == 1) {
            a_reg = C;
        }

        // ALU operation
        Result result = falu(now.alu, A, B);
        C = result.C;
        ovf = result.ovf;
        zf = result.z;
        mf = result.m;

        // Check loop register limits
        if (now.Lpc == 1 && loop_reg >= l_max) {
            std::cerr << "Out of range for loop register" << std::endl;
            exit(0);
        }

        // Branch control
        if (jump) {
            if (now.emit >= p_max) {
                std::cerr << "Program crash" << std::endl;
                exit(0);
            } else {
                pc = now.emit;
            }
        } else {
            if (now.Lpc == 1 && loop_reg != 0) {
                pc = pc;
            } else {
                pc++;
            }
        }

        // Print memory contents and program state after each step
        printf("PC: %d\n", pc);
        printf("ALU Result: %d, Overflow: %d, Zero: %d, Minus: %d\n", C, ovf, zf, mf);
        printf("Memory contents:\n");
        for (int i = 0; i < nmax; i++) {
            printf("%4d ", mem[i]);
            if (i % 8 == 7) {
                printf("\n");
            }
        }
        printf("\n");
    }
}
