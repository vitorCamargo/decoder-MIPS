#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "debug.h"
#define _GNU_SOURCE // necessário porque getline() é extensão GNU

// Campos do formato de instrução.
// opcode  := ir(31 downto 26)
// rs      := ir(25 downto 21);
// rt      := ir(20 downto 16);
// rd      := ir(15 downto 11);
// shamt   := ir(10 downto  6);
// imm     := ir(15 downto  0);
// address := ir(25 downto  0);

/* Definição das Máscaras. */
int mascaraOpCode     = 0xFC000000;
int mascaraRs         = 0x03E00000;
int mascaraRt         = 0x001F0000;
int mascaraRd         = 0x0000F800;
int mascaraShamt      = 0x000007C0;
int mascaraFunct      = 0x0000003F;
int mascaraImmediate  = 0x0000FFFF;
int mascaraAddress    = 0x03FFFFFF;

char* registerName[32] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2",
							"$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5",
                    		"$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4",
							"$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1",
							"$gp", "$sp", "$fp", "$ra"};

/* Função para imprimir um número 32 bits com binário.
   Adaptado de um código encontrado na internet.    */
void bin_prnt_byte(int x) {
	int i, n;
	char buffer [50] = {0};
	
	for (i = 0; i < 32; i++) {
		if ((x & 0x80000000) != 0) {
			// printf("1");
			n = sprintf (buffer, "%s%s", buffer, "1");
		} else {
			// printf("0");
			n = sprintf (buffer, "%s%s", buffer, "0");
		}
		if ((i % 4 == 3) && i < 31) {
			// printf(" "); /* insere um espaço entre nybbles */
			n = sprintf (buffer, "%s%s", buffer, " ");
		}
		x = x << 1;
	}
}

/* Função que recupera o campo OpCode. */
unsigned int getOpCode(unsigned int ir) {
	unsigned int opcode = ((ir & mascaraOpCode) >> 26);
	return opcode;
}

/* Função que recupera o campo registrador Rs. */
unsigned int getRs(unsigned int ir) {
	unsigned int rs = (ir & mascaraRs) >> 21;
	return rs;
}

/* Função que recupera o campo registrador Rt. */
unsigned int getRt(unsigned int ir) {
	unsigned int rt = (ir & mascaraRt) >> 16;
	return rt;
}

/* Função que recupera o campo registrador Rd. */
unsigned int getRd(unsigned int ir) {
	unsigned int rd = (ir & mascaraRd) >> 11;
	return rd;
}

/* Função que recupera o campo Shamt (deslocamento). */
int getShamt(unsigned int ir) {
	unsigned int shamt = (ir & mascaraShamt) >> 6;
	return shamt;
}

/* Função que recupera o campo Funct. */
unsigned int getFunct(unsigned int ir) {
	unsigned int funct = (ir & mascaraFunct);
	return funct;
}

/* Função que recupera o campo imm. */
int getImmediate(unsigned int ir) {
	unsigned int imm = (ir & mascaraImmediate);
	return imm;
}

/* Função que recupera o campo Address. */
unsigned int getAddress(unsigned int ir) {
	unsigned int address = (ir & mascaraAddress);
	return address; 
}

/* Converte um char * representando um binário, para inteiro. */
int intFromBinary(char *s) {
	int inteiro = (int) strtol(s, 0, 2);
	bin_prnt_byte(inteiro);
	
	return inteiro;
}

/* Decodificação das instruções. */
void decodificar(unsigned int ir) {
	switch (getOpCode(ir)) {
		case 0: { // 000000, Aritmética.
			switch (getFunct(ir)) {
				case 0: { // nop
					if(getRd(ir) == 0 && getRs(ir) == 0 && getRt(ir) == 0) {
						printf("nop\n");
					} else { // 000000 -> sll, R-Type. sll rd, rt, shamt
						printf("sll ");
						printf("%s, ", registerName[getRd(ir)]);
						printf("%s, ", registerName[getRt(ir)]);
						printf("%d\n", getShamt(ir));
					}
					break;
				}
				case 2: { // 000010 -> srl, R-Type. srl rd, rt, shamt
					printf("srl ");
                    printf("%s, ", registerName[getRd(ir)]);
                    printf("%s, ", registerName[getRt(ir)]);
                    printf("%d\n", getShamt(ir));
                    break;
				}
				case 3: { // 000011 -> sra, R-Type. sra rd, rt, shamt
					printf("sra ");
                    printf("%s, ", registerName[getRd(ir)]);
                    printf("%s, ", registerName[getRt(ir)]);
                    printf("%d\n", getShamt(ir));
                    break;
				}
				case 4: { // 000100 -> sllv, R-Type. sllv rd, rt, rs
					printf("sllv ");
                    printf("%s, ", registerName[getRd(ir)]);
                    printf("%s, ", registerName[getRt(ir)]);
                    printf("%d\n", registerName[getRs(ir)]);
                    break;
				}
				case 6: { // 000110 -> srlv, R-Type. srlv rd, rt, rs
					printf("srlv ");
                    printf("%s, ", registerName[getRd(ir)]);
                    printf("%s, ", registerName[getRt(ir)]);
                    printf("%d\n", registerName[getRs(ir)]);
                    break;
				}
				case 7: { // 000110 -> srav, R-Type. srav rd, rt, rs
					printf("srav ");
                    printf("%s, ", registerName[getRd(ir)]);
                    printf("%s, ", registerName[getRt(ir)]);
                    printf("%d\n", registerName[getRs(ir)]);
                    break;
				}
				case 8: { // 001000 -> jr, R-Type. jr rs
					printf("jr ");
					printf("%s\n", registerName[getRs(ir)]);
					break;
				}
				case 9: { // 001001 -> jalr, R-Type. jalr rd, rs
					printf("jalr ");
					printf("%s", registerName[getRd(ir)]);
					printf("%s\n", registerName[getRs(ir)]);
					break;
				}
				case 10: { // 001010 -> movz, R-Type. movz rd, rs, rt
					printf("movz ");
					printf("%s", registerName[getRd(ir)]);
					printf("%s", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 11: { // 001011 -> movn, R-Type. movn rd, rs, rt
					printf("movn ");
					printf("%s", registerName[getRd(ir)]);
					printf("%s", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 12: { // 001100 -> syscall, chamada do sistema.
                    printf("syscall \n");
					break;
				}
				case 13: { // 001101 -> break, chamada do sistema.
                    printf("break \n");
					break;
				}
				case 15: { // 001111 -> sync, chamada do sistema.
					printf("sync \n");
					break;
				}
				case 16: { // 010000 -> mfhi, R-Type. mfhi rd
					printf("mfhi ");
					printf("%s\n", registerName[getRd(ir)]);
					break;
				}
				case 17: { // 010001 -> mthi, R-Type. mthi rs
					printf("mthi ");
					printf("%s\n", registerName[getRs(ir)]);
					break;
				}
				case 18: { // 010010 -> mflo, R-Type. mflo rd
					printf("mflo ");
					printf("%s\n", registerName[getRd(ir)]);
					break;
				}
				case 19: { // 010011 -> mtlo, R-Type. mflo rs
					printf("mtlo ");
					printf("%s\n", registerName[getRs(ir)]);
					break;
				}
				case 20: { // 010100 -> dsllv, R-Type. dsllv rd, rt, rs
					printf("dsllv ");
                    printf("%s, ", registerName[getRd(ir)]);
                    printf("%s, ", registerName[getRt(ir)]);
                    printf("%d\n", registerName[getRs(ir)]);
                    break;
				}
				case 22: { // 010110 -> dsrlv, R-Type. dsrlv rd, rt, rs
					printf("dsrlv ");
                    printf("%s, ", registerName[getRd(ir)]);
                    printf("%s, ", registerName[getRt(ir)]);
                    printf("%d\n", registerName[getRs(ir)]);
                    break;
				}
				case 23: { // 010111 -> dsrav, R-Type. dsrav rd, rt, rs
					printf("dsrav ");
                    printf("%s, ", registerName[getRd(ir)]);
                    printf("%s, ", registerName[getRt(ir)]);
                    printf("%d\n", registerName[getRs(ir)]);
                    break;
				}
				case 24: { // 011000 -> mult, R-Type. mult rs, rt
					printf("mult ");
					printf("%s ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 25: { // 011001 -> multu, R-Type. multu rs, rt
					printf("multu ");
					printf("%s ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 26: { // 011010 -> div, R-Type. div rs, rt
					printf("div ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 27: { // 011011 -> divu, R-Type. divu rs, rt
					printf("divu ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 28: { // 011100 -> dmult, R-Type. dmult rs, rt
					printf("dmult ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 29: { // 011101 -> dmultu, R-Type. dmultu rs, rt
					printf("dmultu ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 30: { // 011110 -> ddiv, R-Type. ddiv rs, rt
					printf("ddiv ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 31: { // 011111 -> ddivu, R-Type. ddivu rs, rt
					printf("ddivu ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 32: { // 100000 -> add, R-Type. add rd, rs, rt
					printf("add ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 33: { // 100001 -> addu, R-Type. addu rd, rs, rt
					printf("addu ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 34: { // 100010 -> sub, R-Type. sub rd, rs, rt
					printf("sub ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 35: { // 100011 -> subu, R-Type. subu rd, rs, rt
					printf("subu ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 36: { // 100100 -> and, R-Type. and rd, rs, rt
					printf("and ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 37: { // 100101 -> or, R-Type. or rd, rs, rt
					printf("or ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 38: { // 100110 -> xor, R-Type. xor rd, rs, rt
					printf("xor ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 39: { // 100111 -> nor, R-Type. nor rd, rs, rt
					printf("nor ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 42: { // 101010 -> slt, R-Type. slt rd, rs, rt
					printf("slt ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 43: { // 101011 -> sltu, R-Type. sltu rd, rs, rt
					printf("sltu ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 44: { // 101100 -> dadd, R-Type. dadd rd, rs, rt
					printf("dadd ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 45: { // 101101 -> daddu, R-Type. daddu rd, rs, rt
					printf("daddu ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 46: { // 101110 -> dsub, R-Type. dsub rd, rs, rt
					printf("dsub ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 47: { // 101111 -> dsubu, R-Type. dsubu rd, rs, rt
					printf("dsubu ");
					printf("%s, ", registerName[getRd(ir)]);
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 48: { // 110000 -> tge, R-Type. tge rs, rt
					printf("tge ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 49: { // 110001 -> tgeu, R-Type. tgeu rs, rt
					printf("tgeu ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 50: { // 110010 -> tlt, R-Type. tlt rs, rt
					printf("tlt ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 51: { // 110011 -> tltu, R-Type. tltu rs, rt
					printf("tltu ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
				case 52: { // 110100 -> teq, R-Type. teq rs, rt
					printf("teq ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%s\n", registerName[getRt(ir)]);
					break;
				}
			}
			break;
		}
		case 1: { // 000001
			switch (getRt(ir)) {
				case 0: { // 00000 -> bltz, I-Type. bltz rs, imm
					printf("bltz ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("0x%0.4X\n", getImmediate(ir));
					break;
				}
				case 1: { // 00001 -> bgez, I-Type. bgez rs, imm
					printf("bgez ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("0x%0.4X\n", getImmediate(ir));
					break;
				}
				case 2: { // 00010 -> bgezl, I-Type. bgezl rs, imm
					printf("bgezl ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("0x%0.4X\n", getImmediate(ir));
					break;
				}
				case 8: { // 01000 -> tgei, I-Type. tgei rs, imm
					printf("tgei ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%d\n", getImmediate(ir));
					break;
				}
				case 9: { // 01001 -> tgeiu, I-Type. tgeiu rs, imm
					printf("tgeiu ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%d\n", getImmediate(ir));
					break;
				}
				case 10: { // 01010 -> tlti, I-Type. tlti rs, imm
					printf("tlti ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%d\n", getImmediate(ir));
					break;
				}
				case 11: { // 01011 -> tltiu, I-Type. tltiu rs, imm
					printf("tltiu ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%d\n", getImmediate(ir));
					break;
				}
				case 12: { // 01100 -> teqi, I-Type. teqi rs, imm
					printf("teqi ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%d\n", getImmediate(ir));
					break;
				}
				case 14: { // 01110 -> tnei, I-Type. tnei rs, imm
					printf("tnei ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("%d\n", getImmediate(ir));
					break;
				}
				case 16: { // 10000 -> bltzal, I-Type. bltzal rs, imm
					printf("bltzal ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("0x%0.4X\n", getImmediate(ir));
					break;
				}
				case 17: { // 10001 -> bgezal, I-Type. bgezal rs, imm
					printf("bgezal ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("0x%0.4X\n", getImmediate(ir));
					break;
				}
				case 18: { // 10010 -> bltzall, I-Type. bltzall rs, imm
					printf("bltzall ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("0x%0.4X\n", getImmediate(ir));
					break;
				}
				case 19: { // 10011 -> bgezall, I-Type. bgezall rs, imm
					printf("bgezall ");
					printf("%s, ", registerName[getRs(ir)]);
					printf("0x%0.4X\n", getImmediate(ir));
					break;
				}
			}
			break;
		}
		case 2: { // 000010 -> j, J-Type. j address
			printf("j ");
			printf("0x%0.8X\n", getAddress(ir));
			break;
		}
		case 3: { // 000011 -> jal, J-Type. jal address
			printf("jal ");
			printf("0x%0.8X\n", getAddress(ir));
			break;
		}
		case 4: { // 000100 -> beq, I-Type. beq rs, rt, imm
			printf("beq ");
			printf("%s, ", registerName[getRs(ir)]);
			printf("%s, ", registerName[getRt(ir)]);
			printf("0x%0.4X\n", getImmediate(ir));
			break;
		}
		case 5: { // 000101 -> bne, I-Type. bne rs, rt, imm
			printf("bne ");
			printf("%s, ", registerName[getRs(ir)]);
			printf("%s, ", registerName[getRt(ir)]);
			printf("0x%0.4X\n", getImmediate(ir));
			break;
		}
		case 6: { // 000110 -> blez, I-Type. blez rs, imm
			printf("blez ");
			printf("%s, ", registerName[getRs(ir)]);
			printf("0x%0.4X\n", getImmediate(ir));
			break;
		}
		case 7: { // 000111 -> bgtz, I-Type. bgtz rs, imm
			printf("bgtz ");
			printf("%s, ", registerName[getRs(ir)]);
			printf("0x%0.4X\n", getImmediate(ir));
			break;
		}
		case 8: { // 001000 -> addi, I-Type. addi rt, rs, imm
			printf("addi ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%s, ", registerName[getRs(ir)]);
			printf("%d\n", getImmediate(ir));
			break;
		}
		case 9: { // 001001 -> addiu, I-Type. addiu rt, rs, imm
			printf("addiu ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%s, ", registerName[getRs(ir)]);
			printf("%d\n", getImmediate(ir));
			break;
		}
		case 10: { // 001010 -> slti, I-Type. slti rt, rs, imm
			printf("slti ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%s, ", registerName[getRs(ir)]);
			printf("%d\n", getImmediate(ir));
			break;
		}
		case 11: { // 001011 -> sltiu, I-Type. sltiu rt, rs, imm
			printf("sltiu ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%s, ", registerName[getRs(ir)]);
			printf("%d\n", getImmediate(ir));
			break;
		}
		case 12: { // 001100 -> andi, I-Type. andi rt, rs, imm
			printf("andi ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%s, ", registerName[getRs(ir)]);
			printf("%d\n", getImmediate(ir));
			break;
		}
		case 13: { // 001101 -> ori, I-Type. ori rt, rs, imm
			printf("ori ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%s, ", registerName[getRs(ir)]);
			printf("%d\n", getImmediate(ir));
			break;
		}
		case 14: { // 001110 -> xori, I-Type. xori rt, rs, imm
			printf("xori ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%s, ", registerName[getRs(ir)]);
			printf("%d\n", getImmediate(ir));
			break;
		}
		case 15: { // 001111 -> lui, I-Type. lui rt, imm
			printf("lui ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d\n", getImmediate(ir));
			break;
		}
		case 20: { // 010100 -> beql, I-Type. beql rs, rt, imm
			printf("beql ");
			printf("%s, ", registerName[getRs(ir)]);
			printf("%s, ", registerName[getRt(ir)]);
			printf("0x%0.4X\n", getImmediate(ir));
			break;
		}
		case 21: { // 010101 -> bnel, I-Type. bnel rs, rt, imm
			printf("bnel ");
			printf("%s, ", registerName[getRs(ir)]);
			printf("%s, ", registerName[getRt(ir)]);
			printf("0x%0.4X\n", getImmediate(ir));
			break;
		}
		case 22: { // 010110 -> blezl, I-Type. blezl rs, imm
			printf("blezl ");
			printf("%s, ", registerName[getRs(ir)]);
			printf("0x%0.4X\n", getImmediate(ir));
			break;
		}
		case 23: { // 010111 -> bgtzl, I-Type. bgtzl rs, imm
			printf("bgtzl ");
			printf("%s, ", registerName[getRs(ir)]);
			printf("0x%0.4X\n", getImmediate(ir));
			break;
		}
		case 24: { // 011000 -> daddi, I-Type. daddi rt, rs, imm
			printf("daddi ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%s, ", registerName[getRs(ir)]);
			printf("%d\n", getImmediate(ir));
			break;
		}
		case 25: { // 011001 -> daddiu, I-Type. daddiu rt, rs, imm
			printf("daddiu ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%s, ", registerName[getRs(ir)]);
			printf("%d\n", getImmediate(ir));
			break;
		}
		case 26: { // 011010 -> ldl, I-Type. ldl rt, imm(rs)
			printf("ldl ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 27: { // 011011 -> ldr, I-Type. ldr rt, imm(rs)
			printf("ldr ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 31: { // 011111 -> lui, I-Type. lui rt, imm(rs)
			printf("ui ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 32: { // 100000 -> lb, I-Type. lb rt, imm(rs)
			printf("lb ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 33: { // 100001 -> lh, I-Type. lh rt, imm(rs)
			printf("lh ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 34: { // 100010 -> lwl, I-Type. lwl rt, imm(rs)
			printf("lwl ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 35: { // 100011 -> lw, I-Type. lw rt, imm(rs)
			printf("lw ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 36: { // 100100 -> lbu, I-Type. lbu rt, imm(rs)
			printf("lbu ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 37: { // 100101 -> lhu, I-Type. lhu rt, imm(rs)
			printf("lhu ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 38: { // 100110 -> lwr, I-Type. lwr rt, imm(rs)
			printf("lwr ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 39: { // 100111 -> lwu, I-Type. lwu rt, imm(rs)
			printf("lwu ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 40: { // 101000 -> sb, I-Type. sb rt, imm(rs)
			printf("sb ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 41: { // 101001 -> sh, I-Type. sh rt, imm(rs)
			printf("sh ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 42: { // 101010 -> swl, I-Type. swl rt, imm(rs)
			printf("swl ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 43: { // 101011 -> sw, I-Type. sw rt, imm(rs)
			printf("sw ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 44: { // 101100 -> sdl, I-Type. sdl rt, imm(rs)
			printf("sdl ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 45: { // 101101 -> sdr, I-Type. sdr rt, imm(rs)
			printf("sdr ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 46: { // 101110 -> swr, I-Type. swr rt, imm(rs)
			printf("swr ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 48: { // 110000 -> ll, I-Type. ll rt, imm(rs)
			printf("ll ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 52: { // 110100 -> lld, I-Type. lld rt, imm(rs)
			printf("lld ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 55: { // 110111 -> ld, I-Type. ld rt, imm(rs)
			printf("ld ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 56: { // 111000 -> sc, I-Type. sc rt, imm(rs)
			printf("sc ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 60: { // 111100 -> scd, I-Type. scd rt, imm(rs)
			printf("scd ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		case 63: { // 111111 -> sd, I-Type. sd rt, imm(rs)
			printf("sd ");
			printf("%s, ", registerName[getRt(ir)]);
			printf("%d", getImmediate(ir));
			printf("(%s)\n", registerName[getRs(ir)]);
			break;
		}
		default: // outros casos.
			fprintf(stdout, "Instrução não implementada. OpCode: %d.\n", getOpCode(ir));
	}
}

int main(int argc, char *argv[]) {
	unsigned int ir;
	
	if (argc < 2) {
		fprintf(stderr, "Uso:\n ./decodificador arquivo.b\n");
		return (0);
	}
	
	FILE *arquivo = fopen(argv[1], "r");
	size_t len = 32; // tamanho da linha.
	char *linha = malloc(len);
	
	if (!arquivo) {
		perror(argv[1]);
		exit(1);
	}
	
	while (getline(&linha, &len, arquivo) > 0) {
		// teste: bin_prnt_byte(0x8C130004);
		ir = intFromBinary(linha);
		decodificar(ir);
	}
	
	if (linha)
		free(linha);
	
	fclose(arquivo);
	return 0;
}
