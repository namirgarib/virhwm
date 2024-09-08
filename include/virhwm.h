//
// Created by Namir Garib
//

#ifndef VIRHWM_H
#define VIRHWM_H

#include <vector>
#include <algorithm>
#include <cstring>

#define nmax 64        // MAX NUMBER OF MEMORY CELLS
#define p_max 256      // MAX NUMBER OF PROGRAM COUNTER CYCLES
#define l_max 64       // MAX NUMBER OF LOOP REGISTER CYCLES

//GLOBAL VARIABLES
extern short memory[];
extern int gui(int argc, char **argv);
// ======================================

extern short pc;
extern short a_reg;
extern short ma1_reg;
extern short md1_reg;
extern short ma2_reg;
extern short md2_reg;
extern short loop_reg;
extern short A;	    //left side input of ALU
extern short B;	    //right side input of ALU
extern short C; 	//output of ALU
extern short D; 	//output of MPY

extern bool ovf;    // OVERFLOW FLAG
extern bool zf;     // ZERO FLAG
extern bool mf;     // MINUS FLAG
extern bool pre_ovf;
extern bool pre_zf;
extern bool pre_mf;
extern bool jump;

extern short mem[nmax];
extern short a_regi[8];
extern short b_regi[8];

extern std::vector<short> a_vector;
extern std::vector<short> b_vector;


/* =========== INITIALIZATION FUNCTION TO SET UP VARIABLES =========== */
void initializeMemory();
void updateMemory();

struct MIP {
    short Al, Ao, A1o, A0o, MA1i, MA1l, A1I, MA1o;
    short Byo, MD1i, MD1l, BMD1o, MD2i, MD2l, MA2l, A2I, BMPYo, Lpl, Lpc;
    short r_w, memo, MMD1o, MMD2o, alu, Deo, Beo, ovf, zf, mf, ncld, stp, emit;
};

struct Result {
    short C;
    bool ovf;
    bool z;
    bool m;
};

Result falu(short mode, short a, short b);
short mpy(int m1, int m2);
void microprogramLoop();

#endif //VIRHWM_H
