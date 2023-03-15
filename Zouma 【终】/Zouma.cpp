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

class Point  //�켣��
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

class Path   //ѩ���˶��켣
{

public:
	vector<Point> keyPoints;  //��¼�켣�ϵ����е�
	float sampleInterval;
	vector<Point> allPoints;//����ѩ��Ĺؼ���

	void getAllpoints() //���ѩ���ƶ��Ĺ켣��
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


	void draw()  //�����켣
	{
		setlinecolor(RGB(0, 0, 0));//����������ɫ
		setfillcolor(RGB(0, 0, 0));//���������ɫ
		
		//�����������γ��߶�
		for (int i = 0; i < keyPoints.size() - 1; i++)
		{
			line(keyPoints[i].x, keyPoints[i].y, keyPoints[i + 1].x, keyPoints[i + 1].y);

		}
		
		//ѩ���ƶ����γɵ��߶�
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

COLORREF colors[ColorNum] = { RGB(126,12,110),BLUE+GREEN,RGB(161,205,121),RGB(215,138,111),RGB(217,135,199),BROWN,RGB(132,220,166),RGB(77,77,77) };//ѩ�����ɫ����
class Ball //ѩ����
{

public:
	Point center;//��������
	float radius;//ѩ��뾶
	int colorId;//ѩ�����ɫ����
	int indexInPath;//ѩ��λ�����ƶ�����е����
	int direction;//ѩ����ƶ�����1��ʾ���յ��ƶ���-1������ƶ���0��ʾ��ʱ���ƶ�

	void draw()  //����ѩ��
	{
		setlinecolor(colors[colorId]);//���ñ߿���ɫ
		setfillcolor(colors[colorId]);//�����ڲ���ɫ
		fillcircle(center.x, center.y, radius);//�ԣ�x��y��ΪԲ�ģ���radiusΪ�뾶��Բ����ѩ��
	}

	void movetoIndexInPath(Path path)//��ѩ��λ��Path��allPoints�е�indexInPath���λ��
	{
		center = path.allPoints[indexInPath];
	}

	void initiate(Path path) //��ѩ������Զ���Ĺ켣���
	{
		radius = Radius;
		indexInPath = 0; // ��ʼ��ѩ���λ�����
		direction = 0;//��ʼѩ����ٶ�
		movetoIndexInPath(path); // �ƶ����켣����Ķ�Ӧ���λ��	
		colorId = rand() % ColorNum;//�������ѩ�����ɫ
	}
	
	void changeIndexbyDirection(Path path)//��ѩ�������Զ���켣�ƶ�
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

class Cannon // ���ն���
{

public:
	IMAGE im; // ���ն�ͼƬ
	IMAGE im_rotate; // ���ն���תʱ��ͼƬ
	float x, y; // ���նյ���������
	Ball ball;  // ���նյ�ר��ѩ��
	float angle; // ��ת�Ƕ�

	void draw() // �������Ԫ��
	{
		rotateimage(&im_rotate, &im, angle, RGB(241, 247, 252), false, true);//��ת���ն�
		putimage(x - im.getwidth() / 2, y - im.getheight() / 2, &im_rotate); // ��ʾ��ת��ı��ն�
		ball.draw(); // ���Ʊ��նյ�ѩ��
	}

	void setBallPosition() // ���ɱ��ն�ѩ�������
	{
		ball.center.x = x + 100 * cos(angle) + 38;
		ball.center.y = y + 100 * sin(angle) - 20;
	}

	void updateWithMouseMOVE(int mx, int my) // �ñ��նո������ҡ��
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

	void updateWithRButtonDown() //������Ҽ����ʱ���ı�ѩ�����ɫ
	{
		ball.colorId += 1;
		if (ball.colorId == ColorNum)
		{
			ball.colorId = 0;
		}
	}
};

// ��Balls�����iλ����Ѱ����ǰ����û�к�����ɫһ�����Ҷ��������������������У���ɾ����,���صĽ����ɾ������ѩ��ĸ���
int eraseSameColorBalls(int i, Ball fireball, Path& path, vector <Ball>& balls)
{
	// ��¼��ǰ��Ͳ����ѩ����ɫһ������ţ�����ȥ�ظ����õ���Ӧ��Ҫɾ�������
	vector<int> sameColorIndexes;
	int forward = i;
	int backward = i;
	sameColorIndexes.push_back(i); 

	// ��켣���յ㷽��Ѱ��
	while (forward > 0 && balls[forward].colorId == fireball.colorId)
	{
		sameColorIndexes.push_back(forward);
		if (balls[forward - 1].indexInPath - balls[forward].indexInPath > 2 * Radius / path.sampleInterval)
		{
			break;// ǰ��һ�������������������ѭ��
		} 
		forward--;
	}
	if (forward == 0 && balls[0].colorId == fireball.colorId) // ���������������ӽ��յ���Ǹ���
	{
		sameColorIndexes.push_back(forward);
	}

	// ��켣����㷽��Ѱ��
	while (backward < balls.size() - 1 && balls[backward].colorId == fireball.colorId) // ��û���ҵ����һ���������
	{
		sameColorIndexes.push_back(backward);
		if (balls[backward].indexInPath - balls[backward + 1].indexInPath > 2 * Radius / path.sampleInterval)
		{
			break;// ǰ��һ�������������������ѭ��
		} 
		backward++;
	}
	if (backward == balls.size() - 1 && balls[balls.size() - 1].colorId == fireball.colorId) // ���������������ӽ������Ǹ���
	{
		sameColorIndexes.push_back(backward);
	}

	// ȥ��ͬ����ɫѩ�����ظ������
	sort(sameColorIndexes.begin(), sameColorIndexes.end());
	vector<int>::iterator ite = unique(sameColorIndexes.begin(), sameColorIndexes.end());
	sameColorIndexes.erase(ite, sameColorIndexes.end());

	int NumSameColors = sameColorIndexes.size();
	if (NumSameColors >= 3) // ��ͬ��ɫ����ﵽ3��������
	{
		int minIndex = sameColorIndexes[0];
		int maxIndex = sameColorIndexes[NumSameColors - 1];
		// ����Щ���ɾ��						
		balls.erase(balls.begin() + minIndex, balls.begin() + maxIndex + 1);
		return NumSameColors; // �����ˣ���������ѩ����Ŀ
	}
	return 0; // û������������0
}

Path path; //����ѩ����˶��켣
vector<Ball> balls;//��¼���е�ѩ�򣨳����նյ�
IMAGE im_login,im_bdd, im_end,im_track,im_win,im_gameover; //����ͼƬ������
Cannon cannon;  // ���ն�
int gameStatus = 0;  // ��Ϸ�Ľ���״̬��-1ʧ�ܣ�0������1ʤ��

void startgame()  //��Ϸ������ĳ�ʼ��
{
	srand(time(0));//�����ʼ������
	initgraph(WIDTH, HEIGHT);//����һ������
	cleardevice();//����
	loadimage(&im_track, "./track.jpg");//�����Զ���Ĺ켣ͼƬ
	loadimage(&im_bdd, "bdd.png"); // ������ն�
	loadimage(&im_end, "end.png"); // �����յ�ͼƬ

	//Ϊ�켣�����һЩ�ؼ��㣬�����Զ���켣�Ĺ���
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

	// ���նճ�ʼ��
	cannon.im = im_bdd; // ���ն�ͼƬ
	cannon.angle = 0; // ��ʼ�Ƕ�
	cannon.x = 500;  // ���ĵ�����
	cannon.y = 350;
	cannon.ball.radius = Radius - 3; // ���ն�ѩ��İ뾶
	cannon.ball.colorId = rand() % ColorNum; // ���ն�ѩ����ɫ
	cannon.setBallPosition(); // ���ñ��ն�ѩ���λ������


	//��������ѩ��
	for (int i = 0; i < 10; i++)
	{
		Ball ball;
		ball.initiate(path);//����ѩ����Ӧλ��
		ball.indexInPath = i * (2 * ball.radius / path.sampleInterval);
		ball.movetoIndexInPath(path);
		balls.push_back(ball);
	}

	BeginBatchDraw();//�������Ԫ��
}
	
/*void section()  //�����Ƶ�ѡ��ؿ��ƶ�
{
	initgraph(WIDTH, HEIGHT);//����һ������
	cleardevice();//����
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

void begin()//��Ϸ��ʼ����
{
	initgraph(WIDTH, HEIGHT);//����һ������
	cleardevice();//����
	loadimage(&im_login, "login.png");//������Ϸ��ʼ��ͼƬ
	putimage(0, 0, &im_login);
	int a = 0;
	a = _getch();//��ȡ����������Ϣ
	
	if (a == 13)
	{
		closegraph();//�رյ�ǰ����Ϸ��ʼ����
		startgame();//������Ϸ������ĳ�ʼ��
	}
}

void show()//�����������н���
{
	path.draw();//�����켣
	cleardevice();//���ص��켣��
	putimage(0, 0, &im_track);//��ʾ�Զ���Ĺ켣ͼ
	putimage(30, 10, &im_end);//��ʾ�յ�ͼƬ
	cannon.draw(); //�������ն�
	setbkcolor(RGB(241, 247, 252));
	for (int i = 0; i < balls.size(); i++)//�������ϲ���ѩ��
	{
		balls[i].draw();
	}

	if (gameStatus == 1) //��ת����Ϸʤ������
	{
		loadimage(&im_win, "./win.jpg");
		putimage(0, 0, &im_win);
	}
	else if (gameStatus == -1) //��ת����Ϸʧ�ܽ���
	{
		loadimage(&im_gameover, "./gameover.jpg");
		putimage(0, 0, &im_gameover);	
	}

	FlushBatchDraw();
}

void updateWithoutInput()//��̨�жϺ���
{
	static clock_t start = clock(); // ��¼��һ������ʱ��
	clock_t now = clock(); // ��õ�ǰʱ��
	int nowSecond = (int(now - start) / CLOCKS_PER_SEC);// �������Ŀǰһ�������˶�����
	if (nowSecond % 10 == 0 && nowSecond <= 100 && gameStatus == 0)// 100���ڣ�ʱ��ÿ��10���ӣ�����һ��ѩ��
	{
		Ball ball;  // ����һ��ѩ�����
		ball.initiate(path); // ��ʼ��ѩ��path�ʼ��λ����
		balls.push_back(ball); // ��ѩ��ball��ӵ�balls��
	}
	if (balls.size() == 0)//ѩ�������
	{
		if (nowSecond > 100) // ʱ�䵽�ˣ���Ϸʤ��
		{
			gameStatus = 1;
		}
		return ;
	}
	
	if (balls[0].indexInPath >= path.allPoints.size() - 1)//��һ���򵽴��յ㣬gameover
	{
		gameStatus = -1; 
		return ;
	}

	int i;
	for (i = 0; i < balls.size(); i++)//��������ѩ���ƶ�
	{
		balls[i].direction = 0;
	}

	//balls��ǰ�ƶ���Դ�������������һ��ѩ�����һ��ѩ��direction=1
	//����յ㷽��ǰ��һ��ѩ������ѩ���������У�����directionҲΪ1������directionΪ0
	i = balls.size() - 1; // ���һ��ѩ��
	balls[i].direction = 1; // ���һ��ѩ����ǰ�˶�

	while (i > 0)  // һֱ��ǰ�жϣ���û�б�������ǰ��һ��ѩ��
	{
		// ���ǰ������ѩ����������
		if (balls[i - 1].indexInPath - balls[i].indexInPath <= 2 * Radius / path.sampleInterval)
		{
			balls[i - 1].direction = 1; // ǰһ��ѩ��ķ���Ҳ����ǰ
			// ��ǰһ��ѩ���indexInPath���й��򻯣�ȷ����������
			balls[i - 1].indexInPath = balls[i].indexInPath + 2 * Radius / path.sampleInterval;
			i--;
		}
		else // ��һ��ѩ��ֱ�ӽӴ�����ֹͣ��ǰ�ٶȵĴ���
		{
			break; // ����ѭ��
		}
	}

	for (int i = 0; i < balls.size(); i++)  // ÿһ��ѩ�������direction��������λ��
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

void updateWithInput() //��ȡ������Ϣ����ѩ��Ĳ���������
{
	if (gameStatus != 0) //��Ϸʤ����ʧ�ܣ�����ֱ�ӷ���
	{
		return;
	}

	int i, j;
	MOUSEMSG m;
	while (MouseHit())//��⵱ǰ�Ƿ��������Ϣ
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_MOUSEMOVE)  //����ƶ������б��նյ���ת			
		{
			cannon.updateWithMouseMOVE(m.x, m.y); //���ն���ת��ѩ��Ҳ��Ӧ��ת
		}
		else if (m.uMsg == WM_RBUTTONDOWN)  //����Ҽ���������ı��ն�Ҫ�����ѩ�����ɫ		
		{
			cannon.updateWithRButtonDown();
		}
		else if (m.uMsg == WM_LBUTTONDOWN)//���������
		{
			//balls�е�����ѩ������������������жϣ����Ƿ������һ��ѩ��
			for (i = 0; i < balls.size(); i++)
			{
				float distance = Distance(balls[i].center.x, balls[i].center.y, m.x, m.y);
				if (distance < Radius) // ���������һ��ѩ��
				{
					// �����նյ�ѩ���ƶ�����
					Ball fireball = balls[i];
					fireball.colorId = cannon.ball.colorId;
					balls.insert(balls.begin() + i, fireball);//����һ��ѩ�����vector
					
					//��Balls�����iλ����Ѱ����ǰ����û�к�����ɫһ�����Ҷ���3������������������ǣ���ɾ����,���صĽ����ɾ������ѩ��ĸ���
					int count = eraseSameColorBalls(i, fireball, path, balls);
					if (count >= 3)
					{
						mciSendString(_T("open coin.mp3"), NULL, 0, NULL); 
						mciSendString(_T("play coin.mp3"), NULL, 0, NULL); 
					}
					if (count == 0)// ���û�������Ļ�
					{
						for (j = i; j >= 0; j--) // �ƶ�ǰ���ѩ�������ռ�����²����ѩ��
						{
							if (balls[j].indexInPath - balls[j + 1].indexInPath <= 0)
								balls[j].indexInPath = balls[j + 1].indexInPath +
								2 * Radius / path.sampleInterval;
							else
							{
								break;// ǰ��ѩ����п�϶�������ٴ�����
							}
						}
					}
					cannon.updateWithRButtonDown();//����һ��ѩ���ˢ�±��ն�ѩ�����ɫ
					return; 
				}
			}
		}
	}
}

void gameover()//��Ϸ����ʱ�Ĵ���
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