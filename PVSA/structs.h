#pragma once
#ifndef structs_H
#define structs_H

#include"defines.h"
#include<list>
#include<vector>
using namespace std;
struct Solution
{
	float Vmk;
	float Vepc;
	int batchSeq[105+1];//��1��ʼ
	float Cmax;
	float EPC;
	float F;//�ۺ�����ָ��

	bool operator<(const struct Solution & right)const
	{
		if (this->Cmax==right.Cmax&&this->EPC&&right.EPC)
			return false;
		else
		{
			if (Cmax != right.Cmax)
				return Cmax > right.Cmax;
			else
				return EPC > right.EPC;
		}
	}
};

struct Job
{
	int JobId;//�������
	int JobSize;//�����ߴ�
	int JobPT[kmax + 1];//�����ٲ�ͬ�׶εĴ���ʱ��,��1��ʼ���
};

struct Batch
{
	int BId;//����ţ���1��ʼ

	int BP[kmax + 1];//���ٲ�ͬ�׶����崦��ʱ��,��1��ʼ
	int BC[kmax + 1];//�������ڲ�ͬ�׶ε����ʱ��
	int JNum;//���й�������
	int freeSpace;//��¼����ʣ��ռ�
	Job BJob[106];//�������ŵĹ���
	float SI;//������б��
	int flag;//1��ʾ������û�м������
	int ct;//��ǰ�׶ε��깤ʱ��
	int BS[kmax + 1];//����ͬ�׶εĿ�ʼʱ��
	int MID[kmax + 1];//��ÿ���׶����ĸ�������
};

struct Machine
{
	int V;//�����׶β�ͬ�������ٶ�
	int PW;//���������ڲ�ͬ�׶��µĹ�������
	int avt;//��k�׶Σ�ÿ�������ĵ�ǰ����ʱ��
	list<Batch> List;//��¼�����ϼӹ�����
};

#endif // !structs_H
