#include "SVHSS.h"

int main() {
    BV_Para *bvPara = (BV_Para *) (malloc(sizeof(*bvPara)));
    BV_SK *bvSk = (BV_SK *) (malloc(sizeof(*bvSk)));
    BV_PK *bvPk = (BV_PK *) (malloc(sizeof(*bvPk)));
    BV_Gen(bvPara, bvSk, bvPk);

    hPara pa;
    int repeatTime;

    if (APPLICATION){
        printf("Please input m, k, t and repeat times in order, e.g., 4 1 1 10\n");
        scanf("%d %d %d %d", &pa.m, &pa.k, &pa.t, &repeatTime);
        printf("m=%d k=%d t=%d repeatTime=%d\n", pa.m, pa.k, pa.t, repeatTime);
    }else{
        printf("Please input m, k, t, d, the number of variables and repeat times in order, e.g., 3 4 2 5 1 10\n");
        scanf("%d %d %d %d %d %d", &pa.m, &pa.k, &pa.t, &pa.d, &pa.varNum, &repeatTime);
        printf("m=%d k=%d t=%d d=%d varNum=%d repeatTime=%d\n", pa.m, pa.k, pa.t, pa.d, pa.varNum, repeatTime);
    }
    pa.sNumPerVar = getComNum(pa.m, pa.t);

    basicFunction(&pa, bvPara);
//    appF1(&pa);
//    appF2(&pa);
//    appF3(&pa);
//    appF4(&pa);
//    appF5(&pa);
//    appF6(&pa);
//    appF7(&pa);
//    appF8(&pa);

    SVHSS(repeatTime, pa, bvPara, bvPk, bvSk);

    return 0;
}
