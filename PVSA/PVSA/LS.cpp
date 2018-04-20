#include "stdafx.h"
#include "LS.h"
#include<iostream>
#include<fstream>
using namespace std;

extern Batch B[100]; //���ļ���
extern Solution sol[101];
extern int batchNum;
extern int machineNum[kmax + 1];
extern Machine **M;
// ��ֵ�������飬��1��ʼ����len��Ԫ��
void cpMatrix(int a[], int b[],int len)
{
	for (int i = 1; i <= len; i++)
		a[i] = b[i];
}

//�������ʱ������
bool cTASC(const Batch &b1, const Batch &b2)
{
	return b1.ct < b2.ct;
}

//�������ʱ�併��
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
			int ST;//��ST(k+1)
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
			//cout << "�ӹ�ʱ��=" << ceil((float)tempB[D[h]].BP[k] / M[k][tempB[D[h]].MID[k]].V) << endl;
			//Ѱ����С��t
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
			B[D[h]].BS[k] = t0;//���¿�ʼ�ӹ�ʱ��
			B[D[h]].BC[k] = t0 + ceil((float)B[D[h]].BP[k] / M[k][B[D[h]].MID[k]].V);//�������ʱ��

																								 //���»�������
			it = M[k][B[D[h]].MID[k]].List.begin();
			for (; it != M[k][B[D[h]].MID[k]].List.end(); it++)
				if ((*it).BId == B[D[h]].BId)
				{
					(*it) = B[D[h]];
				}
		}
	}
	//����Ľ�������
	ofstream out("LS.txt", ios::app);
	out << endl;
	out << "----------Right_Shift֮��-------------��" << endl;

	for (int k = 1; k < kmax + 1; k++)
	{
		for (int i = 1; i < machineNum[k] + 1; i++)
		{
			out << "��" << k << "�׶Σ���" << i << "̨������" << endl;
			list<Batch>::iterator it = M[k][i].List.begin();
			for (; it != M[k][i].List.end(); it++)
				out << '\t' << "����" << (*it).BId << '\t' << "��ʼʱ�䣺" << (*it).BS[k] << '\t' << "���ʱ�䣺" << (*it).BC[k] << '\t' << "������" << (*it).MID[k] << endl;
		}
	}
	//����Ľ����EPC
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
// ��ÿ�����Ӧ�ĺ���ֵ
void getFitness(Solution& sol, Batch B[])
{
	int SOL[106];//���ÿһ�׶ε������У�����ĸ���ǰ��ģ�
	cpMatrix(SOL, sol.batchSeq, batchNum);
	Batch tempB[100];
	cpMatrix(tempB, B, batchNum);

	int EPC = 0;
	int Cmax = 0;

	//�����ֻ���ϲ����Ļ�����Ϣ
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
	//����ÿ���׶�
	for (int k = 1; k < kmax + 1; k++)
	{
		//����ÿһ������ʹ��ÿ�����ڵ�ǰ�׶η����ں��ʵĻ�����
		for (int h = 1; h <= batchNum; h++)
		{
			float min = float(INTMAX_MAX);//����ʽ�ӵ���Сֵ
			int selectMachine = -1;//����ѡ��Ļ���
			float max1 = -INTMAX_MAX;
			float max2 = -INTMAX_MAX;
			
			int CT = tempB[SOL[h]].BC[k - 1];//��һ�׶ε����ʱ��
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

			//����������
			if (M[k][selectMachine].List.empty())
				EPC += getEPC1(0, max(CT, M[k][selectMachine].avt), 0, 0);
			else
				EPC += getEPC1(M[k][selectMachine].List.back().BC[k], max(CT, M[k][selectMachine].avt) - M[k][selectMachine].List.back().BC[k], 0, 0);
			//���㹤�����
			EPC += getEPC1(max(CT, M[k][selectMachine].avt), ceil(float(tempB[SOL[h]].BP[k]) / M[k][selectMachine].V), k, selectMachine);

			tempB[SOL[h]].BC[k] = max(CT, M[k][selectMachine].avt) + ceil(float(tempB[SOL[h]].BP[k]) / M[k][selectMachine].V);//��������������ʱ��
			tempB[SOL[h]].ct = tempB[SOL[h]].BC[k];//���ڵ�ǰ�׶ε��깤ʱ�䣬Ϊ�˺��������
			tempB[SOL[h]].BS[k] = max(CT, M[k][selectMachine].avt);//�������Ŀ�ʼʱ��
			tempB[SOL[h]].MID[k] = selectMachine;
			//����ѡ�����̨�����Ŀ���ʱ��
			M[k][selectMachine].avt = tempB[SOL[h]].BC[k];

			//�������������Ӧ�Ļ���������
			M[k][selectMachine].List.push_back(tempB[SOL[h]]);
		}//��ǰ�׶Σ����е������Ѿ��������
		 //�����е����������ʱ���������
		Batch tempBB[100];
		cpMatrix(tempBB, tempB, batchNum);
		sort(tempBB + 1, tempBB + batchNum + 1, cTASC);

		Cmax = tempBB[batchNum].BC[k];
		//��ȡ��һ���׶ε�������
		for (int j = 1; j <= batchNum; j++)
			SOL[j] = tempBB[j].BId;
	}//���н׶ζ��Ѿ����

	 //ǰ�������ʱ������û����ĳ̨�������һ������ɺ󣬻�Ҫ������ã���Ϊ��ʱ����֪��Cmax��
	for (int k = 1; k < kmax + 1; k++)
	{
		for (int i = 1; i < machineNum[k] + 1; i++)
		{
			//������û���κ�һ������һֱ���ڴ���״̬
			if (M[k][i].List.empty())
				EPC += getEPC1(0, Cmax, 0, 0);
			else
				EPC += getEPC1(M[k][i].List.back().BC[k], Cmax - M[k][i].List.back().BC[k], 0, 0);
		}
	}


	//���ÿ���׶�ÿ̨�����ϵ������
	for (int k = 1; k < kmax + 1; k++)
	{
		for (int i = 1; i < machineNum[k] + 1; i++)
		{
			out << "��" << k << "�׶Σ���" << i << "̨������" << endl;
			list<Batch>::iterator it = M[k][i].List.begin();
			for (; it != M[k][i].List.end(); it++)
				out << '\t' << "����" << (*it).BId << '\t' << "��ʼʱ�䣺" << (*it).BS[k] << '\t' << "���ʱ�䣺" << (*it).BC[k] << '\t' << "������" << (*it).MID[k] << endl;
		}
	}
	//����ܵĵ�ѣ�Cmax
	out << "EPC=" << EPC << '\t' << "Cmax=" << Cmax << endl;
	sol.Cmax = Cmax;
	sol.EPC = EPC;

	//Right_Shift�Ľ�
	Right_Shift(sol,tempB);
}


