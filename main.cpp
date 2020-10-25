#include "MyImage.h"
#include "DSpriteManager.h"
#include "WindowsX.h"

Graphics *g_BackBuffer;
Graphics *g_MainBuffer;
Bitmap    *g_Bitmap;
Pen		   *g_pPen;

typedef struct _tgAniInfo
{
	Rect aniRect[10];
	int     aniTime[10];
	int     interval;
	Rect g_center[20];
}ANIINFO, *LPANIINFO;

ANIINFO g_aniInfo[20];
int     g_aniDirection = 12;
int     g_aniIndex = 0;
int     no_aniIndex = 0;

ANIINFO g_aniIndex1;

DWORD g_interval = 0;

CMyImage g_myImage;
CMyImage g_myImage2;
CMyImage g_myImage3;
CMyImage g_myImage4;
CMyImage g_myObjects;

Rect g_rcCollisionObjects[10];

#define MAX_FRAME 2

struct VECTOR2
{
	float  x;
	float  y;
};

VECTOR2 g_vecDir = { 0.0f, 0.0f };
VECTOR2 person_vecPos = { 100, 480 };
VECTOR2 brownH_vecPos = { 400, 80 };
VECTOR2 yellowH_vecPos = { 600, 80 };
VECTOR2 redH_vecPos = { 850, 80 };
VECTOR2 ppC_vecPos = { 370, 220 }; //370 220 //106 230
VECTOR2 whiteC_vecPos = { 600, 220 }; //600 220 // 170 230
VECTOR2 pinkC_vecPos = { 750, 220 }; // 750 220 // 185 300
VECTOR2 blueC_vecPos = { 900, 200 }; // 900 200 // 213 325
VECTOR2 yellowS_vecPos = {430,450}; // 430 450 // 192 468
VECTOR2 blackS_vecPos = { 600,450 }; // 600 450 // 189 502
VECTOR2 Stop_vecPos = { 780,-100 }; //210 270
VECTOR2 Sbot_vecPos = { 880,-100 }; // 230 370

float g_speed = 200.0f;
int choice = 0; //옷 선택 횟수
BOOL start = false; //인게임
BOOL GoodChoice = false; //결정 
BOOL cantouchtop = false; //떨어지는 히든 옷 선택
BOOL cantouchbot = false;

BOOL CheckCollision(Rect rc1, Rect rc2);

void OnUpdate(HWND hWnd, DWORD tick);
void CreateBuffer(HWND hWnd, HDC hDC);
void ReleaseBuffer(HWND hWnd, HDC hDC);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,     int nCmdShow)
{
	WNDCLASS   wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
	wndclass.hIcon = NULL;
    wndclass.hCursor = NULL;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = "myGame";
	
	if(RegisterClass(&wndclass) == 0)
	{
		return 0;		
	}

	RECT rc = { 0, 0, 1024, 768 };
	::AdjustWindowRect(&rc,
		WS_POPUP | WS_THICKFRAME | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX,
		FALSE);

	HWND hwnd = CreateWindow("myGame", "Game Window",
		WS_POPUP | WS_THICKFRAME | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX,
		10, 10, rc.right-rc.left, rc.bottom-rc.top, NULL, NULL, hInstance, NULL);


	if(hwnd == NULL)
	{
		return 0;
	}

	HDC hDC = GetDC(hwnd);


	CreateBuffer(hwnd, hDC);

	g_pPen = new Pen(Color(255, 0, 0), 1.0f);

	g_myImage.Load("./Data/Image/cloth.png");
	g_myImage2.Load("./Data/Image/title.png");
	g_myImage3.Load("./Data/Image/hidden.png");
	g_myImage4.Load("./Data/Image/fanpare.png");

	int frame, type, left, top, width, height, anitime, xCenter, yCenter, xCenterW, yCenterH;
	FILE *fp = fopen("img_ani.txt", "rt");
	fscanf(fp, "%d %d", &frame, &type);
	

	for (int i = 0; i < frame; i++)
	{
		int j = i / MAX_FRAME;
		int k = i % MAX_FRAME;

		if (type == 1)
		{
			fscanf(fp, "%d %d %d %d %d %d %d %d %d", &left, &top, &width, &height, &anitime, &xCenter, &yCenter, &xCenterW, &yCenterH);
			g_aniInfo[j].aniRect[k].X = left;
			g_aniInfo[j].aniRect[k].Y = top;
			g_aniInfo[j].aniRect[k].Width = width;
			g_aniInfo[j].aniRect[k].Height = height;
			g_aniInfo[j].aniTime[k] = anitime;

			g_aniInfo[j].g_center[k].X = xCenter;
			g_aniInfo[j].g_center[k].Y = yCenter;
			g_aniInfo[j].g_center[k].Width = xCenterW;
			g_aniInfo[j].g_center[k].Height = yCenterH;
			g_aniInfo[j].interval = 0;
		}
	}

	fscanf(fp, "%d %d %d %d %d", &left, &top, &width, &height, &anitime);
	g_aniInfo[15].aniRect[0].X = left;
	g_aniInfo[15].aniRect[0].Y = top;
	g_aniInfo[15].aniRect[0].Width = width;
	g_aniInfo[15].aniRect[0].Height = height;

	fclose(fp);


	//옷장 콜라이더
	fp = fopen("img_objects.txt", "rt");
	fscanf(fp, "%d %d", &frame, &type);

	int colX, colY, colWidth, colHeight;

	for (int i = 0; i < frame; i++)
	{
		fscanf(fp, "%d %d %d %d", &colX, &colY, &colWidth, &colHeight);

		g_rcCollisionObjects[i].X = colX;
		g_rcCollisionObjects[i].Y = colY;
		g_rcCollisionObjects[i].Width = colWidth;
		g_rcCollisionObjects[i].Height = colHeight;

	}

	fclose(fp);

	ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

	MSG msg;
	DWORD tick = GetTickCount();
	while(1)
	{	
		//윈도우 메세지가 있을경우 메세지를 처리한다.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else //메세지가 없을 경우 게임 루프를 실행한다.
		{		
			DWORD curTick = GetTickCount();
			OnUpdate(hwnd, curTick - tick);
			tick = curTick;

			
			g_MainBuffer->DrawImage(g_Bitmap, 0, 0);
		}
	}

	delete g_pPen;
	ReleaseBuffer(hwnd, hDC);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch(msg)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case 0x51://갈색머리 //Q 
			yellowH_vecPos.x = 600; yellowH_vecPos.y = 80; //제자리
			redH_vecPos.x = 850; redH_vecPos.y = 80; //제자리
			brownH_vecPos.x = 160; brownH_vecPos.y = 95; // 이동
			choice++;
			break;
		case 0x57://노랑머리 //W
			brownH_vecPos.x = 400; brownH_vecPos.y = 80; //제자리
			redH_vecPos.x = 850; redH_vecPos.y = 80; //제자리
			yellowH_vecPos.x = 160; yellowH_vecPos.y = 95; //이동
			choice++;
			break;
		case 0x45://빨간머리 //E
			brownH_vecPos.x = 400; brownH_vecPos.y = 80; //제자리
			yellowH_vecPos.x = 600; yellowH_vecPos.y = 80; //제자리
			redH_vecPos.x = 240; redH_vecPos.y = 55; //이동
			choice++;
			break;

		case 0x41://보라후드 //A
			ppC_vecPos.x = 106; ppC_vecPos.y = 230;
			whiteC_vecPos.x = 600; whiteC_vecPos.y = 220;
			if (cantouchtop == true)
			{
				Stop_vecPos.x = 780; Stop_vecPos.y = 450;
			}
			choice++;
			break;
		case 0x53://하얀티 //S
			whiteC_vecPos.x = 170; whiteC_vecPos.y = 230;
			ppC_vecPos.x = 370; ppC_vecPos.y = 220;
			if (cantouchtop == true)
			{
				Stop_vecPos.x = 780; Stop_vecPos.y = 450;
			}
			choice++;
			break;

		case 0x44://핑크치마 //D
			pinkC_vecPos.x = 185; pinkC_vecPos.y = 300;
			blueC_vecPos.x = 900; blueC_vecPos.y = 200;
			if (cantouchbot == true)
			{
				Sbot_vecPos.x = 880; Sbot_vecPos.y = 450;
			}
			choice++;
			break;
		case 0x46://청바지 //F
			blueC_vecPos.x = 213; blueC_vecPos.y = 325;
			pinkC_vecPos.x = 750; pinkC_vecPos.y = 220;
			if (cantouchbot == true)
			{
				Sbot_vecPos.x = 880; Sbot_vecPos.y = 450;
			}
			choice++;
			break;

		case 0x5A://노란신발 //Z
			yellowS_vecPos.x = 192; yellowS_vecPos.y = 468;
			blackS_vecPos.x = 600; blackS_vecPos.y = 450;
			choice++;
			break;
		case 0x58://슬리퍼 //X
			blackS_vecPos.x = 189; blackS_vecPos.y = 502;
			yellowS_vecPos.x = 430; yellowS_vecPos.y = 450;
			choice++;
			break;

		case 0x42://모두 제자리
			brownH_vecPos.x = 400; brownH_vecPos.y = 80;
			yellowH_vecPos.x = 600; yellowH_vecPos.y = 80;
			redH_vecPos.x = 850; redH_vecPos.y = 80;

			ppC_vecPos.x = 370; ppC_vecPos.y = 220;
			whiteC_vecPos.x = 600; whiteC_vecPos.y = 220;
			pinkC_vecPos.x = 750; pinkC_vecPos.y = 220;
			blueC_vecPos.x = 900; blueC_vecPos.y = 200;

			yellowS_vecPos.x = 430; yellowS_vecPos.y = 450;
			blackS_vecPos.x = 600; blackS_vecPos.y = 450;

			if (cantouchtop == true && cantouchbot == true)
			{
				Stop_vecPos.x = 780; Stop_vecPos.y = 450;
				Sbot_vecPos.x = 880; Sbot_vecPos.y = 450;
			}
			break; //B

		case 0x4F: //히든 상의 // O
			if (choice >= 10)
			{
				if (Stop_vecPos.y >= 450)
				{
					Stop_vecPos.x = 210; Stop_vecPos.y = 270;
					ppC_vecPos.x = 370; ppC_vecPos.y = 220;
					whiteC_vecPos.x = 600; whiteC_vecPos.y = 220;
					choice++;
				}
				else
				{
					Stop_vecPos.y = 450;
				}
			}
			break;
		case 0x48://히든하의 //h
			if (choice >= 20)
			{
				if (Sbot_vecPos.y >= 450)
				{
					Sbot_vecPos.x = 230; Sbot_vecPos.y = 358;
					pinkC_vecPos.x = 750; pinkC_vecPos.y = 220;
					blueC_vecPos.x = 900; blueC_vecPos.y = 200;
					choice++;
				}
				else
				{
					Sbot_vecPos.y = 450;
				}
			}
			break;
			
			case VK_LEFT: g_aniDirection = 13; g_vecDir.x = -1.0f; g_vecDir.y = 0.0f; break;
			case VK_RIGHT: g_aniDirection = 13; g_vecDir.x = 1.0f; g_vecDir.y = 0.0f; break;
			case VK_DOWN: g_aniDirection = 12; g_vecDir.x = 0.0f; g_vecDir.y = 0.0f; break;
			case 0x31: // 게임 초기화
				g_aniDirection = 12;  g_vecDir.x = 0.0f; g_vecDir.y = 0.0f; person_vecPos.x = 100; person_vecPos.y = 480;
				start = false; GoodChoice = false; cantouchtop = false; cantouchbot = false;
				
				brownH_vecPos.x = 400; brownH_vecPos.y = 80;
				yellowH_vecPos.x = 600; yellowH_vecPos.y = 80;
				redH_vecPos.x = 850; redH_vecPos.y = 80;

				ppC_vecPos.x = 370; ppC_vecPos.y = 220;
				whiteC_vecPos.x = 600; whiteC_vecPos.y = 220;
				pinkC_vecPos.x = 750; pinkC_vecPos.y = 220;
				blueC_vecPos.x = 900; blueC_vecPos.y = 200;

				yellowS_vecPos.x = 430; yellowS_vecPos.y = 450;
				blackS_vecPos.x = 600; blackS_vecPos.y = 450;

				Stop_vecPos.x = 780; Stop_vecPos.y = -100;
				Sbot_vecPos.x = 880; Sbot_vecPos.y = -100;
				
				 choice = 0; break;
			case VK_SPACE: GoodChoice = true; break;
		}
	}
	break;
	case WM_CREATE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}


	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void CreateBuffer(HWND hWnd, HDC hDC)
{
	GdiplusStartupInput			gdiplusStartupInput;
	ULONG_PTR					gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	RECT rc;
	GetClientRect(hWnd, &rc);

	g_Bitmap = new Bitmap(rc.right - rc.left, rc.bottom - rc.top);
	g_BackBuffer = new Graphics(g_Bitmap);
	g_BackBuffer->SetPageUnit(Gdiplus::Unit::UnitPixel);
	
	g_MainBuffer = new Graphics(hDC);
	g_MainBuffer->SetPageUnit(Gdiplus::Unit::UnitPixel);
}

void ReleaseBuffer(HWND hWnd, HDC hDC)
{
	ReleaseDC(hWnd, hDC);

	delete g_Bitmap;
	delete g_BackBuffer;
	delete g_MainBuffer;
}

void OnUpdate(HWND hWnd, DWORD tick)
{
	if (hWnd == NULL)
		return;

	Color color(255, 255, 255);
	g_BackBuffer->Clear(color);

	int xPos;
	int yPos;

	xPos = person_vecPos.x + (g_speed * (tick / 1000.0f) * g_vecDir.x);
	yPos = person_vecPos.y + (g_speed * (tick / 1000.0f) * g_vecDir.y);

	//충돌영역
	Rect rc1(g_aniInfo[12].g_center[g_aniIndex]);
	rc1.X = xPos + g_aniInfo[12].g_center[0].X;
	rc1.Y = yPos + g_aniInfo[12].g_center[0].Y;

	Rect rc_cloths(g_rcCollisionObjects[0]);
	rc_cloths.X = 650 + g_rcCollisionObjects[0].X;
	rc_cloths.Y = 340 + g_rcCollisionObjects[0].Y;

	if (!CheckCollision(rc1, rc_cloths))
	{
		person_vecPos.x = xPos;
		person_vecPos.y = yPos;
	}
	else
	{
		start = true;
	}

	g_interval += tick;

	g_aniInfo[g_aniDirection].interval += tick;
	if (!GoodChoice)
	{
		g_myImage.Draw(g_BackBuffer, 100, 100,
			g_aniInfo[0].aniRect[g_aniIndex].X,
			g_aniInfo[0].aniRect[g_aniIndex].Y,
			g_aniInfo[0].aniRect[g_aniIndex].Width,
			g_aniInfo[0].aniRect[g_aniIndex].Height);//몸

		no_aniIndex = g_aniIndex;

		//머리
		g_myImage.Draw(g_BackBuffer, brownH_vecPos.x, brownH_vecPos.y,
			g_aniInfo[1].aniRect[no_aniIndex].X,
			g_aniInfo[1].aniRect[no_aniIndex].Y,
			g_aniInfo[1].aniRect[no_aniIndex].Width,
			g_aniInfo[1].aniRect[no_aniIndex].Height);

		g_myImage.Draw(g_BackBuffer, yellowH_vecPos.x, yellowH_vecPos.y,
			g_aniInfo[2].aniRect[no_aniIndex].X,
			g_aniInfo[2].aniRect[no_aniIndex].Y,
			g_aniInfo[2].aniRect[no_aniIndex].Width,
			g_aniInfo[2].aniRect[no_aniIndex].Height);

		g_myImage.Draw(g_BackBuffer, redH_vecPos.x, redH_vecPos.y,
			g_aniInfo[3].aniRect[no_aniIndex].X,
			g_aniInfo[3].aniRect[no_aniIndex].Y,
			g_aniInfo[3].aniRect[no_aniIndex].Width,
			g_aniInfo[3].aniRect[no_aniIndex].Height);

		//신발
		g_myImage.Draw(g_BackBuffer, yellowS_vecPos.x, yellowS_vecPos.y,
			g_aniInfo[8].aniRect[no_aniIndex].X,
			g_aniInfo[8].aniRect[no_aniIndex].Y,
			g_aniInfo[8].aniRect[no_aniIndex].Width,
			g_aniInfo[8].aniRect[no_aniIndex].Height);

		g_myImage.Draw(g_BackBuffer, blackS_vecPos.x, blackS_vecPos.y,
			g_aniInfo[9].aniRect[no_aniIndex].X,
			g_aniInfo[9].aniRect[no_aniIndex].Y,
			g_aniInfo[9].aniRect[no_aniIndex].Width,
			g_aniInfo[9].aniRect[no_aniIndex].Height);

		//하의
		g_myImage.Draw(g_BackBuffer, pinkC_vecPos.x, pinkC_vecPos.y,
			g_aniInfo[6].aniRect[no_aniIndex].X,
			g_aniInfo[6].aniRect[no_aniIndex].Y,
			g_aniInfo[6].aniRect[no_aniIndex].Width,
			g_aniInfo[6].aniRect[no_aniIndex].Height);

		g_myImage.Draw(g_BackBuffer, blueC_vecPos.x, blueC_vecPos.y,
			g_aniInfo[7].aniRect[no_aniIndex].X,
			g_aniInfo[7].aniRect[no_aniIndex].Y,
			g_aniInfo[7].aniRect[no_aniIndex].Width,
			g_aniInfo[7].aniRect[no_aniIndex].Height);

		//상의
		g_myImage.Draw(g_BackBuffer, ppC_vecPos.x, ppC_vecPos.y,
			g_aniInfo[4].aniRect[no_aniIndex].X,
			g_aniInfo[4].aniRect[no_aniIndex].Y,
			g_aniInfo[4].aniRect[no_aniIndex].Width,
			g_aniInfo[4].aniRect[no_aniIndex].Height);

		g_myImage.Draw(g_BackBuffer, whiteC_vecPos.x, whiteC_vecPos.y,
			g_aniInfo[5].aniRect[no_aniIndex].X,
			g_aniInfo[5].aniRect[no_aniIndex].Y,
			g_aniInfo[5].aniRect[no_aniIndex].Width,
			g_aniInfo[5].aniRect[no_aniIndex].Height);

		//inform how to game
		g_myImage.Draw(g_BackBuffer, 450, 700,
			g_aniInfo[15].aniRect[0].X,
			g_aniInfo[15].aniRect[0].Y,
			g_aniInfo[15].aniRect[0].Width,
			g_aniInfo[15].aniRect[0].Height);

		if (Stop_vecPos.y >= 450)
		{
			cantouchtop = true;
		}
		if (Sbot_vecPos.y >= 450)
		{
			cantouchbot = true;
		}

		//10번 교체하면 주는 특별 조건 의상
		if (choice >= 10)
		{
			if (Stop_vecPos.y <= 450 && Stop_vecPos.x == 780)
			{
				Stop_vecPos.y = Stop_vecPos.y + 6;
			}

			g_myImage.Draw(g_BackBuffer, Stop_vecPos.x, Stop_vecPos.y,
				g_aniInfo[10].aniRect[no_aniIndex].X,
				g_aniInfo[10].aniRect[no_aniIndex].Y,
				g_aniInfo[10].aniRect[no_aniIndex].Width,
				g_aniInfo[10].aniRect[no_aniIndex].Height);
			g_myImage3.Draw(g_BackBuffer, 700, 500);
		}

		if (choice >= 30)
		{
			if (Sbot_vecPos.y <= 450 && Sbot_vecPos.x == 880)
			{
				Sbot_vecPos.y = Sbot_vecPos.y + 6;
			}

			g_myImage.Draw(g_BackBuffer, Sbot_vecPos.x, Sbot_vecPos.y,
				g_aniInfo[11].aniRect[no_aniIndex].X,
				g_aniInfo[11].aniRect[no_aniIndex].Y,
				g_aniInfo[11].aniRect[no_aniIndex].Width,
				g_aniInfo[11].aniRect[no_aniIndex].Height);
		}
	}
	//title
	if (start == false)
	{
		g_myImage2.Draw(g_BackBuffer, 0, 0);
		g_myImage.Draw(g_BackBuffer, person_vecPos.x, person_vecPos.y,
			g_aniInfo[g_aniDirection].aniRect[no_aniIndex].X,
			g_aniInfo[g_aniDirection].aniRect[no_aniIndex].Y,
			g_aniInfo[g_aniDirection].aniRect[no_aniIndex].Width,
			g_aniInfo[g_aniDirection].aniRect[no_aniIndex].Height);
	}

	//choice
	if (GoodChoice == true)
	{
		g_myImage4.Draw(g_BackBuffer, 0, 0);
		
		g_myImage.Draw(g_BackBuffer, 350, 200, //+250 +100
			g_aniInfo[0].aniRect[1].X,
			g_aniInfo[0].aniRect[1].Y,
			g_aniInfo[0].aniRect[1].Width,
			g_aniInfo[0].aniRect[1].Height);//몸

		if (brownH_vecPos.x < 350)
		{
			g_myImage.Draw(g_BackBuffer, brownH_vecPos.x + 250, brownH_vecPos.y + 100,
				g_aniInfo[1].aniRect[no_aniIndex].X,
				g_aniInfo[1].aniRect[no_aniIndex].Y,
				g_aniInfo[1].aniRect[no_aniIndex].Width,
				g_aniInfo[1].aniRect[no_aniIndex].Height);
		}
		if (yellowH_vecPos.x < 350)
		{
			g_myImage.Draw(g_BackBuffer, yellowH_vecPos.x + 250, yellowH_vecPos.y + 100,
				g_aniInfo[2].aniRect[no_aniIndex].X,
				g_aniInfo[2].aniRect[no_aniIndex].Y,
				g_aniInfo[2].aniRect[no_aniIndex].Width,
				g_aniInfo[2].aniRect[no_aniIndex].Height);
		}
		if (redH_vecPos.x < 350)
		{
			g_myImage.Draw(g_BackBuffer, redH_vecPos.x + 250, redH_vecPos.y + 100,
				g_aniInfo[3].aniRect[no_aniIndex].X,
				g_aniInfo[3].aniRect[no_aniIndex].Y,
				g_aniInfo[3].aniRect[no_aniIndex].Width,
				g_aniInfo[3].aniRect[no_aniIndex].Height);
		}
		if (yellowS_vecPos.x < 350)
		{
			g_myImage.Draw(g_BackBuffer, yellowS_vecPos.x + 250, yellowS_vecPos.y + 100,
				g_aniInfo[8].aniRect[no_aniIndex].X,
				g_aniInfo[8].aniRect[no_aniIndex].Y,
				g_aniInfo[8].aniRect[no_aniIndex].Width,
				g_aniInfo[8].aniRect[no_aniIndex].Height);
		}
		if (blackS_vecPos.x < 350)
		{
			g_myImage.Draw(g_BackBuffer, blackS_vecPos.x + 250, blackS_vecPos.y + 100,
				g_aniInfo[9].aniRect[no_aniIndex].X,
				g_aniInfo[9].aniRect[no_aniIndex].Y,
				g_aniInfo[9].aniRect[no_aniIndex].Width,
				g_aniInfo[9].aniRect[no_aniIndex].Height);
		}
		if (pinkC_vecPos.x < 350)
		{
			g_myImage.Draw(g_BackBuffer, pinkC_vecPos.x + 250, pinkC_vecPos.y + 100,
				g_aniInfo[6].aniRect[no_aniIndex].X,
				g_aniInfo[6].aniRect[no_aniIndex].Y,
				g_aniInfo[6].aniRect[no_aniIndex].Width,
				g_aniInfo[6].aniRect[no_aniIndex].Height);
		}
		if (blueC_vecPos.x < 350)
		{
			g_myImage.Draw(g_BackBuffer, blueC_vecPos.x + 250, blueC_vecPos.y + 100,
				g_aniInfo[7].aniRect[no_aniIndex].X,
				g_aniInfo[7].aniRect[no_aniIndex].Y,
				g_aniInfo[7].aniRect[no_aniIndex].Width,
				g_aniInfo[7].aniRect[no_aniIndex].Height);
		}
		if (ppC_vecPos.x < 350)
		{
			g_myImage.Draw(g_BackBuffer, ppC_vecPos.x + 250, ppC_vecPos.y + 100,
				g_aniInfo[4].aniRect[no_aniIndex].X,
				g_aniInfo[4].aniRect[no_aniIndex].Y,
				g_aniInfo[4].aniRect[no_aniIndex].Width,
				g_aniInfo[4].aniRect[no_aniIndex].Height);
		}
		if (whiteC_vecPos.x < 350)
		{
			g_myImage.Draw(g_BackBuffer, whiteC_vecPos.x + 250, whiteC_vecPos.y + 100,
				g_aniInfo[5].aniRect[no_aniIndex].X,
				g_aniInfo[5].aniRect[no_aniIndex].Y,
				g_aniInfo[5].aniRect[no_aniIndex].Width,
				g_aniInfo[5].aniRect[no_aniIndex].Height);
		}
		if (Stop_vecPos.x < 350)
		{
			g_myImage.Draw(g_BackBuffer, Stop_vecPos.x + 250, Stop_vecPos.y + 100,
				g_aniInfo[10].aniRect[no_aniIndex].X,
				g_aniInfo[10].aniRect[no_aniIndex].Y,
				g_aniInfo[10].aniRect[no_aniIndex].Width,
				g_aniInfo[10].aniRect[no_aniIndex].Height);
		}
		if (Sbot_vecPos.x < 350)
		{
			g_myImage.Draw(g_BackBuffer, Sbot_vecPos.x + 250, Sbot_vecPos.y + 100,
				g_aniInfo[11].aniRect[no_aniIndex].X,
				g_aniInfo[11].aniRect[no_aniIndex].Y,
				g_aniInfo[11].aniRect[no_aniIndex].Width,
				g_aniInfo[11].aniRect[no_aniIndex].Height);
		}
		//새로 배경을 깐다.
		// x가 123 이하라면 x를 + 234하고 해당 배열 이미지를 다시 드로우한다.
	}



	if (g_aniInfo[g_aniDirection].interval >= g_aniInfo[g_aniDirection].aniTime[g_aniIndex])
	{
		g_aniInfo[g_aniDirection].interval = 0;
		g_aniIndex++;
		if (g_aniIndex >= 2)
			g_aniIndex = 0;
	}

}

BOOL CheckCollision(Rect rc1, Rect rc2)
{
	BOOL isCollision = rc1.Intersect(rc2);

	return isCollision;
}