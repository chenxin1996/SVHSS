#include "BV11.h"
void BV_Gen(BV_Para *bvPara, BV_SK *bvSk, BV_PK *bvPk) {
    fmpz_init(bvPara->msg);
    fmpz_init(bvPara->t);
    fmpz_init(bvPara->q);

    //340282366920938463463374607431768211507
    //18446744073709551629
    fmpz_set_str(bvPara->msg, "713623846352979940529142984724747568191373381", 10);//64bits
    fmpz_set_str(bvPara->q, "713623846352979940529142984724747568191373381", 10);
    //fmpz_set_str(bvPara->msg, "1048583", 10);
//    fmpz_set_str(bvPara->q, "17976931348623159077293051907890247336179769789423065727343008115773267580550096313270847732240753602112011387987139335"
//                            "76587897688144166224928474306394741243777678934248654852763022196012460941194530829520850057688381506823424628814739131"
//                            "1054082723716335051068458629823994724593847971630483535632962422413785", 10);
    fmpz_t one;
    fmpz_init(one);
    fmpz_one(one);
    //fmpz_mul_2exp(bvPara->q,one,2048);
    fmpz_set(bvPara->t,bvPara->q);
    fmpz_mod_ctx_init(bvPara->ctx_t, bvPara->t);
    fmpz_mod_ctx_init(bvPara->ctx_q, bvPara->q);
    fmpz_mod_ctx_init(bvPara->ctx_msg, bvPara->msg);
    fmpz_mod_poly_init(bvPara->xN, bvPara->ctx_q);
    flint_randinit(bvPara->state);

    bvPara->N = 64;
    fmpz_mod_poly_set_coeff_ui(bvPara->xN, bvPara->N, 1, bvPara->ctx_q);
    fmpz_mod_poly_set_coeff_ui(bvPara->xN, 0, 1, bvPara->ctx_q);

    //sk
    fmpz_mod_poly_init(bvSk->s, bvPara->ctx_q);
    fmpz_mod_poly_init(bvSk->sqr_s, bvPara->ctx_q);
    fmpz_mod_poly_init(bvSk->cube_s, bvPara->ctx_q);
    GaussRand1(bvSk->s, bvPara->N, bvPara->ctx_q);
    //SK_Distr(bvSk->s,bvPara->N,bvPara->ctx_q);
    fmpz_mod_poly_mulmod(bvSk->sqr_s, bvSk->s, bvSk->s, bvPara->xN, bvPara->ctx_q);
    fmpz_mod_poly_mulmod(bvSk->cube_s, bvSk->sqr_s, bvSk->s, bvPara->xN, bvPara->ctx_q);
    fmpz_mod_poly_mulmod(bvSk->pow4_s, bvSk->cube_s, bvSk->s, bvPara->xN, bvPara->ctx_q);
    //pk
    fmpz_mod_poly_t e0;
    fmpz_mod_poly_init(e0, bvPara->ctx_q);

    fmpz_mod_poly_init(bvPk->a0, bvPara->ctx_q);
    fmpz_mod_poly_init(bvPk->b0, bvPara->ctx_q);
    fmpz_mod_poly_randtest_not_zero(bvPk->a0, bvPara->state, bvPara->N, bvPara->ctx_q);
    GaussRand1(e0, bvPara->N, bvPara->ctx_q);

    fmpz_mod_poly_scalar_mul_fmpz(e0, e0, bvPara->t, bvPara->ctx_q);
    fmpz_mod_poly_mulmod(bvPk->b0, bvPk->a0, bvSk->s, bvPara->xN, bvPara->ctx_q);
    fmpz_mod_poly_add(bvPk->b0, bvPk->b0, e0, bvPara->ctx_q);
}

void BV_Enc(fmpz_mod_poly_t *c, fmpz_t x, BV_Para *bvPara, BV_PK *bvPk) {
    fmpz_mod_poly_t temp, v, e1, e2;
    fmpz_mod_poly_init(temp, bvPara->ctx_q);
    fmpz_mod_poly_init(v, bvPara->ctx_q);
    fmpz_mod_poly_init(e1, bvPara->ctx_q);
    fmpz_mod_poly_init(e2, bvPara->ctx_q);
    GaussRand1(v, bvPara->N, bvPara->ctx_q);
    GaussRand1(e1, bvPara->N, bvPara->ctx_q);
    GaussRand2(e2, bvPara->N, bvPara->ctx_q);

    //
    fmpz_mod_poly_t x_bin;
    fmpz_mod_poly_init(x_bin, bvPara->ctx_q);
    fmpz_mod_poly_set_coeff_fmpz(x_bin, 0, x, bvPara->ctx_q);


    fmpz_mod_poly_mulmod(c[0], bvPk->b0, v, bvPara->xN, bvPara->ctx_q);
    fmpz_mod_poly_scalar_mul_fmpz(temp, e2, bvPara->t, bvPara->ctx_q);
    fmpz_mod_poly_add(c[0], c[0], temp, bvPara->ctx_q);
    fmpz_mod_poly_add(c[0], c[0], x_bin, bvPara->ctx_q);

    fmpz_mod_poly_mulmod(c[1], bvPk->a0, v, bvPara->xN, bvPara->ctx_q);
    fmpz_mod_poly_scalar_mul_fmpz(temp, e1, bvPara->t, bvPara->ctx_q);
    fmpz_mod_poly_add(c[1], c[1], temp, bvPara->ctx_q);
    fmpz_mod_poly_neg(c[1],c[1],bvPara->ctx_q);

    fmpz_mod_poly_clear(temp, bvPara->ctx_q);
    fmpz_mod_poly_clear(v, bvPara->ctx_q);
    fmpz_mod_poly_clear(e1, bvPara->ctx_q);
    fmpz_mod_poly_clear(e2, bvPara->ctx_q);
    fmpz_mod_poly_clear(x_bin, bvPara->ctx_q);
}

void BV_Scalar_Mult(fmpz_mod_poly_t *res, fmpz_mod_poly_t *c1, fmpz_t c, int level, BV_Para *bvPara) {
    for(int i=0;i<level+1;i++)
    {
        fmpz_mod_poly_scalar_mul_fmpz(res[i],c1[i],c,bvPara->ctx_q);
    }
}

void BV_Mult(fmpz_mod_poly_t *res, fmpz_mod_poly_t *c1, fmpz_mod_poly_t *c2, int mut_level, BV_Para *bvPara)
{
    if(mut_level==1)
    {
        //res[1]
        fmpz_mod_poly_mulmod(res[0], c1[0], c2[1], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(res[1], c1[1], c2[0], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_add(res[1], res[0], res[1], bvPara->ctx_q);
        //res[0]
        fmpz_mod_poly_mulmod(res[0], c1[0], c2[0], bvPara->xN, bvPara->ctx_q);
        //res[2]
        fmpz_mod_poly_mulmod(res[2], c1[1], c2[1], bvPara->xN, bvPara->ctx_q);
    }else if (mut_level==2)
    {
        //res[1]
        fmpz_mod_poly_mulmod(res[0], c1[0], c2[1], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(res[1], c1[1], c2[0], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_add(res[1], res[0], res[1], bvPara->ctx_q);
        //res[2]
        fmpz_mod_poly_mulmod(res[2], c1[1], c2[1], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(res[0], c1[0], c2[2], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_add(res[2], res[0], res[2], bvPara->ctx_q);
        //res[3]
        fmpz_mod_poly_mulmod(res[3], c1[1], c2[2], bvPara->xN, bvPara->ctx_q);
        //res[0]
        fmpz_mod_poly_mulmod(res[0], c1[0], c2[0], bvPara->xN, bvPara->ctx_q);
    }else if (mut_level==3)
    {
        //res[1]
        fmpz_mod_poly_mulmod(res[0], c1[0], c2[1], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(res[1], c1[1], c2[0], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_add(res[1], res[0], res[1], bvPara->ctx_q);
        //res[2]
        fmpz_mod_poly_mulmod(res[2], c1[1], c2[1], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(res[0], c1[0], c2[2], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_add(res[2], res[0], res[2], bvPara->ctx_q);
        //res[3]
        fmpz_mod_poly_mulmod(res[3], c1[1], c2[2], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(res[0], c1[0], c2[3], bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_add(res[3], res[0], res[3], bvPara->ctx_q);
        //res[0]
        fmpz_mod_poly_mulmod(res[0], c1[0], c2[0], bvPara->xN, bvPara->ctx_q);
        //res[4]
        fmpz_mod_poly_mulmod(res[4], c1[1], c2[3], bvPara->xN, bvPara->ctx_q);
    }
}

void BV_Add(fmpz_mod_poly_t *res, fmpz_mod_poly_t *c1, fmpz_mod_poly_t *c2,int level, BV_Para *bvPara) {
    for(int i=0;i<level+1;i++)
    {
        fmpz_mod_poly_add(res[i],c1[i],c2[i],bvPara->ctx_q);
    }
}

void BV_Sub(fmpz_mod_poly_t *res, fmpz_mod_poly_t *c1, fmpz_mod_poly_t *c2,int level, BV_Para *bvPara) {
    for(int i=0;i<level+1;i++)
    {
        fmpz_mod_poly_sub(res[i],c1[i],c2[i],bvPara->ctx_q);
    }
}

void BV_Dec(fmpz_t x, fmpz_mod_poly_t *c, int level, BV_Para *bvPara, BV_SK *bvSk) {
    fmpz_mod_poly_t temp1, temp2,temp3,temp4;
    fmpz_mod_poly_init(temp1, bvPara->ctx_q);
    fmpz_mod_poly_init(temp2, bvPara->ctx_q);
    fmpz_mod_poly_init(temp3, bvPara->ctx_q);
    fmpz_mod_poly_init(temp4, bvPara->ctx_q);
    if (level == 1) {
        fmpz_mod_poly_mulmod(temp1, c[1], bvSk->s, bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_add(temp1, temp1, c[0], bvPara->ctx_q);
        fmpz_mod_poly_get_coeff_fmpz(x,temp1,0, bvPara->ctx_q);
        fmpz_mod(x,x,bvPara->msg);
    }
    if (level == 2) {
        fmpz_mod_poly_mulmod(temp1, c[1], bvSk->s, bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(temp2, c[2], bvSk->sqr_s, bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_add(temp1, temp1, c[0], bvPara->ctx_q);
        fmpz_mod_poly_add(temp1, temp1, temp2, bvPara->ctx_q);
        fmpz_mod_poly_get_coeff_fmpz(x,temp1,0, bvPara->ctx_q);
        fmpz_mod(x,x,bvPara->msg);
    }
    if (level == 3) {
        fmpz_mod_poly_mulmod(temp1, c[1], bvSk->s, bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(temp2, c[2], bvSk->sqr_s, bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(temp3, c[3], bvSk->cube_s, bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_add(temp1, temp1, c[0], bvPara->ctx_q);
        fmpz_mod_poly_add(temp1, temp1, temp2, bvPara->ctx_q);
        fmpz_mod_poly_add(temp1, temp1, temp3, bvPara->ctx_q);
        fmpz_mod_poly_get_coeff_fmpz(x,temp1,0, bvPara->ctx_q);
        fmpz_mod(x,x,bvPara->msg);
    }
    if (level == 4) {
        fmpz_mod_poly_mulmod(temp1, c[1], bvSk->s, bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(temp2, c[2], bvSk->sqr_s, bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(temp3, c[3], bvSk->cube_s, bvPara->xN, bvPara->ctx_q);
        fmpz_mod_poly_mulmod(temp4, c[4], bvSk->pow4_s, bvPara->xN, bvPara->ctx_q);

        fmpz_mod_poly_add(temp1, temp1, c[0], bvPara->ctx_q);
        fmpz_mod_poly_add(temp1, temp1, temp2, bvPara->ctx_q);
        fmpz_mod_poly_add(temp1, temp1, temp3, bvPara->ctx_q);
        fmpz_mod_poly_add(temp1, temp1, temp4, bvPara->ctx_q);
        fmpz_mod_poly_get_coeff_fmpz(x,temp1,0, bvPara->ctx_q);
        fmpz_mod(x,x,bvPara->msg);
    }
}