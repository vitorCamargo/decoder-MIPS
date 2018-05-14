#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "debug.h"
#define _GNU_SOURCE // necessário porque getline() é extensão GNU

// Campos do formato de instrução.
// R-Type := 0000 00ss ssst tttt dddd dhhh hhff ffff
// J-Type := oooo ooii iiii iiii iiii iiii iiii iiii
// I-Type := oooo ooss ssst tttt iiii iiii iiii iiii
// o (OpCode); s ($rs); t ($rt); d ($rd); 
// h (shamt); f (function); i (immediate);

char* registerAddress[32] = {"00000", "00001", "00010", "00011", "00100", "00101", "00110",
							"00111", "01000", "01001", "01010", "01011", "01100", "01101",
                    		"01110", "01111", "10000", "10001", "10010", "10011", "10100",
							"10101", "10110", "10111", "11000", "11001", "11010", "11011",
							"11100", "11101", "11110", "11111"};
char* registerName[32] = {"zero", "at", "v0", "v1", "a0", "a1", "a2",
							"a3", "t0", "t1", "t2", "t3", "t4", "t5",
                    		"t6", "t7", "s0", "s1", "s2", "s3", "s4",
							"s5", "s6", "s7", "t8", "t9", "k0", "k1",
							"gp", "sp", "fp", "ra"};

/* Função para imprimir um número 32 bits com binário.
   Adaptado de um código encontrado na internet.    */
void bin_prnt_byte(int x) {
	int i;
	char buffer [50] = {0};
	
	for (i = 0; i < 16; i++) {
		if ((x & 0x8000) != 0) {
			printf("1");
		} else {
			printf("0");
		}
		x = x << 1;
	}
}

void bin_prnt_byte5(int x) {
	int i;
	char buffer [50] = {0};
	
	for (i = 0; i < 5; i++) {
		if ((x & 0x10) != 0) {
			printf("1");
		} else {
			printf("0");
		}
		x = x << 1;
	}
}

/* Função que devolve o nome da função. */
unsigned char* getFunction(char* linha) {
	char *linhaCpy = malloc(strlen(linha));
	strcpy(linhaCpy, linha);

	strtok(linhaCpy, " ");

	return linhaCpy;
}

/* Função que devolve o registrador em uma posição da linha. */
unsigned int getReg(char* linha, int pos) {
	char *pc, *linhaCpy = malloc(strlen(linha));
	strcpy(linhaCpy, linha);
	int i;
	
	pc = strtok(linhaCpy, ", $()");
	for(i = 0; i < pos; i++)
		pc = strtok(NULL, ", $()");

	for(i = 0; i < 32; i++)
		if(strcmp(registerName[i], pc) == 0)
			return i;
}

unsigned char* hex_to_bin(char c) {
	if(c == '0') return "0000";
	else if(c == '1') return "0001";
	else if(c == '2') return "0010";
	else if(c == '3') return "0011";
	else if(c == '4') return "0100";
	else if(c == '5') return "0101";
	else if(c == '6') return "0110";
	else if(c == '7') return "0111";
	else if(c == '8') return "1000";
	else if(c == '9') return "1001";
	else if(c == 'a') return "1010";
	else if(c == 'b') return "1011";
	else if(c == 'c') return "1100";
	else if(c == 'd') return "1101";
	else if(c == 'e') return "1110";
	else return "1111";
}

/* Função que devolve o endereço. */
unsigned char* getAddress(char* linha) {
	char *pc, *linhaCpy = malloc(strlen(linha)), *add = malloc(28);
	int i;
	strcpy(linhaCpy, linha);
	
	pc = strtok(linhaCpy, " ");
	pc = strtok(NULL, " ");

	for(i = 2; i < strlen(pc); i++)
		strcat(add, hex_to_bin(pc[i]));

	for(i = 0; i < strlen(add) - 2; i++)
		add[i] = add[i+2];
	add[i] = '\0';

	return add;
}

/* Função que devolve o imediato. */
unsigned char* getImmAddress(char* linha, int pos) {
	char *pc, *linhaCpy = malloc(strlen(linha)), *add = malloc(16);
	strcpy(linhaCpy, linha);
	int i;
	
	pc = strtok(linhaCpy, ", $");
	for(i = 0; i < pos; i++)
		pc = strtok(NULL, ", $");

	for(i = 2; i < strlen(pc); i++)
		strcat(add, hex_to_bin(pc[i]));

	return add;
}

unsigned int getShamt(char* linha, int pos) {
	char *pc, *linhaCpy = malloc(strlen(linha));
	strcpy(linhaCpy, linha);
	int i;
	
	pc = strtok(linhaCpy, ", $()");
	for(i = 0; i < pos; i++)
		pc = strtok(NULL, ", $()");

	unsigned int valor = strtol(pc, 0, 10);
	return valor;
}

unsigned int getImm(char* linha, int pos) {
	char *pc, *linhaCpy = malloc(strlen(linha));
	strcpy(linhaCpy, linha);
	int i;
	
	pc = strtok(linhaCpy, ", $()");
	for(i = 0; i < pos; i++)
		pc = strtok(NULL, ", $()");

	unsigned int valor = strtol(pc, 0, 10);
	return valor;
}

/* Decodificação das instruções. */
void codificar(unsigned char* linha) {
	char* funct = getFunction(linha);
	if(strcmp(funct, "nop") == 0) { // nop
		printf("00000000000000000000000000000000\n");
	} else if(strcmp(funct, "sll") == 0) { // 000000 -> sll, R-Type. sll rd, rt, shamt
		printf("000000"); // opcode
		printf("00000"); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		bin_prnt_byte5(getShamt(linha, 3)); // shamt
		printf("000000\n"); // funct
	} else if(strcmp(funct, "srl") == 0) { // 000010 -> srl, R-Type. srl rd, rt, shamt
		printf("000000"); // opcode
		printf("00000"); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		bin_prnt_byte5(getShamt(linha, 3)); // shamt
		printf("000010\n"); // funct
	} else if(strcmp(funct, "sra") == 0) { // 000011 -> sra, R-Type. sra rd, rt, shamt
		printf("000000"); // opcode
		printf("00000"); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		bin_prnt_byte5(getShamt(linha, 3)); // shamt
		printf("000011\n"); // funct
	} else if(strcmp(funct, "sllv") == 0) { // 000100 -> sllv, R-Type. sllv rd, rt, rs
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("000100\n"); // funct
	} else if(strcmp(funct, "srlv") == 0) { // 000110 -> srlv, R-Type. srlv rd, rt, rs
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("000110\n"); // funct
	} else if(strcmp(funct, "srav") == 0) { // 000111 -> srav, R-Type. srav rd, rt, rs
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("000111\n"); // funct
	} else if(strcmp(funct, "jr") == 0) { // 001000 -> jr, R-Type. jr rs
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("00000"); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("001000\n"); // funct
	} else if(strcmp(funct, "jalr") == 0) { // 001001 -> jalr, R-Type. jalr rd, rs
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("00000"); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("001001\n"); // funct
	} else if(strcmp(funct, "movz") == 0) { // 001010 -> movz, R-Type. movz rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("001010\n"); // funct
	} else if(strcmp(funct, "movn") == 0) { // 001011 -> movn, R-Type. movn rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("001011\n"); // funct
	} else if(strcmp(funct, "syscall") == 0) { // 001100 -> syscall, chamada do sistema.
		printf("000000"); // opcode
		printf("00000"); // $rs
		printf("00000"); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("001100\n"); // funct
	} else if(strcmp(funct, "break") == 0) { // 001101 -> break, chamada do sistema.
		printf("000000"); // opcode
		printf("00000"); // $rs
		printf("00000"); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("001101\n"); // funct
	} else if(strcmp(funct, "sync") == 0) { // 001111 -> sync, chamada do sistema.
		printf("000000"); // opcode
		printf("00000"); // $rs
		printf("00000"); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("001111\n"); // funct
	} else if(strcmp(funct, "mfhi") == 0) { // 010000 -> mfhi, R-Type. mfhi rd
		printf("000000"); // opcode
		printf("00000"); // $rs
		printf("00000"); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("010000\n"); // funct
	} else if(strcmp(funct, "mthi") == 0) { // 010001 -> mthi, R-Type. mthi rs
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("00000"); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("010000\n"); // funct
	} else if(strcmp(funct, "mflo") == 0) { // 010010 -> mflo, R-Type. mflo rd
		printf("000000"); // opcode
		printf("00000"); // $rs
		printf("00000"); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("010010\n"); // funct
	} else if(strcmp(funct, "mtlo") == 0) { // 010011 -> mtlo, R-Type. mtlo rs
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("00000"); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("010011\n"); // funct
	} else if(strcmp(funct, "dsllv") == 0) { // 010100 -> dsllv, R-Type. dsllv rd, rt, rs
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("010100\n"); // funct
	} else if(strcmp(funct, "dsrlv") == 0) { // 010110 -> dsrlv, R-Type. dsrlv rd, rt, rs
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("010110\n"); // funct
	} else if(strcmp(funct, "dsrav") == 0) { // 010111 -> dsrav, R-Type. dsrav rd, rt, rs
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("010111\n"); // funct
	} else if(strcmp(funct, "mult") == 0) { // 011000 -> mult, R-Type. mult rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("011000\n"); // funct
	} else if(strcmp(funct, "multu") == 0) { // 011001 -> multu, R-Type. multu rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("011001\n"); // funct
	} else if(strcmp(funct, "div") == 0) {  // 011010 -> div, R-Type. div rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("011010\n"); // funct
	} else if(strcmp(funct, "divu") == 0) { // 011011 -> divu, R-Type. divu rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("011011\n"); // funct
	} else if(strcmp(funct, "dmult") == 0) { // 011100 -> dmult, R-Type. dmult rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("011100\n"); // funct
	} else if(strcmp(funct, "dmultu") == 0) { // 011101 -> dmultu, R-Type. dmultu rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("011101\n"); // funct
	} else if(strcmp(funct, "ddiv") == 0) { // 011110 -> ddiv, R-Type. ddiv rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("011110\n"); // funct
	} else if(strcmp(funct, "ddivu") == 0) { // 011111 -> ddivu, R-Type. ddivu rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("011111\n"); // funct
	} else if(strcmp(funct, "add") == 0) { // 100000 -> add, R-Type. add rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("100000\n"); // funct
	} else if(strcmp(funct, "addu") == 0) { // 100001 -> addu, R-Type. addu rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("100001\n"); // funct
	} else if(strcmp(funct, "sub") == 0) { // 100010 -> sub, R-Type. sub rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("100010\n"); // funct
	} else if(strcmp(funct, "subu") == 0) { // 100011 -> subu, R-Type. subu rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("100011\n"); // funct
	} else if(strcmp(funct, "and") == 0) { // 100100 -> and, R-Type. and rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("100100\n"); // funct
	} else if(strcmp(funct, "or") == 0) { // 100101 -> or, R-Type. or rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("100101\n"); // funct
	} else if(strcmp(funct, "xor") == 0) { // 100110 -> xor, R-Type. xor rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("100110\n"); // funct
	} else if(strcmp(funct, "nor") == 0) { // 100111 -> nor, R-Type. nor rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("100111\n"); // funct
	} else if(strcmp(funct, "slt") == 0) { // 101010 -> slt, R-Type. slt rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("101010\n"); // funct
	} else if(strcmp(funct, "sltu") == 0) { // 101011 -> sltu, R-Type. sltu rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("101011\n"); // funct
	} else if(strcmp(funct, "dadd") == 0) { // 101100 -> dadd, R-Type. dadd rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("101100\n"); // funct
	} else if(strcmp(funct, "daddu") == 0) { // 101101 -> daddu, R-Type. dadd rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("101101\n"); // funct
	} else if(strcmp(funct, "dsub") == 0) { // 101110 -> dsub, R-Type. dadd rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("101110\n"); // funct
	} else if(strcmp(funct, "dsubu") == 0) { // 101111 -> dsubu, R-Type. dadd rd, rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 3)]); // $rt
		printf("%s", registerAddress[getReg(linha, 1)]); // $rd
		printf("00000"); // shamt
		printf("101111\n"); // funct
	} else if(strcmp(funct, "tge") == 0) { // 110000 -> tge, R-Type. tge rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("110000\n"); // funct
	} else if(strcmp(funct, "tgeu") == 0) { /// 110001 -> tgeu, R-Type. tgeu rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("110001\n"); // funct
	} else if(strcmp(funct, "tlt") == 0) { // 110010 -> tlt, R-Type. tlt rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("110010\n"); // funct
	} else if(strcmp(funct, "tltu") == 0) { // 110011 -> tltu, R-Type. tltu rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("110011\n"); // funct
	} else if(strcmp(funct, "teq") == 0) { // 110100 -> teq, R-Type. teq rs, rt
		printf("000000"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("00000"); // $rd
		printf("00000"); // shamt
		printf("110100\n"); // funct
	} else if(strcmp(funct, "bltz") == 0) { // 000001 00000 -> bltz, I-Type. bltz rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("00000"); // $rt
		printf("%s\n", getImmAddress(linha, 2)); // imm
	} else if(strcmp(funct, "bgez") == 0) { // 000001 00001 -> bgez, I-Type. bgez rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("00001"); // $rt
		printf("%s\n", getImmAddress(linha, 2)); // imm
	} else if(strcmp(funct, "bgezl") == 0) { // 000001 00010 -> bgezl, I-Type. bgezl rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("00010"); // $rt
		printf("%s\n", getImmAddress(linha, 2)); // imm
	} else if(strcmp(funct, "tgei") == 0) { // 000001 01000 -> tgei, I-Type. tgei rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("01000"); // $rt
		printf("%d\n", getImm(linha, 2)); // imm
	} else if(strcmp(funct, "tgeiu") == 0) { // 000001 01001 -> tgeiu, I-Type. tgeiu rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("01001"); // $rt
		printf("%d\n", getImm(linha, 2)); // imm
	} else if(strcmp(funct, "tlti") == 0) { // 000001 01010 -> tlti, I-Type. tlti rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("01010"); // $rt
		printf("%d\n", getImm(linha, 2)); // imm
	} else if(strcmp(funct, "tltiu") == 0) { // 000001 01011 -> tltiu, I-Type. tltiu rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("01011"); // $rt
		printf("%d\n", getImm(linha, 2)); // imm
	} else if(strcmp(funct, "teqi") == 0) { // 000001 01100 -> teqi, I-Type. teqi rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("01100"); // $rt
		printf("%d\n", getImm(linha, 2)); // imm
	} else if(strcmp(funct, "tnei") == 0) { // 000001 01110 -> tnei, I-Type. tnei rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("01110"); // $rt
		printf("%d\n", getImm(linha, 2)); // imm
	} else if(strcmp(funct, "bltzal") == 0) { // 000001 10000 -> bltzal, I-Type. bltzal rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("10000"); // $rt
		printf("%s\n", getImmAddress(linha, 2)); // imm
	} else if(strcmp(funct, "bgezal") == 0) { // 000001 10001 -> bgezal, I-Type. bgezal rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("10001"); // $rt
		printf("%s\n", getImmAddress(linha, 2)); // imm
	} else if(strcmp(funct, "bltzall") == 0) { // 000001 10010 -> bltzall, I-Type. bltzall rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("10010"); // $rt
		printf("%s\n", getImmAddress(linha, 2)); // imm
	} else if(strcmp(funct, "bgezall") == 0) { // 000001 10011 -> bgezall, I-Type. bgezall rs, imm
		printf("000001"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("10011"); // $rt
		printf("%s\n", getImmAddress(linha, 2)); // imm
	} else if(strcmp(funct, "j") == 0) { // 000010 -> j, J-Type. j address
		printf("000010"); // opcode
		printf("%s\n", getAddress(linha)); // address
	} else if(strcmp(funct, "jal") == 0) { // 000011 -> jal, J-Type. jal address
		printf("000011"); // opcode
		printf("%s\n", getAddress(linha)); // address
	} else if(strcmp(funct, "beq") == 0) { // 000100 -> beq, I-Type. beq rs, rt, imm
		printf("000100"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s\n", getImmAddress(linha, 3)); // imm
	} else if(strcmp(funct, "bne") == 0) { // 000101 -> bne, I-Type. bne rs, rt, imm
		printf("000101"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s\n", getImmAddress(linha, 3)); // imm
	} else if(strcmp(funct, "blez") == 0) { // 000110 -> blez, I-Type. blez rs, imm
		printf("000110"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("00000"); // $rt
		printf("%s\n", getImmAddress(linha, 2)); // imm
	} else if(strcmp(funct, "bgtz") == 0) { // 000111 -> bgtz, I-Type. bgtz rs, imm
		printf("000111"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("00000"); // $rt
		printf("%s\n", getImmAddress(linha, 2)); // imm
	} else if(strcmp(funct, "addi") == 0) { // 001000 -> addi, I-Type. addi rt, rs, imm
		printf("001000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 3)); // imm
		printf("\n");
	} else if(strcmp(funct, "addiu") == 0) { // 001001 -> addiu, I-Type. addiu rt, rs, imm
		printf("001001"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 3)); // imm
		printf("\n");
	} else if(strcmp(funct, "slti") == 0) { // 001010 -> slti, I-Type. slti rt, rs, imm
		printf("001010"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 3)); // imm
		printf("\n");
	} else if(strcmp(funct, "sltiu") == 0) { // 001011 -> sltiu, I-Type. sltiu rt, rs, imm
		printf("001011"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 3)); // imm
		printf("\n");
	} else if(strcmp(funct, "andi") == 0) { // 001100 -> andi, I-Type. andi rt, rs, imm
		printf("001100"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 3)); // imm
		printf("\n");
	} else if(strcmp(funct, "ori") == 0) { // 001101 -> ori, I-Type. ori rt, rs, imm
		printf("001101"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 3)); // imm
		printf("\n");
	} else if(strcmp(funct, "xori") == 0) { // 001110 -> xori, I-Type. xori rt, rs, imm
		printf("001110"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 3)); // imm
		printf("\n");
	} else if(strcmp(funct, "lui") == 0) { // 001111 -> lui, I-Type. lui rt, imm
		printf("001111"); // opcode
		printf("00000"); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		printf("%d\n", getImm(linha, 2)); // imm
	} else if(strcmp(funct, "beql") == 0) { // 010100 -> beql, I-Type. beql rs, rt, imm
		printf("010100"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s\n", getImmAddress(linha, 3)); // imm
	} else if(strcmp(funct, "bnel") == 0) { // 010101 -> bnel, I-Type. bnel rs, rt, imm
		printf("010101"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("%s", registerAddress[getReg(linha, 2)]); // $rt
		printf("%s\n", getImmAddress(linha, 3)); // imm
	} else if(strcmp(funct, "blezl") == 0) {// 010110 -> blezl, I-Type. blezl rs, imm
		printf("010110"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("00000"); // $rt
		printf("%s\n", getImmAddress(linha, 2)); // imm
	} else if(strcmp(funct, "bgtzl") == 0) { // 010111 -> bgtzl, I-Type. bgtzl rs, imm
		printf("010111"); // opcode
		printf("%s", registerAddress[getReg(linha, 1)]); // $rs
		printf("00000"); // $rt
		printf("%s\n", getImmAddress(linha, 2)); // imm
	} else if(strcmp(funct, "daddi") == 0) { // 011000 -> daddi, I-Type. daddi rt, rs, imm
		printf("011000"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 3)); // imm
	} else if(strcmp(funct, "daddiu") == 0) { // 011001 -> daddiu, I-Type. daddiu rt, rs, imm
		printf("011001"); // opcode
		printf("%s", registerAddress[getReg(linha, 2)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 3)); // imm
		printf("\n");
	} else if(strcmp(funct, "ldl") == 0) { // 011010 -> ldl, I-Type. ldl rt, imm(rs)
		printf("011010"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "ldr") == 0) { // 011011 -> ldr, I-Type. ldr rt, imm(rs)
		printf("011011"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "lb") == 0) { // 100000 -> lb, I-Type. lb rt, imm(rs)
		printf("100000"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "lh") == 0) { // 100001 -> lh, I-Type. lh rt, imm(rs)
		printf("100001"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "lwl") == 0) { // 100010 -> lwl, I-Type. lwl rt, imm(rs)
		printf("100010"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "lw") == 0) { // 100011 -> lw, I-Type. lw rt, imm(rs)
		printf("100011"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "lbu") == 0) { // 100100 -> lbu, I-Type. lbu rt, imm(rs)
		printf("100100"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "lhu") == 0) { // 100101 -> lhu, I-Type. lhu rt, imm(rs)
		printf("100101"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "lwr") == 0) { // 100110 -> lwr, I-Type. lwr rt, imm(rs)
		printf("100110"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "lwu") == 0) { // 100111 -> lwu, I-Type. lwu rt, imm(rs)
		printf("100111"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "sb") == 0) { // 101000 -> sb, I-Type. sb rt, imm(rs)
		printf("101000"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "sh") == 0) { // 101000 -> sh, I-Type. sh rt, imm(rs)
		printf("101000"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "swl") == 0) { // 101010 -> swl, I-Type. swl rt, imm(rs)
		printf("101010"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "sw") == 0) { // 101011 -> sw, I-Type. sw rt, imm(rs)
		printf("101011"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "sdl") == 0) { // 101100 -> sdl, I-Type. sdl rt, imm(rs)
		printf("101100"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "sdr") == 0) { // 101101 -> sdr, I-Type. sdr rt, imm(rs)
		printf("101101"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "swr") == 0) { // 101110 -> swr, I-Type. swr rt, imm(rs)
		printf("101110"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "ll") == 0) { // 110000 -> ll, I-Type. ll rt, imm(rs)
		printf("110000"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "lld") == 0) { // 110100 -> lld, I-Type. lld rt, imm(rs)
		printf("110100"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "ld") == 0) { // 110111 -> ld, I-Type. ld rt, imm(rs)
		printf("110111"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "sc") == 0) { // 111000 -> sc, I-Type. sc rt, imm(rs)
		printf("111000"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "scd") == 0) { // 111100 -> scd, I-Type. scd rt, imm(rs)
		printf("111100"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else if(strcmp(funct, "sd") == 0) { // 111111 -> sd, I-Type. sd rt, imm(rs)
		printf("111111"); // opcode
		printf("%s", registerAddress[getReg(linha, 3)]); // $rs
		printf("%s", registerAddress[getReg(linha, 1)]); // $rt
		bin_prnt_byte(getImm(linha, 2)); // imm
		printf("\n");
	} else // outros casos.
		fprintf(stdout, "Instrução não implementada. Função: %s.\n", getFunction(linha));
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Uso:\n ./codificador arquivo.b\n");
		return (0);
	}
	
	FILE *arquivo = fopen(argv[1], "r");
	size_t len = 100; // tamanho da linha.
	char *linha = malloc(len);
	
	if (!arquivo) {
		perror(argv[1]);
		exit(1);
	}
	
	while (getline(&linha, &len, arquivo) > 0) {
		// teste: bin_prnt_byte(0x8C130004);
		strtok(linha, ";");
		codificar(linha);
	}
	
	if (linha)
		free(linha);
	
	fclose(arquivo);
	return 0;
}