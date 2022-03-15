#include "tool.h"

#ifndef MVHSS_BV11_H
#define MVHSS_BV11_H

typedef struct {
    int N;
    fmpz_t t;
    fmpz_t q;
    fmpz_t msg;
    fmpz_mod_ctx_t ctx_q;
    fmpz_mod_ctx_t ctx_t;
    fmpz_mod_ctx_t ctx_msg;
    fmpz_mod_poly_t xN;

    flint_rand_t state;
    int num_item;
    int num_data;
} BV_Para;

typedef struct {
    fmpz_mod_poly_t a0;
    fmpz_mod_poly_t b0;
} BV_PK;

typedef struct {
    fmpz_mod_poly_t s;
    fmpz_mod_poly_t sqr_s;
    fmpz_mod_poly_t cube_s;
    fmpz_mod_poly_t pow4_s;
} BV_SK;

/*!
 * Generate parameters
 * @param bvPara: the parameters
 * @param bvSk: the secret key
 * @param bvPk: the public key
 */
void BV_Gen(BV_Para *bvPara, BV_SK *bvSk, BV_PK *bvPk);

/*!
 * Encrypt algorithm
 * @param c: the result ciphertext
 * @param x: the input plaintext
 * @param bvPara: the parameters
 * @param bvPk: the public key
 */
void BV_Enc(fmpz_mod_poly_t *c, fmpz_t x, BV_Para *bvPara, BV_PK *bvPk);

/*!
 * Multiply scalar with ciphertext
 * @param res: the computation result
 * @param c1: the ciphertext
 * @param c: the scalar
 * @param level: the level of the computation which depends on the length of ciphertext
 * @param bvPara: the parameters
 */
void BV_Scalar_Mult(fmpz_mod_poly_t *res, fmpz_mod_poly_t *c1, fmpz_t c, int level, BV_Para *bvPara);

/*!
 * Multiply ciphertext with ciphertext
 * @param res: the computation result
 * @param c1: one ciphertext
 * @param c2: another ciphertext
 * @param mut_level: the level of multiplication computation which depends on the length of ciphertext
 * @param bvPara: the parameters
 */
void BV_Mult(fmpz_mod_poly_t *res, fmpz_mod_poly_t *c1, fmpz_mod_poly_t *c2, int mut_level, BV_Para *bvPara);

/*!
 * The addition of ciphertexts
 * @param res: the computation result
 * @param c1: one ciphertext
 * @param c2: another ciphertext
 * @param level: the level of computation which depends on the length of ciphertext
 * @param bvPara: the parameters
 */
void BV_Add(fmpz_mod_poly_t *res, fmpz_mod_poly_t *c1, fmpz_mod_poly_t *c2,int level, BV_Para *bvPara);

/*!
 * The addition of ciphertexts
 * @param res: the computation result
 * @param c1: the minuend
 * @param c2: the subtraction
 * @param level: the level of computation which depends on the length of ciphertext
 * @param bvPara: the parameters
 */
void BV_Sub(fmpz_mod_poly_t *res, fmpz_mod_poly_t *c1, fmpz_mod_poly_t *c2,int level, BV_Para *bvPara);

/*!
 * Decrypt algorithm
 * @param x: the output plaintext
 * @param c: the input ciphertext
 * @param level: the level of decryption which depends on the length of ciphertext
 * @param bvPara: the parameters
 * @param bvSk: the secert key
 */
void BV_Dec(fmpz_t x, fmpz_mod_poly_t *c, int level, BV_Para *bvPara, BV_SK *bvSk);

#endif //MVHSS_BV11_H
