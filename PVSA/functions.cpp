#include "stdafx.h"
#include"functions.h"
#include"LS.h"
#include<iostream>
#include<fstream>
#include<time.h>
using namespace std;

int batchNum=0; // 批的个数

//按照第一阶段的加工时间排序
bool PtAsc(const Job &J1, const Job &J2)
{
	return J1.JobPT[1] > J2.JobPT[1];
}

void createInitSol(Solution sol[]) {
	float Vmk= rand() / float(RAND_MAX);
	float Vepc= 1 - Vmk;
	memset(sol, 0, sizeof(sol));
	for (int i = 1; i <= solNum; i++) {
		sol[i].Vmk = Vmk;
		sol[i].Vepc = Vepc;

		// 产生初始的随机序列
		sol[i].batchSeq[0] = -1;

		int flag[101];
		memset(flag, 0, sizeof(flag));
		for (int j = 1; j <= batchNum; j++) {
			int x = rand() % batchNum + 1;
			while (flag[x] != 0) {
				x = rand() % batchNum + 1;
			}
			flag[x] = 1;
			sol[i].batchSeq[j] = x;
		}
		sol[i].Cmax = -1;
		sol[i].EPC = -1;
		sol[i].F = -1;
	}
}

void displaySol(Solution sol[])
{
	for (int i = 1; i <= solNum; i++) {
		cout << "********************第" << i << "个解*************************" << endl;
		cout << sol[i].Vmk << '\t' << sol[i].Vepc << endl;
		
		for (int j = 1; j <= batchNum; j++) {
			cout << sol[i].batchSeq[j] << " ";
		}
		cout << endl;

		cout << "Cmax=" << sol[i].Cmax << '\t' << "EPC=" << sol[i].EPC << '\t' << "F=" << sol[i].F << endl<<endl;
	}
}

void initJob(Job job[])
{
	memset(job, 0, sizeof(job)); //清空数组
	default_random_engine dre((unsigned)time(NULL));// 初始化随机数引擎
	uniform_int_distribution<int> size(1, 10);// 使用给出的范围初始化分布类
	cout << "i=" << endl;
	for (int i = 1; i <= JobNum; i++) {
		Job temp;
		temp.JobId = i;
		temp.JobSize = size(dre);
		uniform_int_distribution<int> pt(10, 50);
		for (int k = 1; k <= kmax; k++) {
			temp.JobPT[k] = pt(dre);
		}
		job[i] = temp;
	}
}

void displayJob(Job job[]) 
{
	ofstream outJob("jobInfo.txt", ios::app);
	clearFile("jobInfo.txt");

	outJob << "工件id" << '\t' << "工件大小" << '\t';
	for (int i = 1; i < kmax + 1; i++)
		outJob << "第" << i << "阶段名义加工时间" << '\t';
	outJob << endl;

	for (int i = 1; i <= JobNum; i++) {
		outJob << job[i].JobId << '\t' << job[i].JobSize << '\t';
		for (int k = 1; k <= kmax; k++) {
			outJob << job[i].JobPT[k] << '\t';
		}
		outJob << endl;
	}
}

void clearFile(string fileName)
{
	ofstream out(fileName, ios::ate);
	out << "";
}

Machine **M;
int machineNum[kmax + 1];//存放每个阶段的机器数量

void initMachine()
{
	ifstream  in2;

	//初始化每个阶段机器的数量
	in2.open("machineNum.txt", ios::in | ios::binary);
	for (int i = 1; i<kmax + 1; i++)
		in2 >> machineNum[i];

	M = new Machine*[kmax + 1];
	for (int i = 0; i < kmax + 1; i++)
		M[i] = new Machine[10];
	M[0][0].PW = 1;

	default_random_engine dre((unsigned)time(NULL));// 初始化随机数引擎
	uniform_int_distribution<int> v1(1, 5);// 使用给出的范围初始化分布类
	uniform_int_distribution<int> pw(5, 10);
	clearFile("machineInfo.txt");

	ofstream out("machineInfo.txt", ios::app);
	out << "阶段" << '\t' << "机器号" << '\t' << "速度" << '\t' << "功率" << endl;

	/*M[1][1].V = 1;
	M[1][1].PW = 7;
	M[1][2].V = 4;
	M[1][2].PW = 8;

	M[2][1].V = 5;
	M[2][1].PW = 8;
	M[2][2].V = 3;
	M[2][2].PW = 5;*/
	for (int i = 1; i < kmax + 1; i++)
	{

		for (int j = 1; j < machineNum[i] + 1; j++)
		{
			out << i << '\t';
			out << j << '\t';
			M[i][j].V = v1(dre);
			M[i][j].PW = pw(dre);
			M[i][j].avt = 0;
			M[i][j].List.clear();

			out << M[i][j].V << '\t' << M[i][j].PW << endl;
		}
	}
}


void getPbk(Job J[], Batch B[])
{
	for (int i = 1; i <= batchNum; i++) //第i个批
	{
		for (int p = 1; p < kmax + 1; p++)// 第p个阶段
		{
			int maxPT = -RAND_MAX;
			for (int j = 1; j <= B[i].JNum; j++)// 第j个工件
			{
				if (B[i].BJob[j].JobPT[p] > maxPT)
				{
					maxPT = B[i].BJob[j].JobPT[p];
				}
			}
			B[i].BP[p] = maxPT;
		}
	}
}

Batch B[100]; //批的集合
//创建一个批
Batch createBatch(int batchId)
{
	Batch temp;
	for (int i = 1; i < kmax + 1; i++)
	{
		temp.BId = batchId;
		temp.BP[i] = -1;
		temp.freeSpace = C;
		temp.JNum = 0;
		temp.flag = 1;
		temp.BC[0] = 0;
	}
	return temp;
}

void BFLPT(Job job[])
{
	memset(B, 0, sizeof(B));
	sort(job+1, job+JobNum+1, PtAsc);//对工件按照加工时间非递增顺序排序

	/*ofstream sortJob("jobInfo.txt", ios::app);
	sortJob << "//排序后的工件信息" << endl;
	displayJob(job);*/

	// 使得B是从1开始编号，0号无效
	Batch temp;
	temp=createBatch(-1);
	temp.freeSpace = -10;
	temp.flag = 0;
	B[0] = temp;
	int initBid = 1;//批的初始编号
	for (int j = 1; j <= JobNum; j++) {
		int min = RAND_MAX;
		int Bid = -1;
		for (int b = 1; b <= batchNum; b++) {
			int diff = B[b].freeSpace - job[j].JobSize;
			if (diff>=0) {
				if (diff < min) {
					min = diff;
					Bid = b;
				}
			}
		}

		if (Bid == -1){
			Batch tempB;
			tempB=createBatch(initBid);
			tempB.freeSpace -= job[j].JobSize;
			tempB.JNum++;
			tempB.BJob[tempB.JNum]=job[j];
			initBid++;
			batchNum++;
			B[batchNum]=tempB;
		}
		else{
			B[Bid].freeSpace -=job[j].JobSize;
			B[Bid].JNum++;
			B[Bid].BJob[B[Bid].JNum]=job[j];
		}
	}

	getPbk(job, B);
	displayBatch(B);
}

void displayBatch(Batch B[])
{
	for (int i = 1; i<=batchNum; i++) {
		cout << "第" << i << "个批的编号：" << B[i].BId << endl;;
		cout << "第" << i << "个批所含工件数：" << B[i].JNum << endl;
		cout << "第" << i << "个批所包含的工件为(编号）:";

		for (int p = 1; p <= B[i].JNum; p++) {
			cout << B[i].BJob[p].JobId << "  ";
		}
		cout << endl;

		for (int j = 1; j < kmax + 1; j++) {//k个阶段
			cout << "    第" << j << "阶段的名义加工时间:" << B[i].BP[j] << endl;
		}
	}
}

/*Solution generateNewSol(Solution sol)
{
	int s1 = rand() % (batchNum - 1) + 1;
	int s2 = rand() % (batchNum - 1) + 1;

	if (s1 > s2)
		swap(s1, s2);

	int batchSeq[105];
	cpMatrix(batchSeq, sol.batchSeq, batchNum);
	int c = 1;
	for (int i = s2; i <= batchNum; i++)
	{
		sol.batchSeq[c++]=batchSeq[i];
	}

	for (int i = 1; i < s1; i++)
		sol.batchSeq[c++]=batchSeq[i];

	for (int i = s1; i < s2; i++)
		sol.batchSeq[c++]=batchSeq[i];

	return sol;
}
*/

/*Solution generateNewSol(Solution sol)
{
	int s1 = rand() % (batchNum - 1) + 1;
	int s2 = rand() % (batchNum - 1) + 1;
	int temp;
	temp = sol.batchSeq[s1];
	sol.batchSeq[s1] = sol.batchSeq[s2];
	sol.batchSeq[s2] = temp;
	return sol;
}*/

Solution generateNewSol(Solution sol,Solution best)
{

	int s1 = rand() % (batchNum - 1) + 1;
	int s2 = rand() % (batchNum - 1) + 1;
	int temp;
	temp = sol.batchSeq[s1];
	sol.batchSeq[s1] = sol.batchSeq[s2];
	sol.batchSeq[s2] = temp;


	 s1 = rand() % (batchNum - 2) + 2;
	 s2 = rand() % (batchNum - 2) + 2;

	while (s1==s2)
	{
		s1 = rand() % (batchNum - 2) + 2;
		s2 = rand() % (batchNum - 2) + 2;
	}

	if (s1 > s2)
		swap(s1, s2);

	//cout << "s1=" << s1 << "  " << "s2=" << s2 << endl;

	/*cout << "best=" << endl;
	for (int i = 1; i <= batchNum; i++)
		cout << best.batchSeq[i] << "  ";
	cout << endl;
	cout << "sol=" << endl;
	for (int i = 1; i <= batchNum; i++)
		cout << sol.batchSeq[i] << " ";
	cout << endl;
	*/
	for (int i = s1; i <= s2; i++)
	{
		int temp;
		temp = best.batchSeq[i];
		best.batchSeq[i] = sol.batchSeq[i];
		sol.batchSeq[i] = temp;
	}

	int c1[105], c2[105];
	memset(c1, 0, sizeof(c1));
	memset(c2, 0, sizeof(c2));

	for (int i = 1; i <= batchNum; i++)
	{
		c1[best.batchSeq[i]]++;
		c2[sol.batchSeq[i]]++;
	}

	for (int i = 1; i <= batchNum; i++)
	{
		if (c2[sol.batchSeq[i]] == 2)
		{
			for (int j = 1; j <= batchNum; j++)
			{
				if (c1[best.batchSeq[j]] == 2)
				{
					c1[best.batchSeq[j]]--;
					c2[sol.batchSeq[i]]--;
					sol.batchSeq[i] = best.batchSeq[j];
					break;
				}
			}
		}
	}
	return sol;
}
set<Solution> updateNDS(Solution sol, set<Solution> NDS)
{
	Solution tempSol = sol;

	int flag = 0;
	set<Solution>::iterator it = NDS.begin();
	for (; it != NDS.end(); )
	{
		if (tempSol.Cmax > (*it).Cmax&&tempSol.EPC > (*it).EPC) {
			flag = 1;
			break;
		}
		else if (tempSol.Cmax < (*it).Cmax&&tempSol.EPC < (*it).EPC) {
			it = NDS.erase(it);
		}
		else if (tempSol.Cmax == (*it).Cmax&&tempSol.EPC == (*it).EPC) {
			flag = 1;
			break;
		}
		else if (tempSol.Cmax == (*it).Cmax || (*it).EPC == tempSol.EPC) {
			if (tempSol.Cmax >(*it).Cmax || tempSol.EPC >(*it).EPC) {
				flag = 1;
				break;
			}
			else {
				it = NDS.erase(it);
			}
		}
		else
			it++;
	}
	if (flag == 0)
		NDS.insert(tempSol);

	return NDS;
}

void printSol(const Solution & sol)
{
	for (int i = 1; i <= batchNum; i++)
		cout << sol.batchSeq[i] << "  ";
	cout << endl;
}
