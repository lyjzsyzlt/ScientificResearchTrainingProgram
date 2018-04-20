#ifndef LS_H
#define LS_H
#include"structs.h"
#include"functions.h"
void getFitness(Solution& sol, Batch B[]);
void Right_Shift(Solution& sol, Batch B[]);
int getEPC1(int t, int detaT, int k, int i);
void cpMatrix(Batch a[], Batch b[], int len);
int min(int a, int b, int c);
bool cTDes(const Batch &b1, const Batch &b2);
bool cTASC(const Batch &b1, const Batch &b2);
void cpMatrix(int a[], int b[], int len);
#endif // !LS_H
