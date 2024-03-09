#include "globalutils.h"

GlobalUtils::GlobalUtils(){

}

//获取随机数 0 ~ 200之间
int GlobalUtils::getRandomNum(uint seed,int limit){

    //设置随机数种子
    qsrand(seed);

    //返回一个随机数
    return qrand()%limit;
}
