#ifndef DLX_EMULATOR_H
#define DLX_EMULATOR_H    

#include <vector>

class DLXInstruction {  
public:
	enum T_DLXInstruction {
		MOV = 0, MVN = 1, ADD = 2, SUB = 3, MUL = 4, DIV = 5, MOD = 6, CMP = 7,
		MOVI = 16, MVNI = 17, ADDI = 18, SUBI = 19, MULI = 20, DIVI = 21, MODI = 22, CMPI = 23,
		CHKI = 24,
		LDW = 32, LDB = 33, POP = 34, STW = 36, STB = 37, PSH = 38,
		RD = 40, WRD= 41, WRH = 42, WRL = 43,
		BEQ = 48, BNE = 49, BLT = 50, BGE = 51, BLE = 52, BGT = 53, BR = 56, BSR = 57, RET = 58,
	};
};

class DLXEmulator
{
public:
	DLXEmulator(void);
	virtual ~DLXEmulator(void);

public:
	int Execute(std::vector<__int32> processImage, int programStartAddress);

private:
	__int32 R[16];
	__int32 IR;
	bool N;
	bool Z;

#define PC R[15]
};

#endif