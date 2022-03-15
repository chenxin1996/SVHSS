#ifndef SVHSS_SVHSS_H
#define SVHSS_SVHSS_H

#include "split.h"
#include "compute.h"

/*!
 * Generate the basic polynomial according parameters
 * @param pa: the parameters from SVHSS
 * @param bvPara: the parameters from cryptosystem
 */
void basicFunction(hPara *pa, BV_Para *bvPara);

/*!
 * Common sentences in the functions of applications
 * @param varNum: the number of variables
 */
void generateFrame(int varNum);

/*!
 * Common sentences in the functions of applications
 * @param sNumPerVar: the number of shares for one variable
 */
void computeNum(int sNumPerVar);

/*!
 * The first polynomial in the application
 * @param pa: the parameters from SVHSS
 */
void appF1(hPara *pa);

/*!
 * The second polynomial in the application
 * @param pa: the parameters from SVHSS
 */
void appF2(hPara *pa);

/*!
 * The third polynomial in the application
 * @param pa: the parameters from SVHSS
 */
void appF3(hPara *pa);

/*!
 * The fourth polynomial in the application
 * @param pa: the parameters from SVHSS
 */
void appF4(hPara *pa);

/*!
 * The fifth polynomial in the application
 * @param pa: the parameters from SVHSS
 */
void appF5(hPara *pa);

/*!
 * The sixth polynomial in the application
 * @param pa: the parameters from SVHSS
 */
void appF6(hPara *pa);

/*!
 * The seventh polynomial in the application
 * @param pa: the parameters from SVHSS
 */
void appF7(hPara *pa);

/*!
 * The eighth polynomial in the application
 * @param pa: the parameters from SVHSS
 */
void appF8(hPara *pa);

/*!
 * The running process of SVHSS scheme
 * @param repeatTime: the number of times the algorithm is repeated
 * @param pa: the parameters from SVHSS
 * @param bvPara: the parameters from cryptosystem
 * @param bvPk: the public key from cryptosystem
 * @param bvSk: the secret key from cryptosystem
 */
void SVHSS(int repeatTime, hPara pa, BV_Para* bvPara, BV_PK* bvPk, BV_SK* bvSk);
#endif //SVHSS_SVHSS_H
