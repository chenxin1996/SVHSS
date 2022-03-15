#ifndef MVHSS_COMPUTE_H
#define MVHSS_COMPUTE_H

#include "BV11.h"

/*!
 * Compute result by servers
 * @param res: the computation result
 * @param serShares: the variable shares of different servers
 * @param sTerms: the share terms which will be computed
 * @param allSTermNum: the number of share terms
 * @param pa: the parameters from SVHSS
 * @param bvPara: the parameters from cryptosystem
 * @param bvPk: the public key from cryptosystem
 */
void compute(cypher *res, share*** serShares, sTerm* sTerms, long long allSTermNum, hPara pa, BV_Para* bvPara, BV_PK* bvPk);

/*!
 * For verification, compute result multiplied by alpha
 * @param alphaRes: the computation result
 * @param serShares: the variable shares of different servers
 * @param alphaSerShares: the variable multiplied by alpha shares of different servers
 * @param sTerms: the share terms which will be computed
 * @param allSTermNum: the number of share terms
 * @param pa: the parameters from SVHSS
 * @param bvPara: the parameters from cryptosystem
 * @param bvPk: the public key from cryptosystem
 */
void alphaCompute(cypher *alphaRes, share*** serShares, share*** alphaSerShares, sTerm* sTerms, long long allSTermNum, hPara pa, BV_Para* bvPara, BV_PK* bvPk);

/*!
 * Compute directly
 * @param directRes: the computation result
 * @param aFunc: the polynomial which will be computed
 * @param vars: the variables
 * @param bvPara: the parameters from cryptosystem
 */
void directCompute(fmpz_t directRes, func aFunc, fmpz_t *vars, BV_Para* bvPara);

/*!
 * Generate combination
 * @param allResSet: the results
 * @param maxDg: the size of one element in the result
 * @param pa: the parameters from SVHSS
 */
void shareCombine(int ***allResSet, int maxDg, hPara pa);

/*!
 * Find the index which the share index will be merged into
 * @param mSIndex: the list of index
 * @param index: the share index will be merged
 * @param pa: the parameters from SVHSS
 * @return
 */
int findMergeInto(int **mSIndex, int *index, hPara pa);

/*!
 * Compute all terms
 * @param aFunc: the polynomial
 * @param tNumByD: the number of terms
 * @param varNum: the number of variables
 * @param maxDg: the maximum degree
 */
void allTermsByDV(func aFunc, int *tNumByD, int varNum, int maxDg);

/*!
 * Compute terms
 * @param terms: the result terms
 * @param currentTerm: one term
 * @param degree: the degree of the term
 * @param varNum: the number of variables
 * @param start: the start index
 * @param level: the index in the current
 */
void geTermsByDV(int **terms, int *currentTerm, int degree, int varNum, int start, int level);
#endif //MVHSS_COMPUTE_H
