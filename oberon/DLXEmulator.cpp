#include <cassert>
#include <cstdio>
#include <vector>
#include "DLXEmulator.h"

using namespace std;

DLXEmulator::DLXEmulator(void)
{
}  

DLXEmulator::~DLXEmulator(void)
{
}

int DLXEmulator::Execute(vector<__int32> processImage, int programStartAddress)
{
	PC = programStartAddress;

	for (;;) {
		int n_pc = PC >> 2;
		if (n_pc > processImage.size()) {
			assert(false);
			return -1;
		}
		IR = processImage[n_pc];

		int op = IR >> 26;
		int a = (IR >> 22) & 0x0F;
		int b = (IR >> 18) & 0x0F;
		int c = IR & 0x0F;

		int im_disp = IR & 0x03FFFF;
		if ((op >> 4) == 0x03) {
			im_disp = IR & 0x03FFFFFF;
		}

		switch ((DLXInstruction::T_DLXInstruction)op) {
			/* F0 Mode */
		case DLXInstruction::MOV: R[a] = R[c] >> b; break;
		case DLXInstruction::MVN: R[a] = -(R[c] >> b); break;
		case DLXInstruction::ADD: R[a] = R[b] + R[c]; break;
		case DLXInstruction::SUB: R[a] = R[b] - R[c]; break;
		case DLXInstruction::MUL: R[a] = R[b] * R[c]; break;
		case DLXInstruction::DIV: R[a] = R[b] / R[c]; break;
		case DLXInstruction::MOD: R[a] = R[b] % R[c]; break;
		case DLXInstruction::CMP: Z = R[b] == R[c]; N = R[b] < R[c]; break;
			/* F1 Mode */
		case DLXInstruction::MOVI: R[a] = im_disp >> b; break;
		case DLXInstruction::MVNI: R[a] = -(im_disp >> b); break;
		case DLXInstruction::ADDI: R[a] = R[b] + im_disp; break;
		case DLXInstruction::SUBI: R[a] = R[b] - im_disp; break;
		case DLXInstruction::MULI: R[a] = R[b] * im_disp; break;
		case DLXInstruction::DIVI: R[a] = R[b] / im_disp; break;
		case DLXInstruction::MODI: R[a] = R[b] % im_disp; break;
		case DLXInstruction::CMPI: Z = R[b] == im_disp; N = R[b] < im_disp; break;
		case DLXInstruction::CHKI: if ((R[a] < 0) || (R[a] >= im_disp)) { R[a] = 0; }; break;
			/* F2 Mode */
		case DLXInstruction::LDW: R[a] = processImage[(R[b] + im_disp) >> 2]; break;
		case DLXInstruction::LDB: R[a] = processImage[(R[b] + im_disp) >> 2] &0xFF; break;
		case DLXInstruction::POP: R[b] -= im_disp; R[a] = processImage[R[b] >> 2]; break;
		case DLXInstruction::STW: processImage[(R[b] + im_disp) >> 2] = R[a]; break;
		case DLXInstruction::STB: processImage[(R[b] + im_disp) >> 2] = R[a] & 0xFF; break;
		case DLXInstruction::PSH: processImage[R[b] >> 2] = R[a]; R[b] += im_disp; break;
			/* Extra R/W Mode */
		case DLXInstruction::RD: break;
		case DLXInstruction::WRD: break;
		case DLXInstruction::WRH: break;
		case DLXInstruction::WRL: break;
			/* F3 Mode */
		case DLXInstruction::BEQ: if (Z) { PC += (im_disp << 2); }; break;
		case DLXInstruction::BLT: if (N) { PC += (im_disp << 2); }; break;
		case DLXInstruction::BLE: if (Z || N) { PC += (im_disp << 2); }; break;
		case DLXInstruction::BR: PC += (im_disp << 2); break;
		case DLXInstruction::BSR: R[14] = PC; PC += (im_disp << 2); break;
		case DLXInstruction::RET: PC = R[im_disp & 0x0F]; if (PC == 0) { /* exit point */ return 0; }; break;
		case DLXInstruction::BNE: if (!Z) { PC += (im_disp << 2); }; break;
		case DLXInstruction::BGE: if (!N) { PC += (im_disp << 2); }; break;
		case DLXInstruction::BGT: if (!(Z || N)) { PC += (im_disp << 2); }; break;
		default:
			return -1;
		}
	}

	return -1;
}