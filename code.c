/*

Equipe 6 - Bit envolvente
Integrantes:
    Alexandre Batistella Bellas     9763168
    Felipe Manfio Barbosa           9771640
    João Vitor Granzotti Machado    9393322
    Tiago Lemes Daneluzzi           8531320

*/

/* After you have implemented your functions, you may safely remove these lines. */
#include <stdio.h>
#include <stdlib.h>
#define not_implemented() fprintf(stderr, "Not implemented\n"); exit(EXIT_FAILURE)

/* Students, you are required to implemented the functions bellow.
   Please, refere to cpu.h for further information. */


//mascara para definir os diferentes estados da maquinas de estados
#define estado_0		   0x9408	   // 1001 0100 0000 1000
#define estado_1		   0x0018      // 0000 0000 0001 1000
#define estado_2    	   0x0014      // 0000 0000 0001 0100
#define estado_3           0x1800      // 0001 1000 0000 0000
#define estado_4           0x4002      // 0100 0000 0000 0010
#define estado_5           0x0802      // 0000 1000 0000 0010
#define estado_6           0x0044      // 0000 0000 0100 0100
#define estado_7           0x0003      // 0000 0000 0000 0011
#define estado_8           0x02A4      // 0000 0010 1010 0100
#define estado_9           0x0480      // 0000 0100 1000 0000

//mascaras para opcodes das operações
#define opCode_Tipo_R	   0x00	     // 000000
#define opCode_LW		   0x23      // 100011
#define opCode_SW    	   0x2b      // 101011
#define opCode_Beq         0x04      // 000100
#define opCode_J           0x02      // 000010

//variaveis globais
char zero;
char overflow;

//A função basicamente rece uma operação pelo parametro alu_op e decide entre
//as operações em um switch, após isso verifica se o resultado da operação,
//guardado em result_alu é zero, se for o ponteiro zero recebe 1.
int alu( int a, int b, char alu_op, int *result_alu, char *zero, char *overflow)
{
    //verifica qual a operção de acordo com os 4 ultimos bits e realiza a mesma
    switch(alu_op){
        case ativa_soma:
            *result_alu = a + b;
            break;
        case ativa_subtracao:
            *result_alu = a - b;
            break;
        case ativa_or:
            *result_alu = a | b;
            break;
        case ativa_and:
            *result_alu = a & b;
            break;
        case ativa_slt:
            if(a<b){
                *result_alu = 0x01;
            }
            else{
                *result_alu = 0x00;
            }
            break;
        default:
            printf("alu_op invalido!");
            break;
    }
    if(*result_alu == 0){
        *zero = ativa_bit_zero;
    }
    else{
        *zero = desativa_bit_zero;
    }
    return 0;
}

//Função que recebe o function (5-0 iR) e os bits de sc ALUOp0 e ALUOp1 e calcula o valor de alu_op
void alu_control(int IR, int sc, char *alu_op){
    //vamos agora verificar os diferentes valores
    switch(((sc & separa_ALUOp0) | (sc & separa_ALUOp1)) >> 5){
        //Essa combinação indica LW e SW
        case 0x0:
            //independe de function
            *alu_op = 0b0010;
            break;
        //Essa combinação indica Branch
        case 0x1:
            //independe de function
            *alu_op = 0b0110;
            break;
        //Essa combinação indica Tipo-R
        case 0x2:
            //Tipo-R depende de function
            //Vamos realizar as diferentes combinações de function
            //function são os ultimos 4 bits de IR
            switch(IR & 0x0f){
                //Essa combinação indica ADD
                case 0b0000:
                    *alu_op = 0b0010;  //retorna o alu_op responsavel pela ADD
                    break;
                //Essa combinação indica SUB
                case 0b0010:
                    *alu_op = 0b0110;  //retorn o alu_op responsavel pela SUB
                    break;
                //Essa combinação indica AND
                case 0b0100:
                    *alu_op = 0b0000;  //retorn o alu_op responsavel pela AND
                    break;
                //Essa combinação indica OR
                case 0b0101:
                    *alu_op = 0b0001;  //retorn o alu_op responsavel pela OR
                    break;
                //Essa combinação indica SLT
                case 0b1010:
                    *alu_op = 0b0111;  //retorn o alu_op responsavel pela SLT
                    break;
                default:
                    break;
            }
            break;
        //Essa combinação indica Tipo-R
        case 0x3:
            //Tipo-R depende de function
            //Vamos realizar as diferentes combinações de function
            switch(IR & 0x0f){
                //Essa combinação indica SUB
                case 0b0010:
                    *alu_op = 0b0110;  //retorn o alu_op responsavel pela SUB
                    break;
                //Essa combinação indica SLT
                case 0b1010:
                    *alu_op = 0b0111;  //retorn o alu_op responsavel pela SLT
                    break;
                default:
                    break;
            }
            break;
        default:
            return;
            break;
    }
}


void control_unit(int IR, short int *sc)
{
    //Conferindo Se é o inicio do programa
    if(IR == -1){
        *sc = estado_0;
    }
    else{
        //Separando o opCode para identificar a operação que pode ser tipo R, LW, SW e etc
        char opCode = (IR & separa_cop) >> 26;

        //O instruction Fetch acontece pra qualquer opCode então vamos verificar se está nesse estado
        if(*sc == ((short int)estado_0)){
            *sc = estado_1; // jogamos para o estado dois então, que também acontece em todas as operações.
        }
        else{
            //com o opCode em mão vamos verificar qual o peração será feita
            switch(opCode){
                case ((short int)opCode_Tipo_R):
                    //Sabendo que estamos em uma Tipo-r vamos verificar o estado atual para calcular o proximo
                    switch(*sc){
                        case ((short int)estado_1):
                            *sc = estado_6;
                            break;
                        case ((short int)estado_6):
                            *sc = estado_7;
                            break;
                        case ((short int)estado_7):
                            *sc = estado_0;
                            break;
                    }
                    break;
                case ((short int)opCode_LW):
                    //Sabemos que estamos em um LW vamos verificar o estado atual pra calcular o proximo
                    switch(*sc){
                        case ((short int)estado_1):
                            *sc = estado_2;
                            break;
                        case ((short int)estado_2):
                            *sc = estado_3;
                            break;
                        case ((short int)estado_3):
                            *sc = estado_4;
                            break;
                        case ((short int)estado_4):
                            *sc = estado_0;
                            break;
                    }
                    break;
                case ((short int)opCode_SW):
                    //Sabemos que estamos em um LW vamos verificar o estado atual pra calcular o proximo
                    switch(*sc){
                        case ((short int)estado_1):
                            *sc = estado_2;
                            break;
                        case ((short int)estado_2):
                            *sc = estado_5;
                            break;
                        case ((short int)estado_5):
                            *sc = estado_0;
                            break;
                    }
                    break;
                case ((short int)opCode_Beq):
                    //Sabemos que estamos em um Beq vamos verificar o estado atual pra calcular o proximo
                    switch(*sc){
                        case ((short int)estado_1):
                            *sc = estado_8;
                            break;
                        case ((short int)estado_8):
                            *sc = estado_0;
                            break;
                    }
                    break;
                case ((short int)opCode_J):
                    //Sabemos que estamos em um Beq vamos verificar o estado atual pra calcular o proximo
                    switch(*sc){
                        case ((short int)estado_1):
                            *sc = estado_9;
                            break;
                        case ((short int)estado_9):
                            *sc = estado_0;
                            break;
                    }
                    break;
                default:
                    printf("opCode invalido!\n");
                    break;
            }
        }
    }
    //not_implemented();
}


void instruction_fetch(short int sc, int PC, int ALUOUT, int IR, int* PCnew, int* IRnew, int* MDRnew)
{
    if(sc == ((short int)estado_0)){
        //coloca em IR o que tem no endereço de mmemoria em PC
        *IRnew = memory[PC];

        //vamos primeiro pegar a operação da ula
        char alu_op;
        alu_control(IR, sc, &alu_op);

        //incrementa o PC usando a ula
        alu(PC, 1, alu_op, &ALUOUT, &zero, &overflow);
        *PCnew = ALUOUT;
        *MDRnew = memory[PC];
		if(*IRnew == 0){
			loop = 0;
			return;
		}
    }
    else{
        return;
    }
}


void decode_register(short int sc, int IR, int PC, int A, int B, int *Anew, int *Bnew, int *ALUOUTnew)
{
    //verificando se estamos em um estado de ID
    if(sc == ((short int)estado_1)){
        //colocando os valores nos buffers para o proximo ciclo
        *Anew = reg[(separa_rs & IR) >> 21]; //separando o rs e colocando no buffer
        *Bnew = reg[(separa_rt & IR) >> 16]; //separando o rt e colocando no buffer
        //eu preciso dar shift pra que fique o valor real
        // 0000100000000000 != 0000000000001000 saca ??

        //colocando no buffer AluOut o resultado de uma possivel brench
        //pra isso precisamos do imediato somado ao PC com um shift de 2 e extendido
        //e vamos usar a ULA pra somar

        //vamos primeiro pegar a operação da ula
        char alu_op;
        alu_control(IR, sc, &alu_op);

        //agora vamos realizar a operação
        alu(PC, (separa_imediato & IR), alu_op, ALUOUTnew, &zero, &overflow);
        //Essa linha cima basicamente soma PC com o imediato em IR e guarda em ALUOTnew
    }
    else{
        return;
    }
}


void exec_calc_end_branch(short int sc, int A, int B, int IR, int PC, int ALUOUT, int *ALUOUTnew, int *PCnew)
{
    //vamos primeiro pegar a operação da ula
    char alu_op;
    alu_control(IR, sc, &alu_op);

    //Vamos verificar se estamos em um estado de EX e escolhendo o que fazer
    switch(sc){
        case ((short int)estado_2):
            //Computa endereços de momoria para LW ou SW
            //devemos somar um endereço em rs com um offset atraves da ULA e guardar o resultado no buffer
            alu(A, IR & separa_imediato, alu_op, ALUOUTnew, &zero, &overflow);
            break;
        case ((short int)estado_6):
            //Iinstruções tipo-R, devemos somar A e B e colocar em alluoutnew
            //primeiro devemos pegar a operação
            alu(A, B, alu_op, ALUOUTnew, &zero, &overflow);
            break;
        case ((short int)estado_8):
            //Finaliza uma branch
            //Se a condição for verdadeira então PC recebe o resultado no buffer
            alu(A, B, alu_op, ALUOUTnew, &zero, &overflow);
            if(zero == 1){
                *PCnew = ALUOUT;
            }
            else{
                return;
            }
            break;
        case ((short int)estado_9):
            //Finalização de uma Jump
            //Devemos colocar o endereço de jump em PC
            *PCnew = ((PC & separa_4bits_PC) | (IR & separa_endereco_jump));
            //A linha acima concatena os quatros prmimeiros vbits de PC com o endereço de jump contido em IR
            break;
        default:
            break;
    }
}


void write_r_access_memory(short int sc, int B, int IR, int ALUOUT, int PC, int *MDRnew, int *IRnew){
    //Vamos verificar se estamos em um estado de MEM e escolher o que fazer
    switch(sc){
        case ((short int)estado_3):
            //Vamos ler algo da memoria para uma LW
            *MDRnew = memory[ALUOUT];
            break;
        case ((short int)estado_5):
            //Vamos escrever algo na memoria para uma sW
            memory[ALUOUT] = reg[(IR & separa_rt) >> 16];
            //Lembrando 111100000000000 != 000000000001111, apesar de ter 32 bits em jogo, o exemplo é ilustrativo
            break;
        case ((short int)estado_7):
            //Vamos finalizar uma operação Tipo-R
            //ao inves de pegar o valor certo que seria 8 ele pega o valor 0
            reg[(IR & separa_rd) >> 11] = ALUOUT;
            //A linha cima coloco o valor em ALUOUT no registrador de endereço dado pelo rd em IR
            break;
        default:
            break;
    }
}

//Essa função é responsavel somente pelo Write Back em uma LW
void write_ref_mem(short int sc, int IR, int MDR, int ALUOUT){
    //Vamos verificar se estamos em um estado de WB e fazer o que devemos
    if(sc == ((short int)estado_4)){
        //Precisamo carregar o valor fornecido para o registrador indicado por rt em IR
        reg[(IR & separa_rt) >> 16] = MDR;
        //A linha acima coloca o valor em MDR no reg[] na posição dada por rt
    }
    else{
        return;
    }
}
