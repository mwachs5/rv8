//
//  riscv-disasm.cc
//

#include <cstdio>
#include <map>
#include <vector>
#include <string>

#include "riscv-types.h"
#include "riscv-format.h"
#include "riscv-opcodes.h"
#include "riscv-imm.h"
#include "riscv-regs.h"
#include "riscv-decode.h"
#include "riscv-csr.h"
#include "riscv-processor.h"
#include "riscv-compression.h"
#include "riscv-format.h"
#include "riscv-disasm.h"

void riscv_disasm_instruction(riscv_decode &dec, riscv_proc_state *proc, riscv_ptr pc, riscv_ptr pc_offset)
{
	const riscv_inst_comp_metadata *comp = riscv_lookup_comp_metadata((riscv_op)dec.op);
	if (comp) {
		dec.op = comp->op;
		dec.type = comp->type;
	}
	const rvf *fmt = riscv_instruction_format[dec.op];

	printf("%016tx: \t", pc - pc_offset);
	switch (dec.sz) {
		case 2: printf("%04x\t\t", *(riscv_hu*)pc); break;
		case 4: printf("%08x\t", *(riscv_wu*)pc); break;
	}
	printf("%s\t", riscv_instruction_name[dec.op]);

	while (*fmt != rvf_z) {
		switch (*fmt) {
			case rvf_b:    printf("("); break;
			case rvf_c:    printf(","); break;
			case rvf_d:    printf(")"); break;
			case rvf_rd:   printf("%s", riscv_i_registers[dec.rd]); break;
			case rvf_rs1:  printf("%s", riscv_i_registers[dec.rs1]); break;
			case rvf_rs2:  printf("%s", riscv_i_registers[dec.rs2]); break;
			case rvf_frd:  printf("%s", riscv_f_registers[dec.rd]); break;
			case rvf_frs1: printf("%s", riscv_f_registers[dec.rs1]); break;
			case rvf_frs2: printf("%s", riscv_f_registers[dec.rs2]); break;
			case rvf_frs3: printf("%s", riscv_f_registers[dec.rs3]); break;
			case rvf_irs1: printf("%d", dec.rs1); break;
			case rvf_imm:  printf("%lld", dec.imm); break;
			case rvf_ipc:  printf("%lld \t# 0x%016tx", dec.imm, pc - pc_offset + dec.imm); break;
			case rvf_csr:
			{
				const riscv_csr_metadata *csr = riscv_lookup_csr_metadata(dec.imm);
				if (csr) printf("%s", csr->csr_name);
				else printf("%llu", dec.imm);
				break;
			}
			case rvf_z: break;
		}
		fmt++;
	}
	printf("\n");
}