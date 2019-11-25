/*
 * @Author: Deng Cai 
 * @Date: 2019-10-31 11:25:51 
 * @Last Modified by: Deng Cai
 * @Last Modified time: 2019-10-31 18:59:04
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TF tm->transition
//	add a new transition function as state q_i’s j_th function; the other\
//		parameters are the same as those in structure function.
#define NEW_TF(i,j,now_,new_,move_,q_) \
            TF[i][j].now=now_;\
            TF[i][j].new=new_;\
            TF[i][j].move=move_;TF[i][j].q=q_
#define FN tm->func_num

typedef struct function {
    char now;
    char new;
    int move;
    int q;
} function;

typedef struct {
    int Q;
    int q1;
    int reject;
    int accept;
    char *inputAlphabet;
    char *tapeAlphabet;
    function **transition;

    int i_num;
    int t_num;
    int *func_num;
} TM;

void produce_tm(TM *tm)
{
    // create all states.
    tm->Q = 14;
    tm->q1 = 1;
    tm->reject = 13;
    tm->accept = 14;

    // complete the input alphabet.
    tm->i_num = 4;
    tm->inputAlphabet = (char*)malloc(4);
    tm->inputAlphabet[0] = 'a';
    tm->inputAlphabet[1] = 'b';
    tm->inputAlphabet[2] = 'c';
    tm->inputAlphabet[3] = 'd';

    // complete the tape alphabet.
    tm->t_num = 8;
    tm->tapeAlphabet = (char*)malloc(8);
    tm->tapeAlphabet[0] = 'a';
    tm->tapeAlphabet[1] = 'b';
    tm->tapeAlphabet[2] = 'c';
    tm->tapeAlphabet[3] = 'd';
    tm->tapeAlphabet[4] = 'x';
    tm->tapeAlphabet[5] = 'y';
    tm->tapeAlphabet[6] = 'z';
    tm->tapeAlphabet[7] = '\0';

    // count the number of every state’s functions.
    FN = (int*)malloc(sizeof(int)*(tm->Q+1));
    FN[0] = 0;
    FN[1] = 5;
    FN[2] = 5;
    FN[3] = 4;
    FN[4] = 4;
    FN[5] = 2;
    FN[6] = 7;
    FN[7] = 3;
    FN[8] = 2;
    FN[9] = 3;
    FN[10] = 2;
    FN[11] = 3;
    FN[12] = 2;
    FN[13] = 0;
    FN[14] = 0;

    // create all transition functions.
    TF = (function**)malloc((1 + tm->Q) * sizeof(function*));
    TF[0] = NULL;
    TF[1] = (function*)malloc(sizeof(function)*5);
    TF[2] = (function*)malloc(sizeof(function)*5);
    TF[3] = (function*)malloc(sizeof(function)*4);
    TF[4] = (function*)malloc(sizeof(function)*4);
    TF[5] = (function*)malloc(sizeof(function)*2);
    TF[6] = (function*)malloc(sizeof(function)*7);
    TF[7] = (function*)malloc(sizeof(function)*3);
    TF[8] = (function*)malloc(sizeof(function)*2);
    TF[9] = (function*)malloc(sizeof(function)*3);
    TF[10] = (function*)malloc(sizeof(function)*2);
    TF[11] = (function*)malloc(sizeof(function)*3);
    TF[12] = (function*)malloc(sizeof(function)*2);
    TF[13] = NULL;
    TF[14] = NULL;
    NEW_TF(1,0,'a','\0',1,2);
    NEW_TF(1,1,'b','b',1,13);
    NEW_TF(1,2,'c','c',1,13);
    NEW_TF(1,3,'d','d',1,13);
    NEW_TF(1,4,'\0','\0',1,13);
    NEW_TF(2,0,'a','a',1,2);
    NEW_TF(2,1,'b','x',1,3);
    NEW_TF(2,2,'c','c',1,13);
    NEW_TF(2,3,'d','d',1,13);
    NEW_TF(2,4,'\0','\0',1,13);
    NEW_TF(3,0,'b','b',1,3);
    NEW_TF(3,1,'c','y',1,4);
    NEW_TF(3,2,'d','d',1,13);
    NEW_TF(3,3,'\0','\0',1,13);
    NEW_TF(4,0,'c','c',1,4);
    NEW_TF(4,1,'z','z',1,4);
    NEW_TF(4,2,'d','z',1,5);
    NEW_TF(4,3,'\0','\0',1,13);
    NEW_TF(5,0,'d','d',-1,7);
    NEW_TF(5,1,'\0','\0',-1,6);
    NEW_TF(6,0,'\0','\0',1,14);
    NEW_TF(6,1,'x','x',-1,6);
    NEW_TF(6,2,'y','y',-1,6);
    NEW_TF(6,3,'z','z',-1,6);
    NEW_TF(6,4,'a','a',1,13);
    NEW_TF(6,5,'b','b',1,13);
    NEW_TF(6,6,'c','c',1,13);
    NEW_TF(7,0,'c','c',-1,7);
    NEW_TF(7,1,'z','z',-1,7);
    NEW_TF(7,2,'y','y',1,8);
    NEW_TF(8,0,'c','y',1,4);
    NEW_TF(8,1,'z','z',-1,9);
    NEW_TF(9,0,'y','c',-1,9);
    NEW_TF(9,1,'b','b',-1,9);
    NEW_TF(9,2,'x','x',1,10);
    NEW_TF(10,0,'b','x',1,3);
    NEW_TF(10,1,'c','c',-1,11);
    NEW_TF(11,0,'x','b',-1,11);
    NEW_TF(11,1,'a','a',-1,11);
    NEW_TF(11,2,'\0','\0',1,12);
    NEW_TF(12,0,'a','\0',1,2);
    NEW_TF(12,1,'b','b',1,13);
}

int run_tm(char *input, TM *tm)
{
    int q = 1;
    int i = 0, j;
    char c;
    int rej = tm->reject;
    int acc = tm->accept;

    while(q!=rej && q!=acc)
    {
        c = input[i];
        for(j=0;j<FN[q];j++)
        {
            if(TF[q][j].now==c)
                break;
        }
        if(j==FN[q])    // illegal input for this state.
            return 0;
        input[i] = TF[q][j].new;    // replace the current symbol.
        i += TF[q][j].move; // the head moves to the left or right.
        q = TF[q][j].q; // go to the new state.
    }

    if(q==rej)  return 0;
    if(q==acc)  return 1;
    return -1;
}

void main()
{
    char *input;
    FILE *fp;
    fp = fopen("./input.txt","r");
    int size = 512;
    int i;
    int row = 0;
    
    TM tm;
    produce_tm(&tm);

    while(1)
    {
        input = (char*)malloc(size);
        if(fgets(input,size,fp)==NULL)
            break;
        for(i=0;i<size;i++)
        {
            if(input[i]=='\r' || input[i]=='\n')
            {
                input[i] = '\0';
                break;
            }
            if(input[i]=='\0')
                break;
        }
        if(i==size)
        {
            fseek(fp,-size,SEEK_CUR);
            size += 128;
        }
        else 
        {
            size = 512;
            int x = run_tm(input,&tm);
            row ++;
            if(x)
                printf("Row %d is accepted.\n",row);
            else 
                printf("Row %d is rejected.\n",row);
        }
        free(input);
    }
}