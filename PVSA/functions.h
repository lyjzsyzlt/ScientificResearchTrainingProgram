#ifndef functions_H
#define functions_H
#include"structs.h"
#include"defines.h"
#include<random>
#include<set>
#include<string>
using namespace std;

void createInitSol(Solution sol[]); // ������ʼ��

void displaySol(Solution sol[]); //�������������

void initJob(Job job[]); //��ʼ��������Ϣ

void displayJob(Job job[]); // ���������Ϣ���ļ�

void clearFile(string fileName); // ���ָ�����ļ�

void initMachine(); // ��ʼ��������Ϣ

void BFLPT(Job job[]); // ����

void displayBatch(Batch B[]); // �������Ϣ

Solution generateNewSol(Solution sol,Solution best); //��ǰ��sol�����µĿ��н�

int updateNDS(Solution sol, set<Solution> &NDS1);

void printSol(const Solution& sol);

void reverse(int a[], int begin, int end);
Solution twoPointsSwap(Solution sol, Solution best);
int getPos(Solution sol, int batch);

// ���̶��㷨
int RWS(double P[],int N,int flag[]);

// �����NDS��ѡ��һ����
Solution selectFromNDS(set<Solution> &NDS1);
#endif // !functions_H
