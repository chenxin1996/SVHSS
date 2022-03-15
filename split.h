#include "BV11.h"

#ifndef MVHSS_SPLIT_H
#define MVHSS_SPLIT_H

/*!
 * Split variables into shares
 * @param serShares: the variable shares of different servers
 * @param alphaSerShares: the variable multiplied by alpha shares of different servers
 * @param vars: the variables
 * @param alphaVars: the variables multiplied by alpha
 * @param alpha: the alpha
 * @param pa: the parameters from SVHSS
 * @param bvPara: the parameters from cryptosystem
 * @param bvPk: the public key from cryptosystem
 */
void split(share*** serShares, share*** alphaSerShares, fmpz_t* vars, fmpz_t* alphaVars, fmpz_t alpha, hPara pa, BV_Para* bvPara, BV_PK* bvPk);

#endif //MVHSS_SPLIT_H
