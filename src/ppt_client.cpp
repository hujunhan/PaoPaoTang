#define SHOW_CONSOLE
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <windows.h>
#include <graphics.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#define MaxW 800
#define MaxH 800
#define width 20
#define height 20
int direct = 2;
int direct1 = 2;
int count = 0;
char map[width][height] = { 0 };//map数组里 1代表可炸的墙 2代表不可炸的树 3代表泡
int playerx = 19;
int playery = 19;
int playerx1 = 0;
int playery1 = 0;
int AIx = 0;
int AIy = 0;
void DrawTreeWall();//生成随机树地图，树定义为炸不掉的墙，呼吁大家保护树木
void PlayerMove(char ch);
void Bomb(int x, int y);
void ShowFire(int x, int y);//还没有加
void JudgeBomb();//利用clock判断爆炸
void loadimage();
void MoveBack();
int WhereToGo();
void AIMove();
void PlayerMove1(char ch);
void drawall();
PIMAGE grass, fire, p, playerbehind, playerfront, playerleft, playerright, tree, wall,win,lose,start;
int playerdead = 0;
int playerdead1 = 0;
int online();


/*以下是搜索用到的*/
int drawmap(void);
char way[40][40][2] = { 0 };//存路径
char reway[40][2] = { 0 }; //用来回溯时存放路径
char searchmap[22][22] = { 0 };//地图
int findsolution(int x, int y); //回溯路径的函数
void drawsolution();//画出结果的函数

int go(int lx);//lx是第layer层里有几个点，就是分了几个叉 
int flag = 0;//找到终点就结束进程 
int layer = 1;//层数 
int reindex = 0;//用于输出的index
int reflag = 0;//用于输出的flag
int ifno = 1;//用于判断是否没有路
			 /*以上是搜索用到的*/

struct bomb
{
	int isbomb = 0;//默认不是炸弹
	int power = 1;//威力功能已经加入
	clock_t t;
	int x;
	int y;
} pmap[width][height];

int main()
{
	initgraph(MaxW, MaxH);
	loadimage();
	putimage(0, 0, MaxW, MaxH, start, 0, 0, 500, 346, SRCCOPY);
	playerx = playery = 19;
	playery1 = playerx1 = 0;
	online();
	Sleep(2000);
	closegraph();
	return 0;
}
int online()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}

	SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sclient == INVALID_SOCKET)
	{
		printf("invalid socket !");
		return 0;
	}
	char* myip;
	char ip[100];
	myip = ip;
	printf("ip:");
	scanf("%s", myip);
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8888);
	serAddr.sin_addr.S_un.S_addr = inet_addr(myip);
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !");
		closesocket(sclient);
		return 0;
	}
	char sendData[1] = { 0 };
	char recData[1];
	char* rec;
	rec = *map;
	struct send_info
	{
		int smap[20][20];
	} getmap;
	recv(sclient, rec, sizeof(map), 0);
	while (1)
	{
		if (kbhit()) {

			sendData[0] = getch();
			PlayerMove(sendData[0]);
			send(sclient, sendData, 1, 0);
		}
		else
		{
			sendData[0] = 0;
			send(sclient, sendData, 1, 0);
		}
		recv(sclient, recData, 1, 0);
		if (recData[1] != 0)
		{
			PlayerMove1(recData[0]);
		}

		cleardevice();
		drawall();
		JudgeBomb();
		if (playerdead == 1)
		{
			putimage(0, 0, MaxW, MaxH, lose, 0, 0, 480, 360, SRCCOPY);
			break;
		}
		else if (playerdead1 == 1)
		{
			putimage(0, 0, MaxW, MaxH, win, 0, 0, 480, 360, SRCCOPY);
			break;
		}
		Sleep(0);

	}
}
void DrawTreeWall()
{
	int i, j;
	time_t t;
	srand((unsigned)time(&t));


	for (i = 0; i <= (width - 1); i = i + 1)
		for (j = 0; j <= (height - 1); j = j + 1)
		{
			if ((i + j) >= 4 && (i + j) <= 36 && !(rand() % 6) && (i + j)>0)
			{
				map[i][j] = 2;
				putimage(MaxW / width * i, MaxH / height * j, tree);
			}
		}
	for (i = 0; i <= (width - 1); i = i + 1)
		for (j = 0; j <= (height - 1); j = j + 1)
		{

			if (!(rand() % 3) && (i + j)>4 && (i + j) <= 36)
			{
				if (map[i][j] == 0)
				{
					map[i][j] = 1;
					putimage(MaxW / width * i, MaxH / height * j, wall);
				}
			}
		}

}
void PlayerMove(char ch)
{

	switch (ch) {
	case 's':
	{
		if (map[playerx][playery + 1] == 0 && playerx <= (width - 1) && playerx >= 0 && playery >= 0 && (playery + 1) <= (height - 1))
		{
			playery++;
			direct = 2;
		}
		break;
	}
	case 'a':
	{
		if (map[playerx - 1][playery] == 0 && playerx <= (width - 1) && (playerx - 1) >= 0 && playery >= 0 && (playery) <= (height - 1))
		{
			direct = 4;
			playerx--;
		}
		break;
	}
	case 'w':
	{
		if (map[playerx][playery - 1] == 0 && playerx <= (width - 1) && playerx >= 0 && (playery - 1) >= 0 && (playery) <= (height - 1))
		{
			direct = 8;
			playery--;
		}
		break;
	}
	case 'd':
	{
		if (map[playerx + 1][playery] == 0 && (playerx + 1) <= (width - 1) && playerx >= 0 && playery >= 0 && (playery) <= (height - 1))
		{
			direct = 6;
			playerx++;
		}
		break;
	}
	case 32:
	{
		if (map[playerx][playery] == 0)
		{

			pmap[playerx][playery].isbomb = map[playerx][playery] = 3;
			pmap[playerx][playery].t = clock();
		}
		break;
	}
	}

}

void PlayerMove1(char ch)
{

	switch (ch) {
	case 's':
	{
		if (map[playerx1][playery1 + 1] == 0 && playerx1 <= (width - 1) && playerx1 >= 0 && playery1 >= 0 && (playery1 + 1) <= (height - 1))
		{
			playery1++;
			direct1 = 2;
		}
		break;
	}
	case 'a':
	{
		if (map[playerx1 - 1][playery1] == 0 && playerx1 <= (width - 1) && (playerx1 - 1) >= 0 && playery1 >= 0 && (playery1) <= (height - 1))
		{
			direct1 = 4;
			playerx1--;
		}
		break;
	}
	case 'w':
	{
		if (map[playerx1][playery1 - 1] == 0 && playerx1 <= (width - 1) && playerx1 >= 0 && (playery1 - 1) >= 0 && (playery1) <= (height - 1))
		{
			direct1 = 8;
			playery1--;
		}
		break;
	}
	case 'd':
	{
		if (map[playerx1 + 1][playery1] == 0 && (playerx1 + 1) <= (width - 1) && playerx1 >= 0 && playery1 >= 0 && (playery1) <= (height - 1))
		{
			direct1 = 6;
			playerx1++;
		}
		break;
	}
	case 32:
	{
		if (map[playerx1][playery1] == 0)
		{

			pmap[playerx1][playery1].isbomb = map[playerx1][playery1] = 3;
			pmap[playerx1][playery1].t = clock();
		}
		break;
	}
	}

}

void Bomb(int x, int y)
{
	int i, j;
	int power;
	//x = pmap[x][y].x;
	//y = pmap[x][y].y;
	power = pmap[x][y].power;

	for (i = 0; i <= power; i++)
	{
		if (((x - i) == playerx&&y == playery))
			playerdead = 1;
		if ((x - i) == playerx1&&y == playery1)
			playerdead1 = 1;
		if (map[x - i][y] == 3)//如果是泡就引爆这个泡
		{
			pmap[x - i][y].isbomb = map[x - i][y] = 0;
			Bomb(x - i, y);
			break;
		}
		else if (map[x - i][y] == 1)//如果是墙就炸掉，不能接着炸了
		{

			map[x - i][y] = 0;
			break;
		}
		else if (map[x - i][y] == 2)//如果是树，说明不能接着炸了，退出
			break;
	}
	for (i = 0; i <= power; i++)
	{
		if (((x + i) == playerx&&y == playery))
			playerdead = 1;
		if ((x + i) == playerx1&&y == playery1)
			playerdead1 = 1;
		if (map[x + i][y] == 3)
		{
			pmap[x + i][y].isbomb = map[x + i][y] = 0;
			Bomb(x + i, y);
			break;
		}
		else if (map[x + i][y] == 1)
		{

			map[x + i][y] = 0;
			break;
		}
		else if (map[x + i][y] == 2)
			break;
	}
	for (i = 0; i <= power; i++)
	{
		if ((x == playerx && (y - i) == playery))
			playerdead = 1;
		if (x == playerx1 && (y - i) == playery1)
			playerdead1 = 1;
		if (map[x][y - i] == 3)
		{
			pmap[x][y - i].isbomb = map[x][y - i] = 0;
			Bomb(x, y - i);
			break;
		}

		else if (map[x][y - i] == 1)
		{

			map[x][y - i] = 0;
			break;
		}
		else if (map[x][y - i] == 2)
			break;
	}
	for (i = 0; i <= power; i++)
	{
		if ((x == playerx && (y + i) == playery))
			playerdead = 1;
		if (x == playerx1 && (y + i) == playery1)
			playerdead1 = 1;
		if (map[x][y + i] == 3)
		{
			pmap[x][y + i].isbomb = map[x][y + i] = 0;
			Bomb(x, y + i);
			break;
		}
		else if (map[x][y + i] == 1)
		{

			map[x][y + i] = 0;
			break;
		}
		else if (map[x][y + i] == 2)
			break;
	}

}
void JudgeBomb()
{
	int i, j;
	double dt;
	clock_t nowt;
	for (i = 0; i <= width - 1; i++)
		for (j = 0; j <= height - 1; j++)
		{
			if (pmap[i][j].isbomb == 0)
				continue;
			nowt = clock();
			dt = (nowt - pmap[i][j].t) / CLOCKS_PER_SEC;
			if (dt > 1.0)
			{
				Bomb(i, j);
			}
		}
}
void ShowFire(int x, int y)
{
	putimage(MaxW / width * x, MaxH / height * y, fire);
}
void loadimage()
{
	start = newimage();
	win = newimage();
	lose = newimage();
	fire = newimage();
	grass = newimage();
	p = newimage();
	playerfront = newimage();
	playerbehind = newimage();
	playerleft = newimage();
	playerright = newimage();
	tree = newimage();
	wall = newimage();
	getimage(fire, "fire.png", MaxW / width, MaxH / height);
	getimage(grass, "grass.png", MaxW / width, MaxH / height);
	getimage(p, "p.png", MaxW / width, MaxH / height);
	getimage(playerbehind, "playerbehind.png", MaxW / width, MaxH / height);
	getimage(playerfront, "playerfront.png", MaxW / width, MaxH / height);
	getimage(playerleft, "playerleft.png", MaxW / width, MaxH / height);
	getimage(playerright, "playerright.png", MaxW / width, MaxH / height);
	getimage(tree, "tree.png", MaxW / width, MaxH / height);
	getimage(wall, "wall.png", MaxW / width, MaxH / height);
	getimage(win, "win.png");
	getimage(lose, "lose.png");
	getimage(start, "start.png");
}
void drawall()
{
	int i, j;
	for (i = 0; i <= width - 1; i++)
		for (j = 0; j <= height - 1; j++)
		{
			if (map[i][j] == 1)
				putimage(MaxW / width * i, MaxH / height * j, wall);
			if (map[i][j] == 2)
				putimage(MaxW / width * i, MaxH / height * j, tree);
			if (map[i][j] == 3)
				putimage(MaxW / width * i, MaxH / height * j, p);
			if (map[i][j] == 0 )
				putimage(MaxW / width * i, MaxH / height * j, grass);
		}
	if (direct == 2)
		putimage(MaxW / width * playerx, MaxH / height * playery, playerfront);
	if (direct == 4)
		putimage(MaxW / width * playerx, MaxH / height * playery, playerleft);
	if (direct == 6)
		putimage(MaxW / width * playerx, MaxH / height * playery, playerright);
	if (direct == 8)
		putimage(MaxW / width * playerx, MaxH / height * playery, playerbehind);
	if (map[playerx][playery] == 3)
		putimage_alphablend(NULL, p, MaxW / width*playerx, MaxH / height*playery, 90, 0, 0, 40, 40);
	if (direct1 == 2)
		putimage(MaxW / width * playerx1, MaxH / height * playery1, playerfront);
	if (direct1 == 4)
		putimage(MaxW / width * playerx1, MaxH / height * playery1, playerleft);
	if (direct1 == 6)
		putimage(MaxW / width * playerx1, MaxH / height * playery1, playerright);
	if (direct1 == 8)
		putimage(MaxW / width * playerx1, MaxH / height * playery1, playerbehind);
	if (map[playerx1][playery1] == 3)
		putimage_alphablend(NULL, p, MaxW / width*playerx1, MaxH / height*playery1, 90, 0, 0, 40, 40);

}
int drawmap(void)//画地图 
{
	int i, j;
	int x, y;

	char c, check;
	for (i = 0; i <= (width + 1); i++)
		for (j = 0; j <= (height + 1); j++)
		{
			if (i == 0 || i == (width + 1) || j == 0 || j == (height + 1))
				searchmap[i][j] = '#';
		}
	for (i = 0; i <= (width - 1); i++)
		for (j = 0; j <= (height - 1); j++)
			if (map[i][j] == 2)
				searchmap[i + 1][j + 1] = '#';
	AIy = AIx = 19;

	return 0;
}

void MoveBack()
{
	int i = reindex - 1;
	AIx = reway[i + 2][0] - 1;
	AIy = reway[i + 2][1] - 1;
	reindex = reindex + 2;
	Sleep(500);
}

int go(int lx)
{

	int x, y;//点坐标 
	int	index = 0;
	int i, j;

	if (flag == 1)
		return 0;
	if (ifno == 0)
		return 0;
	ifno = 0;

	for (i = 0; i < lx; i++)//分叉	
	{
		x = way[layer - 1][i][0];
		y = way[layer - 1][i][1];
		if (x == (1) && y == (1))
		{
			findsolution(1, 1);
			flag = 1;
		}

		if (searchmap[x + 1][y] == 0)//周围是否是空地
		{
			searchmap[x + 1][y] = layer + 1;
			way[layer][index][0] = x + 1;
			way[layer][index][1] = y;
			index++;
			ifno = 1;
		}
		if (searchmap[x][y + 1] == 0)
		{
			searchmap[x][y + 1] = layer + 1;
			way[layer][index][0] = x;
			way[layer][index][1] = y + 1;
			index++;
			ifno = 1;
		}
		if (searchmap[x - 1][y] == 0)
		{
			searchmap[x - 1][y] = layer + 1;
			way[layer][index][0] = x - 1;
			way[layer][index][1] = y;
			index++;
			ifno = 1;
		}
		if (searchmap[x][y - 1] == 0)
		{
			searchmap[x][y - 1] = layer + 1;
			way[layer][index][0] = x;
			way[layer][index][1] = y - 1;
			index++;
			ifno = 1;

		}

	}


	layer++;
	go(index);

}
int findsolution(int x, int y)//回溯结果
{

	if (reflag == 1)
		return 0;
	if (x == 20 && y == 20)
	{
		reway[reindex][0] = 20;
		reway[reindex][1] = 20;
		reflag = 1;
	}

	while (searchmap[x + 1][y] == searchmap[x][y] - 1)//回溯
	{
		reway[reindex][0] = ++x;
		reway[reindex][1] = y;
		reindex++;


	}
	while (searchmap[x][y + 1] == searchmap[x][y] - 1)
	{
		reway[reindex][0] = x;
		reway[reindex][1] = ++y;
		reindex++;
	}
	while (searchmap[x - 1][y] == searchmap[x][y] - 1)
	{
		reway[reindex][0] = --x;
		reway[reindex][1] = y;
		reindex++;
	}
	while (searchmap[x][y - 1] == searchmap[x][y] - 1)
	{
		reway[reindex][0] = x;
		reway[reindex][1] = --y;
		reindex++;
	}
	findsolution(x, y);
}
int WhereToGo()
{
	int i = (reindex - 1);
	if ((reway[i - 1][0] - reway[i][0]) == 1)
	{
		reindex--;
		return 4;
	}
	if ((reway[i - 1][0] - reway[i][0]) == -1)
	{
		reindex--;
		return 2;
	}
	if ((reway[i - 1][1] - reway[i][1]) == 1)
	{
		reindex--;
		return 3;
	}
	if ((reway[i - 1][1] - reway[i][1]) == -1)
	{
		reindex--;
		return 1;
	}


}
void AIMove()
{
	int choice;
	choice = WhereToGo();

	switch (choice) {
	case 3:
	{
		if (map[AIx][AIy + 1] == 1)
		{

			pmap[AIx][AIy].isbomb = map[AIx][AIy] = 3;
			pmap[AIx][AIy].t = clock();
			MoveBack();
			MoveBack();


		}
		if (map[AIx][AIy + 1] == 0 && AIx <= (width - 1) && AIx >= 0 && AIy >= 0 && (AIy + 1) <= (height - 1))
		{

			++AIy;
		}
		break;
	}
	case 2:
	{
		if (map[AIx - 1][AIy] == 1)
		{
			pmap[AIx][AIy].isbomb = map[AIx][AIy] = 3;
			pmap[AIx][AIy].t = clock();
			MoveBack();
			MoveBack();

		}
		if (map[AIx - 1][AIy] == 0 && AIx <= (width - 1) && (AIx - 1) >= 0 && AIy >= 0 && (AIy) <= (height - 1))
		{
			AIx--;
		}
		break;
	}
	case 1:
	{
		if (map[AIx][AIy - 1] == 1)
		{

			pmap[AIx][AIy].isbomb = map[AIx][AIy] = 3;
			pmap[AIx][AIy].t = clock();
			MoveBack();
			MoveBack();

		}
		if (map[AIx][AIy - 1] == 0 && AIx <= (width - 1) && AIx >= 0 && (AIy - 1) >= 0 && (AIy) <= (height - 1))
		{
			AIy--;
		}
		break;
	}
	case 4:
	{
		if (map[AIx + 1][AIy] == 1)
		{
			pmap[AIx][AIy].isbomb = map[AIx][AIy] = 3;
			pmap[AIx][AIy].t = clock();
			MoveBack();
			MoveBack();

		}
		if (map[AIx + 1][AIy] == 0 && (AIx + 1) <= (width - 1) && AIx >= 0 && AIy >= 0 && (AIy) <= (height - 1))
		{
			AIx++;
		}
		break;
	}
	case 5:

	{
		break;
	}
	}

	/*choice = random(8);

	if(choice==1)
	{
	if (map[AIx][AIy] == 0)
	{
	DrawP(AIx, AIy, 1);
	pmap[AIx][AIy].isbomb = map[AIx][AIy] = 3;
	pmap[AIx][AIy].t = clock();
	}
	}*/
}