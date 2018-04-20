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

set<Solution> updateNDS(Solution sol, set<Solution> NDS);

void printSol(const Solution& sol);
#endif // !functions_H
