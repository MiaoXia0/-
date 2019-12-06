#include <iostream>
#include <string>
#include <fstream>
using namespace std;
#define MAXVERTEX 32//最大顶点数
#define INF 1000000//定义一个大数以便寻找最小值
typedef char vertextype;//顶点
typedef int arctype;//边
#pragma region 邻接表的结构体
typedef struct ArcNode
{
	int adjvex;//下标
	arctype wigth;//权值
	struct ArcNode* next;//下一个顶点
}ArcNode;//边
typedef struct VertexNode
{
	vertextype data;//顶点上的数据
	ArcNode* firstarc;//第一个邻接顶点
}VertexNode, AdjList[MAXVERTEX];//顶点
typedef struct
{
	AdjList adjlist;//邻接表
	int numvertex;//顶点数
	int numarc;//边数
}GraphAdjList;//邻接表
#pragma endregion
#pragma region 邻接表的操作
//创建邻接表
void CreateAdjListGraph(GraphAdjList& G, int nv, int na)
{
	ifstream infile;
	ArcNode* e;
	G.numvertex = nv;//读取顶点数
	G.numarc = na;//读取边数
	for (int i = 0; i < G.numvertex; i++)//建立顶点表
	{
		G.adjlist[i].data = i;;//顶点数据为0, 1, 2...
		G.adjlist[i].firstarc = NULL;//将表边指针置为空
	}
	infile.open("arcs.txt");
	for (int k = 0; k < G.numarc; k++)
	{
		int i, j, w;
		infile >> i >> j >> w;//边两顶点及权值
		e = new ArcNode;//创建一个边节点指针
		e->adjvex = j;
		e->wigth = w;
		e->next = G.adjlist[i].firstarc;
		G.adjlist[i].firstarc = e;
		//无向图，对称创建邻接表
		e = new ArcNode;//创建一个表边节点指针
		e->adjvex = i;
		e->wigth = w;
		e->next = G.adjlist[j].firstarc;
		G.adjlist[j].firstarc = e;
	}
}
//打印邻接表（未使用）
void PrintfGraphAdjList(GraphAdjList G)
{
	for (int i = 0; i < G.numvertex; i++)
	{
		ArcNode* p = G.adjlist[i].firstarc;
		cout << G.adjlist[i].data << "\t";
		while (p)
		{
			cout << p->adjvex << ' ' << p->wigth << "\t";
			p = p->next;
		}
		cout << endl;
	}
}
//返回图G中从f连接到其他顶点的边的数量
int arcNum(GraphAdjList G, int f) {
	int i = 0;
	ArcNode* p;
	p = G.adjlist[f].firstarc;
	while (p != NULL) {
		i++;
		p = p->next;
	}
	return i;
}
//计算图G中从f到其他顶点的单边距离
void PathNow(GraphAdjList G, int f, float* D) {
	ArcNode* p;
	p = G.adjlist[f].firstarc;
	for (int i = 0; p != NULL; i++) {
		D[p->adjvex] = p->wigth;
		p = p->next;
	}
}
//求从f到t的最短路径
void ShortestPath(GraphAdjList G, int f, int t) {
	int P[MAXVERTEX];//最短路径终点的前驱点
	float D[MAXVERTEX];//从起点到某点的长度
	float D1[MAXVERTEX];//当前最小点到其他点的距离
	int finished[MAXVERTEX];//当前顶点是否已计算（是否属于S集）
	int n = arcNum(G, f);//顶点数
	if (f == t) {
		cout << "0: 起点与终点相同" << endl;
		system("pause");
		return;
	}
	ifstream infile;
	for (int i = 0; i < G.numvertex; i++) {//初始化数组
		P[i] = f;
		D[i] = INF;
		D1[i] = INF;
		finished[i] = 0;
	}
	PathNow(G, f, D);//找到起点到某点的当前长度记录到数组D中
	D[f] = 0;//当前点最短路径为0
	finished[f] = 1;//当前点已完成
	P[f] = -1;//当前点没有前驱点
	for (int i = 1; i < G.numvertex; i++) {
		int min = INF + 1;
		int j;//存储更小路径的下标
		for (int i = 0; i < G.numvertex; i++) {
			if (finished[i] == 0 && D[i] < min) {
				j = i;
				min = D[i];
			}
		}
		finished[j] = 1;
		PathNow(G, j, D1);//求当前顶点点到其他顶点的距离
		D1[j] = 0;//当前顶点到自己的距离为0
		for (int i = 0; i < G.numvertex; i++) {
			if (finished[i] == 0 && (D[j] + D1[i] < D[i])) {//由当前最小点到某点距离比从初始点这点更短
				D[i] = D[j] + D1[i];
				P[i] = j;
			}
		}
	}
	int pre;
	string buildings[MAXVERTEX];
	infile.open("buildings.txt");
	infile.seekg(sizeof(int) * 2, ios::cur);
	for (int i = 0; i < G.numvertex; i++) {
		infile >> buildings[i];
	}
	infile.close();
	cout << D[t] << ": " << "[" << t << "]" << buildings[t];
	pre = P[t];
	while (pre != f) {
		cout << "←" << "[" << pre << "]" << buildings[pre];
		pre = P[pre];
	}
	cout << "←" << "[" << f << "]" << buildings[f];
	cout << endl;
	system("pause");
}
//最小生成树并输出树中的边
void Prim(GraphAdjList G) {
	int flag[MAXVERTEX] = { 0 };
	int mincost, k = -1, j, t;
	ifstream infile;
	ArcNode* p;
	int arcs[MAXVERTEX][MAXVERTEX] = { 0 };
	float D[MAXVERTEX][MAXVERTEX];
	for (int n = 0; n < G.numvertex; n++) {
		for (int i = 0; i < G.numvertex; i++) {
			D[n][i] = INF;//
			PathNow(G, n, D[n]);//求出所有点到其他点的路径长度
		}
	}
	flag[0] = 1;//顶点0加入图
	for (int n = 0; n < G.numvertex; n++) {//循环顶点数量次以将所有顶点加入
		mincost = INF;//设最小为一个大数
		for (j = 0; j < G.numvertex; j++) {//对于所有顶点
			if (flag[j] == 1) {//若已加入图
				for (int i = 0; i < G.numvertex; i++) {//对于加入图的所有顶点到所有其他顶点
					if (D[j][i] < mincost && flag[i] == 0) {//如果距离小于最小距离且未加入图
						k = i;//记录这个顶点
						mincost = D[j][i];//此边是最小的边
						t = j;//记录当前最短边的起点
					}
				}
			}
		}
		if (k != -1)//若k改变了
			arcs[t][k] = 1;//对应的边为加入图中 j最后又自增了所以-1
		flag[k] = 1;//顶点加入图中
	}
	string buildings[MAXVERTEX];
	infile.open("buildings.txt");
	infile.seekg(sizeof(int) * 2, ios::cur);
	for (int i = 0; i < G.numvertex; i++) {
		infile >> buildings[i];
	}
	infile.close();
	for (int m = 0; m < G.numvertex; m++) {
		for (int n = 0; n < G.numvertex; n++) {
			if (arcs[m][n] == 1) {
				cout << "[" << m << "]" << buildings[m] << "-" << "[" << n << "]" << buildings[n] << endl;
			}
		}
	}
	system("pause");
}
#pragma endregion
#pragma region 各种功能函数
//菜单
int menu() {
	int n;
	cout << "欢迎来到河北大学！" << endl;
	cout << "菜单：" << endl;
	cout << "[1]查询景点信息" << endl;
	cout << "[2]查询两景点之间最短路线" << endl;
	cout << "[3]查看通信线路" << endl;
	cout << "[0]退出" << endl;
	cout << "请输入选项（0-3）：";
	cin >> n;
	return n;
}
//查询信息的菜单
void infOf(int n) {
	int c;
	ifstream infile;
	string buildings[MAXVERTEX], introduces[MAXVERTEX];
	infile.open("buildings.txt");
	infile.seekg(sizeof(int) * 2, ios::cur);//跳过顶点数和边数
	for (int i = 0; i < n; i++) {
		infile >> buildings[i];//文件中读取建筑列表
	}
	infile.close();

	while (1) {
		do {
			system("cls");
			for (int i = 0; i < n; i++) {

				cout << "[" << i << "]" << buildings[i] << endl;
			}
			cout << "[-1]退出" << endl;
			cout << "请输入选项：";
			cin >> c;
			if (c == -1) {
				return;//输入-1退出
			}

		} while (c < 0 || c > 9);

		infile.open("introduces.txt");
		for (int i = 0; i < n; i++) {
			infile >> introduces[i];
		}
		infile.close();
		cout << buildings[c] << "：" << introduces[c] << endl;
		system("pause");
		system("cls");
	}
}
#pragma endregion
//主函数
int main(int argc, char* argv[])
{
	ifstream infile;
	string names[MAXVERTEX];
	int choice, nv, na, from, to, gn;
	string str[16];
	GraphAdjList G;//声明邻接表变量
	infile.open("graph.txt");//从graph.txt中读取图形
	infile >> gn;
	for (int i = 0; i < gn; i++) {
		infile >> str[i];
	}
	infile.close();
	infile.open("buildings.txt");//从buildings.txt读取建筑数量、路的数量以及名称
	infile >> nv;
	infile >> na;
	for (int i = 0; i < nv; i++)
	{
		infile >> names[i];
	}
	infile.close();
	CreateAdjListGraph(G, nv, na);//建立无向邻接表存储图，建立时从文件中读取边和权值
	while (1) {//输出图像并打开菜单
		for (int i = 0; i < gn; i++) {
			cout << str[i] << endl;
		}
		choice = menu();
		switch (choice) {
		case 1://查看信息
			infOf(nv);//将顶点数作为参数输入
			break;
		case 2://寻找最短路径
			cout << "输入两个编号：";
			cin >> from >> to;
			ShortestPath(G, from, to);
			break;
		case 3://建立最小生成树作为通信网络
			Prim(G);
			break;
		case 0:
			exit(0);//退出返回0
		default:
			break;//输错直接跳出switch
		}
		system("cls");
	}
	return 0;
}