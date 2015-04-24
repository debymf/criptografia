/*
 * Deborah Mendes Ferreira - 09/0042735
 * 
 * Programa para Cifra de Imagens
 * 
 * Uso: cifraimagem [-e|-d] [-v|-h|-t] imagem_entrada imagem_saida
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ppm.h"

/* Protótipos das funções */
void cifra_vigenere(unsigned char *buf, int len);
void cifra_hill(unsigned char *buf, int len);
void decifra_hill(unsigned char *buf, int len);
void cifra_tea_ecb(unsigned char *buf, int len);
void decifra_tea_ecb(unsigned char *buf, int len);
void cifra_tea_cbc(unsigned char *buf, int len);
void decifra_tea_cbc(unsigned char *buf, int len);

/* Arquivos passados como parâmetro para o programa */
FILE *entrada, *saida;

/* Programa Principal */
int main(int argc, char *argv[]) {
	int cifrar = 0;
	pic imagem;

	/* Checagem do número de parâmetros */
	if (argc != 5) {
		fprintf(stderr,
				"\nUso: cifraimagem [-e|-d] [-v|-h|-t1|-t2] imagem_entrada imagem_saida.\n");
		exit(1);
	}
	/* Checagem do modo de operação */
	if (argv[1][0] != '-' || strlen(argv[1]) != 2) {
		fprintf(stderr,
				"\nUtilize -e para encriptação e -d para decriptação.\n");
		exit(1);
	}
	cifrar = (argv[1][1] == 'e');
	/* Checagem do algoritmo passado como parâmetro */
	if (argv[2][0] != '-' || strlen(argv[1]) != 2) {
		fprintf(stderr, "\nAlgoritmo desconhecido, utilize [-v|-h|-t1|-t2].\n");
		exit(1);
	}
	/* Checagem da existência e permissões dos arquivos passados como parâmetros */
	if ((entrada = fopen(argv[3], "rb")) == NULL) {
		fprintf(stderr, "\nNão é possível ler arquivo de entrada.\n");
		exit(1);
	}
	if ((saida = fopen(argv[4], "wb")) == NULL) {
		fprintf(stderr, "\nNão é possível gravar em arquivo de saída.\n");
		exit(1);
	}

	if (ppm_read(entrada, &imagem) == 0) {
		fprintf(stderr, "\nNão é possível ler imagem de entrada.\n");
		exit(1);
	}

	/* Passagem do controle para o algoritmo escolhido */
	switch (argv[2][1]) {
		case 'v':
			cifra_vigenere(imagem.pix, imagem.nx * imagem.ny * 3);
			break;
		case 'h':
			if (cifrar)
				cifra_hill(imagem.pix, imagem.nx * imagem.ny * 3);
			else
				decifra_hill(imagem.pix, imagem.nx * imagem.ny * 3);
			break;
		case 't':
			if (argv[2][2] == '1') {
				if (cifrar)
					cifra_tea_ecb(imagem.pix, imagem.nx * imagem.ny * 3);
				else
					decifra_tea_ecb(imagem.pix, imagem.nx * imagem.ny * 3);
				break;
			}
			if (argv[2][2] == '2') {
				if (cifrar)
					cifra_tea_cbc(imagem.pix, imagem.nx * imagem.ny * 3);
				else
					decifra_tea_cbc(imagem.pix, imagem.nx * imagem.ny * 3);
				break;
			}
	}

	if (ppm_write(saida, &imagem) == 0) {
		fprintf(stderr, "\nNão é possível gravar imagem de saída.\n");
		exit(1);
	}

	fclose(entrada);
	fclose(saida);
}

/* Implementa a Cifra de Vigenére */
void cifra_vigenere(unsigned char *buf, int len) {
	int tamanho_chave;
	int indice;
	unsigned int caractere;
	unsigned int criptograma;
	unsigned char *chave;

	printf("Digite o tamanho da chave: ");
	if (scanf("%d", &tamanho_chave) != 1) {
		fprintf(stderr, "\nTamanho de chave inválido.\n");
		exit(1);
	}
	chave = (char *)malloc((tamanho_chave + 1) * (sizeof(char)));
	getchar();

	printf("Digite a chave: ");
	for (indice = 0; indice < tamanho_chave; indice++) {
		if (scanf("%u", &caractere) != 1) {
			fprintf(stderr, "\nFalha ao ler elemento da chave.\n");
			exit(1);
		}
		chave[indice] = caractere % 256;
	}
	chave[tamanho_chave] = '\0';

	indice = 0;
	for (indice = 0; indice < len; indice++) {
		buf[indice] = buf[indice] ^ chave[indice % tamanho_chave];
	}
	free(chave);
}

/* Verifica se a matriz é invertivel, retorna 0 se não for e 1 caso seja. */
int verifica_invertivel(int **key){
	int det;

	det = (key[0][0] * key[1][1]) - (key[0][1] * key[1][0]);
	
	if (mdc(det, 256)==1) return 1;
	else return 0;
}

int mdc(int a, int b) {
	int i, menor;
	int resultado;

	if(a == b)
		return(a);
	if(a < b)
		menor = a;
	else
		menor = b;
	for(i = 1; i <= menor; i++)
		if(((a % i) == 0) && ((b % i) == 0))
			resultado = i;
	return(resultado);
}

/* Função para ler a chave da cifra de Hill */
int **chave_cifra_hill(){
	int i,j;
	int** key;

	key = (int **) calloc(2, 1+sizeof (int));
	key[0] = calloc(2, sizeof(int));
	key[1] = calloc(2, sizeof(int));
 
	for (i=0;i<2;i++){
		for (j=0;j<2;j++){
			do{
				printf("Digite a chave para A[%d,%d](0-255)\n", j+1,i+1);
				scanf("%d", &(key[j][i]));
				if ((key[j][i]<0)||(key[j][i]>255)) printf("ERRO: Chave deve estar entre 0 e 255!\n");
			}while ((key[j][i]<0)||(key[j][i]>255));
		}
	}

	return key;
}

/* Função que cifra a imagem utilizando a cifra de hill */
void cifra_hill(unsigned char *buf, int len) {
	int **key;
	int indice;
	int invertivel;
	int a;

	do{
		key = chave_cifra_hill();

		invertivel = verifica_invertivel(key);

		if (!invertivel){
			printf("ERRO: Esta matriz não é invertivel módulo 256, insira outra matriz!\n");
		}
	}while (!invertivel);

	for (indice = 0; indice < len; indice=indice+2) {
		if (indice+1<len){
			a = (buf[indice] * key[0][0]) + (buf[indice + 1] * key[1][0]);
			buf[indice+1] = ((buf[indice] * key[0][1]) + (buf[indice+1] * key[1][1])) % 256;
			buf[indice] = a % 256;
		}
	}
}

/* Função para encontrar o inverso do determinante */
int encontra_inverso(int determinante){
	int tmp1=0;
	int inv_det;
	int n=0;

	while (tmp1 != 1){
		tmp1 = (determinante*n) % 256;
		if (tmp1 == 1) 
			inv_det = n;
		n++;
	}

	return inv_det;
}

/* Função para decifrar a imagem que foi cifrada com cifra de hill */
void decifra_hill(unsigned char *buf, int len) {
	int **key;
	int indice;
	int invertivel;
	int a;
	int determinante;
	int inversa[2][2];
	int inverso_det;
	int i,j;
	int menos=1;

	do{
		key = chave_cifra_hill();
		invertivel = verifica_invertivel(key);
		if (!invertivel){
			printf("ERRO: Esta matriz não é invertivel módulo 256, insira outra matriz!\n");
		}
	}while (!invertivel);

	determinante = (key[0][0] * key[1][1]) - (key[1][0] * key[0][1]);
	inverso_det = encontra_inverso(determinante % 256);

	for(i=0;i<2;i++){
		for(j=0;j<2;j++){
			inversa[i][j] = key[1-j][1-i];
			if (i!=j){
				inversa[i][j] = inversa[i][j] * -1;
			}
			inversa[i][j] = (inversa[i][j] * inverso_det) % 256;
			if (inversa[i][j]<0){
				inversa[i][j] = inversa[i][j] + 256;
			}
		}
	}

	for (indice = 0; indice < len; indice=indice+2) {
		if (indice+1<len){
			a = (buf[indice] * inversa[0][0]) + (buf[indice + 1] * inversa[1][0]);
			buf[indice+1] = ((buf[indice] * inversa[0][1]) + (buf[indice+1] * inversa[1][1])) % 256;
			buf[indice] = a % 256;
		}
	}
}

/* Função para cifrar utilizando TEA, obtida do artigo disponibilizado */
void cifra_tea(long* v, long* k) {
	unsigned long y=v[0],z=v[1], sum=0,
	delta=0x9e3779b9,
	n=32 ;

	while (n-->0) {
	sum += delta ;
	y += ((z<<4)+k[0]) ^ (z+sum) ^ ((z>>5)+k[1]) ;
	z += ((y<<4)+k[2]) ^ (y+sum) ^ ((y>>5)+k[3]) ;
	}
	v[0]=y ; v[1]=z ; 
}

/* Função para ler as chaves para TEA */
long *chave_tea(){
	unsigned long int *key;
	int i;

	key = (long *) calloc(4, sizeof (long));
	printf("Digite as 4 chaves(em hexadecimal):\n");
	scanf("%lx %lx %lx %lx", &key[0], &key[1], &key[2], &key[3]);

	return key;
}

/* Função que cifra com TEA utilizando o modo ECB */
void cifra_tea_ecb(unsigned char *buf, int len) {
	long *key;
	long *data;
	int indice=0;

	data = (long *) calloc(2, sizeof (long));

	key = chave_tea();

	while ((indice<len)&&(indice+7<len)){
		data[0] = (buf[indice] << 24);
		data[0] = data[0] | (buf[indice+1] << 16);
		data[0] = data[0] | (buf[indice+2] << 8);
		data[0] = data[0] | (buf[indice+3]);
		data[1] = (buf[indice+4] << 24);
		data[1] = data[1] | (buf[indice+5] << 16);
		data[1] = data[1] | (buf[indice+6] << 8);
		data[1] = data[1] | (buf[indice+7]);
		cifra_tea(data, key);
		buf[indice] = data[0] >> 24;
		buf[indice+1] = data[0] >> 16;
		buf[indice+2] = data[0] >> 8;
		buf[indice+3] = data[0];
		buf[indice+4] = data[1] >> 24;
		buf[indice+5] = data[1] >> 16;
		buf[indice+6] = data[1] >> 8;
		buf[indice+7] = data[1];
		indice = indice + 8;	
	}

}

/* Função para decifrar um bloco em TEA, obtida do paper disponilizado*/
void decifra_tea(long* v,long* k) {
	unsigned long n=32, sum, y=v[0], z=v[1],
	delta=0x9e3779b9 ;
	sum=delta<<5 ;

	while (n-->0) {
		z-= ((y<<4)+k[2]) ^ (y+sum) ^ ((y>>5)+k[3]) ;
		y-= ((z<<4)+k[0]) ^ (z+sum) ^ ((z>>5)+k[1]) ;
		sum-=delta ; 
	}

	v[0]=y ; v[1]=z ; 
}

/* Função que decifra uma imagem cifrada com TEA em modo ECB*/
void decifra_tea_ecb(unsigned char *buf, int len) {
	long *key;
	long *data;
	int indice=0;

	data = (long *) calloc(2, sizeof (long));

	key = chave_tea();

	while ((indice<len)&&(indice+7<len)){
		data[0] = (buf[indice] << 24);
		data[0] = data[0] | (buf[indice+1] << 16);
		data[0] = data[0] | (buf[indice+2] << 8);
		data[0] = data[0] | (buf[indice+3]);
		data[1] = (buf[indice+4] << 24);
		data[1] = data[1] | (buf[indice+5] << 16);
		data[1] = data[1] | (buf[indice+6] << 8);
		data[1] = data[1] | (buf[indice+7]);
		decifra_tea(data, key);
		buf[indice] = data[0] >> 24;
		buf[indice+1] = data[0] >> 16;
		buf[indice+2] = data[0] >> 8;
		buf[indice+3] = data[0];
		buf[indice+4] = data[1] >> 24;
		buf[indice+5] = data[1] >> 16;
		buf[indice+6] = data[1] >> 8;
		buf[indice+7] = data[1];
		indice = indice + 8;	
	}
}

/* Função que cifra TEA utilizando o modo CBC */
void cifra_tea_cbc(unsigned char *buf, int len) {
	long *key;
	long *data;
	int indice=0;
	long *plain_text;

	data = (long *) calloc(2, sizeof (long));
	plain_text = (long *) calloc(2, sizeof (long));

	key = chave_tea();
/* Vetor de Inicialização */
	data[0] = 0x25061991;
	data[1] = 0x19031997;
	while ((indice<len)&&(indice+7<len)){
		plain_text[0] = (buf[indice] << 24);
		plain_text[0] = plain_text[0] | (buf[indice+1] << 16);
		plain_text[0] = plain_text[0] | (buf[indice+2] << 8);
		plain_text[0] = plain_text[0] | (buf[indice+3]);
		plain_text[1] = (buf[indice+4] << 24);
		plain_text[1] = plain_text[1] | (buf[indice+5] << 16);
		plain_text[1] = plain_text[1] | (buf[indice+6] << 8);
		plain_text[1] = plain_text[1] | (buf[indice+7]);

		data[0] = plain_text[0] ^ data[0];
		data[1] = plain_text[1] ^ data[1];

		cifra_tea(data, key);

		buf[indice] = data[0] >> 24;
		buf[indice+1] = data[0] >> 16;
		buf[indice+2] = data[0] >> 8;
		buf[indice+3] = data[0];
		buf[indice+4] = data[1] >> 24;
		buf[indice+5] = data[1] >> 16;
		buf[indice+6] = data[1] >> 8;
		buf[indice+7] = data[1];
		indice = indice + 8;	
	}

}

/* Função que decifra TEA utilizando o modo CBC */
void decifra_tea_cbc(unsigned char *buf, int len) {
	long *key;
	long *data;
	int indice=0;
	long *cipher_text;
	long *cipher_text_aux;

	data = (long *) calloc(2, sizeof (long));
	cipher_text = (long *) calloc(2, sizeof (long));
	cipher_text_aux = (long *) calloc(2, sizeof (long));

	key = chave_tea();
/* Vetor de Inicialização */
	data[0] = 0x25061991;
	data[1] = 0x19031997;
	while ((indice<len)&&(indice+7<len)){
		cipher_text[0] = (buf[indice] << 24);
		cipher_text[0] = cipher_text[0] | (buf[indice+1] << 16);
		cipher_text[0] = cipher_text[0] | (buf[indice+2] << 8);
		cipher_text[0] = cipher_text[0] | (buf[indice+3]);
		cipher_text[1] = (buf[indice+4] << 24);
		cipher_text[1] = cipher_text[1] | (buf[indice+5] << 16);
		cipher_text[1] = cipher_text[1] | (buf[indice+6] << 8);
		cipher_text[1] = cipher_text[1] | (buf[indice+7]);
		cipher_text_aux[0] = cipher_text[0];
		cipher_text_aux[1] = cipher_text[1];

		decifra_tea(cipher_text, key);

		data[0] = data[0] ^ cipher_text[0];
		data[1] = data[1] ^ cipher_text[1];
	
		buf[indice] = data[0] >> 24;
		buf[indice+1] = data[0] >> 16;
		buf[indice+2] = data[0] >> 8;
		buf[indice+3] = data[0];
		buf[indice+4] = data[1] >> 24;
		buf[indice+5] = data[1] >> 16;
		buf[indice+6] = data[1] >> 8;
		buf[indice+7] = data[1];
		
		data[0] = cipher_text_aux[0];
		data[1] = cipher_text_aux[1];
		indice = indice + 8;	
	}

}
