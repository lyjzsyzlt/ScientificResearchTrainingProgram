#include "stdafx.h"
#include "LS.h"
#include<iostream>
#include<fstream>
using namespace std;

extern Batch B[100]; //批的集合
extern Solution sol[101];
extern int batchNum;
extern int machineNum[kmax + 1];
extern Machine **M;
// 赋值整型数组，从1开始复制len个元素
void cpMatrix(int a[], int b[],int len)
{
	for (int i = 1; i <= len; i++)
		a[i] = b[i];
}

//按照完成时间升序
bool cTASC(const Batch &b1, const Batch &b2)
{
	return b1.ct < b2.ct;
}

//按照完成时间降序
bool cTDes(const Batch &b1, const Batch &b2)
{
	return b1.ct > b2.ct;
}

int min(int a, int b, int c)
{
	int m;
	if (a < b)
		m = a;
	else
		m = b;
	if (c < m)
		m = c;
	return m;
}

void cpMatrix(Batch a[], Batch b[], int len)
{
	for (int i = 1; i <= len; i++)
		a[i] = b[i];
}

int getEPC1(int t, int detaT, int k, int i)
{
	int n = (t + 3) / 24;
	if (t >= 24 * n - 3 && t < 24 * n + 7)
	{
		if (t + detaT >= 24 * n + 7)
			return (24 * n + 7 - t) * 5 * M[k][i].PW + getEPC1(24 * n + 7, detaT - (24 * n + 7 - t), k, i);
		else
			return detaT * 5 * M[k][i].PW;
	}
	if (t >= 24 * n + 7 && t < 24 * n + 11)
	{
		if (t + detaT >= 24 * n + 11)
			return (24 * n + 11 - t) * 8 * M[k][i].PW + getEPC1(24 * n + 11, detaT - (24 * n + 11 - t), k, i);
		else
			return detaT * 8 * M[k][i].PW;
	}
	if (t >= 24 * n + 11 && t < 24 * n + 17)
	{
		if (t + detaT >= 24 * n + 17)
			return (24 * n + 17 - t) * 10 * M[k][i].PW + getEPC1(24 * n + 17, detaT - (24 * n + 17 - t), k, i);
		else
			return detaT * 10 * M[k][i].PW;
	}
	if (t >= 24 * n + 17 && t < 24 * n + 21)
	{
		if (t + detaT >= 24 * n + 21)
			return (24 * n + 21 - t) * 8 * M[k][i].PW + getEPC1(24 * n + 21, detaT - (24 * n + 21 - t), k, i);
		else
			return detaT * 8 * M[k][i].PW;
	}
}

void Right_Shift(Solution& sol,Batch B[])
{
	int newEPC = 0;
	for (int k = kmax; k >= 1; k--)
	{
		Batch tempBB[100];
		cpMatrix(tempBB, B, batchNum);
		int D[100];
		memset(D, -1, sizeof(D));

		for (int i = 1; i <=batchNum; i++)
			tempBB[i].ct = tempBB[i].BC[k];

		sort(tempBB+1, tempBB+batchNum+1, cTDes);
		int c = 1;
		for (int i = 1; i <= batchNum; i++)
			D[c++] = tempBB[i].BId;

		for (int h = 1; h <=batchNum; h++)
		{
			int tmin = B[D[h]].BS[k];
			int STnext;
			int ST;//即ST(k+1)
			list<Batch>::iterator it = M[k][B[D[h]].MID[k]].List.begin();
			for (; it != M[k][B[D[h]].MID[k]].List.end(); it++)
			{
				if ((*it).BId == B[D[h]].BId)
					break;
			}

			it++;
			if (it == M[k][B[D[h]].MID[k]].List.end())
				STnext = 9999999;
			else
				STnext = (*it).BS[k];
			if (k == kmax)
				ST = 9999999;
			else
				ST = B[D[h]].BS[k + 1];
			int tmax = min(sol.Cmax, STnext, ST) - ceil((float)B[D[h]].BP[k] / M[k][B[D[h]].MID[k]].V);
			//cout << "min(sol.Cmax, STnext, ST)" << sol.Cmax<<"   "<< STnext<<"    "<<ST<< min(sol.Cmax, STnext, ST) << endl;
			//cout << "加工时间=" << ceil((float)tempB[D[h]].BP[k] / M[k][tempB[D[h]].MID[k]].V) << endl;
			//寻找最小的t
			int minEPC = 99999999;
			int t0;
			//cout << "tmin=" << tmin << "  tmax=" << tmax << endl;
			for (int t = tmin; t <= tmax; t++)
			{
				int tempEPC = getEPC1(t, ceil((float)B[D[h]].BP[k] / M[k][B[D[h]].MID[k]].V), k, B[D[h]].MID[k]);
				if (tempEPC <= minEPC)
				{
					minEPC = tempEPC;
					t0 = t;
				}

			}
			//cout << "t0=" << t0 << endl;
			B[D[h]].BS[k] = t0;//更新开始加工时间
			B[D[h]].BC[k] = t0 + ceil((float)B[D[h]].BP[k] / M[k][B[D[h]].MID[k]].V);//更新完成时间

																								 //更新机器队列
			it = M[k][B[D[h]].MID[k]].List.begin();
			for (; it != M[k][B[D[h]].MID[k]].List.end(); it++)
				if ((*it).BId == B[D[h]].BId)
				{
					(*it) = B[D[h]];
				}
		}
	}
	//输出改进后的情况
	ofstream out("LS.txt", ios::app);
	out << endl;
	out << "----------Right_Shift之后-------------：" << endl;

	for (int k = 1; k < kmax + 1; k++)
	{
		for (int i = 1; i < machineNum[k] + 1; i++)
		{
			out << "第" << k << "阶段，第" << i << "台机器：" << endl;
			list<Batch>::iterator it = M[k][i].List.begin();
			for (; it != M[k][i].List.end(); it++)
				out << '\t' << "批：" << (*it).BId << '\t' << "开始时间：" << (*it).BS[k] << '\t' << "完成时间：" << (*it).BC[k] << '\t' << "机器：" << (*it).MID[k] << endl;
		}
	}
	//计算改进后的EPC
	for (int k = 1; k < kmax + 1; k++)
	{
		for (int i = 1; i < machineNum[k] + 1; i++)
		{
			if (M[k][i].List.empty())
				newEPC += getEPC1(0, sol.Cmax, 0, 0);
			else
			{
				list<Batch>::iterator it = M[k][i].List.begin();
				list<Batch>::iterator it1;
				for (; it != M[k][i].List.end(); it++)
				{
					if (it == M[k][i].List.begin())
					{
						newEPC += getEPC1(0, (*it).BS[k], 0, 0);
						newEPC += getEPC1((*it).BS[k], ceil((float)(*it).BP[k] / M[k][i].V), k, i);
					}
					else
					{
						it1 = (--it);
						it++;
						newEPC += getEPC1((*it1).BC[k], (*it).BS[k] - (*it1).BC[k], 0, 0);
						newEPC += getEPC1((*it).BS[k], ceil((float)(*it).BP[k] / M[k][i].V), k, i);
					}
				}
				newEPC += getEPC1(M[k][i].List.back().BC[k], sol.Cmax - M[k][i].List.back().BC[k], 0, 0);
			}
		}
	}
	out << "newEPC=" << newEPC << '\t' << "Cmax=" << sol.Cmax << endl;
	sol.EPC = newEPC;
	out << "=================================================" << endl << endl;
}
// 求每个解对应的函数值
void getFitness(Solution& sol, Batch B[])
{
	int SOL[106];//存放每一阶段的批序列（后面的覆盖前面的）
	cpMatrix(SOL, sol.batchSeq, batchNum);
	Batch tempB[100];
	cpMatrix(tempB, B, batchNum);

	int EPC = 0;
	int Cmax = 0;

	//清除上只蚂蚁残留的机器信息
	for (int i = 1; i < kmax + 1; i++)
	{
		for (int j = 1; j < machineNum[i] + 1; j++)
		{
			M[i][j].avt = 0;
			M[i][j].List.clear();
		}
	}
	
	ofstream out("LS.txt", ios::app);
	out << "---------------------------------------" << endl;

	for (int i = 1; i <= batchNum; i++)
		out << SOL[i] << " ";
	out << endl;
	//对于每个阶段
	for (int k = 1; k < kmax + 1; k++)
	{
		//遍历每一个批，使得每个批在当前阶段分配在合适的机器上
		for (int h = 1; h <= batchNum; h++)
		{
			float min = float(INTMAX_MAX);//整个式子的最小值
			int selectMachine = -1;//最终选择的机器
			float max1 = -INTMAX_MAX;
			float max2 = -INTMAX_MAX;
			
			int CT = tempB[SOL[h]].BC[k - 1];//上一阶段的完成时间
			//cout << "............" << endl;
			
			for (int i = 1; i < machineNum[k] + 1; i++)
			{
				int temp1;
				temp1 = max(CT, M[k][i].avt) + ceil((float)tempB[SOL[h]].BP[k] / M[k][i].V) - CT;
				
				if (temp1 > max1)
					max1 = temp1;

				float temp2 = getEPC1(max(CT, M[k][i].avt), ceil((float)tempB[SOL[h]].BP[k] / M[k][i].V), k, i);
				if (temp2 > max2)
					max2 = temp2;
				
			}
			for (int i = 1; i < machineNum[k] + 1; i++)
			{
				float temp = sol.Vmk * (max(CT, M[k][i].avt) + ceil((float)tempB[SOL[h]].BP[k] / M[k][i].V) - CT) / max1 + sol.Vepc * getEPC1(max(CT, M[k][i].avt), ceil((float)tempB[SOL[h]].BP[k] / M[k][i].V), k, i) / max2;
				if (temp < min)
				{
					min = temp;
					selectMachine = i;
				}
			}

			//计算待机电费
			if (M[k][selectMachine].List.empty())
				EPC += getEPC1(0, max(CT, M[k][selectMachine].avt), 0, 0);
			else
				EPC += getEPC1(M[k][selectMachine].List.back().BC[k], max(CT, M[k][selectMachine].avt) - M[k][selectMachine].List.back().BC[k], 0, 0);
			//计算工作电费
			EPC += getEPC1(max(CT, M[k][selectMachine].avt), ceil(float(tempB[SOL[h]].BP[k]) / M[k][selectMachine].V), k, selectMachine);

			tempB[SOL[h]].BC[k] = max(CT, M[k][selectMachine].avt) + ceil(float(tempB[SOL[h]].BP[k]) / M[k][selectMachine].V);//更新这个批的完成时间
			tempB[SOL[h]].ct = tempB[SOL[h]].BC[k];//批在当前阶段的完工时间，为了后面的排序
			tempB[SOL[h]].BS[k] = max(CT, M[k][selectMachine].avt);//更新批的开始时间
			tempB[SOL[h]].MID[k] = selectMachine;
			//更新选择的这台机器的可用时间
			M[k][selectMachine].avt = tempB[SOL[h]].BC[k];

			//将这个批加入相应的机器队列中
			M[k][selectMachine].List.push_back(tempB[SOL[h]]);
		}//当前阶段，所有的批都已经分配好了
		 //将所有的批按照完成时间递增排序
		Batch tempBB[100];
		cpMatrix(tempBB, tempB, batchNum);
		sort(tempBB + 1, tempBB + batchNum + 1, cTASC);

		Cmax = tempBB[batchNum].BC[k];
		//获取下一个阶段的批序列
		for (int j = 1; j <= batchNum; j++)
			SOL[j] = tempBB[j].BId;
	}//所有阶段都已经完成

	 //前面计算电费时，由于没计算某台机器最后一个批完成后，还要待机多久（因为当时还不知道Cmax）
	for (int k = 1; k < kmax + 1; k++)
	{
		for (int i = 1; i < machineNum[k] + 1; i++)
		{
			//机器上没有任何一个批，一直处于待机状态
			if (M[k][i].List.empty())
				EPC += getEPC1(0, Cmax, 0, 0);
			else
				EPC += getEPC1(M[k][i].List.back().BC[k], Cmax - M[k][i].List.back().BC[k], 0, 0);
		}
	}


	//输出每个阶段每台机器上的批情况
	for (int k = 1; k < kmax + 1; k++)
	{
		for (int i = 1; i < machineNum[k] + 1; i++)
		{
			out << "第" << k << "阶段，第" << i << "台机器：" << endl;
			list<Batch>::iterator it = M[k][i].List.begin();
			for (; it != M[k][i].List.end(); it++)
				out << '\t' << "批：" << (*it).BId << '\t' << "开始时间：" << (*it).BS[k] << '\t' << "完成时间：" << (*it).BC[k] << '\t' << "机器：" << (*it).MID[k] << endl;
		}
	}
	//输出总的电费，Cmax
	out << "EPC=" << EPC << '\t' << "Cmax=" << Cmax << endl;
	sol.Cmax = Cmax;
	sol.EPC = EPC;

	//Right_Shift改进
	Right_Shift(sol,tempB);
}


