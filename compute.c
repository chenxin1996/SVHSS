#include "compute.h"

void compute(cypher *res, share*** serShares, sTerm* sTerms, long long allSTermNum, hPara pa, BV_Para* bvPara, BV_PK* bvPk)
{
    /*compute*/
    //allSTermNum
    fmpz_t *plianRes = malloc(sizeof(fmpz_t)*pa.m);
    for(int i=0; i<pa.m; i++){
        fmpz_init(plianRes[i]);
        fmpz_zero(plianRes[i]);
    }
    for(long long i=0; i<allSTermNum; i++)
    {
        //sTerms[i].ser computes sTerms.con with mSerShares[sTerms[i].ser]
        fmpz_t multiPlain;
        fmpz_init(multiPlain);
        fmpz_one(multiPlain);
        int cyCnter=0;
        int *cyRecord = malloc(sizeof(int)*pa.k);
        for(int j=0; j<pa.k; j++){
            cyRecord[j]=-1;
        }
        for(int j=0; j<sTerms[i].conLen; j++)
        {
            if (serShares[sTerms[i].ser][sTerms[i].tCon[j]][sTerms[i].sCon[j]].state==0)//multiple all plaintexts
            {
                fmpz_mod_mul(multiPlain, multiPlain, serShares[sTerms[i].ser][sTerms[i].tCon[j]][sTerms[i].sCon[j]].plain, bvPara->ctx_msg);
            }else if (serShares[sTerms[i].ser][sTerms[i].tCon[j]][sTerms[i].sCon[j]].state==1)//record cyphertext information
            {
                cyRecord[cyCnter++]=j;
            }
        }

        if (DEBUG){
            if(cyCnter>pa.k){
                printf("%lld ERROR!! %d\n", i, cyCnter);
            }
        }

        //multiple cyphertext
        if(cyCnter==0){
            //no cyphertext: encrypt the plaintext result to get temp result
            if (sTerms[i].symbol==1){
                fmpz_mod_add(plianRes[sTerms[i].ser], plianRes[sTerms[i].ser], multiPlain, bvPara->ctx_msg);
            }else{
                fmpz_mod_sub(plianRes[sTerms[i].ser], plianRes[sTerms[i].ser], multiPlain, bvPara->ctx_msg);
            }
        }else if (cyCnter==1){
            //one cyphertext: multiple the cyphertext with multiPlain
            cypher temp = malloc(sizeof(fmpz_mod_poly_t) * 2);
            fmpz_mod_poly_init(temp[0], bvPara->ctx_q);
            fmpz_mod_poly_init(temp[1], bvPara->ctx_q);

            int j = cyRecord[0];
            BV_Scalar_Mult(temp, serShares[sTerms[i].ser][sTerms[i].tCon[j]][sTerms[i].sCon[j]].cph, multiPlain, 1, bvPara);
            if (sTerms[i].symbol==1){
                BV_Add(res[sTerms[i].ser], res[sTerms[i].ser], temp, 1, bvPara);
            }else{
                BV_Sub(res[sTerms[i].ser], res[sTerms[i].ser], temp, 1, bvPara);
            }
        }else if(cyCnter>=2){
            cypher *array = malloc(sizeof(cypher)*(pa.k-1));
            for(int j=0; j<pa.k-1; j++){
                array[j] = malloc(sizeof(fmpz_mod_poly_t) * (j+3));
                for(int k=0; k<(j+3); k++){
                    fmpz_mod_poly_init(array[j][k], bvPara->ctx_q);
                }
            }

            BV_Mult(array[0], serShares[sTerms[i].ser][sTerms[i].tCon[cyRecord[0]]][sTerms[i].sCon[cyRecord[0]]].cph, serShares[sTerms[i].ser][sTerms[i].tCon[cyRecord[1]]][sTerms[i].sCon[cyRecord[1]]].cph, 1, bvPara);
            int lpTime = cyCnter-2, index = 0;
            while (lpTime>0){
                BV_Mult(array[index+1], serShares[sTerms[i].ser][sTerms[i].tCon[cyRecord[index+2]]][sTerms[i].sCon[cyRecord[index+2]]].cph, array[index], index+2, bvPara);
                index++;
                lpTime--;
            }

            BV_Scalar_Mult(array[cyCnter-2], array[cyCnter-2], multiPlain, cyCnter, bvPara);

            if (sTerms[i].symbol==1){
                BV_Add(res[sTerms[i].ser], res[sTerms[i].ser], array[cyCnter-2], cyCnter, bvPara);
            }else{
                BV_Sub(res[sTerms[i].ser], res[sTerms[i].ser], array[cyCnter-2], cyCnter, bvPara);
            }
        }
        fmpz_clear(multiPlain);
    }
    //at last, encrypt the plaintext and add it to cyphertext
    cypher temp = malloc(sizeof(fmpz_mod_poly_t) * 2);
    fmpz_mod_poly_init(temp[0], bvPara->ctx_q);
    fmpz_mod_poly_init(temp[1], bvPara->ctx_q);
    for(int i=0; i<pa.m; i++){
        BV_Enc(temp, plianRes[i], bvPara, bvPk);
        // add temp to final result
        BV_Add(res[sTerms[i].ser], res[sTerms[i].ser], temp, 1, bvPara);
    }
}

void alphaCompute(cypher *alphaRes, share*** serShares, share*** alphaSerShares, sTerm* sTerms, long long allSTermNum, hPara pa, BV_Para* bvPara, BV_PK* bvPk)
{
    /*compute*/
    //allSTermNum
    fmpz_t *plainRes = malloc(sizeof(fmpz_t)*pa.m);
    for(int i=0; i<pa.m; i++){
        fmpz_init(plainRes[i]);
        fmpz_zero(plainRes[i]);
    }
    for(long long i=0; i<allSTermNum; i++)
    {
        //sTerms[i].ser computes sTerms.con with mSerShares[sTerms[i].ser]
        fmpz_t multiPlain;
        fmpz_init(multiPlain);
        fmpz_one(multiPlain);
        int cyCnter=0, flag=0;
        int *cyRecord = malloc(sizeof(int)*pa.k);
        for(int j=0; j<pa.k; j++){
            cyRecord[j]=-1;
        }

        //choose j=0 to multiple alpha
        if (alphaSerShares[sTerms[i].ser][sTerms[i].tCon[0]][sTerms[i].sCon[0]].state==0)//multiple all plaintexts
        {
            fmpz_mod_mul(multiPlain, multiPlain, alphaSerShares[sTerms[i].ser][sTerms[i].tCon[0]][sTerms[i].sCon[0]].plain, bvPara->ctx_msg);
        }else if (alphaSerShares[sTerms[i].ser][sTerms[i].tCon[0]][sTerms[i].sCon[0]].state==1)//record cyphertext information
        {
            cyRecord[cyCnter++]=0;
            flag=1;
        }

        for(int j=1; j<sTerms[i].conLen; j++)
        {
            if (serShares[sTerms[i].ser][sTerms[i].tCon[j]][sTerms[i].sCon[j]].state==0)//multiple all plaintexts
            {
                fmpz_mod_mul(multiPlain, multiPlain, serShares[sTerms[i].ser][sTerms[i].tCon[j]][sTerms[i].sCon[j]].plain, bvPara->ctx_msg);
            }else if (serShares[sTerms[i].ser][sTerms[i].tCon[j]][sTerms[i].sCon[j]].state==1)//record cyphertext information
            {
                cyRecord[cyCnter++]=j;
            }
        }


        if (DEBUG){
            if(cyCnter>pa.k){
                printf("%lld ERROR!! %d\n", i, cyCnter);
            }
        }

        //multiple cyphertext
        if(cyCnter==0){
            //no cyphertext: encrypt the plaintext result to get temp result
            if (sTerms[i].symbol==1){
                fmpz_mod_add(plainRes[sTerms[i].ser], plainRes[sTerms[i].ser], multiPlain, bvPara->ctx_msg);
            }else{
                fmpz_mod_sub(plainRes[sTerms[i].ser], plainRes[sTerms[i].ser], multiPlain, bvPara->ctx_msg);
            }
        }else if (cyCnter==1){
            //one cyphertext: multiple the cyphertext with multiPlain
            cypher temp = malloc(sizeof(fmpz_mod_poly_t) * 2);
            fmpz_mod_poly_init(temp[0], bvPara->ctx_q);
            fmpz_mod_poly_init(temp[1], bvPara->ctx_q);

            int j = cyRecord[0];
            if (flag){
                BV_Scalar_Mult(temp, alphaSerShares[sTerms[i].ser][sTerms[i].tCon[j]][sTerms[i].sCon[j]].cph, multiPlain, 1, bvPara);
            }else{
                BV_Scalar_Mult(temp, serShares[sTerms[i].ser][sTerms[i].tCon[j]][sTerms[i].sCon[j]].cph, multiPlain, 1, bvPara);
            }

            if (sTerms[i].symbol==1){
                BV_Add(alphaRes[sTerms[i].ser], alphaRes[sTerms[i].ser], temp, 1, bvPara);
            }else{
                BV_Sub(alphaRes[sTerms[i].ser], alphaRes[sTerms[i].ser], temp, 1, bvPara);
            }
        }else if (cyCnter>=2){
            cypher *array = malloc(sizeof(cypher)*(pa.k-1));
            for(int j=0; j<pa.k-1; j++){
                array[j] = malloc(sizeof(fmpz_mod_poly_t) * (j+3));
                for(int k=0; k<(j+3); k++){
                    fmpz_mod_poly_init(array[j][k], bvPara->ctx_q);
                }
            }

            if (flag){
                BV_Mult(array[0], alphaSerShares[sTerms[i].ser][sTerms[i].tCon[cyRecord[0]]][sTerms[i].sCon[cyRecord[0]]].cph, serShares[sTerms[i].ser][sTerms[i].tCon[cyRecord[1]]][sTerms[i].sCon[cyRecord[1]]].cph, 1, bvPara);
                int lpTime = cyCnter-2, index = 0;
                while (lpTime>0){
                    BV_Mult(array[index+1], serShares[sTerms[i].ser][sTerms[i].tCon[cyRecord[index+2]]][sTerms[i].sCon[cyRecord[index+2]]].cph, array[index], index+2, bvPara);
                    index++;
                    lpTime--;
                }
            }else{
                BV_Mult(array[0], serShares[sTerms[i].ser][sTerms[i].tCon[cyRecord[0]]][sTerms[i].sCon[cyRecord[0]]].cph, serShares[sTerms[i].ser][sTerms[i].tCon[cyRecord[1]]][sTerms[i].sCon[cyRecord[1]]].cph, 1, bvPara);
                int lpTime = cyCnter-2, index = 0;
                while (lpTime>0){
                    BV_Mult(array[index+1], serShares[sTerms[i].ser][sTerms[i].tCon[cyRecord[index+2]]][sTerms[i].sCon[cyRecord[index+2]]].cph, array[index], index+2, bvPara);
                    index++;
                    lpTime--;
                }
            }

            BV_Scalar_Mult(array[cyCnter-2], array[cyCnter-2], multiPlain, cyCnter, bvPara);

            if (sTerms[i].symbol==1){
                BV_Add(alphaRes[sTerms[i].ser], alphaRes[sTerms[i].ser], array[cyCnter-2], cyCnter, bvPara);
            }else{
                BV_Sub(alphaRes[sTerms[i].ser], alphaRes[sTerms[i].ser], array[cyCnter-2], cyCnter, bvPara);
            }
        }

        fmpz_clear(multiPlain);
    }
    //at last, encrypt the plaintext and add it to cyphertext
    cypher temp = malloc(sizeof(fmpz_mod_poly_t) * 2);
    fmpz_mod_poly_init(temp[0], bvPara->ctx_q);
    fmpz_mod_poly_init(temp[1], bvPara->ctx_q);
    for(int i=0; i<pa.m; i++){
        BV_Enc(temp, plainRes[i], bvPara, bvPk);
        // add temp to final result
        BV_Add(alphaRes[sTerms[i].ser], alphaRes[sTerms[i].ser], temp, 1, bvPara);
    }
}

void directCompute(fmpz_t directRes, func aFunc, fmpz_t *vars, BV_Para* bvPara)
{
    for(int i=0; i<aFunc.tNum; i++)
    {
        fmpz_t termRes;
        fmpz_init(termRes);
        fmpz_one(termRes);
        for(int j=0; j<aFunc.tDegree[i]; j++)
        {
            fmpz_mod_mul(termRes, termRes, vars[aFunc.tCon[i][j]], bvPara->ctx_msg);
        }
        if (aFunc.tSymbol[i]==1){
            fmpz_mod_add(directRes, directRes, termRes, bvPara->ctx_msg);
        }else{
            fmpz_mod_sub(directRes, directRes, termRes, bvPara->ctx_msg);
        }
    }
}

void shareCombine(int ***allResSet, int maxDg, hPara pa){
    for (int de = 1; de <= maxDg ; de++) {
        int currentSet[de];

        //Get all share term
        cartesianProduct(allResSet[de], pa.sNumPerVar,currentSet,de ,0);
    }
};

int findMergeInto(int **mSIndex, int *index, hPara pa) {
    int maxNumber=0;
    int maxNumberIndex=0;
    for(int i=0;i<pa.sNumPerVar;i++)
    {
        int numOfSame=0;
        for(int j=0; j<pa.t; j++)
        {
            if (isInSet(index[j], mSIndex[i], pa.t))
            {
                numOfSame++;
            }
        }
        if(numOfSame>maxNumber)
        {
            maxNumber=numOfSame;
            maxNumberIndex=i;
        }
    }
    return maxNumberIndex;
}

void allTermsByDV(func aFunc, int *tNumByD, int varNum, int maxDg){

    int tIndex=0;
    for(int i=1; i<=maxDg; i++){
        int *currentTerm = malloc(sizeof(int)*i);
        int **terms = malloc(sizeof(int *)*tNumByD[i]);
        for(int j=0; j<tNumByD[i]; j++){
            terms[j] = malloc(sizeof(int)*i);
        }
        geTermsByDV(terms, currentTerm, i, varNum, 0, 0);

        if (DEBUG){
            //check terms
            printf("terms\n");
            for(int j=0; j<tNumByD[i]; j++){
                for(int k=0; k<i; k++){
                    printf("%d", terms[j][k]);
                }
                printf("\n");
            }
        }
        for(int j=0; j<tNumByD[i]; j++){
            for(int k=0; k<i; k++){
                aFunc.tCon[tIndex][k]=terms[j][k];
            }
            tIndex++;
        }
    }
}

int termsIndex;
void geTermsByDV(int **terms, int *currentTerm, int degree, int varNum, int start, int level){
    if (level==0){
        termsIndex=0;
    }
    if (level==degree){
        for(int i=0; i<degree; i++){
            terms[termsIndex][i]=currentTerm[i];
        }
        termsIndex++;
    }else{
        for(int i=start; i<varNum; i++){
            currentTerm[level]=i;
            geTermsByDV(terms, currentTerm, degree, varNum, i, level+1);
        }
    }
}


