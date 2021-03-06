// PVSA.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include"functions.h"
#include"LS.h"
#include<iostream>
#include<time.h>
#include<random>
using namespace std;

extern int batchNum; //批的个数
extern Batch B[100]; //批的集合

int main()
{
	clock_t begin = clock();

	srand((unsigned)time(NULL));
	/*初始化工件信息*/
	Job job[105];
	initJob(job); 
	displayJob(job);
	
	/*初始化机器信息*/
	initMachine();

	/*BFLPT进行分批*/
	BFLPT(job);
	
	set<Solution> NDS;
	NDS.clear();
	
	for (int T = 0; T < 30; T++)
	{
		/*创建初始解*/
		Solution sol[101];
		createInitSol(sol);
		//displaySol(sol);

		/*对每个解进行评价*/
		//clearFile("LS.txt");
		float Fbest = float(INTMAX_MAX);//当前最好的综合评价函数值
		float Fworest = float(-INTMAX_MAX);
		Solution best; // 最优解
		for (int i = 1; i <= solNum; i++)
		{
			
			getFitness(sol[i], B);

			float tempF = sol[i].Cmax*sol[i].Vmk + sol[i].EPC*sol[i].Vepc;
			//cout << "tempF="<<tempF << endl;
			if (tempF < Fbest)
			{
				Fbest = tempF;
				best = sol[i];
			}
			if (tempF > Fworest)
			{
				Fworest = tempF;
			}
		}
		
		Solution X = best; // 当前解
		float Fx = Fbest; // 当前解的函数值
		float temperature = (Fbest - Fworest) / log(0.65);
		//cout << "temperature=" << temperature << endl;
		float preBest = (float)INTMAX_MAX; // 上一个最优值
		int g2 = 0;
		int t = 1;

		cout << "temperature=" << temperature << endl;
		while(temperature>0.001)
		{
			int g1 = 0;
			do
			{
				//printSol(X);
				Solution X1 = generateNewSol(X,best); // X邻域产生的一个解X1
				//printSol(X1);
				getFitness(X1, B);
				float tempF = X1.Cmax*X1.Vmk + X1.EPC*X1.Vepc;
				//cout << "tempF=" << tempF << endl;
				if (tempF <Fx) // 比当前解好
				{			
					X = X1;
					Fx = tempF;
					if (Fx < Fbest)
					{
						best = X;
						Fbest = Fx;
						g1 = 0;
					}
					else
						g1++;
				}
				else if (0.2>rand() / float(RAND_MAX))
				{
					//cout << "EXP=" << EXP << endl;
					X = X1;
					Fx = tempF;
					g1++;
				}
				//cout << "EXP=" << exp((Fx - tempF) / temperature) << endl;
			} while (g1 < 15); // 内循环

							   // 退火
			//temperature = lambd * temperature;
			temperature = 0.05*temperature;
			t++;

			//cout << "Fbest=" << Fbest << '\t' << "preBest=" << preBest << endl;
			if (Fbest != preBest)
			{
				g2 = 0;
				preBest = Fbest;
			}
			else
				g2++;

			if (g2 > 15)
				break;

		}
		cout <<"T="<<T<<'\t'<< X.Cmax << '\t' << X.EPC << endl;
		NDS=updateNDS(best, NDS);
		cout << "NDS.size()="<<NDS.size() << endl;
	}


	// 运行后面70代
	for (int T = 30; T < 100; T++)
	{
		Solution best;// 最优解
		float temperature; // 当前温度
		float Fbest; // 当前最好解的评价值

		if (rand() / float(RAND_MAX) < 0.2)
		{
			cout << "xiao" << endl;
			/*创建初始解*/
			Solution sol[101];
			createInitSol(sol);
			//displaySol(sol);

			/*对每个解进行评价*/
			//clearFile("LS.txt");
			Fbest = float(INTMAX_MAX);//当前最好的综合评价函数值
			float Fworest = float(-INTMAX_MAX);
			for (int i = 1; i <= solNum; i++)
			{

				getFitness(sol[i], B);

				float tempF = sol[i].Cmax*sol[i].Vmk + sol[i].EPC*sol[i].Vepc;
				//cout << "tempF="<<tempF << endl;
				if (tempF < Fbest)
				{
					Fbest = tempF;
					best = sol[i];
				}
				if (tempF > Fworest)
				{
					Fworest = tempF;
				}
			}
		}

		else
		{
			/*随机从NDS中选择一个解作为当前的初始解*/
			int select = rand() % NDS.size();
			set<Solution>::iterator it = NDS.begin();
			for (int i = 0; it != NDS.end(); it++, i++)
			{
				if (i == select)
					break;
			}
			Solution Sol = (*it);
			Sol.Vmk = rand() / float(RAND_MAX);
			Sol.Vepc = 1 - Sol.Vmk;
			/*cout << Sol.Vmk << '\t' << Sol.Vepc << endl;

			for (int j = 1; j <= batchNum; j++) {
			cout << Sol.batchSeq[j] << " ";
			}
			cout << endl;

			cout << "Cmax=" << Sol.Cmax << '\t' << "EPC=" << Sol.EPC << '\t' << "F=" << Sol.F << endl << endl;*/

			//displaySol(sol);

			/*对每个解进行评价*/
			//clearFile("LS.txt");
			Fbest = Sol.Cmax*Sol.Vmk + Sol.EPC*Sol.Vepc;//当前最好的综合评价函数值
			Sol.F = Fbest;
			best = Sol; // 最优解
		}
		
		

		Solution X = best; // 当前解
		float Fx = Fbest; // 当前解的函数值
		temperature = Fbest;
		cout << "temperature=" << temperature << endl;
		//cout << "temperature=" << temperature << endl;
		float preBest = (float)INTMAX_MAX; // 上一个最优值
		int g2 = 0;
		int t = 1;
		//cout << "*****" << endl;
		while (temperature>0.001)
		{
			int g1 = 0;
			do
			{
				Solution X1 = generateNewSol(X, best); // X邻域产生的一个解X1
				getFitness(X1, B);
				float tempF = X1.Cmax*X1.Vmk + X1.EPC*X1.Vepc;
				//cout << "tempF=" << tempF << endl;
				if (tempF <Fx) // 比当前解好
				{
					X = X1;
					Fx = tempF;
					if (Fx < Fbest)
					{
						best = X;
						Fbest = Fx;
						g1 = 0;
					}
					else
						g1++;
				}
				else if (0.2>rand() / float(RAND_MAX))
				{
					X = X1;
					Fx = tempF;
					g1++;
				}
			} while (g1 < 15); // 内循环
			//cout << "--------" << endl;
							   // 退火
							   //temperature = lambd * temperature;
			temperature = 0.05*temperature;
			t++;

			//cout << "Fbest=" << Fbest << '\t' << "preBest=" << preBest << endl;
			if (Fbest != preBest)
			{
				g2 = 0;
				preBest = Fbest;
			}
			else
				g2++;

			if (g2 > 15)
				break;

		}
		cout << "T=" << T << '\t' << X.Cmax << '\t' << X.EPC << endl;
		NDS = updateNDS(best, NDS);
		cout << "NDS.size()=" << NDS.size() << endl;
	}

	set<Solution>::iterator it = NDS.begin();
	for (; it != NDS.end(); it++)
		cout << (*it).Cmax << '\t' << (*it).EPC << endl;


	set<Solution>::iterator it1 = NDS.begin();
	set<Solution>::iterator it2 = NDS.end();
	it2--;
	int xmk = (*it1).Cmax + sigma * ((*it1).Cmax - (*it2).Cmax);
	int xepc = (*it2).EPC + sigma * ((*it2).EPC - (*it1).EPC);

	cout << "maxmk=" << (*it1).Cmax << '\t' << "minmk=" << (*it2).Cmax << endl;
	cout << "maxepc=" << (*it2).EPC << '\t' << "minepc=" << (*it1).EPC << endl;

	cout << "xmk=" << xmk << '\t' << "xepc=" << xepc << endl;
	int sum = 0;
	for (it1; it1 != NDS.end(); it1++)
	{
		sum += abs(((*it1).Cmax - xmk)*((*it1).EPC - xepc));
	}
	cout << "sum=" << sum << endl;

	clock_t end = clock();
	cout << "Running time: " << (double)(end - begin) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
    return 0;
}          

