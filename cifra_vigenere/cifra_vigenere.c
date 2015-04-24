/**
Deborah Mendes Ferreira - 09/0042735
Programa compilado utilizando GCC.

O resultado da decifração do criptograma está no arquivo mensagem.txt

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* Struct que armazena a chave encontrada */
typedef struct {
    	int a;
    	int b;
} Key;

/* Struct que armazena todos os diferentes elementos*/
typedef struct {
    	int cont;
    	int value;
    	float frequency;
} Elements;

/* Struct que armazena as letras do alfabeto */
typedef struct {
	float frequency;
	char letter;
}Alphabet;

/* Funcao para calcular o mdc, peguei do arquivo para criar o criptograma */
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

/* Função que decifra, dado um criptograma, o A e o B da chave */
void decipher(int msg[], int a, int b){
	int find=0;
	int tmp1;
	int inverseA;
	int n =0;
	int x;
	FILE *fp;

	fp = fopen("mensagem.txt", "w");
	while (find==0){
			tmp1 =(a*n)%256;
			if (tmp1 == 1){
				find = 1;
				inverseA = n;
			}
			n++;
		}

	n=0;
	printf("\nMensagem:\n\n");
	while (n<10291){
		x = (inverseA *((msg[n]) - b))%256;
		printf("%c", x);
		putc(x, fp);
		n++;
	}

	fclose(fp);
}

/* Resolve um sistema linear de congruencia, entre duas posiveis solucoes*/
Key solveEquation(int x1, int y1, int x2, int y2){
	Key k;
	int a;
	int b;
	int inverseA;
	int tmp1;
	int n=0;
	int find=0;

	while (find==0){
		tmp1 =((x2-x1)*n)%256;
		if (tmp1 == 1){
			find = 1;
			inverseA = n;
		}

		n++;
	}

	a = ((y2 - y1) * inverseA)%256;
	if (a<0){
		a = 256 + a;
	}
	b = y1 - (x1 * a);
	b = b%256;

	if (b<0){
		b = 256 + b;
	}

	k.a = a;
	k.b = b;
	return k;

}

/* Pega toda a msg e armazena em um array, para facilitar a manipulação de inteiros*/
void readMsg(int *msg){
    char numberRead[4];
    char c;
    FILE *fp;
    int j,i=0;

    fp = fopen("criptograma-individual.txt", "r");
    while ((c = getc(fp)) != EOF)
    {
        if ((char) c == ' '){
            *msg = atoi(numberRead);
            i = 0;
            for (j=0;j<4;j++){
                numberRead[j]='\0';
            }
            msg++;
            continue;
        }
        numberRead[i] = c;
        i++;
    }

    fclose(fp);

}

/* Le o alfabeto e suas frequencias do arquivo fornecido*/
void readAlphabet(Alphabet *alphabet){
    char numberRead[8];
    char c;
    FILE *fp;
    int j=0;

    fp = fopen("portugues.txt", "r");
    while ((c = getc(fp)) != EOF)
    {
        if (c == '\''){
            (*alphabet).letter =  getc(fp);
			c = getc(fp);
        }

        if (c == '0'){
            numberRead[0] = c;
            j=1;
            while (j<8){
            	numberRead[j] = getc(fp);
            	j++;
            }
            (*alphabet).frequency = atof(numberRead);
            alphabet++;
        }
    }
    fclose(fp);
}


int main(){
    Elements diffElements[35];

    Elements firstPlaceCript;
    Elements secondPlaceCript;
    Elements thirdPlaceCript;

    Alphabet firstPlaceAlpha;
    Alphabet secondPlaceAlpha;
    Alphabet thirdPlaceAlpha;

    Key k;

    int msg[10291];
    int msgTmp[10291];
    int j=0, i=0,cont=0,n=0;
    int thisNumber;
    Alphabet alphabet[36];

    /* Devemos ler o criptograma e alfabeto */
    readMsg(&msg[0]);
    while (j<10291){
    	msgTmp[j] = msg[j];
    	j++;
    }
    j=0;
    readAlphabet(&alphabet[0]);

    /* Agora que foi lido o alfabeto com suas frequencias e o criptograma, podemos analisar a frequencia de cada elemento no criptograma */
    while (i<10291){
         j = i;
         if (msg[i]!=256){
             thisNumber = msg[i];
             msg[i] = 256;
             cont = 1;
             while (j<10291){
                 if (thisNumber == msg[j]){
                     msg[j] = 256;
                     cont++;
                 }
                 j++;
             }
             diffElements[n].cont = cont;
             diffElements[n].value = thisNumber;
             diffElements[n].frequency = ((float)cont/10291);
             n++;
         }

         i++;
     }

    /* Temos todas as frequencias do portugues e do criptograma, vamos verificar agora qual o maior elemento de cada um */

   /*Primeiro encontramos os 3 caracteres mais frequentes do PT-BR */
    n=0;
    firstPlaceAlpha.frequency = 0;
    secondPlaceAlpha.frequency = 0;
    thirdPlaceAlpha.frequency = 0;
    while (n<36){
    	if ((alphabet[n].frequency)>firstPlaceAlpha.frequency){
    		firstPlaceAlpha = alphabet[n];
    	}else {
    		if ((alphabet[n].frequency)>secondPlaceAlpha.frequency){
    			secondPlaceAlpha = alphabet[n];

    		}else {
    			if ((alphabet[n].frequency)>thirdPlaceAlpha.frequency){
    				thirdPlaceAlpha = alphabet[n];
    			}

    		}

    	}

    	n++;
    }

    /* Agora analisamos as frequencias dos elementos do Criptograma */
    n=0;
    firstPlaceCript.frequency = 0;
    secondPlaceCript.frequency = 0;
    thirdPlaceCript.frequency = 0;
    while (n<36){
    	if ((diffElements[n].frequency)>firstPlaceCript.frequency){
    		firstPlaceCript = diffElements[n];
    	}else {
    		if ((diffElements[n].frequency)>secondPlaceCript.frequency){
    			secondPlaceCript = diffElements[n];

    		}else {
    			if ((diffElements[n].frequency)>thirdPlaceCript.frequency){
    				thirdPlaceCript = diffElements[n];
    			}

    		}

    	}

    	n++;
    }

    /** Tentamos os valores dos mais frequentes **/
    k = solveEquation(firstPlaceAlpha.letter, firstPlaceCript.value, secondPlaceAlpha.letter, secondPlaceCript.value);

    if (mdc(k.a,256)==1){
        printf("Chave - k: (%d,%d)\n", k.a,k.b);
    	decipher(msgTmp, k.a,k.b);
    }

    /* Obtive o resultado correto com o primeiro conjunto de valores, por isso encerro o codigo por aqui*/


    return 0;
}
