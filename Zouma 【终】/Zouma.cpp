#include<graphics.h>
#include<conio.h>
#include <time.h>
#include<vector>
#include<cmath>
#include<iostream>
#include <algorithm>
#include<stdio.h>
#include<mmsystem.h>
#include<windows.h>
#include<tchar.h>
#pragma comment(lib,"Winmm.lib")

using namespace std;

#define WIDTH 1300   
#define HEIGHT 910   
#define Radius 25    
#define ColorNum 8 

class Point  //轨迹点
{
public:
	float x, y;  
	Point()
	{}
	Point(float x0, float y0) 
	{
		x = x0;
		y = y0;
	}
};

class Path   //雪球运动轨迹
{

public:
	vector<Point> keyPoints;  //记录轨迹上的所有点
	float sampleInterval;
	vector<Point> allPoints;//用于雪球的关键点

	void getAllpoints() //获得雪球移动的轨迹点
	{
		int i = 0;
		for (i = 0; i < keyPoints.size() - 1; i++)
		{
			float xd = keyPoints[i + 1].x - keyPoints[i].x;
			float yd = keyPoints[i + 1].y - keyPoints[i].y;
			float length = sqrt(xd * xd + yd * yd);
			int num = length / sampleInterval;
			for (int j = 0; j < num; j++)
			{
				float x_sample = keyPoints[i].x + j * xd / num;
				float y_sample = keyPoints[i].y + j * yd / num;
				allPoints.push_back(Point(x_sample, y_sample));
			}
		}
		allPoints.push_back(Point(keyPoints[i].x, keyPoints[i].y));
	}


	void draw()  //画出轨迹
	{
		setlinecolor(RGB(0, 0, 0));//设置线条颜色
		setfillcolor(RGB(0, 0, 0));//设置填充颜色
		
		//各个点连接形成线段
		for (int i = 0; i < keyPoints.size() - 1; i++)
		{
			line(keyPoints[i].x, keyPoints[i].y, keyPoints[i + 1].x, keyPoints[i + 1].y);

		}
		
		//雪球移动点形成的线段
		for (int i = 0; i < allPoints.size(); i++)
		{
			fillcircle(allPoints[i].x, allPoints[i].y, 2);
		}
	}

	~Path() 
	{
		keyPoints.clear();  
		allPoints.clear();
	}
};

COLORREF colors[ColorNum] = { RGB(126,12,110),BLUE+GREEN,RGB(161,205,121),RGB(215,138,111),RGB(217,135,199),BROWN,RGB(132,220,166),RGB(77,77,77) };//雪球的颜色种类
class Ball //雪球类
{

public:
	Point center;//球心坐标
	float radius;//雪球半径
	int colorId;//雪球的颜色代码
	int indexInPath;//雪球位置在移动轨道中的序号
	int direction;//雪球的移动方向，1表示向终点移动，-1向起点移动，0表示暂时不移动

	void draw()  //画出雪球
	{
		setlinecolor(colors[colorId]);//设置边框颜色
		setfillcolor(colors[colorId]);//设置内部颜色
		fillcircle(center.x, center.y, radius);//以（x，y）为圆心，以radius为半径画圆代表雪球
	}

	void movetoIndexInPath(Path path)//将雪球定位到Path的allPoints中的indexInPath序号位置
	{
		center = path.allPoints[indexInPath];
	}

	void initiate(Path path) //把雪球放在自定义的轨迹起点
	{
		radius = Radius;
		indexInPath = 0; // 初始化雪球的位置序号
		direction = 0;//初始雪球的速度
		movetoIndexInPath(path); // 移动到轨迹上面的对应序号位置	
		colorId = rand() % ColorNum;//随机生成雪球的颜色
	}
	
	void changeIndexbyDirection(Path path)//让雪球沿着自定义轨迹移动
	{
		if (direction == 1 && indexInPath + 1 < path.allPoints.size())
		{
			indexInPath++;
		}
		else if (direction == -1 && indexInPath - 1 >= 0)
		{
			indexInPath--;
		}
	}
};

class Cannon // 冰墩墩类
{

public:
	IMAGE im; // 冰墩墩图片
	IMAGE im_rotate; // 冰墩墩旋转时的图片
	float x, y; // 冰墩墩的中心坐标
	Ball ball;  // 冰墩墩的专属雪球
	float angle; // 旋转角度

	void draw() // 绘制相关元素
	{
		rotateimage(&im_rotate, &im, angle, RGB(241, 247, 252), false, true);//旋转冰墩墩
		putimage(x - im.getwidth() / 2, y - im.getheight() / 2, &im_rotate); // 显示旋转后的冰墩墩
		ball.draw(); // 绘制冰墩墩的雪球
	}

	void setBallPosition() // 生成冰墩墩雪球的坐标
	{
		ball.center.x = x + 100 * cos(angle) + 38;
		ball.center.y = y + 100 * sin(angle) - 20;
	}

	void updateWithMouseMOVE(int mx, int my) // 让冰墩墩跟随鼠标摇摆
	{
		
		float xs = mx - x;
		float ys = my - y;
		float length = sqrt(xs * xs + ys * ys);
		if (length > 4) 
		{
			angle = atan2(-ys, xs); 
			ball.center.x = x + 100 * xs / length + 38;
			ball.center.y = y + 100 * ys / length - 20;
		}
	}

	void updateWithRButtonDown() //当鼠标右键点击时，改变雪球的颜色
	{
		ball.colorId += 1;
		if (ball.colorId == ColorNum)
		{
			ball.colorId = 0;
		}
	}
};

// 在Balls中序号i位置球，寻找其前后有没有和他颜色一样，且多余个连续靠近的球，如果有，就删除掉,返回的结果是删除掉的雪球的个数
int eraseSameColorBalls(int i, Ball fireball, Path& path, vector <Ball>& balls)
{
	// 记录下前后和插入的雪球颜色一样的序号，后面去重复，得到对应的要删除的序号
	vector<int> sameColorIndexes;
	int forward = i;
	int backward = i;
	sameColorIndexes.push_back(i); 

	// 向轨迹的终点方向寻找
	while (forward > 0 && balls[forward].colorId == fireball.colorId)
	{
		sameColorIndexes.push_back(forward);
		if (balls[forward - 1].indexInPath - balls[forward].indexInPath > 2 * Radius / path.sampleInterval)
		{
			break;// 前面一个球和这个球间距过大，跳出循环
		} 
		forward--;
	}
	if (forward == 0 && balls[0].colorId == fireball.colorId) // 处理特殊情况，最接近终点的那个球
	{
		sameColorIndexes.push_back(forward);
	}

	// 向轨迹的起点方向寻找
	while (backward < balls.size() - 1 && balls[backward].colorId == fireball.colorId) // 还没有找到最后一个加入的球
	{
		sameColorIndexes.push_back(backward);
		if (balls[backward].indexInPath - balls[backward + 1].indexInPath > 2 * Radius / path.sampleInterval)
		{
			break;// 前面一个球和这个球间距过大，跳出循环
		} 
		backward++;
	}
	if (backward == balls.size() - 1 && balls[balls.size() - 1].colorId == fireball.colorId) // 处理特殊情况，最接近起点的那个球
	{
		sameColorIndexes.push_back(backward);
	}

	// 去除同样颜色雪球中重复的序号
	sort(sameColorIndexes.begin(), sameColorIndexes.end());
	vector<int>::iterator ite = unique(sameColorIndexes.begin(), sameColorIndexes.end());
	sameColorIndexes.erase(ite, sameColorIndexes.end());

	int NumSameColors = sameColorIndexes.size();
	if (NumSameColors >= 3) // 相同颜色的球达到3个或以上
	{
		int minIndex = sameColorIndexes[0];
		int maxIndex = sameColorIndexes[NumSameColors - 1];
		// 把这些球给删掉						
		balls.erase(balls.begin() + minIndex, balls.begin() + maxIndex + 1);
		return NumSameColors; // 消除了，返回消除雪球数目
	}
	return 0; // 没有消除，返回0
}

Path path; //定义雪球的运动轨迹
vector<Ball> balls;//记录所有的雪球（除冰墩墩的
IMAGE im_login,im_bdd, im_end,im_track,im_win,im_gameover; //用于图片的载入
Cannon cannon;  // 冰墩墩
int gameStatus = 0;  // 游戏的进行状态，-1失败，0正常，1胜利

void startgame()  //游戏主界面的初始化
{
	srand(time(0));//随机初始化种子
	initgraph(WIDTH, HEIGHT);//开设一个窗口
	cleardevice();//清屏
	loadimage(&im_track, "./track.jpg");//导入自定义的轨迹图片
	loadimage(&im_bdd, "bdd.png"); // 导入冰墩墩
	loadimage(&im_end, "end.png"); // 导入终点图片

	//为轨迹类添加一些关键点，用于自定义轨迹的构建
	path.keyPoints.push_back(Point(63, 368));
	path.keyPoints.push_back(Point(63, 755));
	path.keyPoints.push_back(Point(186, 847));
	path.keyPoints.push_back(Point(911, 847));
	path.keyPoints.push_back(Point(911, 733));
	path.keyPoints.push_back(Point(376, 733));
	path.keyPoints.push_back(Point(265, 658));
	path.keyPoints.push_back(Point(265, 265));
	path.keyPoints.push_back(Point(319, 196));
	path.keyPoints.push_back(Point(1038, 196));
	path.keyPoints.push_back(Point(1100, 271));
	path.keyPoints.push_back(Point(1122, 840));
	path.keyPoints.push_back(Point(1244, 840));
	path.keyPoints.push_back(Point(1239, 125));
	path.keyPoints.push_back(Point(1150, 58));
	path.keyPoints.push_back(Point(190, 63));
	
	path.sampleInterval = Radius / 5; 
	path.getAllpoints();    

	// 冰墩墩初始化
	cannon.im = im_bdd; // 冰墩墩图片
	cannon.angle = 0; // 初始角度
	cannon.x = 500;  // 中心点坐标
	cannon.y = 350;
	cannon.ball.radius = Radius - 3; // 冰墩墩雪球的半径
	cannon.ball.colorId = rand() % ColorNum; // 冰墩墩雪球颜色
	cannon.setBallPosition(); // 设置冰墩墩雪球的位置坐标


	//添加最初的雪球
	for (int i = 0; i < 10; i++)
	{
		Ball ball;
		ball.initiate(path);//安放雪球到相应位置
		ball.indexInPath = i * (2 * ball.radius / path.sampleInterval);
		ball.movetoIndexInPath(path);
		balls.push_back(ball);
	}

	BeginBatchDraw();//绘制相关元素
}
	
/*void section()  //待完善的选择关卡制度
{
	initgraph(WIDTH, HEIGHT);//开设一个窗口
	cleardevice();//清屏
	loadimage(&im_section, "section.png");
	putimage(0, 0, &im_section);
	int a = 0;
	a = _getch();
	switch (a)
	{
	case 97:

		break;
	case 98:
		break;
	}
}*/

void begin()//游戏开始界面
{
	initgraph(WIDTH, HEIGHT);//开设一个窗口
	cleardevice();//清屏
	loadimage(&im_login, "login.png");//载入游戏开始的图片
	putimage(0, 0, &im_login);
	int a = 0;
	a = _getch();//读取键盘输入信息
	
	if (a == 13)
	{
		closegraph();//关闭当前的游戏开始界面
		startgame();//进行游戏主界面的初始化
	}
}

void show()//绘制整个运行界面
{
	path.draw();//画出轨迹
	cleardevice();//隐藏掉轨迹线
	putimage(0, 0, &im_track);//显示自定义的轨迹图
	putimage(30, 10, &im_end);//显示终点图片
	cannon.draw(); //画出冰墩墩
	setbkcolor(RGB(241, 247, 252));
	for (int i = 0; i < balls.size(); i++)//画出待合并的雪球
	{
		balls[i].draw();
	}

	if (gameStatus == 1) //跳转到游戏胜利界面
	{
		loadimage(&im_win, "./win.jpg");
		putimage(0, 0, &im_win);
	}
	else if (gameStatus == -1) //跳转到游戏失败界面
	{
		loadimage(&im_gameover, "./gameover.jpg");
		putimage(0, 0, &im_gameover);	
	}

	FlushBatchDraw();
}

void updateWithoutInput()//后台判断函数
{
	static clock_t start = clock(); // 记录第一次运行时刻
	clock_t now = clock(); // 获得当前时刻
	int nowSecond = (int(now - start) / CLOCKS_PER_SEC);// 计算程序目前一共运行了多少秒
	if (nowSecond % 10 == 0 && nowSecond <= 100 && gameStatus == 0)// 100秒内，时间每过10秒钟，新增一批雪球
	{
		Ball ball;  // 定义一个雪球对象
		ball.initiate(path); // 初始化雪球到path最开始的位置上
		balls.push_back(ball); // 把雪球ball添加到balls中
	}
	if (balls.size() == 0)//雪球已清空
	{
		if (nowSecond > 100) // 时间到了，游戏胜利
		{
			gameStatus = 1;
		}
		return ;
	}
	
	if (balls[0].indexInPath >= path.allPoints.size() - 1)//第一个球到达终点，gameover
	{
		gameStatus = -1; 
		return ;
	}

	int i;
	for (i = 0; i < balls.size(); i++)//先让所有雪球不移动
	{
		balls[i].direction = 0;
	}

	//balls向前移动的源动力来自最后面一个雪球，最后一个雪球direction=1
	//如果终点方向前面一个雪球和这个雪球正好相切，则其direction也为1，否则direction为0
	i = balls.size() - 1; // 最后一个雪球
	balls[i].direction = 1; // 最后一个雪球向前运动

	while (i > 0)  // 一直向前判断，还没有遍历到最前面一个雪球
	{
		// 如果前后两个雪球正好相切
		if (balls[i - 1].indexInPath - balls[i].indexInPath <= 2 * Radius / path.sampleInterval)
		{
			balls[i - 1].direction = 1; // 前一个雪球的方向也是向前
			// 对前一个雪球的indexInPath进行规则化，确保正好相切
			balls[i - 1].indexInPath = balls[i].indexInPath + 2 * Radius / path.sampleInterval;
			i--;
		}
		else // 有一个雪球不直接接触，就停止向前速度的传递
		{
			break; // 跳出循环
		}
	}

	for (int i = 0; i < balls.size(); i++)  // 每一个雪球根据其direction更新他的位置
	{
		balls[i].movetoIndexInPath(path);
		balls[i].changeIndexbyDirection(path);
	}
	Sleep(50); 
}

float Distance(float x1, float y1, float x2, float y2)
{
	float xd = x1 - x2;
	float yd = y1 - y2;
	float length = sqrt(xd * xd + yd * yd);
	return length;
}

void updateWithInput() //读取鼠标的信息进行雪球的插入与消除
{
	if (gameStatus != 0) //游戏胜利或失败，函数直接返回
	{
		return;
	}

	int i, j;
	MOUSEMSG m;
	while (MouseHit())//检测当前是否有鼠标消息
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_MOUSEMOVE)  //鼠标移动，进行冰墩墩的旋转			
		{
			cannon.updateWithMouseMOVE(m.x, m.y); //冰墩墩旋转，雪球也相应旋转
		}
		else if (m.uMsg == WM_RBUTTONDOWN)  //鼠标右键点击，更改冰墩墩要发射的雪球的颜色		
		{
			cannon.updateWithRButtonDown();
		}
		else if (m.uMsg == WM_LBUTTONDOWN)//鼠标左键点击
		{
			//balls中的所有雪球的坐标和鼠标点击坐标判断，看是否点中了一个雪球
			for (i = 0; i < balls.size(); i++)
			{
				float distance = Distance(balls[i].center.x, balls[i].center.y, m.x, m.y);
				if (distance < Radius) // 鼠标点击中了一个雪球
				{
					// 将冰墩墩的雪球移动到此
					Ball fireball = balls[i];
					fireball.colorId = cannon.ball.colorId;
					balls.insert(balls.begin() + i, fireball);//复制一个雪球插入vector
					
					//在Balls中序号i位置球，寻找其前后有没有和他颜色一样，且多余3个连续靠近的球，如果是，就删除掉,返回的结果是删除掉的雪球的个数
					int count = eraseSameColorBalls(i, fireball, path, balls);
					if (count >= 3)
					{
						mciSendString(_T("open coin.mp3"), NULL, 0, NULL); 
						mciSendString(_T("play coin.mp3"), NULL, 0, NULL); 
					}
					if (count == 0)// 如果没有消除的话
					{
						for (j = i; j >= 0; j--) // 移动前面的雪球，留出空间放下新插入的雪球
						{
							if (balls[j].indexInPath - balls[j + 1].indexInPath <= 0)
								balls[j].indexInPath = balls[j + 1].indexInPath +
								2 * Radius / path.sampleInterval;
							else
							{
								break;// 前面雪球间有空隙，不用再处理了
							}
						}
					}
					cannon.updateWithRButtonDown();//发射一个雪球后刷新冰墩墩雪球的颜色
					return; 
				}
			}
		}
	}
}

void gameover()//游戏结束时的处理
{
	balls.clear();
}

int main()
{
	
	begin();
	mciSendString(_T("open game_music.mp3"), NULL, 0, NULL);
	mciSendString(_T("play game_music.mp3"), NULL, 0, NULL);
	int a = 0;
	while (1)
	{
		show();
		updateWithInput();
		updateWithoutInput();
	}
	mciSendString(_T("close game_music.mp3"), NULL, 0, NULL);
	gameover();
	return 0;
}