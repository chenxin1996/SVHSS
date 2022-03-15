#ifndef MVHSS_TOOL_H
#define MVHSS_TOOL_H

#include <memory.h>
#include <math.h>
#include <flint/fmpz_mod_poly.h>

typedef fmpz_mod_poly_t* cypher;

#define PI 3.1415926535898

#define DEBUG 0 //whether debug (0-no; 1-yes)
#define SOLO 1 //1-monomial; 0-polynomial
#define APPLICATION 0//whether specify the polynomial yourself (0-no; 1-yes)

typedef struct
{
    int m;//the number of all servers
    int d;//the maximum degree
    int t;//the number of bad servers
    int varNum;//the number of variables
    int k;//the maximum times of the multiplication of cyphertext;
    int sNumPerVar;//divide one variable into sNumPerVar(=choose t from m) shares
    int **sIndex;//the index of shares
} hPara;

typedef struct{
    int* sIndex;//the index of share
    int state;//only plaintext(0), only cypher(1), both(2)
    fmpz_t plain;//not encrypted data
    cypher cph;//encrypted data
} share;

typedef struct{
    int tNum;//the number of terms
    int* tDegree;//the vector of degree of terms
    int* tSymbol;//the vector of symbols(addition[+1] or subtraction[-1]) of terms
    int** tCon;//the content of terms
} func;

typedef struct{
    int conLen;//the number of variables in the term
    int symbol;//the symbol (addition[+1] or subtraction[-1]) of share term
    int* tCon;//the variable indices of the term
    int* sCon;//the share indices of the term
    int ser;//the server who is responsible for computing the term
} sTerm;

/*!
 * Generate some kind of Gauss
 * @param e: the noise
 * @param N: ciphertext mod x^N+1
 * @param ctx: the filed
 */
void GaussRand1(fmpz_mod_poly_t e, int N, fmpz_mod_ctx_t ctx);

/*!
 * Generate some kind of Gauss
 * @param e: the noise
 * @param N: ciphertext mod x^N+1
 * @param ctx: the filed
 */
void GaussRand2(fmpz_mod_poly_t e, int N, fmpz_mod_ctx_t ctx);

/*!
 * Convert fmpz_t to fmpz_mod_poly_t
 * @param out: the fmpz_mod_poly_t output
 * @param in: the fmpz_t input
 * @param bits: the number of bits
 * @param ctx: the field
 */
void Decimal2Bin(fmpz_mod_poly_t out, fmpz_t in, int bits, fmpz_mod_ctx_t ctx);

/*!
 * Generate combination
 * @param arr: the elements in the arr will be combined
 * @param n: the length of arr
 * @param r: the length of combination
 * @param index: the index of arr
 * @param data: the array stores the intermediate results
 * @param i: the index of data
 * @param result: all combination results
 */
void combinationUtil(int arr[], int n, int r, int index, int data[], int i, int **result);

/*!
 * Whether one number in the set
 * @param num: the number
 * @param set: the set
 * @param setSize: the size of sest
 * @return: 0 means one number is not in the set, 1 means in
 */
int isInSet(int num, int *set, int setSize);

/*!
 * Compute the Cartesian product between sets
 * @param resSet: the results
 * @param setLength: the length of set
 * @param currentSet: the one element in the results set
 * @param numSets: the number of sets
 * @param times: the number of done cartesian products
 */
void cartesianProduct(int** resSet, int setLength, int* currentSet, int numSets, int times);

/*!
 * Find the index of minimum value in the list
 * @param set: the list
 * @param setSize: the size of list
 * @return: the index of minimum value
 */
int findMinVIdx(int* set, int setSize);

/*!
 * Calculate how many possibilities are there to choose b from a
 * @param a: the total
 * @param b: number of choices
 * @return the number of possibilities
 */
int getComNum(int a, int b);
#endif //MVHSS_TOOL_H
