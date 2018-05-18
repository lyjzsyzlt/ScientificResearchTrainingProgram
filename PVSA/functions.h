#ifndef functions_H
#define functions_H
#include"structs.h"
#include"defines.h"
#include<random>
#include<set>
#include<string>
using namespace std;

void createInitSol(Solution sol[]); // 产生初始解

void displaySol(Solution sol[]); //输出各个解的情况

void initJob(Job job[]); //初始化工件信息

void displayJob(Job job[]); // 输出工件信息到文件

void clearFile(string fileName); // 清空指定的文件

void initMachine(); // 初始化机器信息

void BFLPT(Job job[]); // 分批

void displayBatch(Batch B[]); // 输出批信息

Solution generateNewSol(Solution sol,Solution best); //当前解sol产生新的可行解

int updateNDS(Solution sol, set<Solution> &NDS1);

void printSol(const Solution& sol);

void reverse(int a[], int begin, int end);
Solution twoPointsSwap(Solution sol, Solution best);
int getPos(Solution sol, int batch);

// 轮盘赌算法
int RWS(double P[],int N,int flag[]);

// 随机从NDS中选择一个解
Solution selectFromNDS(set<Solution> &NDS1);
#endif // !functions_H
