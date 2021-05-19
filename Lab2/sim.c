//Joey Rupert

#include <stdio.h>
#include "shell.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

//Inst.		Opcode		Funct.
//add		0			20hex (#32) DONE
//addu		0			21hex (#33) DONE
//sub		0			22hex (#34) DONE
//subu		0			23hex (#35) DONE
//and		0			24hex (#36) DONE

//addi		8 DONE
//addiu		9 DONE
//lw		23hex (#35) DONE
//sw		2bhex (#43) DONE
//bgtz		7

//j			2

uint8_t op;
uint32_t instruction;
uint32_t addr_rs, addr_rt, addr_rd;
uint32_t rs, rt, rd;      //to save the register value
uint8_t func;
uint16_t itemp;         //for save immidiate number

  //please take a loot at https://inst.eecs.berkeley.edu/~cs61c/resources/MIPS_Green_Sheet.pdf
  //				 or
  //  https://web.cse.ohio-state.edu/~crawfis.3/cse675-02/Slides/MIPS%20Instruction%20Set.pdf

void fetch()
{
	printf("fetch ");
	instruction = mem_read_32(CURRENT_STATE.PC); //the instruction 
	NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void decode() //executing I- and J-type commands in this function.
{
	//printf("decode ");
	op = instruction >> 26; //find the 31-26 bit


  //  printf("the op: %x ",op);
	// if op==0, it is R type
	if (op == 0) {     

		addr_rs = (instruction >> 21) & 0x1f;
		addr_rt = (instruction >> 16) & 0x1f;
		addr_rd = (instruction >> 11) & 0x1f;
		rs = CURRENT_STATE.REGS[addr_rs];   //2^5=32
		rt = CURRENT_STATE.REGS[addr_rt];
		//  rd=CURRENT_STATE.REGS[addr_rd];
		func = instruction & 0x3f;

	}

	if (op == 8) {          //addi:001000       
		printf("addi instruction \n");
		addr_rs = (instruction >> 21) & 0x1f;
		addr_rt = (instruction >> 16) & 0x1f;
		rs = CURRENT_STATE.REGS[addr_rs];   //2^5=32
		itemp = instruction & 0x0000ffff;

		if (itemp > 32767) {    //2^15-1=32767 is the largest number
			itemp = ~itemp + 1;
			rt = rs - itemp;
		}
		else {
			rt = rs + itemp;
		}
		NEXT_STATE.REGS[addr_rt] = rt; // It is fine if you just execute here instead of executing in execute() function. 

	} else if (op == 9) {   //addiu
		printf("addi instruction \n");
		addr_rs = (instruction >> 21) & 0x1f;
		addr_rt = (instruction >> 16) & 0x1f;
		rs = CURRENT_STATE.REGS[addr_rs];   //2^5=32
		itemp = instruction & 0x0000ffff;

		if (itemp > 65535) {
			itemp = ~itemp + 1;
			rt = rs - itemp;
		}
		else {
			rt = rs + itemp;
		}
		NEXT_STATE.REGS[addr_rt] = rt; // It is fine if you just execute here instead of executing in execute() function. 

	} else if (op == 35) {	//lw
		addr_rs = (instruction >> 21) & 0x1f;
		addr_rt = (instruction >> 16) & 0x1f;
		rs = CURRENT_STATE.REGS[addr_rs];
		itemp = instruction & 0x0000ffff;
		rt = mem_read_32(rs + itemp);
		NEXT_STATE.REGS[addr_rt] = rt;
	} else if (op == 43) {
		addr_rs = (instruction >> 21) & 0x1f;
		addr_rt = (instruction >> 16) & 0x1f;
		rs = CURRENT_STATE.REGS[addr_rs];
		rt = CURRENT_STATE.REGS[addr_rt];
		itemp = instruction & 0x0000ffff;
		mem_write_32(rs+itemp, rt);
	} else if (op == 2) {
		addr_rd = (instruction >> 0) & 0x1f;
		rd = CURRENT_STATE.REGS[addr_rd];
		CURRENT_STATE.PC = rd;
	} else if (op == 7) {
		addr_rs = (instruction >> 21) & 0x1f;
		addr_rt = (instruction >> 16) & 0x1f;
		rs = CURRENT_STATE.REGS[addr_rs];
		rt = CURRENT_STATE.REGS[addr_rt];
		itemp = instruction & 0x0000ffff;
		if (itemp > 32767) {
			CURRENT_STATE.PC += 4*(-itemp+1);
		}
		else {
			CURRENT_STATE.PC += 4*itemp;
		}
	}
}

void execute()
{
	printf("execute");
	if (op == 0) {
		switch (func) {
		case 32:      //add:100000
			printf("add instruction \n");
			rd = 0;
			//      if(rt>2147483647)
			rd = rt + rs;
			NEXT_STATE.REGS[addr_rd] = rd;
			break;
		
		case 33:	//addu
			printf("addu instruction \n");
			rd = 0;
			rd = abs(rt) + abs(rs);
			NEXT_STATE.REGS[addr_rd] = rd;
			break;
		
		case 34:	//sub
			printf("sub instruction \n");
			rd = 0;
			rd = rt - rs;
			NEXT_STATE.REGS[addr_rd] = rd;
			break;
		
		case 35:	//subu
			printf("subu instruction \n");
			rd = 0;
			rd = abs(rt) - abs(rs);
			NEXT_STATE.REGS[addr_rd] = rd;
			break;
		
		case 36:	//and
			printf("and instruction \n");
			rd = 0;
			rd = rt & rs;
			NEXT_STATE.REGS[addr_rd] = rd;
			break;
		
		case 12:   //system call:001100
			if (CURRENT_STATE.REGS[2] == 10) {  //v0==10 then exit
				printf("systemcall: exit\n");
				RUN_BIT = FALSE;
			}
			if (CURRENT_STATE.REGS[2] == 1) {   //v0==1: print int
				rt = CURRENT_STATE.REGS[4];   //a0 is 4th register
				printf("\n print the number:   %d \n ", rt);

			}
			break;
		default:
			break;
		}

	


	}
}
void process_instruction()
{
	/* execute one instruction here. You should use CURRENT_STATE and modify
	 * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
	 * access memory. */
	fetch();
	decode();
	execute();

}
