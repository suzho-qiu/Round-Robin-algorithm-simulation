#include <iostream>
#include <queue>
#include <iomanip>
#include <fstream>
using namespace std;

//进程结构体
typedef struct {
	char name;        //进程名
	int ArrivalTime;  //到达时间
	int ServiceTime;  //服务时间
	int FinishedTime; //完成时间
	int WholeTime;    //周转时间
	double WeightWholeTime;  //带权周转时间
}RR;

static queue<RR>RRqueue;  //声明等待队列
static queue<RR>Pqueue;  //声明总进程队列
static double AverageWT = 0, AverageWWT = 0;
static int q;  //时间片长度
static int n;  //进程个数
static RR RRarray[100];  //进程数组
static char processMoment[100];  //存储每个过程执行进程的名称
static int processMomentPoint = 0;  //表明当前执行了多少个过程
static int processSTime[50];  //存储每个过程的开始时刻
static int processFTime[50];  //存储每个过程的结束时刻

//输入函数_文件读取模式
int Input1() {
	ifstream inData;
	inData.open("./RR_data.txt");
	//程序健壮性考量
	if (!inData) {
		cout << "文件读取失败!" << endl;
		cout << "****************************************************************" << endl;
		return -1;
	}
	else {
		cout << "文件读取成功!" << endl;
	}
	inData >> n;  //从RR_data.txt中读取

	inData >> q;  //从RR_data.txt中读取时间片长度

	//从RR_data.txt中读取各个进程的名称、到达时间和服务时间
	for (int i = 0; i < n; i++) {
		inData >> RRarray[i].name;
	}
	for (int i = 0; i < n; i++) {
		inData >> RRarray[i].ArrivalTime;
	}
	for (int i = 0; i < n; i++) {
		inData >> RRarray[i].ServiceTime;
	}
	inData.close();
	cout << "****************************************************************" << endl;
	return 0;
}

//输入函数_手动输入模式
void Input2() {
	cout << "欢迎进入手动输入模式!" << endl;
	cout << "请输入进程个数 n = ";
	cin >> n;  //输入进程个数
	cout << "请输入时间片长度 q = ";
	cin >> q;  //输入时间片长度
	//输入每个进程的进程名
	cout << "请输入各进程名称：" << endl;
	for (int i = 0; i<n; i++) {
		cin >> RRarray[i].name;
	}
	//输入每个进程的到达时间
	cout << "请输入各进程的到达时间：" << endl;
	for (int i = 0; i < n; i++) {
		cin >> RRarray[i].ArrivalTime;
	}
	//输入每个进程的服务时间
	cout << "请输入各进程的服务时间：" << endl;
	for (int i = 0; i < n; i++) {
		cin >> RRarray[i].ServiceTime;
	}
	cout << "****************************************************************" << endl;
}

//初始信息输出函数
void Output() {
	//输出用户所输入的信息
	cout << "The information of processes is the following:" << endl;
	cout << setw(10) << "processName" << " ";
	cout << setw(10) << "ArrivalTime" << " ";
	cout << setw(10) << "ServiceTime" << " " << endl;
	for (int i = 0; i < n; i++) {
		cout << setw(10) << RRarray[i].name << " ";
		cout << setw(10) << RRarray[i].ArrivalTime << " ";
		cout << setw(10) << RRarray[i].ServiceTime << " " << endl;
	}
	cout << "****************************************************************" << endl;
}

//模拟函数_模拟调度算法过程
void RR_Simulate() {
	int CurrentTime = 0;  //当前时间
	int tempTime;  //声明此变量控制CurrentTime的累加时间，当前进程的服务时间小于时间片q时起作用
	int ncopy = n;  //存放进程总数的拷贝
	int shortest = 0;  //存放到达时间最近的进程的下标
	RR RRcopy[100];  //存放进程数组的拷贝

	//拷贝进程数组
	for (int m = 0; m < n; m++) {
		RRcopy[m] = RRarray[m];
	}

	//遍历进程数组，将除到达时间最大进程以外的所有进程，按到达顺序依次进入总进程队列
	while (ncopy != 1) {
		shortest = 0;
		//找到最近到达的进程下标，存入shortest
		for (int m = 0; m < ncopy - 1; m++) {
			if (RRcopy[m].ArrivalTime > RRcopy[m + 1].ArrivalTime) {
				shortest = m + 1;
			}
		}
		Pqueue.push(RRcopy[shortest]);  //将最近到达的进程进入总队列
		//将进队的进程移除出拷贝进程数组
		if (shortest == (ncopy - 1)) {
			ncopy--;
		}
		else {
			for (int m = shortest; m < ncopy - 1; m++) {
				RRcopy[m] = RRcopy[m + 1];
			}
			ncopy--;
		}
	}
	Pqueue.push(RRcopy[0]);  //将到达时间最大的进程进入总队列

	//当等待队列为空且总进程队列为空时，跳出循环
	while (!RRqueue.empty() || !Pqueue.empty()) {
		//使得满足进程的到达时间小于当前时间的进程都进入队列
		while (!Pqueue.empty() && Pqueue.front().ArrivalTime <= CurrentTime) {
			RRqueue.push(Pqueue.front());
			Pqueue.pop();
		}
		//当等待队列进程已全部执行完，但还有进程没有到达时，将当前时间更新至最快到达进程的到达时间
		if (RRqueue.empty() && !Pqueue.empty()) {
			CurrentTime = Pqueue.front().ArrivalTime;
		}
		//使得满足进程的到达时间小于当前时间的进程都进入队列
		while (!Pqueue.empty() && Pqueue.front().ArrivalTime <= CurrentTime) {
			RRqueue.push(Pqueue.front());
			Pqueue.pop();
		}
		
		//确认该次时间片分配的时间tempTime
		if (RRqueue.front().ServiceTime < q) {
			tempTime = RRqueue.front().ServiceTime;
		}
		else {
			tempTime = q;
		}

		RRqueue.front().ServiceTime -= q;  //进程每执行一次，就将其服务时间-q
		processSTime[processMomentPoint] = CurrentTime;  //记录每个过程的开始时刻
		CurrentTime += tempTime;
		processFTime[processMomentPoint] = CurrentTime;  //记录每个过程的结束时刻
		processMoment[processMomentPoint] = RRqueue.front().name;//记录每个过程执行的进程名
		processMomentPoint++;

		//使得满足进程的到达时间小于当前时间的进程都进入队列
		while (!Pqueue.empty() && Pqueue.front().ArrivalTime <= CurrentTime) {
			RRqueue.push(Pqueue.front());
			Pqueue.pop();
		}

		//把执行完的进程退出队列
		if (RRqueue.front().ServiceTime <= 0) {
			RRqueue.pop();  //如果进程的服务时间小于等于0，即该进程已经服务完了，将其退栈
		}
		else {
			//将队首移到队尾
			RRqueue.push(RRqueue.front());
			RRqueue.pop();
		}
	}
}

//输出函数_输出模拟执行过程
void display1() {
	int time1 = 0;  //标明取出第几次过程的开始时刻
	int time2 = 0;  //标明取出第几次过程的结束时刻
	int count = 0;
	cout << "各进程的执行时刻信息：" << endl;
	cout << "  " << processSTime[time1] << "时刻 --> " << setw(2) << processFTime[time2] << "时刻";
	for (int i = 0; i < processMomentPoint; i++) {
		count = 0;
		cout << setw(3) << processMoment[i] << setw(3) << endl;

		while (RRarray[count].name != processMoment[i] && count < n) {
			count++;
		}
		RRarray[count].FinishedTime = processFTime[time2];

		if (i < processMomentPoint - 1) {
			time1++;
			time2++;
			cout << setw(3) << processSTime[time1] << "时刻" << " --> " << setw(2) << processFTime[time2] << "时刻" << setw(3);
		}
	}
	cout << endl;
}

//计算函数_计算进程相关信息
void RR_Calculate() {
	//周转时间、带权周转时间、平均周转时间、带权平均周转时间的计算
	//周转时间 = 完成时间 - 到达时间
	//带权周转时间 = 周转时间/服务时间
	for (int i = 0; i < n; i++) {
		RRarray[i].WholeTime = RRarray[i].FinishedTime - RRarray[i].ArrivalTime;
		RRarray[i].WeightWholeTime = (double)RRarray[i].WholeTime / RRarray[i].ServiceTime;
	}

	double x = 0, y = 0;
	for (int i = 0; i < n; i++) {
		x += RRarray[i].WholeTime;
		y += RRarray[i].WeightWholeTime;
	}
	AverageWT = x / n;
	AverageWWT = y / n;
}

//输出函数_输出进程相关信息
void display2() {
	cout << "****************************************************************" << endl;
	cout << "RR调度算法执行后：进程相关信息如下：" << endl;
	cout << setw(10) << "进程名（ID）" << " ";
	cout << setw(10) << "到达时间" << " ";
	cout << setw(10) << "服务时间" << " ";
	cout << setw(10) << "完成时间" << " ";
	cout << setw(10) << "周转时间" << " ";
	cout << setw(10) << "带权周转时间" << endl;
	for (int i = 0; i < n; i++) {
		cout << setw(10) << RRarray[i].name << " ";
		cout << setw(10) << RRarray[i].ArrivalTime << " ";
		cout << setw(10) << RRarray[i].ServiceTime << " ";
		cout << setw(10) << RRarray[i].FinishedTime << " ";
		cout << setw(10) << RRarray[i].WholeTime << " ";
		cout << setw(10) << RRarray[i].WeightWholeTime << " " << endl;
	}
	cout << "所有进程的平均周转时间 = " << AverageWT << endl;
	cout << "所有进程的平均带权周转时间 = " << AverageWWT << endl;
	cout << "****************************************************************" << endl;
}

int main() {
	cout << "请选择输入模式：" << endl;
	cout << "1.文件读取模式" << endl;
	cout << "2.用户输入模式" << endl;
	int i, j;
	j = 0;
	while (j==0) {
		cout << "请输入(1/2)：";
		cin >> i;
		cout << "****************************************************************" << endl;
		switch (i) {
		case 1:
			int a;
			a = Input1();
			if (a == -1) {
				cout << "请检查文件是否存在!" << endl;
				system("pause");
				return -1;
			}
			j = 1;
			break;
		case 2:
			Input2();
			j = 1;
			break;
		default:cout << "输入有误，请重新输入!" << endl;
		}
	}
	Output();
	RR_Simulate();
	display1();
	RR_Calculate();
	display2();
	system("pause");
	return 0;
}