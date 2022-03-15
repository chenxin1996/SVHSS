#include "tool.h"
#define PI 3.1415926535898

void GaussRand1(fmpz_mod_poly_t e, int N, fmpz_mod_ctx_t ctx) {
    double res_standard;
    int deviation = 8;
    int res;
    for (int i = 0; i < N; i++) {
        res_standard = sqrt(-2.0 * log(rand() / (RAND_MAX + 1.0))) * sin(2.0 * PI * rand() / (RAND_MAX + 1.0));
        res = res_standard * deviation;
        fmpz_mod_poly_set_coeff_si(e, i, res, ctx);
    }
}

void GaussRand2(fmpz_mod_poly_t e, int N, fmpz_mod_ctx_t ctx) {
    double res_standard;
    int deviation = 512;
    int res;
    for (int i = 0; i < N; i++) {
        res_standard = sqrt(-2.0 * log(rand() / (RAND_MAX + 1.0))) * sin(2.0 * PI * rand() / (RAND_MAX + 1.0));
        res = res_standard * deviation;
        fmpz_mod_poly_set_coeff_si(e, i, res, ctx);
    }
}


void Decimal2Bin(fmpz_mod_poly_t out, fmpz_t in, int bits, fmpz_mod_ctx_t ctx) {
    fmpz_t rem;
    fmpz_init(rem);
    for (int i = 0; i < bits; i++) {
        fmpz_mod_ui(rem,in,2);
        fmpz_mod_poly_set_coeff_fmpz(out, i, rem,ctx);
        fmpz_sub(in,in,rem);
        fmpz_divexact_ui(in,in,2);
    }
    fmpz_clear(rem);
}

int num;//for storing the result of combination
void combinationUtil(int arr[], int n, int r, int index, int data[], int i, int **result)
{
    if (i==0)
    {
        num=0;
    }
    // Current combination is ready, print it
    if (index == r)
    {
        for (int j=0; j<r; j++)
        {
            result[num][j]=data[j];
        }
        num++;
        return;
    }
    // When no more elements are there to put in data[]
    if (i >= n)
        return;
    // current is included, put next at next location
    data[index] = arr[i];
    combinationUtil(arr, n, r, index+1, data, i+1, result);

    // current is excluded, replace it with next (Note that
    // i+1 is passed, but index is not changed)
    combinationUtil(arr, n, r, index, data, i+1, result);
}

int isInSet(int num, int *set, int setSize)
{
    for(int i=0;i<setSize;i++)
    {
        if (num==set[i])
            return 1;
    }
    return 0;
}

int counter;
void cartesianProduct(int** resSet, int setLength, int* currentSet, int numSets, int times){
    int i,j;

    if (times==0)
    {
        counter=0;
    }

    if(times==numSets){
        int nowIndex = counter++;
        for(i=0;i<times;i++){
            resSet[nowIndex][i]=currentSet[i];
        }
    }else{
        for(j=0;j<setLength;j++){
            currentSet[times] = j;
            cartesianProduct(resSet, setLength,currentSet,numSets,times+1);
        }
    }
}

int findMinVIdx(int* set, int setSize)
{
    int minV = set[0];
    int minVIdx = 0;
    for(int i=1; i<setSize; i++)
    {
        if(set[i]<minV)
        {
            minV = set[i];
            minVIdx = i;
        }
    }
    return minVIdx;
}

//the number of all possibilities for choosing b from a
int getComNum(int a, int b)
{
    double divided=1, divide=1;
    for(int i=0;i<b;i++)
    {
        divided *= a-i;
        divide *= b-i;
    }
    return (int)(divided/divide);
}

