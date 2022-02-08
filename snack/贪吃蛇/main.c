#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <malloc.h>
#include <time.h>
#include <stdbool.h>

#define X 20
#define Y 20

enum Dir {enum_east, enum_south, enum_west, enum_north};

struct Snake
{
	int iX;
	int iY;
	enum Dir emDir; ///
};

struct Node
{
	struct Snake stSnake;
	int iCount;
	struct Node* pPre;
	struct Node* pNext;
};

struct Node* g_pHead = NULL;
struct Node* g_pEnd = NULL;

char g_Back[X][Y];
int g_quit = -1;
int g_stop = -1;
int g_speed = 500;

struct Record
{
	time_t t;
	int socre;
};

void Head(void);
void Frame(void);
void GotoXY(unsigned short hang, unsigned short lie);
void CreateSnake(int iCount);
void Free(void);
void DrawSnake(void);
void Move(void);
void Drop(void);
void KeyState(void);
void Food(void);
void Appand(void);
bool Eat(void);
bool IsDie(void);
void SetSocre(void);
void NewGame(void);
void Change(void);
void Save(void);
void Read(void);

int main(void)
{
	srand((unsigned int)time(NULL));
	Head();
	Frame();
	g_pHead;
	CreateSnake(3);
	DrawSnake();
	Food();
	SetSocre();
	//GotoXY(5, 10);  //0 0
	//printf("asdasdasd");
	while (1)
	{
		KeyState();

		if (1 == g_quit)
			break;
		if (1 == g_stop)
		{
			if (true == Eat())
			{
				SetSocre();
				Food();
			}

			if (true == IsDie())
			{
				Save();
				system("pause>0");
				continue;
			}

			Drop();
			Move();
			DrawSnake();
		}
		
		Sleep(g_speed);
	}

	Free(); //蛇死也记录
	return 0;
}

void Read(void)
{
	struct Record re;

	//写入文件
	FILE* pFile = NULL;
	errno_t res = fopen_s(&pFile, "record.dat", "r");
	if (res != 0 || NULL == pFile)
		return;
	int hang = 9;
	while (0 != fread(&re, sizeof(re), 1, pFile))
	{
		GotoXY(hang++, 76);
		char str[50] = {0};
		ctime_s(str, 50, &re.t);
		printf("分数：%d, 时间：%s", re.socre, str);
	}

	fclose(pFile);
}
void Save(void)
{
	struct Record re;
	re.socre = g_pHead->iCount;
	re.t = time(NULL);
	//写入文件
	FILE* pFile = NULL;
	errno_t res = fopen_s(&pFile, "record.dat", "a");
	if (res != 0 || NULL == pFile)
		return;
	fwrite(&re, sizeof(re), 1, pFile);
	fclose(pFile);
}
void Change(void)
{
	static int g = 1;

	switch (g % 3)
	{
	case 1:  //中等
		GotoXY(5, 60);
		g_speed = 350;
		printf("中等");
		break;
	case 2:  //困难
		GotoXY(5, 60);
		g_speed = 200;
		printf("困难");
		break;
	case 0:  //简单
		GotoXY(5, 60);
		g_speed = 500;
		printf("简单");
	}
	g++;
}
void NewGame(void)
{
	system("cls");
	Free();
	Head();
	Frame();
	CreateSnake(3);
	DrawSnake();
	Food();
	SetSocre();
}
void SetSocre(void)
{
	GotoXY(6, 59);
	printf("%d", g_pHead->iCount);
}
bool IsDie(void)
{
	if (1 == g_Back[g_pHead->stSnake.iX][g_pHead->stSnake.iY])
	{
		GotoXY(18, 50);
		printf("Game Over!");
		return true;
	}
	return false;
}
bool Eat(void)
{
	if (2 == g_Back[g_pHead->stSnake.iX][g_pHead->stSnake.iY])
	{
		g_Back[g_pHead->stSnake.iX][g_pHead->stSnake.iY] = 0;
		Appand();
		return true;
	}
	return false;
}
void Appand(void)
{
	//创建节点
	struct Node* pTemp = (struct Node*)malloc(sizeof(struct Node));
	if (NULL == pTemp)
		return;
	//节点成员赋值
	g_pHead->iCount++;

	pTemp->pPre = NULL;
	pTemp->pNext = NULL;
	pTemp->stSnake.emDir = g_pEnd->stSnake.emDir;
	switch (g_pEnd->stSnake.emDir)
	{
	case enum_east:
		pTemp->stSnake.iX = g_pEnd->stSnake.iX;
		pTemp->stSnake.iY = g_pEnd->stSnake.iY - 1;
		break;
	case enum_south:
		pTemp->stSnake.iX = g_pEnd->stSnake.iX - 1;
		pTemp->stSnake.iY = g_pEnd->stSnake.iY;
		break;
	case enum_west:
		pTemp->stSnake.iX = g_pEnd->stSnake.iX;
		pTemp->stSnake.iY = g_pEnd->stSnake.iY + 1;
		break;
	case enum_north:
		pTemp->stSnake.iX = g_pEnd->stSnake.iX + 1;
		pTemp->stSnake.iY = g_pEnd->stSnake.iY;
	}
	//链接到链表的尾巴上
	pTemp->pPre = g_pEnd;
	g_pEnd->pNext = pTemp;
	g_pEnd = pTemp;
}
void Food(void)
{
	while (1)
	{
		//产生一组坐标
		int x = rand() % (X-2) + 1;
		int y = rand() % (Y-2) + 1;
		//判断是否在蛇身上  
		struct Node* pT = g_pHead;
		while (pT != NULL)
		{
			if (x == pT->stSnake.iX && y == pT->stSnake.iY)
				break;
			pT = pT->pNext;
		}
		//在的话就下一次
		if (NULL != pT)
			continue;
		//不在，那就是合理的位置
		else
		{
			//赋值到背景数组上
			g_Back[x][y] = 2;
			//画食物
			GotoXY(9 + x, 35 + y * 2);
			printf("□");
			break;
		}
	}
}
void KeyState(void)
{
	if (GetAsyncKeyState(VK_UP))
	{
		//if (g_pHead->stSnake.emDir != enum_south)
		g_pHead->stSnake.emDir = enum_north;
		/*if (g_pHead->stSnake.emDir == enum_north)
		{
			Drop();
			Move();
			DrawSnake();
		}*/
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		g_pHead->stSnake.emDir = enum_south;
	}
	else if (GetAsyncKeyState(VK_LEFT))
	{
		g_pHead->stSnake.emDir = enum_west;
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		g_pHead->stSnake.emDir = enum_east;
	}
	else if (GetAsyncKeyState('Q'))
	{
		Save();
		NewGame();
	}
	else if (GetAsyncKeyState(VK_ESCAPE))
	{
		Save();
		g_quit = 1;
	}
	else if (GetAsyncKeyState(VK_RETURN))
	{
		g_stop = -g_stop;
	}
	else if (GetAsyncKeyState(VK_TAB))
	{
		Change();
	}
	else if (GetAsyncKeyState('1'))
	{
		Read();
	}
}
void Drop(void)
{
	GotoXY(9 + g_pEnd->stSnake.iX, 35 + g_pEnd->stSnake.iY * 2);
	printf("  ");
}
void Move(void)
{
	if (NULL == g_pHead)
		return;
	//从后向前赋值，前一个节点赋值给后一个节点
	struct Node* pT = g_pEnd;
	while (pT != g_pHead)
	{
		pT->stSnake = pT->pPre->stSnake;
		pT = pT->pPre;
	}
	//单独处理头
	switch (g_pHead->stSnake.emDir)
	{
	case enum_east:
		g_pHead->stSnake.iY++;
		break;
	case enum_south:
		g_pHead->stSnake.iX++;
		break;
	case enum_west:
		g_pHead->stSnake.iY--;
		break;
	case enum_north:
		g_pHead->stSnake.iX--;
		break;
	}
}
void DrawSnake(void)
{
	if (NULL == g_pHead)
		return;

	struct Node* pT = g_pHead;
	while (pT != NULL)
	{
		GotoXY(9 + pT->stSnake.iX, 35 + pT->stSnake.iY * 2);
		printf("●");
		pT = pT->pNext;
	}
}
void CreateSnake(int iCount)
{
	//参数合法性检测
	if (iCount <= 0)
		return;
	//创建
	for (int i = 0; i < iCount; i++)
	{
		//申请节点
		struct Node* pT = (struct Node*)malloc(sizeof(struct Node));
		if (NULL == pT)
			return;
		//节点成员赋值
		pT->iCount = 0;
		pT->pNext = NULL;
		pT->pPre = NULL;
		pT->stSnake.emDir = enum_west;
		pT->stSnake.iX = 0;
		pT->stSnake.iY = 0;
		//将节点连在链表上
		if (NULL == g_pHead)
		{
			g_pHead = pT;
			g_pEnd = pT;
			g_pHead->iCount = 1;
			g_pHead->stSnake.iX = rand()%(X - 2) + 1;
			g_pHead->stSnake.iY = rand() % (Y - iCount - 1) + 1;
		}
		else
		{
			g_pEnd->pNext = pT;
			pT->pPre = g_pEnd;
			g_pEnd = pT;

			g_pHead->iCount += 1;
			g_pEnd->stSnake.iX = g_pEnd->pPre->stSnake.iX;	//同一行，不同列
			g_pEnd->stSnake.iY = g_pEnd->pPre->stSnake.iY + 1;//+1列
		}
	}
}
void Free(void)
{
	if (NULL == g_pHead)
		return;
	struct Node* pT = g_pHead;
	while (NULL != pT)
	{
		//记录被删除的节点
		struct Node* pp = pT;
		//节点往下走
		pT = pT->pNext;
		//释放记录
		free(pp);
	}
	g_pHead = NULL;
	g_pEnd = NULL;
}
void GotoXY(unsigned short hang, unsigned short lie)
{
	COORD cd = {lie, hang};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cd);
}
void Frame(void)
{
	//对边框进行赋值
	for (int i = 0; i < X; i++)
	{
		for (int j = 0; j < Y; j++)
		{
			if (0 == i || 0 == j || i == X - 1 || j == Y - 1)
				g_Back[i][j] = 1;
			else
				g_Back[i][j] = 0;
		}
	}
	//输出
	for (int i = 0; i < X; i++)
	{
		GotoXY(i+9, 35);
		for (int j = 0; j < Y; j++)
		{
			if (1 == g_Back[i][j])
				printf("■");
			else
				printf("  ");
		}
		putchar('\n');
	}
}
void Head(void)
{
	printf("\t\t\t\t\t>>>>>>>>>>  贪吃蛇  <<<<<<<<<<\n");
	printf("\t\t\t\t\t>>>>>  Enter 启动/暂停   <<<<<\n");
	printf("\t\t\t\t\t>>>>> ↑←↓→ 控制方向  <<<<<\n");
	printf("\t\t\t\t\t>>>>>   1 查看历史记录   <<<<<\n");
	printf("\t\t\t\t\t>>>>>   Q 重新开始       <<<<<\n");
	printf("\t\t\t\t\t>>>>> tab 切换难度: 简单 <<<<<\n");
	printf("\t\t\t\t\t>>>>>    当前长度: 0     <<<<<\n");
	printf("\t\t\t\t\t>>>>>   ESC  退出游戏    <<<<<\n");
	printf("\t\t\t\t\t>>>>>>>>>>>>>>><<<<<<<<<<<<<<<\n");
}