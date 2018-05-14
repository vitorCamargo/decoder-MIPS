// Escreva uma função recursiva que recebe 1 vetor e seu tamanho, e um
//índice. O retorno deve ser 1 se esse vetor representa um max Heap, ou
//0, caso contrário
//    a. Se o índice for de um nó folha, é maxHeap (percorreu todos os
//       nós não folha e todos eram)
//    b. Se o índice não é nó folha, verifica se o elemento é maior que os
//       2 filhos, e chama recursivamente a função para os filhos.

#include <stdio.h>

int v[7];

int isMaxHeap(int* v, int n, int i) {
	if(i >= n/2) return 1;
	else if(v[i] > v[2*i+1] && v[i] > v[2*i+2]) isMaxHeap(v, n, i+1);
	else return 0;
}

void main() {
	int i, j;
	for(i = 0; i < 7; i++)
		scanf("%d", &v[i]);
	
	printf("Digite a posição a ser verificada: ");
	scanf("%d", &j);
	
	printf("%d", isMaxHeap(v, 7, j));
}
