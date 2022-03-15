#include "SVHSS.h"

func aFunc;
fmpz_t *vars;
long long allSTermNum=0;

void basicFunction(hPara *pa, BV_Para *bvPara){
    vars = malloc(sizeof(fmpz_t) * pa->varNum);
    flint_rand_t state;
    flint_randinit(state);
    for(int i=0; i<pa->varNum;i++)
    {
        fmpz_init(vars[i]);
        fmpz_randm(vars[i], state,bvPara->msg);
    }
    /*********************prepare the computation task*********************/
    if (SOLO){
        aFunc.tNum=1;
        aFunc.tDegree=(int *)malloc(sizeof(int)*aFunc.tNum);
        aFunc.tSymbol=(int *)malloc(sizeof(int)*aFunc.tNum);
        aFunc.tCon=(int **)malloc(sizeof(int*)*aFunc.tNum);
        aFunc.tDegree[0]=pa->d;
        aFunc.tSymbol[0]=1;
        aFunc.tCon[0]=(int *)malloc(sizeof(int)*aFunc.tDegree[0]);
        for(int j=0; j<aFunc.tDegree[0]; j++)
        {
            aFunc.tCon[0][j]=0;
        }
        allSTermNum+=pow(pa->sNumPerVar, aFunc.tDegree[0]);

    }else{
        pa->d=4;
        int *tNumByD = malloc(sizeof(int)*(pa->d+1));
        int *tNumByDSum = malloc(sizeof(int)*(pa->d+1));
        tNumByDSum[0]=0;
        for(int i=1; i<=pa->d; i++){
            tNumByD[i]=getComNum(pa->varNum+i-1, i);
            tNumByDSum[i] = tNumByDSum[i-1] + tNumByD[i];
        }

        aFunc.tNum= getComNum(pa->d+pa->varNum, pa->d)-1;//choose maxDg from maxDg+pa.varNum
        aFunc.tDegree=(int *)malloc(sizeof(int)*aFunc.tNum);
        aFunc.tCon=(int **)malloc(sizeof(int*)*aFunc.tNum);

        int de=1;
        for(int i=0; i<aFunc.tNum; i++)
        {
            if (i+1>tNumByDSum[de]){de++;}
            aFunc.tDegree[i]=de;
            aFunc.tSymbol[i]=1;
            if (DEBUG){printf("d=%d\n", aFunc.tDegree[i]);}
            aFunc.tCon[i]=(int *)malloc(sizeof(int)*aFunc.tDegree[i]);
            allSTermNum+=pow(pa->sNumPerVar, aFunc.tDegree[i]);
        }
        //find all terms by degree and variable
        allTermsByDV(aFunc, tNumByD, pa->varNum, pa->d);

        if (DEBUG){
            //check aFunc.tCon
            printf("aFunc.tCon\n");
            for(int i=0; i<aFunc.tNum; i++){
                for(int j=0; j<aFunc.tDegree[i]; j++){
                    printf("%d", aFunc.tCon[i][j]);
                }
                printf("\n");
            }
        }
    }
}

void generateFrame(int varNum){
    vars = malloc(sizeof(fmpz_t) * varNum);
    flint_rand_t state;
    flint_randinit(state);
    fmpz_t two;
    fmpz_init(two);
    fmpz_set_str(two, "2", 10);
    for(int i=0; i<varNum;i++)
    {
        fmpz_init(vars[i]);
        fmpz_randm(vars[i], state, two);
    }

    aFunc.tDegree = malloc(sizeof(int)*aFunc.tNum);
    aFunc.tSymbol = malloc(sizeof(int)*aFunc.tNum);
    aFunc.tCon = malloc(sizeof(int*)*aFunc.tNum);
}

void computeNum(int sNumPerVar){
    for (int i = 0; i < aFunc.tNum; i++) {
        aFunc.tCon[i] = malloc(sizeof(int)*aFunc.tDegree[i]);
        allSTermNum+=pow(sNumPerVar, aFunc.tDegree[i]);
    }
}

//f1=c2 + b1*c1 - b1*c2 - b2*c2 + b2*c3 + b1*b2*c2 - b1*b2*c3
//[b1, b2, c1, c2, c3]
void appF1(hPara *pa){
    pa->varNum = 5;
    pa->d = 3;
    aFunc.tNum = 7;

    generateFrame(pa->varNum);

    aFunc.tSymbol[0] = 1;
    aFunc.tDegree[0] = 1;
    aFunc.tSymbol[1] = 1;
    aFunc.tDegree[1] = 2;
    aFunc.tSymbol[2] = -1;
    aFunc.tDegree[2] = 2;
    aFunc.tSymbol[3] = -1;
    aFunc.tDegree[3] = 2;
    aFunc.tSymbol[4] = 1;
    aFunc.tDegree[4] = 2;
    aFunc.tSymbol[5] = 1;
    aFunc.tDegree[5] = 3;
    aFunc.tSymbol[6] = -1;
    aFunc.tDegree[6] = 3;

    computeNum(pa->sNumPerVar);

    //[1]c2
    aFunc.tCon[0][0] = 3;
    //[2]b1*c1
    aFunc.tCon[1][0] = 0;
    aFunc.tCon[1][1] = 2;
    //[3]- b1*c2
    aFunc.tCon[2][0] = 0;
    aFunc.tCon[2][1] = 3;
    //[4]- b2*c2
    aFunc.tCon[3][0] = 1;
    aFunc.tCon[3][1] = 3;
    //[5] b2*c3
    aFunc.tCon[4][0] = 1;
    aFunc.tCon[4][1] = 4;
    //[6] b1*b2*c2
    aFunc.tCon[5][0] = 0;
    aFunc.tCon[5][1] = 1;
    aFunc.tCon[5][2] = 3;
    //[7] - b1*b2*c3
    aFunc.tCon[6][0] = 0;
    aFunc.tCon[6][1] = 1;
    aFunc.tCon[6][2] = 4;
}

//f2=c3 + b1*c1 - b1*c3 + b2*c2 - b2*c3 - b3*c3 + b3*c4 - b1*b2*c2 + b1*b2*c3 + b1*b3*c3 - b1*b3*c4 + b2*b3*c3 - b2*b3*c4 - b1*b2*b3*c3 + b1*b2*b3*c4
//[b1, b2, b3, c1, c2, c3, c4]
//[0,  1,  2,  3,  4,  5,  6]
void appF2(hPara *pa){
    pa->varNum = 7;
    pa->d = 4;
    aFunc.tNum = 15;

    generateFrame(pa->varNum);

    aFunc.tSymbol[0] = 1;
    aFunc.tDegree[0] = 1;
    aFunc.tSymbol[1] = 1;
    aFunc.tDegree[1] = 2;
    aFunc.tSymbol[2] = -1;
    aFunc.tDegree[2] = 2;
    aFunc.tSymbol[3] = 1;
    aFunc.tDegree[3] = 2;
    aFunc.tSymbol[4] = -1;
    aFunc.tDegree[4] = 2;
    aFunc.tSymbol[5] = -1;
    aFunc.tDegree[5] = 2;
    aFunc.tSymbol[6] = 1;
    aFunc.tDegree[6] = 2;
    aFunc.tSymbol[7] = -1;
    aFunc.tDegree[7] = 3;
    aFunc.tSymbol[8] = 1;
    aFunc.tDegree[8] = 3;
    aFunc.tSymbol[9] = 1;
    aFunc.tDegree[9] = 3;
    aFunc.tSymbol[10] = -1;
    aFunc.tDegree[10] = 3;
    aFunc.tSymbol[11] = 1;
    aFunc.tDegree[11] = 3;
    aFunc.tSymbol[12] = -1;
    aFunc.tDegree[12] = 3;
    aFunc.tSymbol[13] = -1;
    aFunc.tDegree[13] = 4;
    aFunc.tSymbol[14] = 1;
    aFunc.tDegree[14] = 4;

    computeNum(pa->sNumPerVar);

    //[1]c3
    aFunc.tCon[0][0] = 5;
    //[2]b1*c1
    aFunc.tCon[1][0] = 0;
    aFunc.tCon[1][1] = 3;
    //[3]- b1*c3
    aFunc.tCon[2][0] = 0;
    aFunc.tCon[2][1] = 5;
    //[4]b2*c2
    aFunc.tCon[3][0] = 1;
    aFunc.tCon[3][1] = 4;
    //[5]- b2*c3
    aFunc.tCon[4][0] = 1;
    aFunc.tCon[4][1] = 5;
    //[6]- b3*c3
    aFunc.tCon[5][0] = 2;
    aFunc.tCon[5][1] = 5;
    //[7]b3*c4
    aFunc.tCon[6][0] = 2;
    aFunc.tCon[6][1] = 6;
    //[8]- b1*b2*c2
    aFunc.tCon[7][0] = 0;
    aFunc.tCon[7][1] = 1;
    aFunc.tCon[7][2] = 4;
    //[9]b1*b2*c3
    aFunc.tCon[8][0] = 0;
    aFunc.tCon[8][1] = 1;
    aFunc.tCon[8][2] = 5;
    //[10]b1*b3*c3
    aFunc.tCon[9][0] = 0;
    aFunc.tCon[9][1] = 2;
    aFunc.tCon[9][2] = 5;
    //[11]- b1*b3*c4
    aFunc.tCon[10][0] = 0;
    aFunc.tCon[10][1] = 2;
    aFunc.tCon[10][2] = 6;
    //[12]b2*b3*c3
    aFunc.tCon[11][0] = 1;
    aFunc.tCon[11][1] = 2;
    aFunc.tCon[11][2] = 5;
    //[13]- b2*b3*c4
    aFunc.tCon[12][0] = 1;
    aFunc.tCon[12][1] = 2;
    aFunc.tCon[12][2] = 6;
    //[14]- b1*b2*b3*c3
    aFunc.tCon[13][0] = 0;
    aFunc.tCon[13][1] = 1;
    aFunc.tCon[13][2] = 2;
    aFunc.tCon[13][3] = 5;
    //[15]b1*b2*b3*c4
    aFunc.tCon[14][0] = 0;
    aFunc.tCon[14][1] = 1;
    aFunc.tCon[14][2] = 2;
    aFunc.tCon[14][3] = 6;
}
//f3=c3 + b1*c1 - b1*c3 + b2*c2 - b2*c3 - b3*c3 + b3*c4 - b1*b2*c2 + b1*b2*c3 + b1*b3*c3 - b1*b3*c4 + b2*b3*c3 - b2*b3*c4 - b3*b4*c4 + b3*b4*c5 - b1*b2*b3*c3 + b1*b2*b3*c4 + b1*b3*b4*c4 - b1*b3*b4*c5 + b2*b3*b4*c4 - b2*b3*b4*c5 - b1*b2*b3*b4*c4 + b1*b2*b3*b4*c5
//[b1, b2, b3, b4, c1, c2, c3, c4, c5]
//[0,  1,  2,  3,  4,  5,  6,  7,  8]
void appF3(hPara *pa){
    pa->varNum = 9;
    pa->d = 5;
    aFunc.tNum = 23;

    generateFrame(pa->varNum);

    aFunc.tSymbol[0] = 1;
    aFunc.tDegree[0] = 1;
    aFunc.tSymbol[1] = 1;
    aFunc.tDegree[1] = 2;
    aFunc.tSymbol[2] = -1;
    aFunc.tDegree[2] = 2;
    aFunc.tSymbol[3] = 1;
    aFunc.tDegree[3] = 2;
    aFunc.tSymbol[4] = -1;
    aFunc.tDegree[4] = 2;
    aFunc.tSymbol[5] = -1;
    aFunc.tDegree[5] = 2;
    aFunc.tSymbol[6] = 1;
    aFunc.tDegree[6] = 2;
    aFunc.tSymbol[7] = -1;
    aFunc.tDegree[7] = 3;
    aFunc.tSymbol[8] = 1;
    aFunc.tDegree[8] = 3;
    aFunc.tSymbol[9] = 1;
    aFunc.tDegree[9] = 3;
    aFunc.tSymbol[10] = -1;
    aFunc.tDegree[10] = 3;
    aFunc.tSymbol[11] = 1;
    aFunc.tDegree[11] = 3;
    aFunc.tSymbol[12] = -1;
    aFunc.tDegree[12] = 3;
    aFunc.tSymbol[13] = -1;
    aFunc.tDegree[13] = 3;
    aFunc.tSymbol[14] = 1;
    aFunc.tDegree[14] = 3;
    aFunc.tSymbol[15] = -1;
    aFunc.tDegree[15] = 4;
    aFunc.tSymbol[16] = 1;
    aFunc.tDegree[16] = 4;
    aFunc.tSymbol[17] = 1;
    aFunc.tDegree[17] = 4;
    aFunc.tSymbol[18] = -1;
    aFunc.tDegree[18] = 4;
    aFunc.tSymbol[19] = 1;
    aFunc.tDegree[19] = 4;
    aFunc.tSymbol[20] = -1;
    aFunc.tDegree[20] = 4;
    aFunc.tSymbol[21] = -1;
    aFunc.tDegree[21] = 5;
    aFunc.tSymbol[22] = 1;
    aFunc.tDegree[22] = 5;

    computeNum(pa->sNumPerVar);

    //[1]c3
    aFunc.tCon[0][0] = 6;
    //[2]b1*c1
    aFunc.tCon[1][0] = 0;
    aFunc.tCon[1][1] = 4;
    // [3]- b1*c3
    aFunc.tCon[2][0] = 0;
    aFunc.tCon[2][1] = 6;
    // [4]+ b2*c2
    aFunc.tCon[3][0] = 1;
    aFunc.tCon[3][1] = 5;
    // [5]- b2*c3
    aFunc.tCon[4][0] = 1;
    aFunc.tCon[4][1] = 6;
    // [6]- b3*c3
    aFunc.tCon[5][0] = 2;
    aFunc.tCon[5][1] = 6;
    // [7]+ b3*c4
    aFunc.tCon[6][0] = 2;
    aFunc.tCon[6][1] = 7;
    // [8]- b1*b2*c2
    aFunc.tCon[7][0] = 0;
    aFunc.tCon[7][1] = 1;
    aFunc.tCon[7][2] = 5;
    // [9]+ b1*b2*c3
    aFunc.tCon[8][0] = 0;
    aFunc.tCon[8][1] = 1;
    aFunc.tCon[8][2] = 6;
    // [10]+ b1*b3*c3
    aFunc.tCon[9][0] = 0;
    aFunc.tCon[9][1] = 2;
    aFunc.tCon[9][2] = 6;
    // [11]- b1*b3*c4
    aFunc.tCon[10][0] = 0;
    aFunc.tCon[10][1] = 2;
    aFunc.tCon[10][2] = 7;
    // [12]+ b2*b3*c3
    aFunc.tCon[11][0] = 1;
    aFunc.tCon[11][1] = 2;
    aFunc.tCon[11][2] = 6;
    // [13]- b2*b3*c4
    aFunc.tCon[12][0] = 1;
    aFunc.tCon[12][1] = 2;
    aFunc.tCon[12][2] = 7;
    // [14]- b3*b4*c4
    aFunc.tCon[13][0] = 2;
    aFunc.tCon[13][1] = 3;
    aFunc.tCon[13][2] = 7;
    // [15]+ b3*b4*c5
    aFunc.tCon[14][0] = 2;
    aFunc.tCon[14][1] = 3;
    aFunc.tCon[14][2] = 8;
    // [16]- b1*b2*b3*c3
    aFunc.tCon[15][0] = 0;
    aFunc.tCon[15][1] = 1;
    aFunc.tCon[15][2] = 2;
    aFunc.tCon[15][3] = 6;
    // [17]+ b1*b2*b3*c4
    aFunc.tCon[16][0] = 0;
    aFunc.tCon[16][1] = 1;
    aFunc.tCon[16][2] = 2;
    aFunc.tCon[16][3] = 7;
    // [18]+ b1*b3*b4*c4
    aFunc.tCon[17][0] = 0;
    aFunc.tCon[17][1] = 2;
    aFunc.tCon[17][2] = 3;
    aFunc.tCon[17][3] = 7;
    // [19]- b1*b3*b4*c5
    aFunc.tCon[18][0] = 0;
    aFunc.tCon[18][1] = 2;
    aFunc.tCon[18][2] = 3;
    aFunc.tCon[18][3] = 8;
    // [20]+ b2*b3*b4*c4
    aFunc.tCon[19][0] = 1;
    aFunc.tCon[19][1] = 2;
    aFunc.tCon[19][2] = 3;
    aFunc.tCon[19][3] = 7;
    // [21]- b2*b3*b4*c5
    aFunc.tCon[20][0] = 1;
    aFunc.tCon[20][1] = 2;
    aFunc.tCon[20][2] = 3;
    aFunc.tCon[20][3] = 8;
    // [22]- b1*b2*b3*b4*c4
    aFunc.tCon[21][0] = 0;
    aFunc.tCon[21][1] = 1;
    aFunc.tCon[21][2] = 2;
    aFunc.tCon[21][3] = 3;
    aFunc.tCon[21][4] = 7;
    // [23]+ b1*b2*b3*b4*c5
    aFunc.tCon[22][0] = 0;
    aFunc.tCon[22][1] = 1;
    aFunc.tCon[22][2] = 2;
    aFunc.tCon[22][3] = 3;
    aFunc.tCon[22][4] = 8;
}
//f4=c4 + b1*c1 + b2*c2 - b1*c4 - b2*c4 + b3*c3 - b3*c4 - b4*c4 + b4*c5 - b1*b2*c2 + b1*b2*c4 - b1*b3*c3 + b1*b3*c4 - b2*b3*c3 + b1*b4*c4 + b2*b3*c4 - b1*b4*c5 + b2*b4*c4 - b2*b4*c5 + b3*b4*c4 - b3*b4*c5 - b4*b5*c5 + b4*b5*c6 + b1*b2*b3*c3 - b1*b2*b3*c4 - b1*b2*b4*c4 + b1*b2*b4*c5 - b1*b3*b4*c4 + b1*b3*b4*c5 - b2*b3*b4*c4 + b2*b3*b4*c5 + b1*b4*b5*c5 - b1*b4*b5*c6 + b2*b4*b5*c5 - b2*b4*b5*c6 + b3*b4*b5*c5 - b3*b4*b5*c6 + b1*b2*b3*b4*c4 - b1*b2*b3*b4*c5 - b1*b2*b4*b5*c5 + b1*b2*b4*b5*c6 - b1*b3*b4*b5*c5 + b1*b3*b4*b5*c6 - b2*b3*b4*b5*c5 + b2*b3*b4*b5*c6 + b1*b2*b3*b4*b5*c5 - b1*b2*b3*b4*b5*c6
//[b1, b2, b3, b4, b5, c1, c2, c3, c4, c5, c6]
//[0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10]
void appF4(hPara *pa){
    pa->varNum = 11;
    pa->d = 6;
    aFunc.tNum = 47;

    generateFrame(pa->varNum);
    aFunc.tSymbol[0] = 1;
    aFunc.tDegree[0] = 1;
    aFunc.tSymbol[1] = 1;
    aFunc.tDegree[1] = 2;
    aFunc.tSymbol[2] = 1;
    aFunc.tDegree[2] = 2;
    aFunc.tSymbol[3] = -1;
    aFunc.tDegree[3] = 2;
    aFunc.tSymbol[4] = -1;
    aFunc.tDegree[4] = 2;
    aFunc.tSymbol[5] = 1;
    aFunc.tDegree[5] = 2;
    aFunc.tSymbol[6] = -1;
    aFunc.tDegree[6] = 2;
    aFunc.tSymbol[7] = -1;
    aFunc.tDegree[7] = 2;
    aFunc.tSymbol[8] = 1;
    aFunc.tDegree[8] = 2;
    aFunc.tSymbol[9] = -1;
    aFunc.tDegree[9] = 3;
    aFunc.tSymbol[10] = 1;
    aFunc.tDegree[10] = 3;
    aFunc.tSymbol[11] = -1;
    aFunc.tDegree[11] = 3;
    aFunc.tSymbol[12] = 1;
    aFunc.tDegree[12] = 3;
    aFunc.tSymbol[13] = -1;
    aFunc.tDegree[13] = 3;
    aFunc.tSymbol[14] = 1;
    aFunc.tDegree[14] = 3;
    aFunc.tSymbol[15] = 1;
    aFunc.tDegree[15] = 3;
    aFunc.tSymbol[16] = -1;
    aFunc.tDegree[16] = 3;
    aFunc.tSymbol[17] = 1;
    aFunc.tDegree[17] = 3;
    aFunc.tSymbol[18] = -1;
    aFunc.tDegree[18] = 3;
    aFunc.tSymbol[19] = 1;
    aFunc.tDegree[19] = 3;
    aFunc.tSymbol[20] = -1;
    aFunc.tDegree[20] = 3;
    aFunc.tSymbol[21] = -1;
    aFunc.tDegree[21] = 3;
    aFunc.tSymbol[22] = 1;
    aFunc.tDegree[22] = 3;
    aFunc.tSymbol[23] = 1;
    aFunc.tDegree[23] = 4;
    aFunc.tSymbol[24] = -1;
    aFunc.tDegree[24] = 4;
    aFunc.tSymbol[25] = -1;
    aFunc.tDegree[25] = 4;
    aFunc.tSymbol[26] = 1;
    aFunc.tDegree[26] = 4;
    aFunc.tSymbol[27] = -1;
    aFunc.tDegree[27] = 4;
    aFunc.tSymbol[28] = 1;
    aFunc.tDegree[28] = 4;
    aFunc.tSymbol[29] = -1;
    aFunc.tDegree[29] = 4;
    aFunc.tSymbol[30] = 1;
    aFunc.tDegree[30] = 4;
    aFunc.tSymbol[31] = 1;
    aFunc.tDegree[31] = 4;
    aFunc.tSymbol[32] = -1;
    aFunc.tDegree[32] = 4;
    aFunc.tSymbol[33] = 1;
    aFunc.tDegree[33] = 4;
    aFunc.tSymbol[34] = -1;
    aFunc.tDegree[34] = 4;
    aFunc.tSymbol[35] = 1;
    aFunc.tDegree[35] = 4;
    aFunc.tSymbol[36] = -1;
    aFunc.tDegree[36] = 4;
    aFunc.tSymbol[37] = 1;
    aFunc.tDegree[37] = 5;
    aFunc.tSymbol[38] = -1;
    aFunc.tDegree[38] = 5;
    aFunc.tSymbol[39] = -1;
    aFunc.tDegree[39] = 5;
    aFunc.tSymbol[40] = 1;
    aFunc.tDegree[40] = 5;
    aFunc.tSymbol[41] = -1;
    aFunc.tDegree[41] = 5;
    aFunc.tSymbol[42] = 1;
    aFunc.tDegree[42] = 5;
    aFunc.tSymbol[43] = -1;
    aFunc.tDegree[43] = 5;
    aFunc.tSymbol[44] = 1;
    aFunc.tDegree[44] = 5;
    aFunc.tSymbol[45] = 1;
    aFunc.tDegree[45] = 6;
    aFunc.tSymbol[46] = -1;
    aFunc.tDegree[46] = 6;
    computeNum(pa->sNumPerVar);

    //[1]c4
    aFunc.tCon[0][0] = 8;
    // [2]+ b1*c1
    aFunc.tCon[1][0] = 0;
    aFunc.tCon[1][1] = 5;
    // [3]+ b2*c2
    aFunc.tCon[2][0] = 1;
    aFunc.tCon[2][1] = 6;
    // [4]- b1*c4
    aFunc.tCon[3][0] = 0;
    aFunc.tCon[3][1] = 8;
    // [5]- b2*c4
    aFunc.tCon[4][0] = 1;
    aFunc.tCon[4][1] = 8;
    // [6]+ b3*c3
    aFunc.tCon[5][0] = 2;
    aFunc.tCon[5][1] = 7;
    // [7]- b3*c4
    aFunc.tCon[6][0] = 2;
    aFunc.tCon[6][1] = 8;
    // [8]- b4*c4
    aFunc.tCon[7][0] = 3;
    aFunc.tCon[7][1] = 8;
    // [9]+ b4*c5
    aFunc.tCon[8][0] = 3;
    aFunc.tCon[8][1] = 9;
    // [10]- b1*b2*c2
    aFunc.tCon[9][0] = 0;
    aFunc.tCon[9][1] = 1;
    aFunc.tCon[9][2] = 6;
    // [11]+ b1*b2*c4
    aFunc.tCon[10][0] = 0;
    aFunc.tCon[10][1] = 1;
    aFunc.tCon[10][2] = 8;
    // [12]- b1*b3*c3
    aFunc.tCon[11][0] = 0;
    aFunc.tCon[11][1] = 2;
    aFunc.tCon[11][2] = 7;
    // [13]+ b1*b3*c4
    aFunc.tCon[12][0] = 0;
    aFunc.tCon[12][1] = 2;
    aFunc.tCon[12][2] = 8;
    // [14]- b2*b3*c3
    aFunc.tCon[13][0] = 1;
    aFunc.tCon[13][1] = 2;
    aFunc.tCon[13][2] = 7;
    // [15]+ b1*b4*c4
    aFunc.tCon[14][0] = 0;
    aFunc.tCon[14][1] = 3;
    aFunc.tCon[14][2] = 8;
    // [16]+ b2*b3*c4
    aFunc.tCon[15][0] = 1;
    aFunc.tCon[15][1] = 2;
    aFunc.tCon[15][2] = 8;
    // [17]- b1*b4*c5
    aFunc.tCon[16][0] = 0;
    aFunc.tCon[16][1] = 3;
    aFunc.tCon[16][2] = 9;
    // [18]+ b2*b4*c4
    aFunc.tCon[17][0] = 1;
    aFunc.tCon[17][1] = 3;
    aFunc.tCon[17][2] = 8;
    // [19]- b2*b4*c5
    aFunc.tCon[18][0] = 1;
    aFunc.tCon[18][1] = 3;
    aFunc.tCon[18][2] = 9;
    // [20]+ b3*b4*c4
    aFunc.tCon[19][0] = 2;
    aFunc.tCon[19][1] = 3;
    aFunc.tCon[19][2] = 8;
    // [21]- b3*b4*c5
    aFunc.tCon[20][0] = 2;
    aFunc.tCon[20][1] = 3;
    aFunc.tCon[20][2] = 9;
    // [22]- b4*b5*c5
    aFunc.tCon[21][0] = 3;
    aFunc.tCon[21][1] = 4;
    aFunc.tCon[21][2] = 9;
    // [23]+ b4*b5*c6
    aFunc.tCon[22][0] = 3;
    aFunc.tCon[22][1] = 4;
    aFunc.tCon[22][2] = 10;
    // [24]+ b1*b2*b3*c3
    aFunc.tCon[23][0] = 0;
    aFunc.tCon[23][1] = 1;
    aFunc.tCon[23][2] = 2;
    aFunc.tCon[23][3] = 7;
    // [25]- b1*b2*b3*c4
    aFunc.tCon[24][0] = 0;
    aFunc.tCon[24][1] = 1;
    aFunc.tCon[24][2] = 2;
    aFunc.tCon[24][3] = 8;
    // [26]- b1*b2*b4*c4
    aFunc.tCon[25][0] = 0;
    aFunc.tCon[25][1] = 1;
    aFunc.tCon[25][2] = 3;
    aFunc.tCon[25][3] = 8;
    // [27]+ b1*b2*b4*c5
    aFunc.tCon[26][0] = 0;
    aFunc.tCon[26][1] = 1;
    aFunc.tCon[26][2] = 3;
    aFunc.tCon[26][3] = 9;
    // [28]- b1*b3*b4*c4
    aFunc.tCon[27][0] = 0;
    aFunc.tCon[27][1] = 2;
    aFunc.tCon[27][2] = 3;
    aFunc.tCon[27][3] = 8;
    // [29]+ b1*b3*b4*c5
    aFunc.tCon[28][0] = 0;
    aFunc.tCon[28][1] = 2;
    aFunc.tCon[28][2] = 3;
    aFunc.tCon[28][3] = 9;
    // [30]- b2*b3*b4*c4
    aFunc.tCon[29][0] = 1;
    aFunc.tCon[29][1] = 2;
    aFunc.tCon[29][2] = 3;
    aFunc.tCon[29][3] = 8;
    // [31]+ b2*b3*b4*c5
    aFunc.tCon[30][0] = 1;
    aFunc.tCon[30][1] = 2;
    aFunc.tCon[30][2] = 3;
    aFunc.tCon[30][3] = 9;
    // [32]+ b1*b4*b5*c5
    aFunc.tCon[31][0] = 0;
    aFunc.tCon[31][1] = 3;
    aFunc.tCon[31][2] = 4;
    aFunc.tCon[31][3] = 9;
    // [33]- b1*b4*b5*c6
    aFunc.tCon[32][0] = 0;
    aFunc.tCon[32][1] = 3;
    aFunc.tCon[32][2] = 4;
    aFunc.tCon[32][3] = 10;
    // [34]+ b2*b4*b5*c5
    aFunc.tCon[33][0] = 1;
    aFunc.tCon[33][1] = 3;
    aFunc.tCon[33][2] = 4;
    aFunc.tCon[33][3] = 9;
    // [35]- b2*b4*b5*c6
    aFunc.tCon[34][0] = 1;
    aFunc.tCon[34][1] = 3;
    aFunc.tCon[34][2] = 4;
    aFunc.tCon[34][3] = 10;
    // [36]+ b3*b4*b5*c5
    aFunc.tCon[35][0] = 2;
    aFunc.tCon[35][1] = 3;
    aFunc.tCon[35][2] = 4;
    aFunc.tCon[35][3] = 9;
    // [37]- b3*b4*b5*c6
    aFunc.tCon[36][0] = 2;
    aFunc.tCon[36][1] = 3;
    aFunc.tCon[36][2] = 4;
    aFunc.tCon[36][3] = 10;
    // [38]+ b1*b2*b3*b4*c4
    aFunc.tCon[37][0] = 0;
    aFunc.tCon[37][1] = 1;
    aFunc.tCon[37][2] = 2;
    aFunc.tCon[37][3] = 3;
    aFunc.tCon[37][4] = 8;
    // [39]- b1*b2*b3*b4*c5
    aFunc.tCon[38][0] = 0;
    aFunc.tCon[38][1] = 1;
    aFunc.tCon[38][2] = 2;
    aFunc.tCon[38][3] = 3;
    aFunc.tCon[38][4] = 9;
    // [40]- b1*b2*b4*b5*c5
    aFunc.tCon[39][0] = 0;
    aFunc.tCon[39][1] = 1;
    aFunc.tCon[39][2] = 3;
    aFunc.tCon[39][3] = 4;
    aFunc.tCon[39][4] = 9;
    // [41]+ b1*b2*b4*b5*c6
    aFunc.tCon[40][0] = 0;
    aFunc.tCon[40][1] = 1;
    aFunc.tCon[40][2] = 3;
    aFunc.tCon[40][3] = 4;
    aFunc.tCon[40][4] = 10;
    // [42]- b1*b3*b4*b5*c5
    aFunc.tCon[41][0] = 0;
    aFunc.tCon[41][1] = 2;
    aFunc.tCon[41][2] = 3;
    aFunc.tCon[41][3] = 4;
    aFunc.tCon[41][4] = 9;
    // [43]+ b1*b3*b4*b5*c6
    aFunc.tCon[42][0] = 0;
    aFunc.tCon[42][1] = 2;
    aFunc.tCon[42][2] = 3;
    aFunc.tCon[42][3] = 4;
    aFunc.tCon[42][4] = 10;
    // [44]- b2*b3*b4*b5*c5
    aFunc.tCon[43][0] = 1;
    aFunc.tCon[43][1] = 2;
    aFunc.tCon[43][2] = 3;
    aFunc.tCon[43][3] = 4;
    aFunc.tCon[43][4] = 9;
    // [45]+ b2*b3*b4*b5*c6
    aFunc.tCon[44][0] = 1;
    aFunc.tCon[44][1] = 2;
    aFunc.tCon[44][2] = 3;
    aFunc.tCon[44][3] = 4;
    aFunc.tCon[44][4] = 10;
    // [46]+ b1*b2*b3*b4*b5*c5
    aFunc.tCon[45][0] = 0;
    aFunc.tCon[45][1] = 1;
    aFunc.tCon[45][2] = 2;
    aFunc.tCon[45][3] = 3;
    aFunc.tCon[45][4] = 4;
    aFunc.tCon[45][5] = 9;
    // [47]- b1*b2*b3*b4*b5*c6
    aFunc.tCon[46][0] = 0;
    aFunc.tCon[46][1] = 1;
    aFunc.tCon[46][2] = 2;
    aFunc.tCon[46][3] = 3;
    aFunc.tCon[46][4] = 4;
    aFunc.tCon[46][5] = 10;
}

//f5=c1 - b1*c1 - b2*c1 + b1*c3 + b2*c2 + b1*b2*c1 - b1*b2*c2 - b1*b3*c3 + b1*b3*c4
//[b1, b2, b3, c1, c2, c3, c4]
//[0,  1,  2,  3,  4,  5,  6]
void appF5(hPara *pa){
    pa->varNum = 7;
    pa->d = 3;
    aFunc.tNum = 9;

    generateFrame(pa->varNum);
    aFunc.tSymbol[0] = 1;
    aFunc.tDegree[0] = 1;
    aFunc.tSymbol[1] = -1;
    aFunc.tDegree[1] = 2;
    aFunc.tSymbol[2] = -1;
    aFunc.tDegree[2] = 2;
    aFunc.tSymbol[3] = 1;
    aFunc.tDegree[3] = 2;
    aFunc.tSymbol[4] = 1;
    aFunc.tDegree[4] = 2;
    aFunc.tSymbol[5] = 1;
    aFunc.tDegree[5] = 3;
    aFunc.tSymbol[6] = -1;
    aFunc.tDegree[6] = 3;
    aFunc.tSymbol[7] = -1;
    aFunc.tDegree[7] = 3;
    aFunc.tSymbol[8] = 1;
    aFunc.tDegree[8] = 3;
    computeNum(pa->sNumPerVar);

    //[1]c1
    aFunc.tCon[0][0] = 3;
    // [2]- b1*c1
    aFunc.tCon[1][0] = 0;
    aFunc.tCon[1][1] = 3;
    // [3]- b2*c1
    aFunc.tCon[2][0] = 1;
    aFunc.tCon[2][1] = 3;
    // [4]+ b1*c3
    aFunc.tCon[3][0] = 0;
    aFunc.tCon[3][1] = 5;
    // [5]+ b2*c2
    aFunc.tCon[4][0] = 1;
    aFunc.tCon[4][1] = 4;
    // [6]+ b1*b2*c1
    aFunc.tCon[5][0] = 0;
    aFunc.tCon[5][1] = 1;
    aFunc.tCon[5][2] = 3;
    // [7]- b1*b2*c2
    aFunc.tCon[6][0] = 0;
    aFunc.tCon[6][1] = 1;
    aFunc.tCon[6][2] = 4;
    // [8]- b1*b3*c3
    aFunc.tCon[7][0] = 0;
    aFunc.tCon[7][1] = 2;
    aFunc.tCon[7][2] = 5;
    // [9]+ b1*b3*c4
    aFunc.tCon[8][0] = 0;
    aFunc.tCon[8][1] = 2;
    aFunc.tCon[8][2] = 6;
}

//f6=c1 - b1*c1 - b2*c1 + b2*c3 - b4*c1 + b1*c5 + b4*c2 + b1*b2*c1 - b1*b2*c3 + b1*b4*c1 - b1*b4*c2 + b2*b4*c1 - b2*b4*c2 - b1*b3*c5 - b2*b5*c3 + b1*b3*c7 + b2*b5*c4 - b1*b6*c5 + b1*b6*c6 - b1*b2*b4*c1 + b1*b2*b4*c2 + b1*b2*b5*c3 - b1*b2*b5*c4 + b1*b3*b6*c5 - b1*b3*b6*c6 - b1*b3*b7*c7 + b1*b3*b7*c8
//[b1, b2, b3, b4, b5, b6, b7, c1, c2, c3, c4, c5, c6, c7, c8]
//[0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14]
void appF6(hPara *pa){
    pa->varNum = 15;
    pa->d = 4;
    aFunc.tNum = 27;

    generateFrame(pa->varNum);

    aFunc.tSymbol[0] = 1;
    aFunc.tDegree[0] = 1;
    aFunc.tSymbol[1] = -1;
    aFunc.tDegree[1] = 2;
    aFunc.tSymbol[2] = -1;
    aFunc.tDegree[2] = 2;
    aFunc.tSymbol[3] = 1;
    aFunc.tDegree[3] = 2;
    aFunc.tSymbol[4] = -1;
    aFunc.tDegree[4] = 2;
    aFunc.tSymbol[5] = 1;
    aFunc.tDegree[5] = 2;
    aFunc.tSymbol[6] = 1;
    aFunc.tDegree[6] = 2;
    aFunc.tSymbol[7] = 1;
    aFunc.tDegree[7] = 3;
    aFunc.tSymbol[8] = -1;
    aFunc.tDegree[8] = 3;
    aFunc.tSymbol[9] = 1;
    aFunc.tDegree[9] = 3;
    aFunc.tSymbol[10] = -1;
    aFunc.tDegree[10] = 3;
    aFunc.tSymbol[11] = 1;
    aFunc.tDegree[11] = 3;
    aFunc.tSymbol[12] = -1;
    aFunc.tDegree[12] = 3;
    aFunc.tSymbol[13] = -1;
    aFunc.tDegree[13] = 3;
    aFunc.tSymbol[14] = -1;
    aFunc.tDegree[14] = 3;
    aFunc.tSymbol[15] = 1;
    aFunc.tDegree[15] = 3;
    aFunc.tSymbol[16] = 1;
    aFunc.tDegree[16] = 3;
    aFunc.tSymbol[17] = -1;
    aFunc.tDegree[17] = 3;
    aFunc.tSymbol[18] = 1;
    aFunc.tDegree[18] = 3;
    aFunc.tSymbol[19] = -1;
    aFunc.tDegree[19] = 4;
    aFunc.tSymbol[20] = 1;
    aFunc.tDegree[20] = 4;
    aFunc.tSymbol[21] = 1;
    aFunc.tDegree[21] = 4;
    aFunc.tSymbol[22] = -1;
    aFunc.tDegree[22] = 4;
    aFunc.tSymbol[23] = 1;
    aFunc.tDegree[23] = 4;
    aFunc.tSymbol[24] = -1;
    aFunc.tDegree[24] = 4;
    aFunc.tSymbol[25] = -1;
    aFunc.tDegree[25] = 4;
    aFunc.tSymbol[26] = 1;
    aFunc.tDegree[26] = 4;

    computeNum(pa->sNumPerVar);

    //[1]c1
    aFunc.tCon[0][0] = 7;
    // [2]- b1*c1
    aFunc.tCon[1][0] = 0;
    aFunc.tCon[1][1] = 7;
    // [3]- b2*c1
    aFunc.tCon[2][0] = 1;
    aFunc.tCon[2][1] = 7;
    // [4]+ b2*c3
    aFunc.tCon[3][0] = 1;
    aFunc.tCon[3][1] = 9;
    // [5]- b4*c1
    aFunc.tCon[4][0] = 3;
    aFunc.tCon[4][1] = 7;
    // [6]+ b1*c5
    aFunc.tCon[5][0] = 0;
    aFunc.tCon[5][1] = 11;
    // [7]+ b4*c2
    aFunc.tCon[6][0] = 3;
    aFunc.tCon[6][1] = 8;
    // [8]+ b1*b2*c1
    aFunc.tCon[7][0] = 0;
    aFunc.tCon[7][1] = 1;
    aFunc.tCon[7][2] = 7;
    // [9]- b1*b2*c3
    aFunc.tCon[8][0] = 0;
    aFunc.tCon[8][1] = 1;
    aFunc.tCon[8][2] = 9;
    // [10]+ b1*b4*c1
    aFunc.tCon[9][0] = 0;
    aFunc.tCon[9][1] = 3;
    aFunc.tCon[9][2] = 7;
    // [11]- b1*b4*c2
    aFunc.tCon[10][0] = 0;
    aFunc.tCon[10][1] = 3;
    aFunc.tCon[10][2] = 8;
    // [12]+ b2*b4*c1
    aFunc.tCon[11][0] = 1;
    aFunc.tCon[11][1] = 3;
    aFunc.tCon[11][2] = 7;
    // [13]- b2*b4*c2
    aFunc.tCon[12][0] = 1;
    aFunc.tCon[12][1] = 3;
    aFunc.tCon[12][2] = 8;
    // [14]- b1*b3*c5
    aFunc.tCon[13][0] = 0;
    aFunc.tCon[13][1] = 2;
    aFunc.tCon[13][2] = 11;
    // [15]- b2*b5*c3
    aFunc.tCon[14][0] = 1;
    aFunc.tCon[14][1] = 4;
    aFunc.tCon[14][2] = 9;
    // [16]+ b1*b3*c7
    aFunc.tCon[15][0] = 0;
    aFunc.tCon[15][1] = 2;
    aFunc.tCon[15][2] = 13;
    // [17]+ b2*b5*c4
    aFunc.tCon[16][0] = 1;
    aFunc.tCon[16][1] = 4;
    aFunc.tCon[16][2] = 10;
    // [18]- b1*b6*c5
    aFunc.tCon[17][0] = 0;
    aFunc.tCon[17][1] = 5;
    aFunc.tCon[17][2] = 11;
    // [19]+ b1*b6*c6
    aFunc.tCon[18][0] = 0;
    aFunc.tCon[18][1] = 5;
    aFunc.tCon[18][2] = 12;
    // [20]- b1*b2*b4*c1
    aFunc.tCon[19][0] = 0;
    aFunc.tCon[19][1] = 1;
    aFunc.tCon[19][2] = 3;
    aFunc.tCon[19][3] = 7;
    // [21]+ b1*b2*b4*c2
    aFunc.tCon[20][0] = 0;
    aFunc.tCon[20][1] = 1;
    aFunc.tCon[20][2] = 3;
    aFunc.tCon[20][3] = 8;
    // [22]+ b1*b2*b5*c3
    aFunc.tCon[21][0] = 0;
    aFunc.tCon[21][1] = 1;
    aFunc.tCon[21][2] = 4;
    aFunc.tCon[21][3] = 9;
    // [23]- b1*b2*b5*c4
    aFunc.tCon[22][0] = 0;
    aFunc.tCon[22][1] = 1;
    aFunc.tCon[22][2] = 4;
    aFunc.tCon[22][3] = 10;
    // [24]+ b1*b3*b6*c5
    aFunc.tCon[23][0] = 0;
    aFunc.tCon[23][1] = 2;
    aFunc.tCon[23][2] = 5;
    aFunc.tCon[23][3] = 11;
    // [25]- b1*b3*b6*c6
    aFunc.tCon[24][0] = 0;
    aFunc.tCon[24][1] = 2;
    aFunc.tCon[24][2] = 5;
    aFunc.tCon[24][3] = 12;
    // [26]- b1*b3*b7*c7
    aFunc.tCon[25][0] = 0;
    aFunc.tCon[25][1] = 2;
    aFunc.tCon[25][2] = 6;
    aFunc.tCon[25][3] = 13;
    // [27]+ b1*b3*b7*c8
    aFunc.tCon[26][0] = 0;
    aFunc.tCon[26][1] = 2;
    aFunc.tCon[26][2] = 6;
    aFunc.tCon[26][3] = 14;
}
//f7=c4 + b2*c1 - b1*c4 - b2*c4 + b4*c2 + b1*c6 - b4*c4 - b7*c4 + b7*c5 - b1*b2*c1 + b1*b2*c4 - b1*b4*c2 - b2*b4*c2 + b1*b4*c4 - b1*b3*c6 + b2*b4*c4 - b1*b5*c6 + b1*b7*c4 - b1*b7*c5 + b2*b7*c4 + b1*b3*c10 + b1*b5*c8 - b2*b7*c5 - b1*b8*c6 + b4*b7*c4 + b1*b8*c7 - b4*b7*c5 + b1*b2*b4*c2 - b1*b2*b4*c4 + b1*b3*b6*c3 - b1*b2*b7*c4 + b1*b2*b7*c5 + b1*b3*b5*c6 - b1*b4*b7*c4 - b1*b3*b5*c8 + b1*b4*b7*c5 - b2*b4*b7*c4 + b1*b3*b8*c6 + b2*b4*b7*c5 - b1*b3*b8*c7 - b1*b3*b6*c10 + b1*b5*b8*c6 - b1*b5*b8*c7 - b1*b5*b9*c8 - b1*b3*b10*c10 + b1*b5*b9*c9 + b1*b3*b10*c11 + b1*b2*b4*b7*c4 - b1*b2*b4*b7*c5 - b1*b3*b5*b8*c6 + b1*b3*b5*b8*c7 + b1*b3*b5*b9*c8 - b1*b3*b5*b9*c9 + b1*b3*b6*b10*c10 - b1*b3*b6*b10*c11
//[b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11]
//[0,  1,  2,  3,  4,  5,  6,  7,  8,  9,   10, 11, 12, 13, 14, 15, 16, 17, 18, 19,  20]
void appF7(hPara *pa){
    pa->varNum = 21;
    pa->d = 5;
    aFunc.tNum = 55;

    generateFrame(pa->varNum);
    aFunc.tSymbol[0] = 1;
    aFunc.tDegree[0] = 1;
    aFunc.tSymbol[1] = 1;
    aFunc.tDegree[1] = 2;
    aFunc.tSymbol[2] = -1;
    aFunc.tDegree[2] = 2;
    aFunc.tSymbol[3] = -1;
    aFunc.tDegree[3] = 2;
    aFunc.tSymbol[4] = 1;
    aFunc.tDegree[4] = 2;
    aFunc.tSymbol[5] = 1;
    aFunc.tDegree[5] = 2;
    aFunc.tSymbol[6] = -1;
    aFunc.tDegree[6] = 2;
    aFunc.tSymbol[7] = -1;
    aFunc.tDegree[7] = 2;
    aFunc.tSymbol[8] = 1;
    aFunc.tDegree[8] = 2;
    aFunc.tSymbol[9] = -1;
    aFunc.tDegree[9] = 3;
    aFunc.tSymbol[10] = 1;
    aFunc.tDegree[10] = 3;
    aFunc.tSymbol[11] = -1;
    aFunc.tDegree[11] = 3;
    aFunc.tSymbol[12] = -1;
    aFunc.tDegree[12] = 3;
    aFunc.tSymbol[13] = 1;
    aFunc.tDegree[13] = 3;
    aFunc.tSymbol[14] = -1;
    aFunc.tDegree[14] = 3;
    aFunc.tSymbol[15] = 1;
    aFunc.tDegree[15] = 3;
    aFunc.tSymbol[16] = -1;
    aFunc.tDegree[16] = 3;
    aFunc.tSymbol[17] = 1;
    aFunc.tDegree[17] = 3;
    aFunc.tSymbol[18] = -1;
    aFunc.tDegree[18] = 3;
    aFunc.tSymbol[19] = 1;
    aFunc.tDegree[19] = 3;
    aFunc.tSymbol[20] = 1;
    aFunc.tDegree[20] = 3;
    aFunc.tSymbol[21] = 1;
    aFunc.tDegree[21] = 3;
    aFunc.tSymbol[22] = -1;
    aFunc.tDegree[22] = 3;
    aFunc.tSymbol[23] = -1;
    aFunc.tDegree[23] = 3;
    aFunc.tSymbol[24] = 1;
    aFunc.tDegree[24] = 3;
    aFunc.tSymbol[25] = 1;
    aFunc.tDegree[25] = 3;
    aFunc.tSymbol[26] = -1;
    aFunc.tDegree[26] = 3;
    aFunc.tSymbol[27] = 1;
    aFunc.tDegree[27] = 4;
    aFunc.tSymbol[28] = -1;
    aFunc.tDegree[28] = 4;
    aFunc.tSymbol[29] = 1;
    aFunc.tDegree[29] = 4;
    aFunc.tSymbol[30] = -1;
    aFunc.tDegree[30] = 4;
    aFunc.tSymbol[31] = 1;
    aFunc.tDegree[31] = 4;
    aFunc.tSymbol[32] = 1;
    aFunc.tDegree[32] = 4;
    aFunc.tSymbol[33] = -1;
    aFunc.tDegree[33] = 4;
    aFunc.tSymbol[34] = -1;
    aFunc.tDegree[34] = 4;
    aFunc.tSymbol[35] = 1;
    aFunc.tDegree[35] = 4;
    aFunc.tSymbol[36] = -1;
    aFunc.tDegree[36] = 4;
    aFunc.tSymbol[37] = 1;
    aFunc.tDegree[37] = 4;
    aFunc.tSymbol[38] = 1;
    aFunc.tDegree[38] = 4;
    aFunc.tSymbol[39] = -1;
    aFunc.tDegree[39] = 4;
    aFunc.tSymbol[40] = -1;
    aFunc.tDegree[40] = 4;
    aFunc.tSymbol[41] = 1;
    aFunc.tDegree[41] = 4;
    aFunc.tSymbol[42] = -1;
    aFunc.tDegree[42] = 4;
    aFunc.tSymbol[43] = -1;
    aFunc.tDegree[43] = 4;
    aFunc.tSymbol[44] = -1;
    aFunc.tDegree[44] = 4;
    aFunc.tSymbol[45] = 1;
    aFunc.tDegree[45] = 4;
    aFunc.tSymbol[46] = 1;
    aFunc.tDegree[46] = 4;
    aFunc.tSymbol[47] = 1;
    aFunc.tDegree[47] = 5;
    aFunc.tSymbol[48] = -1;
    aFunc.tDegree[48] = 5;
    aFunc.tSymbol[49] = -1;
    aFunc.tDegree[49] = 5;
    aFunc.tSymbol[50] = 1;
    aFunc.tDegree[50] = 5;
    aFunc.tSymbol[51] = 1;
    aFunc.tDegree[51] = 5;
    aFunc.tSymbol[52] = -1;
    aFunc.tDegree[52] = 5;
    aFunc.tSymbol[53] = 1;
    aFunc.tDegree[53] = 5;
    aFunc.tSymbol[54] = -1;
    aFunc.tDegree[54] = 5;
    computeNum(pa->sNumPerVar);

    //[1]c4
    aFunc.tCon[0][0] = 13;
    // [2]+ b2*c1
    aFunc.tCon[1][0] = 1;
    aFunc.tCon[1][1] = 10;
    // [3]- b1*c4
    aFunc.tCon[2][0] = 0;
    aFunc.tCon[2][1] = 13;
    // [4]- b2*c4
    aFunc.tCon[3][0] = 1;
    aFunc.tCon[3][1] = 13;
    // [5]+ b4*c2
    aFunc.tCon[4][0] = 3;
    aFunc.tCon[4][1] = 11;
    // [6]+ b1*c6
    aFunc.tCon[5][0] = 0;
    aFunc.tCon[5][1] = 15;
    // [7]- b4*c4
    aFunc.tCon[6][0] = 3;
    aFunc.tCon[6][1] = 13;
    // [8]- b7*c4
    aFunc.tCon[7][0] = 6;
    aFunc.tCon[7][1] = 13;
    // [9]+ b7*c5
    aFunc.tCon[8][0] = 6;
    aFunc.tCon[8][1] = 14;
    // [10]- b1*b2*c1
    aFunc.tCon[9][0] = 0;
    aFunc.tCon[9][1] = 1;
    aFunc.tCon[9][2] = 10;
    // [11]+ b1*b2*c4
    aFunc.tCon[10][0] = 0;
    aFunc.tCon[10][1] = 1;
    aFunc.tCon[10][2] = 13;
    // [12]- b1*b4*c2
    aFunc.tCon[11][0] = 0;
    aFunc.tCon[11][1] = 3;
    aFunc.tCon[11][2] = 11;
    // [13]- b2*b4*c2
    aFunc.tCon[12][0] = 1;
    aFunc.tCon[12][1] = 3;
    aFunc.tCon[12][2] = 11;
    // [14]+ b1*b4*c4
    aFunc.tCon[13][0] = 0;
    aFunc.tCon[13][1] = 3;
    aFunc.tCon[13][2] = 13;
    // [15]- b1*b3*c6
    aFunc.tCon[14][0] = 0;
    aFunc.tCon[14][1] = 2;
    aFunc.tCon[14][2] = 15;
    // [16]+ b2*b4*c4
    aFunc.tCon[15][0] = 1;
    aFunc.tCon[15][1] = 3;
    aFunc.tCon[15][2] = 13;
    // [17]- b1*b5*c6
    aFunc.tCon[16][0] = 0;
    aFunc.tCon[16][1] = 4;
    aFunc.tCon[16][2] = 15;
    // [18]+ b1*b7*c4
    aFunc.tCon[17][0] = 0;
    aFunc.tCon[17][1] = 6;
    aFunc.tCon[17][2] = 13;
    // [19]- b1*b7*c5
    aFunc.tCon[18][0] = 0;
    aFunc.tCon[18][1] = 6;
    aFunc.tCon[18][2] = 14;
    // [20]+ b2*b7*c4
    aFunc.tCon[19][0] = 1;
    aFunc.tCon[19][1] = 6;
    aFunc.tCon[19][2] = 13;
    // [21]+ b1*b3*c10
    aFunc.tCon[20][0] = 0;
    aFunc.tCon[20][1] = 2;
    aFunc.tCon[20][2] = 19;
    // [22]+ b1*b5*c8
    aFunc.tCon[21][0] = 0;
    aFunc.tCon[21][1] = 4;
    aFunc.tCon[21][2] = 17;
    // [23]- b2*b7*c5
    aFunc.tCon[22][0] = 1;
    aFunc.tCon[22][1] = 6;
    aFunc.tCon[22][2] = 14;
    // [24]- b1*b8*c6
    aFunc.tCon[23][0] = 0;
    aFunc.tCon[23][1] = 7;
    aFunc.tCon[23][2] = 15;
    // [25]+ b4*b7*c4
    aFunc.tCon[24][0] = 3;
    aFunc.tCon[24][1] = 6;
    aFunc.tCon[24][2] = 13;
    // [26]+ b1*b8*c7
    aFunc.tCon[25][0] = 0;
    aFunc.tCon[25][1] = 7;
    aFunc.tCon[25][2] = 16;
    // [27]- b4*b7*c5
    aFunc.tCon[26][0] = 3;
    aFunc.tCon[26][1] = 6;
    aFunc.tCon[26][2] = 14;
    // [28]+ b1*b2*b4*c2
    aFunc.tCon[27][0] = 0;
    aFunc.tCon[27][1] = 1;
    aFunc.tCon[27][2] = 3;
    aFunc.tCon[27][3] = 11;
    // [29]- b1*b2*b4*c4
    aFunc.tCon[28][0] = 0;
    aFunc.tCon[28][1] = 1;
    aFunc.tCon[28][2] = 3;
    aFunc.tCon[28][3] = 13;
    // [30]+ b1*b3*b6*c3
    aFunc.tCon[29][0] = 0;
    aFunc.tCon[29][1] = 2;
    aFunc.tCon[29][2] = 5;
    aFunc.tCon[29][3] = 12;
    // [31]- b1*b2*b7*c4
    aFunc.tCon[30][0] = 0;
    aFunc.tCon[30][1] = 1;
    aFunc.tCon[30][2] = 6;
    aFunc.tCon[30][3] = 13;
    // [32]+ b1*b2*b7*c5
    aFunc.tCon[31][0] = 0;
    aFunc.tCon[31][1] = 1;
    aFunc.tCon[31][2] = 6;
    aFunc.tCon[31][3] = 14;
    // [33]+ b1*b3*b5*c6
    aFunc.tCon[32][0] = 0;
    aFunc.tCon[32][1] = 2;
    aFunc.tCon[32][2] = 4;
    aFunc.tCon[32][3] = 15;
    // [34]- b1*b4*b7*c4
    aFunc.tCon[33][0] = 0;
    aFunc.tCon[33][1] = 3;
    aFunc.tCon[33][2] = 6;
    aFunc.tCon[33][3] = 13;
    // [35]- b1*b3*b5*c8
    aFunc.tCon[34][0] = 0;
    aFunc.tCon[34][1] = 2;
    aFunc.tCon[34][2] = 4;
    aFunc.tCon[34][3] = 17;
    // [36]+ b1*b4*b7*c5
    aFunc.tCon[35][0] = 0;
    aFunc.tCon[35][1] = 3;
    aFunc.tCon[35][2] = 6;
    aFunc.tCon[35][3] = 14;
    // [37]- b2*b4*b7*c4
    aFunc.tCon[36][0] = 1;
    aFunc.tCon[36][1] = 3;
    aFunc.tCon[36][2] = 6;
    aFunc.tCon[36][3] = 13;
    // [38]+ b1*b3*b8*c6
    aFunc.tCon[37][0] = 0;
    aFunc.tCon[37][1] = 2;
    aFunc.tCon[37][2] = 7;
    aFunc.tCon[37][3] = 15;
    // [39]+ b2*b4*b7*c5
    aFunc.tCon[38][0] = 1;
    aFunc.tCon[38][1] = 3;
    aFunc.tCon[38][2] = 6;
    aFunc.tCon[38][3] = 14;
    // [40]- b1*b3*b8*c7
    aFunc.tCon[39][0] = 0;
    aFunc.tCon[39][1] = 2;
    aFunc.tCon[39][2] = 7;
    aFunc.tCon[39][3] = 16;
    // [41]- b1*b3*b6*c10
    aFunc.tCon[40][0] = 0;
    aFunc.tCon[40][1] = 2;
    aFunc.tCon[40][2] = 5;
    aFunc.tCon[40][3] = 19;
    // [42]+ b1*b5*b8*c6
    aFunc.tCon[41][0] = 0;
    aFunc.tCon[41][1] = 4;
    aFunc.tCon[41][2] = 7;
    aFunc.tCon[41][3] = 15;
    // [43]- b1*b5*b8*c7
    aFunc.tCon[42][0] = 0;
    aFunc.tCon[42][1] = 4;
    aFunc.tCon[42][2] = 7;
    aFunc.tCon[42][3] = 16;
    // [44]- b1*b5*b9*c8
    aFunc.tCon[43][0] = 0;
    aFunc.tCon[43][1] = 4;
    aFunc.tCon[43][2] = 8;
    aFunc.tCon[43][3] = 17;
    // [45]- b1*b3*b10*c10
    aFunc.tCon[44][0] = 0;
    aFunc.tCon[44][1] = 2;
    aFunc.tCon[44][2] = 9;
    aFunc.tCon[44][3] = 19;
    // [46]+ b1*b5*b9*c9
    aFunc.tCon[45][0] = 0;
    aFunc.tCon[45][1] = 4;
    aFunc.tCon[45][2] = 8;
    aFunc.tCon[45][3] = 18;
    // [47]+ b1*b3*b10*c11
    aFunc.tCon[46][0] = 0;
    aFunc.tCon[46][1] = 2;
    aFunc.tCon[46][2] = 9;
    aFunc.tCon[46][3] = 20;
    // [48]+ b1*b2*b4*b7*c4
    aFunc.tCon[47][0] = 0;
    aFunc.tCon[47][1] = 1;
    aFunc.tCon[47][2] = 3;
    aFunc.tCon[47][3] = 6;
    aFunc.tCon[47][4] = 13;
    // [49]- b1*b2*b4*b7*c5
    aFunc.tCon[48][0] = 0;
    aFunc.tCon[48][1] = 1;
    aFunc.tCon[48][2] = 3;
    aFunc.tCon[48][3] = 6;
    aFunc.tCon[48][4] = 14;
    // [50]- b1*b3*b5*b8*c6
    aFunc.tCon[49][0] = 0;
    aFunc.tCon[49][1] = 2;
    aFunc.tCon[49][2] = 4;
    aFunc.tCon[49][3] = 7;
    aFunc.tCon[49][4] = 15;
    // [51]+ b1*b3*b5*b8*c7
    aFunc.tCon[50][0] = 0;
    aFunc.tCon[50][1] = 2;
    aFunc.tCon[50][2] = 4;
    aFunc.tCon[50][3] = 7;
    aFunc.tCon[50][4] = 16;
    // [52]+ b1*b3*b5*b9*c8
    aFunc.tCon[51][0] = 0;
    aFunc.tCon[51][1] = 2;
    aFunc.tCon[51][2] = 4;
    aFunc.tCon[51][3] = 8;
    aFunc.tCon[51][4] = 17;
    // [53]- b1*b3*b5*b9*c9
    aFunc.tCon[52][0] = 0;
    aFunc.tCon[52][1] = 2;
    aFunc.tCon[52][2] = 4;
    aFunc.tCon[52][3] = 8;
    aFunc.tCon[52][4] = 18;
    // [54]+ b1*b3*b6*b10*c10
    aFunc.tCon[53][0] = 0;
    aFunc.tCon[53][1] = 2;
    aFunc.tCon[53][2] = 5;
    aFunc.tCon[53][3] = 9;
    aFunc.tCon[53][4] = 19;
    // [55]- b1*b3*b6*b10*c11
    aFunc.tCon[54][0] = 0;
    aFunc.tCon[54][1] = 2;
    aFunc.tCon[54][2] = 5;
    aFunc.tCon[54][3] = 9;
    aFunc.tCon[54][4] = 20;
}

//f8=c4 - b1*c4 - b2*c4 + b4*c2 + b1*c7 - b4*c4 - b7*c4 + b2*c10 + b7*c8 + b1*b3*c1 + b1*b2*c4 - b1*b4*c2 - b2*b4*c2 + b1*b4*c4 + b1*b6*c3 + b2*b4*c4 - b1*b3*c7 + b1*b7*c4 - b1*b2*c10 + b2*b5*c6 + b2*b7*c4 - b1*b6*c7 + b2*b8*c5 + b4*b7*c4 - b1*b7*c8 - b2*b5*c10 - b2*b7*c8 - b1*b10*c7 - b4*b7*c8 - b2*b8*c10 - b2*b12*c10 + b1*b10*c14 + b2*b12*c11 - b7*b11*c8 + b7*b11*c9 + b1*b2*b4*c2 - b1*b2*b4*c4 - b1*b3*b6*c3 - b1*b2*b5*c6 - b1*b2*b7*c4 - b1*b2*b8*c5 - b1*b4*b7*c4 + b1*b3*b6*c7 - b2*b4*b7*c4 + b1*b2*b5*c10 + b1*b2*b7*c8 + b1*b4*b7*c8 - b2*b5*b8*c5 + b1*b2*b8*c10 + b1*b3*b10*c7 + b2*b4*b7*c8 - b2*b5*b9*c6 + b1*b6*b10*c7 + b1*b2*b12*c10 + b2*b5*b8*c10 - b1*b2*b12*c11 + b1*b7*b11*c8 - b1*b3*b10*c14 - b1*b7*b11*c9 + b2*b5*b9*c12 + b2*b7*b11*c8 + b2*b5*b12*c10 - b2*b7*b11*c9 - b2*b5*b12*c11 + b4*b7*b11*c8 - b1*b6*b10*c14 - b4*b7*b11*c9 + b2*b8*b12*c10 - b2*b8*b12*c11 - b1*b10*b14*c14 + b1*b10*b14*c15 + b1*b2*b4*b7*c4 + b1*b2*b5*b8*c5 - b1*b2*b4*b7*c8 + b1*b2*b5*b9*c6 - b1*b2*b5*b8*c10 - b1*b3*b6*b10*c7 - b1*b2*b5*b9*c12 - b1*b2*b7*b11*c8 - b1*b2*b5*b12*c10 + b1*b2*b7*b11*c9 + b1*b2*b5*b12*c11 - b1*b4*b7*b11*c8 + b1*b4*b7*b11*c9 - b2*b4*b7*b11*c8 - b1*b2*b8*b12*c10 + b2*b4*b7*b11*c9 + b1*b2*b8*b12*c11 + b1*b3*b6*b10*c14 - b2*b5*b8*b12*c10 + b2*b5*b8*b12*c11 - b2*b5*b9*b13*c12 + b1*b3*b10*b14*c14 + b2*b5*b9*b13*c13 - b1*b3*b10*b14*c15 + b1*b6*b10*b14*c14 - b1*b6*b10*b14*c15 + b1*b2*b4*b7*b11*c8 - b1*b2*b4*b7*b11*c9 + b1*b2*b5*b8*b12*c10 - b1*b2*b5*b8*b12*c11 + b1*b2*b5*b9*b13*c12 - b1*b2*b5*b9*b13*c13 - b1*b3*b6*b10*b14*c14 + b1*b3*b6*b10*b14*c15
//[b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15]
//[0,  1,  2,  3,  4,  5,  6,  7,  8,  9,   10,  11,  12,  13,  14, 15, 16, 17, 18, 19, 20, 21, 22, 23,  24,  25,  26,  27,  28]
void appF8(hPara *pa){
    pa->varNum = 29;
    pa->d = 6;
    aFunc.tNum = 105;

    generateFrame(pa->varNum);
    aFunc.tSymbol[0] = 1;
    aFunc.tDegree[0] = 1;
    aFunc.tSymbol[1] = -1;
    aFunc.tDegree[1] = 2;
    aFunc.tSymbol[2] = -1;
    aFunc.tDegree[2] = 2;
    aFunc.tSymbol[3] = 1;
    aFunc.tDegree[3] = 2;
    aFunc.tSymbol[4] = 1;
    aFunc.tDegree[4] = 2;
    aFunc.tSymbol[5] = -1;
    aFunc.tDegree[5] = 2;
    aFunc.tSymbol[6] = -1;
    aFunc.tDegree[6] = 2;
    aFunc.tSymbol[7] = 1;
    aFunc.tDegree[7] = 2;
    aFunc.tSymbol[8] = 1;
    aFunc.tDegree[8] = 2;
    aFunc.tSymbol[9] = 1;
    aFunc.tDegree[9] = 3;
    aFunc.tSymbol[10] = 1;
    aFunc.tDegree[10] = 3;
    aFunc.tSymbol[11] = -1;
    aFunc.tDegree[11] = 3;
    aFunc.tSymbol[12] = -1;
    aFunc.tDegree[12] = 3;
    aFunc.tSymbol[13] = 1;
    aFunc.tDegree[13] = 3;
    aFunc.tSymbol[14] = 1;
    aFunc.tDegree[14] = 3;
    aFunc.tSymbol[15] = 1;
    aFunc.tDegree[15] = 3;
    aFunc.tSymbol[16] = -1;
    aFunc.tDegree[16] = 3;
    aFunc.tSymbol[17] = 1;
    aFunc.tDegree[17] = 3;
    aFunc.tSymbol[18] = -1;
    aFunc.tDegree[18] = 3;
    aFunc.tSymbol[19] = 1;
    aFunc.tDegree[19] = 3;
    aFunc.tSymbol[20] = 1;
    aFunc.tDegree[20] = 3;
    aFunc.tSymbol[21] = -1;
    aFunc.tDegree[21] = 3;
    aFunc.tSymbol[22] = 1;
    aFunc.tDegree[22] = 3;
    aFunc.tSymbol[23] = 1;
    aFunc.tDegree[23] = 3;
    aFunc.tSymbol[24] = -1;
    aFunc.tDegree[24] = 3;
    aFunc.tSymbol[25] = -1;
    aFunc.tDegree[25] = 3;
    aFunc.tSymbol[26] = -1;
    aFunc.tDegree[26] = 3;
    aFunc.tSymbol[27] = -1;
    aFunc.tDegree[27] = 3;
    aFunc.tSymbol[28] = -1;
    aFunc.tDegree[28] = 3;
    aFunc.tSymbol[29] = -1;
    aFunc.tDegree[29] = 3;
    aFunc.tSymbol[30] = -1;
    aFunc.tDegree[30] = 3;
    aFunc.tSymbol[31] = 1;
    aFunc.tDegree[31] = 3;
    aFunc.tSymbol[32] = 1;
    aFunc.tDegree[32] = 3;
    aFunc.tSymbol[33] = -1;
    aFunc.tDegree[33] = 3;
    aFunc.tSymbol[34] = 1;
    aFunc.tDegree[34] = 3;
    aFunc.tSymbol[35] = 1;
    aFunc.tDegree[35] = 4;
    aFunc.tSymbol[36] = -1;
    aFunc.tDegree[36] = 4;
    aFunc.tSymbol[37] = -1;
    aFunc.tDegree[37] = 4;
    aFunc.tSymbol[38] = -1;
    aFunc.tDegree[38] = 4;
    aFunc.tSymbol[39] = -1;
    aFunc.tDegree[39] = 4;
    aFunc.tSymbol[40] = -1;
    aFunc.tDegree[40] = 4;
    aFunc.tSymbol[41] = -1;
    aFunc.tDegree[41] = 4;
    aFunc.tSymbol[42] = 1;
    aFunc.tDegree[42] = 4;
    aFunc.tSymbol[43] = -1;
    aFunc.tDegree[43] = 4;
    aFunc.tSymbol[44] = 1;
    aFunc.tDegree[44] = 4;
    aFunc.tSymbol[45] = 1;
    aFunc.tDegree[45] = 4;
    aFunc.tSymbol[46] = 1;
    aFunc.tDegree[46] = 4;
    aFunc.tSymbol[47] = -1;
    aFunc.tDegree[47] = 4;
    aFunc.tSymbol[48] = 1;
    aFunc.tDegree[48] = 4;
    aFunc.tSymbol[49] = 1;
    aFunc.tDegree[49] = 4;
    aFunc.tSymbol[50] = 1;
    aFunc.tDegree[50] = 4;
    aFunc.tSymbol[51] = -1;
    aFunc.tDegree[51] = 4;
    aFunc.tSymbol[52] = 1;
    aFunc.tDegree[52] = 4;
    aFunc.tSymbol[53] = 1;
    aFunc.tDegree[53] = 4;
    aFunc.tSymbol[54] = 1;
    aFunc.tDegree[54] = 4;
    aFunc.tSymbol[55] = -1;
    aFunc.tDegree[55] = 4;
    aFunc.tSymbol[56] = 1;
    aFunc.tDegree[56] = 4;
    aFunc.tSymbol[57] = -1;
    aFunc.tDegree[57] = 4;
    aFunc.tSymbol[58] = -1;
    aFunc.tDegree[58] = 4;
    aFunc.tSymbol[59] = 1;
    aFunc.tDegree[59] = 4;
    aFunc.tSymbol[60] = 1;
    aFunc.tDegree[60] = 4;
    aFunc.tSymbol[61] = 1;
    aFunc.tDegree[61] = 4;
    aFunc.tSymbol[62] = -1;
    aFunc.tDegree[62] = 4;
    aFunc.tSymbol[63] = -1;
    aFunc.tDegree[63] = 4;
    aFunc.tSymbol[64] = 1;
    aFunc.tDegree[64] = 4;
    aFunc.tSymbol[65] = -1;
    aFunc.tDegree[65] = 4;
    aFunc.tSymbol[66] = -1;
    aFunc.tDegree[66] = 4;
    aFunc.tSymbol[67] = 1;
    aFunc.tDegree[67] = 4;
    aFunc.tSymbol[68] = -1;
    aFunc.tDegree[68] = 4;
    aFunc.tSymbol[69] = -1;
    aFunc.tDegree[69] = 4;
    aFunc.tSymbol[70] = 1;
    aFunc.tDegree[70] = 4;
    aFunc.tSymbol[71] = 1;
    aFunc.tDegree[71] = 4;
    aFunc.tSymbol[72] = 1;
    aFunc.tDegree[72] = 5;
    aFunc.tSymbol[73] = -1;
    aFunc.tDegree[73] = 5;
    aFunc.tSymbol[74] = 1;
    aFunc.tDegree[74] = 5;
    aFunc.tSymbol[75] = -1;
    aFunc.tDegree[75] = 5;
    aFunc.tSymbol[76] = -1;
    aFunc.tDegree[76] = 5;
    aFunc.tSymbol[77] = -1;
    aFunc.tDegree[77] = 5;
    aFunc.tSymbol[78] = -1;
    aFunc.tDegree[78] = 5;
    aFunc.tSymbol[79] = -1;
    aFunc.tDegree[79] = 5;
    aFunc.tSymbol[80] = 1;
    aFunc.tDegree[80] = 5;
    aFunc.tSymbol[81] = 1;
    aFunc.tDegree[81] = 5;
    aFunc.tSymbol[82] = -1;
    aFunc.tDegree[82] = 5;
    aFunc.tSymbol[83] = 1;
    aFunc.tDegree[83] = 5;
    aFunc.tSymbol[84] = -1;
    aFunc.tDegree[84] = 5;
    aFunc.tSymbol[85] = -1;
    aFunc.tDegree[85] = 5;
    aFunc.tSymbol[86] = 1;
    aFunc.tDegree[86] = 5;
    aFunc.tSymbol[87] = 1;
    aFunc.tDegree[87] = 5;
    aFunc.tSymbol[88] = 1;
    aFunc.tDegree[88] = 5;
    aFunc.tSymbol[89] = -1;
    aFunc.tDegree[89] = 5;
    aFunc.tSymbol[90] = 1;
    aFunc.tDegree[90] = 5;
    aFunc.tSymbol[91] = -1;
    aFunc.tDegree[91] = 5;
    aFunc.tSymbol[92] = 1;
    aFunc.tDegree[92] = 5;
    aFunc.tSymbol[93] = 1;
    aFunc.tDegree[93] = 5;
    aFunc.tSymbol[94] = -1;
    aFunc.tDegree[94] = 5;
    aFunc.tSymbol[95] = 1;
    aFunc.tDegree[95] = 5;
    aFunc.tSymbol[96] = -1;
    aFunc.tDegree[96] = 5;
    aFunc.tSymbol[97] = 1;
    aFunc.tDegree[97] = 5;
    aFunc.tSymbol[98] = -1;
    aFunc.tDegree[98] = 5;
    aFunc.tSymbol[99] = 1;
    aFunc.tDegree[99] = 5;
    aFunc.tSymbol[100] = -1;
    aFunc.tDegree[100] = 6;
    aFunc.tSymbol[101] = 1;
    aFunc.tDegree[101] = 6;
    aFunc.tSymbol[102] = -1;
    aFunc.tDegree[102] = 6;
    aFunc.tSymbol[103] = -1;
    aFunc.tDegree[103] = 6;
    aFunc.tSymbol[104] = 1;
    aFunc.tDegree[104] = 6;

    computeNum(pa->sNumPerVar);

    //[1]c4
    aFunc.tCon[0][0] = 17;
    // [2]- b1*c4
    aFunc.tCon[1][0] = 0;
    aFunc.tCon[1][1] = 17;
    // [3]- b2*c4
    aFunc.tCon[2][0] = 1;
    aFunc.tCon[2][1] = 17;
    // [4]+ b4*c2
    aFunc.tCon[3][0] = 3;
    aFunc.tCon[3][1] = 16;
    // [5] + b1*c7
    aFunc.tCon[4][0] = 0;
    aFunc.tCon[4][1] = 20;
    // [6]- b4*c4
    aFunc.tCon[5][0] = 3;
    aFunc.tCon[5][1] = 17;
    // [7]- b7*c4
    aFunc.tCon[6][0] = 6;
    aFunc.tCon[6][1] = 17;
    // [8]+ b2*c10
    aFunc.tCon[7][0] = 1;
    aFunc.tCon[7][1] = 23;
    // [9]+ b7*c8
    aFunc.tCon[8][0] = 6;
    aFunc.tCon[8][1] = 21;
    // [10]+ b1*b3*c1
    aFunc.tCon[9][0] = 0;
    aFunc.tCon[9][1] = 2;
    aFunc.tCon[9][2] = 14;
    // [11]+ b1*b2*c4
    aFunc.tCon[10][0] = 0;
    aFunc.tCon[10][1] = 1;
    aFunc.tCon[10][2] = 17;
    // [12]- b1*b4*c2
    aFunc.tCon[11][0] = 0;
    aFunc.tCon[11][1] = 3;
    aFunc.tCon[11][2] = 15;
    // [13]- b2*b4*c2
    aFunc.tCon[12][0] = 1;
    aFunc.tCon[12][1] = 3;
    aFunc.tCon[12][2] = 15;
    // [14] + b1*b4*c4
    aFunc.tCon[13][0] = 0;
    aFunc.tCon[13][1] = 3;
    aFunc.tCon[13][2] = 17;
    // [15]+ b1*b6*c3
    aFunc.tCon[14][0] = 0;
    aFunc.tCon[14][1] = 5;
    aFunc.tCon[14][2] = 17;
    // [16]+ b2*b4*c4
    aFunc.tCon[15][0] = 1;
    aFunc.tCon[15][1] = 3;
    aFunc.tCon[15][2] = 17;
    // [17]- b1*b3*c7
    aFunc.tCon[16][0] = 0;
    aFunc.tCon[16][1] = 2;
    aFunc.tCon[16][2] = 20;
    // [18]+ b1*b7*c4
    aFunc.tCon[17][0] = 0;
    aFunc.tCon[17][1] = 6;
    aFunc.tCon[17][2] = 17;
    // [19]- b1*b2*c10
    aFunc.tCon[18][0] = 0;
    aFunc.tCon[18][1] = 1;
    aFunc.tCon[18][2] = 23;
    // [20]+ b2*b5*c6
    aFunc.tCon[19][0] = 1;
    aFunc.tCon[19][1] = 4;
    aFunc.tCon[19][2] = 19;
    // [21]+ b2*b7*c4
    aFunc.tCon[20][0] = 1;
    aFunc.tCon[20][1] = 6;
    aFunc.tCon[20][2] = 17;
    // [22]- b1*b6*c7
    aFunc.tCon[21][0] = 0;
    aFunc.tCon[21][1] = 5;
    aFunc.tCon[21][2] = 20;
    // [23]+ b2*b8*c5
    aFunc.tCon[22][0] = 1;
    aFunc.tCon[22][1] = 7;
    aFunc.tCon[22][2] = 18;
    // [24]+ b4*b7*c4
    aFunc.tCon[23][0] = 3;
    aFunc.tCon[23][1] = 6;
    aFunc.tCon[23][2] = 17;
    // [25]- b1*b7*c8
    aFunc.tCon[24][0] = 0;
    aFunc.tCon[24][1] = 6;
    aFunc.tCon[24][2] = 21;
    // [26]- b2*b5*c10
    aFunc.tCon[25][0] = 1;
    aFunc.tCon[25][1] = 4;
    aFunc.tCon[25][2] = 23;
    // [27]- b2*b7*c8
    aFunc.tCon[26][0] = 0;
    aFunc.tCon[26][1] = 6;
    aFunc.tCon[26][2] = 21;
    // [28]- b1*b10*c7
    aFunc.tCon[27][0] = 0;
    aFunc.tCon[27][1] = 9;
    aFunc.tCon[27][2] = 20;
    // [29]- b4*b7*c8
    aFunc.tCon[28][0] = 3;
    aFunc.tCon[28][1] = 6;
    aFunc.tCon[28][2] = 21;
    // [30]- b2*b8*c10
    aFunc.tCon[29][0] = 1;
    aFunc.tCon[29][1] = 7;
    aFunc.tCon[29][2] = 23;
    // [31]- b2*b12*c10
    aFunc.tCon[30][0] = 1;
    aFunc.tCon[30][1] = 11;
    aFunc.tCon[30][2] = 23;
    // [32]+ b1*b10*c14
    aFunc.tCon[31][0] = 0;
    aFunc.tCon[31][1] = 9;
    aFunc.tCon[31][2] = 27;
    // [33]+ b2*b12*c11
    aFunc.tCon[32][0] = 1;
    aFunc.tCon[32][1] = 11;
    aFunc.tCon[32][2] = 24;
    // [34]- b7*b11*c8
    aFunc.tCon[33][0] = 6;
    aFunc.tCon[33][1] = 10;
    aFunc.tCon[33][2] = 21;
    // [35]+ b7*b11*c9
    aFunc.tCon[34][0] = 6;
    aFunc.tCon[34][1] = 10;
    aFunc.tCon[34][2] = 22;
    // [36]+ b1*b2*b4*c2
    aFunc.tCon[35][0] = 0;
    aFunc.tCon[35][1] = 1;
    aFunc.tCon[35][2] = 3;
    aFunc.tCon[35][3] = 15;
    // [37]- b1*b2*b4*c4
    aFunc.tCon[36][0] = 0;
    aFunc.tCon[36][1] = 1;
    aFunc.tCon[36][2] = 3;
    aFunc.tCon[36][3] = 17;
    // [38]- b1*b3*b6*c3
    aFunc.tCon[37][0] = 0;
    aFunc.tCon[37][1] = 2;
    aFunc.tCon[37][2] = 5;
    aFunc.tCon[37][3] = 16;
    // [39]- b1*b2*b5*c6
    aFunc.tCon[38][0] = 0;
    aFunc.tCon[38][1] = 1;
    aFunc.tCon[38][2] = 4;
    aFunc.tCon[38][3] = 19;
    // [40]- b1*b2*b7*c4
    aFunc.tCon[39][0] = 0;
    aFunc.tCon[39][1] = 1;
    aFunc.tCon[39][2] = 6;
    aFunc.tCon[39][3] = 17;
    // [41]- b1*b2*b8*c5
    aFunc.tCon[40][0] = 0;
    aFunc.tCon[40][1] = 1;
    aFunc.tCon[40][2] = 7;
    aFunc.tCon[40][3] = 18;
    // [42]- b1*b4*b7*c4
    aFunc.tCon[41][0] = 0;
    aFunc.tCon[41][1] = 3;
    aFunc.tCon[41][2] = 6;
    aFunc.tCon[41][3] = 17;
    // [43]+ b1*b3*b6*c7
    aFunc.tCon[42][0] = 0;
    aFunc.tCon[42][1] = 2;
    aFunc.tCon[42][2] = 5;
    aFunc.tCon[42][3] = 20;
    // [44]- b2*b4*b7*c4
    aFunc.tCon[43][0] = 1;
    aFunc.tCon[43][1] = 3;
    aFunc.tCon[43][2] = 6;
    aFunc.tCon[43][3] = 17;
    // [45]+ b1*b2*b5*c10
    aFunc.tCon[44][0] = 0;
    aFunc.tCon[44][1] = 1;
    aFunc.tCon[44][2] = 4;
    aFunc.tCon[44][3] = 23;
    // [46]+ b1*b2*b7*c8
    aFunc.tCon[45][0] = 0;
    aFunc.tCon[45][1] = 1;
    aFunc.tCon[45][2] = 6;
    aFunc.tCon[45][3] = 21;
    // [47]+ b1*b4*b7*c8
    aFunc.tCon[46][0] = 0;
    aFunc.tCon[46][1] = 3;
    aFunc.tCon[46][2] = 6;
    aFunc.tCon[46][3] = 21;
    // [48]- b2*b5*b8*c5
    aFunc.tCon[47][0] = 1;
    aFunc.tCon[47][1] = 4;
    aFunc.tCon[47][2] = 7;
    aFunc.tCon[47][3] = 18;
    // [49]+ b1*b2*b8*c10
    aFunc.tCon[48][0] = 0;
    aFunc.tCon[48][1] = 1;
    aFunc.tCon[48][2] = 7;
    aFunc.tCon[48][3] = 23;
    // [50]+ b1*b3*b10*c7
    aFunc.tCon[49][0] = 0;
    aFunc.tCon[49][1] = 2;
    aFunc.tCon[49][2] = 9;
    aFunc.tCon[49][3] = 20;
    // [51]+ b2*b4*b7*c8
    aFunc.tCon[50][0] = 1;
    aFunc.tCon[50][1] = 3;
    aFunc.tCon[50][2] = 6;
    aFunc.tCon[50][3] = 21;
    // [52]- b2*b5*b9*c6
    aFunc.tCon[51][0] = 1;
    aFunc.tCon[51][1] = 4;
    aFunc.tCon[51][2] = 8;
    aFunc.tCon[51][3] = 19;
    // [53]+ b1*b6*b10*c7
    aFunc.tCon[52][0] = 0;
    aFunc.tCon[52][1] = 5;
    aFunc.tCon[52][2] = 9;
    aFunc.tCon[52][3] = 20;
    // [54]+ b1*b2*b12*c10
    aFunc.tCon[53][0] = 0;
    aFunc.tCon[53][1] = 1;
    aFunc.tCon[53][2] = 11;
    aFunc.tCon[53][3] = 23;
    // [55]+ b2*b5*b8*c10
    aFunc.tCon[54][0] = 1;
    aFunc.tCon[54][1] = 4;
    aFunc.tCon[54][2] = 7;
    aFunc.tCon[54][3] = 23;
    // [56]- b1*b2*b12*c11
    aFunc.tCon[55][0] = 0;
    aFunc.tCon[55][1] = 1;
    aFunc.tCon[55][2] = 11;
    aFunc.tCon[55][3] = 24;
    // [57]+ b1*b7*b11*c8
    aFunc.tCon[56][0] = 0;
    aFunc.tCon[56][1] = 6;
    aFunc.tCon[56][2] = 10;
    aFunc.tCon[56][3] = 21;
    // [58]- b1*b3*b10*c14
    aFunc.tCon[57][0] = 0;
    aFunc.tCon[57][1] = 2;
    aFunc.tCon[57][2] = 9;
    aFunc.tCon[57][3] = 27;
    // [59]- b1*b7*b11*c9
    aFunc.tCon[58][0] = 0;
    aFunc.tCon[58][1] = 6;
    aFunc.tCon[58][2] = 10;
    aFunc.tCon[58][3] = 22;
    // [60]+ b2*b5*b9*c12
    aFunc.tCon[59][0] = 1;
    aFunc.tCon[59][1] = 4;
    aFunc.tCon[59][2] = 8;
    aFunc.tCon[59][3] = 25;
    // [61]+ b2*b7*b11*c8
    aFunc.tCon[60][0] = 1;
    aFunc.tCon[60][1] = 6;
    aFunc.tCon[60][2] = 10;
    aFunc.tCon[60][3] = 21;
    // [62]+ b2*b5*b12*c10
    aFunc.tCon[61][0] = 1;
    aFunc.tCon[61][1] = 4;
    aFunc.tCon[61][2] = 11;
    aFunc.tCon[61][3] = 23;
    // [63]- b2*b7*b11*c9
    aFunc.tCon[62][0] = 1;
    aFunc.tCon[62][1] = 6;
    aFunc.tCon[62][2] = 10;
    aFunc.tCon[62][3] = 22;
    // [64]- b2*b5*b12*c11
    aFunc.tCon[63][0] = 1;
    aFunc.tCon[63][1] = 4;
    aFunc.tCon[63][2] = 11;
    aFunc.tCon[63][3] = 24;
    // [65]+ b4*b7*b11*c8
    aFunc.tCon[64][0] = 3;
    aFunc.tCon[64][1] = 6;
    aFunc.tCon[64][2] = 10;
    aFunc.tCon[64][3] = 21;
    // [66]- b1*b6*b10*c14
    aFunc.tCon[65][0] = 0;
    aFunc.tCon[65][1] = 5;
    aFunc.tCon[65][2] = 9;
    aFunc.tCon[65][3] = 27;
    // [67]- b4*b7*b11*c9
    aFunc.tCon[66][0] = 3;
    aFunc.tCon[66][1] = 6;
    aFunc.tCon[66][2] = 10;
    aFunc.tCon[66][3] = 22;
    // [68]+ b2*b8*b12*c10
    aFunc.tCon[67][0] = 1;
    aFunc.tCon[67][1] = 7;
    aFunc.tCon[67][2] = 11;
    aFunc.tCon[67][3] = 23;
    // [69]- b2*b8*b12*c11
    aFunc.tCon[68][0] = 1;
    aFunc.tCon[68][1] = 7;
    aFunc.tCon[68][2] = 11;
    aFunc.tCon[68][3] = 24;
    // [70]- b1*b10*b14*c14
    aFunc.tCon[69][0] = 0;
    aFunc.tCon[69][1] = 9;
    aFunc.tCon[69][2] = 13;
    aFunc.tCon[69][3] = 27;
    // [71]+ b1*b10*b14*c15
    aFunc.tCon[70][0] = 0;
    aFunc.tCon[70][1] = 9;
    aFunc.tCon[70][2] = 13;
    aFunc.tCon[70][3] = 28;
    // [72]+ b1*b2*b4*b7*c4
    aFunc.tCon[71][0] = 0;
    aFunc.tCon[71][1] = 1;
    aFunc.tCon[71][2] = 3;
    aFunc.tCon[71][3] = 6;
    aFunc.tCon[71][4] = 17;
    // [73]+ b1*b2*b5*b8*c5
    aFunc.tCon[72][0] = 0;
    aFunc.tCon[72][1] = 1;
    aFunc.tCon[72][2] = 4;
    aFunc.tCon[72][3] = 7;
    aFunc.tCon[72][4] = 18;
    // [74]- b1*b2*b4*b7*c8
    aFunc.tCon[73][0] = 0;
    aFunc.tCon[73][1] = 1;
    aFunc.tCon[73][2] = 3;
    aFunc.tCon[73][3] = 6;
    aFunc.tCon[73][4] = 21;
    // [75]+ b1*b2*b5*b9*c6
    aFunc.tCon[74][0] = 0;
    aFunc.tCon[74][1] = 1;
    aFunc.tCon[74][2] = 4;
    aFunc.tCon[74][3] = 8;
    aFunc.tCon[74][4] = 19;
    // [76]- b1*b2*b5*b8*c10
    aFunc.tCon[75][0] = 0;
    aFunc.tCon[75][1] = 1;
    aFunc.tCon[75][2] = 4;
    aFunc.tCon[75][3] = 7;
    aFunc.tCon[75][4] = 23;
    // [77]- b1*b3*b6*b10*c7
    aFunc.tCon[76][0] = 0;
    aFunc.tCon[76][1] = 2;
    aFunc.tCon[76][2] = 5;
    aFunc.tCon[76][3] = 9;
    aFunc.tCon[76][4] = 20;
    // [78]- b1*b2*b5*b9*c12
    aFunc.tCon[77][0] = 0;
    aFunc.tCon[77][1] = 1;
    aFunc.tCon[77][2] = 4;
    aFunc.tCon[77][3] = 8;
    aFunc.tCon[77][4] = 25;
    // [79]- b1*b2*b7*b11*c8
    aFunc.tCon[78][0] = 0;
    aFunc.tCon[78][1] = 1;
    aFunc.tCon[78][2] = 6;
    aFunc.tCon[78][3] = 10;
    aFunc.tCon[78][4] = 21;
    // [80]- b1*b2*b5*b12*c10
    aFunc.tCon[79][0] = 0;
    aFunc.tCon[79][1] = 1;
    aFunc.tCon[79][2] = 4;
    aFunc.tCon[79][3] = 11;
    aFunc.tCon[79][4] = 23;
    // [81]+ b1*b2*b7*b11*c9
    aFunc.tCon[80][0] = 0;
    aFunc.tCon[80][1] = 1;
    aFunc.tCon[80][2] = 6;
    aFunc.tCon[80][3] = 10;
    aFunc.tCon[80][4] = 22;
    // [82]+ b1*b2*b5*b12*c11
    aFunc.tCon[81][0] = 0;
    aFunc.tCon[81][1] = 1;
    aFunc.tCon[81][2] = 4;
    aFunc.tCon[81][3] = 11;
    aFunc.tCon[81][4] = 24;
    // [83]- b1*b4*b7*b11*c8
    aFunc.tCon[82][0] = 0;
    aFunc.tCon[82][1] = 3;
    aFunc.tCon[82][2] = 6;
    aFunc.tCon[82][3] = 10;
    aFunc.tCon[82][4] = 21;
    // [84]+ b1*b4*b7*b11*c9
    aFunc.tCon[83][0] = 0;
    aFunc.tCon[83][1] = 3;
    aFunc.tCon[83][2] = 6;
    aFunc.tCon[83][3] = 10;
    aFunc.tCon[83][4] = 22;
    // [85]- b2*b4*b7*b11*c8
    aFunc.tCon[84][0] = 1;
    aFunc.tCon[84][1] = 3;
    aFunc.tCon[84][2] = 6;
    aFunc.tCon[84][3] = 10;
    aFunc.tCon[84][4] = 21;
    // [86]- b1*b2*b8*b12*c10
    aFunc.tCon[85][0] = 0;
    aFunc.tCon[85][1] = 1;
    aFunc.tCon[85][2] = 7;
    aFunc.tCon[85][3] = 11;
    aFunc.tCon[85][4] = 23;
    // [87]+ b2*b4*b7*b11*c9
    aFunc.tCon[86][0] = 1;
    aFunc.tCon[86][1] = 3;
    aFunc.tCon[86][2] = 6;
    aFunc.tCon[86][3] = 10;
    aFunc.tCon[86][4] = 22;
    // [88]+ b1*b2*b8*b12*c11
    aFunc.tCon[87][0] = 0;
    aFunc.tCon[87][1] = 1;
    aFunc.tCon[87][2] = 7;
    aFunc.tCon[87][3] = 11;
    aFunc.tCon[87][4] = 24;
    // [89]+ b1*b3*b6*b10*c14
    aFunc.tCon[88][0] = 0;
    aFunc.tCon[88][1] = 2;
    aFunc.tCon[88][2] = 5;
    aFunc.tCon[88][3] = 9;
    aFunc.tCon[88][4] = 27;
    // [90]- b2*b5*b8*b12*c10
    aFunc.tCon[89][0] = 1;
    aFunc.tCon[89][1] = 4;
    aFunc.tCon[89][2] = 7;
    aFunc.tCon[89][3] = 11;
    aFunc.tCon[89][4] = 23;
    // [91]+ b2*b5*b8*b12*c11
    aFunc.tCon[90][0] = 1;
    aFunc.tCon[90][1] = 4;
    aFunc.tCon[90][2] = 7;
    aFunc.tCon[90][3] = 11;
    aFunc.tCon[90][4] = 24;
    // [92]- b2*b5*b9*b13*c12
    aFunc.tCon[91][0] = 1;
    aFunc.tCon[91][1] = 4;
    aFunc.tCon[91][2] = 8;
    aFunc.tCon[91][3] = 12;
    aFunc.tCon[91][4] = 25;
    // [93]+ b1*b3*b10*b14*c14
    aFunc.tCon[92][0] = 0;
    aFunc.tCon[92][1] = 2;
    aFunc.tCon[92][2] = 9;
    aFunc.tCon[92][3] = 13;
    aFunc.tCon[92][4] = 27;
    // [94]+ b2*b5*b9*b13*c13
    aFunc.tCon[93][0] = 1;
    aFunc.tCon[93][1] = 4;
    aFunc.tCon[93][2] = 8;
    aFunc.tCon[93][3] = 12;
    aFunc.tCon[93][4] = 26;
    // [95]- b1*b3*b10*b14*c15
    aFunc.tCon[94][0] = 0;
    aFunc.tCon[94][1] = 2;
    aFunc.tCon[94][2] = 9;
    aFunc.tCon[94][3] = 13;
    aFunc.tCon[94][4] = 28;
    // [96]+ b1*b6*b10*b14*c14
    aFunc.tCon[95][0] = 0;
    aFunc.tCon[95][1] = 5;
    aFunc.tCon[95][2] = 9;
    aFunc.tCon[95][3] = 13;
    aFunc.tCon[95][4] = 27;
    // [97]- b1*b6*b10*b14*c15
    aFunc.tCon[96][0] = 0;
    aFunc.tCon[96][1] = 5;
    aFunc.tCon[96][2] = 9;
    aFunc.tCon[96][3] = 13;
    aFunc.tCon[96][4] = 28;
    // [98]+ b1*b2*b4*b7*b11*c8
    aFunc.tCon[97][0] = 0;
    aFunc.tCon[97][1] = 1;
    aFunc.tCon[97][2] = 3;
    aFunc.tCon[97][3] = 6;
    aFunc.tCon[97][4] = 10;
    aFunc.tCon[97][5] = 21;
    // [99]- b1*b2*b4*b7*b11*c9
    aFunc.tCon[98][0] = 0;
    aFunc.tCon[98][1] = 1;
    aFunc.tCon[98][2] = 3;
    aFunc.tCon[98][3] = 6;
    aFunc.tCon[98][4] = 10;
    aFunc.tCon[98][5] = 22;
    // [100]+ b1*b2*b5*b8*b12*c10
    aFunc.tCon[99][0] = 0;
    aFunc.tCon[99][1] = 1;
    aFunc.tCon[99][2] = 4;
    aFunc.tCon[99][3] = 7;
    aFunc.tCon[99][4] = 11;
    aFunc.tCon[99][5] = 23;
    // [101]- b1*b2*b5*b8*b12*c11
    aFunc.tCon[100][0] = 0;
    aFunc.tCon[100][1] = 1;
    aFunc.tCon[100][2] = 4;
    aFunc.tCon[100][3] = 7;
    aFunc.tCon[100][4] = 11;
    aFunc.tCon[100][5] = 24;
    // [102]+ b1*b2*b5*b9*b13*c12
    aFunc.tCon[101][0] = 0;
    aFunc.tCon[101][1] = 1;
    aFunc.tCon[101][2] = 4;
    aFunc.tCon[101][3] = 8;
    aFunc.tCon[101][4] = 12;
    aFunc.tCon[101][5] = 25;
    // [103]- b1*b2*b5*b9*b13*c13
    aFunc.tCon[102][0] = 0;
    aFunc.tCon[102][1] = 1;
    aFunc.tCon[102][2] = 4;
    aFunc.tCon[102][3] = 8;
    aFunc.tCon[102][4] = 12;
    aFunc.tCon[102][5] = 26;
    // [104]- b1*b3*b6*b10*b14*c14
    aFunc.tCon[103][0] = 0;
    aFunc.tCon[103][1] = 2;
    aFunc.tCon[103][2] = 5;
    aFunc.tCon[103][3] = 9;
    aFunc.tCon[103][4] = 13;
    aFunc.tCon[103][5] = 27;
    // [105]+ b1*b3*b6*b10*b14*c15
    aFunc.tCon[104][0] = 0;
    aFunc.tCon[104][1] = 2;
    aFunc.tCon[104][2] = 5;
    aFunc.tCon[104][3] = 9;
    aFunc.tCon[104][4] = 13;
    aFunc.tCon[104][5] = 28;
}



void SVHSS(int repeatTime, hPara pa, BV_Para* bvPara, BV_PK* bvPk, BV_SK* bvSk){
    /**************prepareSharesIndex**************/
    int *tmpArr= malloc(sizeof(int)*pa.m);
    for(int i=0;i<pa.m;i++)
    {
        tmpArr[i]=i+1;
    }
    //combination before merging
    int *data = malloc(sizeof(int)*pa.t);
    pa.sIndex = (int **)(malloc(sizeof(int *) * pa.sNumPerVar));
    for(int i=0; i<pa.sNumPerVar; i++)
    {
        pa.sIndex[i]= ((int*) malloc(sizeof(int) * pa.t));
    }
    combinationUtil(tmpArr, pa.m, pa.t, 0, data, 0, pa.sIndex);

    //free memory
    free(tmpArr);
    tmpArr=NULL;
    free(data);
    data=NULL;
    /********************************************split********************************************/
    //[serverIndex][varIndex][shareIndex]
    share ***serShares = malloc(sizeof(share **) * pa.m);
    share ***alphaSerShares = malloc(sizeof(share **) * pa.m);
    for(int i=0; i<pa.m; i++)
    {
        serShares[i] = malloc(sizeof(share *) * pa.varNum);
        alphaSerShares[i] = malloc(sizeof(share *) * pa.varNum);
        for(int j=0; j<pa.varNum; j++)
        {
            serShares[i][j] = malloc(sizeof(share) * pa.sNumPerVar);
            alphaSerShares[i][j] = malloc(sizeof(share) * pa.sNumPerVar);
        }
    }

    fmpz_t *alphaVars = malloc(sizeof(fmpz_t) * pa.varNum);
    fmpz_t alpha;
    split(serShares, alphaSerShares, vars, alphaVars, alpha, pa, bvPara, bvPk);
    //direct computation
    fmpz_t directRes;
    fmpz_init(directRes);
    fmpz_zero(directRes);
    directCompute(directRes, aFunc, vars, bvPara);

    if (DEBUG){
        for(int i=0; i<pa.varNum;i++)
        {
            printf("vars[%d]:", i);
            fmpz_print(vars[i]);
            printf("\n");
        }
        printf("alpha:");
        fmpz_print(alpha);
        printf("\n");
    }

    //free memory
    free(vars);
    vars=NULL;
    free(alphaVars);
    alphaVars=NULL;
    /********************************************Compute********************************************/
    //Calculate all possible combinations ahead of time
    long long *allResSetNum = malloc(sizeof(long long)*(pa.d+1));
    int ***allResSet = malloc(sizeof(int **)*(pa.d+1));
    int **countRes = malloc(sizeof(int*)*(pa.d+1));//Count the number of occurrences of "share" and record the minimum number
    for (int de = 1; de <= pa.d ; de++) {
        allResSetNum[de] = pow(pa.sNumPerVar, de);//the number of share terms of degree de
        allResSet[de] = malloc(sizeof(int *)*allResSetNum[de]);
        countRes[de] = malloc(sizeof(int *)*allResSetNum[de]);
        for(long long i=0; i<allResSetNum[de]; i++)
        {
            allResSet[de][i] = malloc(sizeof(int)*de);
        }
    }
    shareCombine(allResSet, pa.d, pa);

    if(DEBUG){
        //check allResSet
        for (int de = 1; de <= pa.d ; de++) {
            printf("\n%lld terms combination\n", allResSetNum[de]);
            for(int i=0; i<allResSetNum[de]; i++)
            {
                for(int j=0; j<de; j++)
                {
                    printf("%d", allResSet[de][i][j]);
                }
                printf("\n");
            }
        }
    }

    //compute the state (encrypted or not encrypted)
    for (int de = 1; de <= pa.d ; de++) {
        for(long long i=0; i<allResSetNum[de];i++)
        {
            int sCnt[pa.m];
            for (int j = 0; j < pa.m; j++) {//virtual server index
                sCnt[j]=0;
                for(int k=0; k<de; k++)
                {
                    sCnt[j]=sCnt[j]+serShares[j][0][allResSet[de][i][k]].state;
                }
            }

            countRes[de][i] = findMinVIdx(sCnt, pa.m);

            if(DEBUG){
                //check sCnt
                for (int j = 0; j < pa.m; j++) {
                    printf("%d-%d,", j, sCnt[j]);
                }

                printf("minVIndx:%d\n", countRes[de][i]);
            }
        }
    }

    //alloc computation for the servers
    long long sTermsIndex = 0;
    sTerm* sTerms = malloc(sizeof(sTerm)*allSTermNum);
    for(int i=0; i<aFunc.tNum; i++)
    {
        //according degree to get the share term by spliting
        for(long long j=0; j<allResSetNum[aFunc.tDegree[i]]; j++)
        {
            sTerms[sTermsIndex].conLen = aFunc.tDegree[i];
            sTerms[sTermsIndex].symbol = aFunc.tSymbol[i];
            sTerms[sTermsIndex].tCon = malloc(sizeof(int)*sTerms[sTermsIndex].conLen);
            sTerms[sTermsIndex].sCon = malloc(sizeof(int)*sTerms[sTermsIndex].conLen);
            for(int k=0; k<sTerms[sTermsIndex].conLen; k++)
            {
                sTerms[sTermsIndex].tCon[k] =  aFunc.tCon[i][k];//the variable information
                sTerms[sTermsIndex].sCon[k] = allResSet[aFunc.tDegree[i]][j][k];//the share information
            }
            sTerms[sTermsIndex].ser = countRes[aFunc.tDegree[i]][j];//not real server index, the order of left servers
            sTermsIndex++;
        }
    }

    if(DEBUG){
        //check sTerms
        printf("allSTermNum:%lld\n", allSTermNum);
        for(long long i=0; i<allSTermNum;i++)
        {
            printf("conLen:%d\n", sTerms[i].conLen);
            printf("symbol:%d\n", sTerms[i].symbol);
            for (int j = 0; j < sTerms[i].conLen; j++)
            {
                printf("v%d-s%d  ", sTerms[i].tCon[j], sTerms[i].sCon[j]);
            }
            printf("===server:%d\n", sTerms[i].ser);
        }
    }

    //free momery
    for (int de = 1; de <= pa.d ; de++) {
        for(long long i=0; i<allResSetNum[de]; i++)
        {
            free(allResSet[de][i]);
            allResSet[de][i]=NULL;
        }
        free(allResSet[de]);
        allResSet[de]=NULL;
        free(countRes[de]);
        countRes[de]=NULL;
    }
    free(allResSet);
    allResSet=NULL;
    free(countRes);
    countRes=NULL;
    free(allResSetNum);
    allResSetNum=NULL;

    //Close different servers and compute
    clock_t start, end;
    double time = 0;
    cypher *res = malloc(sizeof(cypher)*pa.m);
    cypher *alphaRes = malloc(sizeof(cypher)*pa.m);

    for(int i=0; i<pa.m; i++){
        res[i] = (cypher)(malloc(sizeof(fmpz_mod_poly_t) * (pa.k+1)));
        alphaRes[i] = (cypher)(malloc(sizeof(fmpz_mod_poly_t) * (pa.k+1)));
        for(int j=0; j<=pa.k; j++){
            fmpz_mod_poly_init(res[i][j], bvPara->ctx_q);
            fmpz_mod_poly_init(alphaRes[i][j], bvPara->ctx_q);
        }
    }
    start = clock();
    for (int i = 0; i < repeatTime; i++) {
        compute(res, serShares, sTerms, allSTermNum, pa, bvPara, bvPk);
        alphaCompute(alphaRes, serShares, alphaSerShares, sTerms, allSTermNum, pa, bvPara, bvPk);
    }
    end = clock();
    time = (double) ((end - start)/repeatTime )/ CLOCKS_PER_SEC;
    printf("server running time: %f ms\n", time * 1000);

    //free memory
    free(sTerms);
    sTerms=NULL;
    for(int i=0; i<pa.m; i++)
    {
        for(int j=0; j<pa.varNum; j++)
        {
            free(serShares[i][j]);
            serShares[i][j]=NULL;

            free(alphaSerShares[i][j]);
            alphaSerShares[i][j]=NULL;
        }
        free(serShares[i]);
        serShares[i]=NULL;

        free(alphaSerShares[i]);
        alphaSerShares[i]=NULL;
    }
    free(serShares);
    serShares=NULL;

    free(alphaSerShares);
    alphaSerShares=NULL;

    /******************************************Dec and Verification*********************************/
    start = clock();
    for (int re = 0; re < repeatTime; re++) {
        cypher fRes, fAlphaRes;

        fRes = (cypher)(malloc(sizeof(fmpz_mod_poly_t) * (pa.k+1)));
        fAlphaRes = (cypher)(malloc(sizeof(fmpz_mod_poly_t) * (pa.k+1)));
        for(int i=0; i<=pa.k; i++){
            fmpz_mod_poly_init(fRes[i], bvPara->ctx_q);
            fmpz_mod_poly_zero(fRes[i], bvPara->ctx_q);

            fmpz_mod_poly_init(fAlphaRes[i], bvPara->ctx_q);
            fmpz_mod_poly_zero(fAlphaRes[i], bvPara->ctx_q);
        }
        for(int i=0; i<pa.m; i++){
            BV_Add(fRes, fRes, res[i], pa.k, bvPara);
            BV_Add(fAlphaRes, fAlphaRes, alphaRes[i], pa.k, bvPara);
        }

        fmpz_t decRes, alphaDecRes, tmp;
        fmpz_init(decRes);
        fmpz_init(alphaDecRes);

        BV_Dec(decRes, fRes, pa.k, bvPara, bvSk);
        BV_Dec(alphaDecRes, fAlphaRes, pa.k, bvPara, bvSk);

        fmpz_mod(decRes, decRes, bvPara->msg);
        fmpz_mod(alphaDecRes, alphaDecRes, bvPara->msg);

        fmpz_mod_mul(tmp, alpha, decRes, bvPara->ctx_msg);

        if (DEBUG){
            //check tmp
            printf("decRes:\n");
            fmpz_print(decRes);
            printf("\n");
            printf("tmp:\n");
            fmpz_print(tmp);
            printf("\n");
            printf("alphaDecRes:\n");
            fmpz_print(alphaDecRes);
            printf("\n");
        }

        if (fmpz_equal(tmp, alphaDecRes)){
            printf("Verification Successes!!!\n");
        }else{
            printf("Verification Fails!!\n");
        }
    }
    end = clock();
    time = (double) ((end - start)/repeatTime) / CLOCKS_PER_SEC;
    printf("client running time: %f ms\n", time * 1000);
    if (DEBUG){
        printf("directRes:");
        fmpz_print(directRes);
        printf("\n");
    }
}

