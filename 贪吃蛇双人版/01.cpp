#include <windows.h>
#include <iostream>
#include <cstdlib>
#include "Snake.h"
#include <ctime>
#include "resource.h"

//声音
#include <mmsystem.h> 
#pragma comment (lib, "winmm.lib")
//
//控制声音
static int killer_1 = 0, killer_2 = 0;
static BOOL FirstBlood = FALSE;
time_t begin_1 = 0, begin_2 = 0;
//
Snake snake_1[300];
Snake snake_2[300];

static int N_1 = 4;
static int N_2 = 4;

static int Foodx;
static int Foody;
static TCHAR Food[] = TEXT("■");
static int cxChar, cyChar, cxClients, cyClients, cxHang, cyLie;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK  AboutWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void voice(int &killer, int &killed, time_t &begin, BOOL &firstBlood);
void Add_Body(int who);
void Go();
int EatFood();
void CreateFood();


int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd)
{
	HWND hwnd;
	WNDCLASS wndclass;
	MSG msg;
	TCHAR szName[] = TEXT("贪吃蛇双人版");

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbWndExtra = 0;
	wndclass.cbClsExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = TEXT("Snake");
	wndclass.lpszClassName = szName;


	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("对不起，由于您的电脑配置过低，无法运行此程序，请升级后再来"), TEXT("错误"), MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szName, TEXT("贪吃蛇"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
// 	MessageBox(hwnd, TEXT("玩法：1P W上S下A左D右，2P为方向键。当A玩家的头碰到B位玩家的身子时，B玩家的身子会减1。当身子长度小于4时，为失败。祝您游戏愉快"), TEXT("温习提示"), MB_OK);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);
//	SetTimer(hwnd, 1, 50, NULL);//计时器

	while (GetMessage(&msg, hwnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	TEXTMETRIC tm;
	PAINTSTRUCT ps;
	RECT rect;
	static HMENU hMenu;
	POINT point;
	static HINSTANCE hinstance;

	srand((unsigned)time(NULL));

	static TCHAR szBuffer[15];
	static int WhoEat;

	switch (message)
	{
	case WM_CREATE:
		hinstance = ((LPCREATESTRUCT)lParam)->hInstance;
		hdc = GetDC(hwnd);
		GetClientRect(hwnd, &rect);
		GetTextMetrics(hdc, &tm);
		cyChar = tm.tmHeight;
		cxChar = tm.tmAveCharWidth;
		rect.right = rect.right;
		cyClients = rect.bottom;
		cxClients = rect.right;
		cxHang = rect.bottom / cyChar;
		cyLie = rect.right / cxChar;
		ReleaseDC(hwnd, hdc);
		snake_1[0].SetX(8);
		snake_1[1].SetX(7);
		snake_1[2].SetX(6);
		snake_1[3].SetX(5);
		snake_1[0].SetY(6);
		snake_1[1].SetY(6);
		snake_1[2].SetY(6);
		snake_1[3].SetY(6);
		snake_1[0].SetBody(TEXT("+"));
		snake_1[1].SetBody(TEXT("0"));
		snake_1[2].SetBody(TEXT("0"));
		snake_1[3].SetBody(TEXT("0"));
		snake_2[0].SetX(8);
		snake_2[1].SetX(7);
		snake_2[2].SetX(6);
		snake_2[3].SetX(5);
		snake_2[0].SetY(15);
		snake_2[1].SetY(15);
		snake_2[2].SetY(15);
		snake_2[3].SetY(15);
		snake_2[0].SetBody(TEXT("+"));
		snake_2[1].SetBody(TEXT("0"));
		snake_2[2].SetBody(TEXT("0"));
		snake_2[3].SetBody(TEXT("0"));

		hMenu = LoadMenu(hinstance, TEXT("Snake"));
		hMenu = GetSubMenu(hMenu, 0);
		PlaySound(TEXT("Sound//sound.wav"), NULL, SND_ASYNC | SND_FILENAME);
		DialogBox(hinstance, TEXT("Teach"), hwnd, AboutWndProc);
		PlaySound(NULL, NULL, SND_PURGE);
		return 0;

	case WM_RBUTTONUP:
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		ClientToScreen(hwnd, &point);
		TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
		return 0;
	/*case WM_KILLFOCUS:
		for (int index = 0; index < 2;index++)
		{
			if (PushButton[index] == (HWND)wParam)
			{
				SetFocus(hwnd);
			}
		}
		return 0;*/
	case WM_SIZE:
		GetClientRect(hwnd, &rect);
		cyClients = HIWORD(lParam);
		cxClients = LOWORD(lParam);
		cxHang = rect.bottom / cyChar;
		cyLie = rect.right / cxChar;
		CreateFood();
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SetBkMode(hdc, TRANSPARENT);
		GetClientRect(hwnd, &rect);
		MoveToEx(hdc, cxChar*14+cxChar, cyChar, NULL);
		LineTo(hdc, (rect.right - cxChar)-cxChar*14, cyChar);
		LineTo(hdc, (rect.right - cxChar) - cxChar * 14, cyChar*2);
		LineTo(hdc, rect.right - cxChar, cyChar*2);
		LineTo(hdc, rect.right - cxChar, rect.bottom - cyChar);
		LineTo(hdc, cxChar, rect.bottom - cyChar);
		LineTo(hdc, cxChar, cyChar*2);
		LineTo(hdc, cxChar * 14 + cxChar, cyChar * 2);
		LineTo(hdc, cxChar * 14+cxChar, cyChar);
		for (int index = 0; index < N_1;index++)
		{
			
			TextOut(hdc, snake_1[index].ShowX()*cxChar, snake_1[index].ShowY()*cyChar, snake_1[index].ShowBody(), lstrlen(snake_1[index].ShowBody()));
		}
		for (int index = 0; index < N_2;index++)
		{
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, snake_2[index].ShowX()*cxChar, snake_2[index].ShowY()*cyChar, snake_2[index].ShowBody(), lstrlen(snake_2[index].ShowBody()));
			SetTextColor(hdc, RGB(0, 0, 0));
		}
		TextOut(hdc, Foodx*cxChar, Foody*cyChar, Food, lstrlen(Food));
		TextOut(hdc, cxChar * 2, cyChar, szBuffer, wsprintf(szBuffer, TEXT("1P蛇现在长%-3d"), N_1));
		SetTextColor(hdc, RGB(0, 255, 0));
		TextOut(hdc, cxChar * (cyLie-14), cyChar, szBuffer, wsprintf(szBuffer, TEXT("2P蛇现在长%-3d"), N_2));
		SetTextColor(hdc, RGB(0, 0, 0));
		

		return 0;

	case WM_TIMER:
		Go();
		switch (snake_1[0].ShowWay())
		{
		case Left:
			snake_1[0].SetOldX(snake_1[0].ShowX());
			snake_1[0].SetX(snake_1[0].ShowX() - 1);
			if (snake_1[0].ShowX() < 1)
			{
				snake_1[0].SetX(cyLie - 1);
			}
			break;
		case Up:
			snake_1[0].SetOldY(snake_1[0].ShowY());
			snake_1[0].SetY(snake_1[0].ShowY() - 1);
			if (snake_1[0].ShowY() < 1)
			{
				snake_1[0].SetY(cxHang - 1);
			}
			break;
		case Rigth:
			snake_1[0].SetOldX(snake_1[0].ShowX());
			snake_1[0].SetX(snake_1[0].ShowX() + 1);
			if (snake_1[0].ShowX() > cyLie - 1)
			{
				snake_1[0].SetX(1);
			}
			break;
		case Down:
			snake_1[0].SetOldY(snake_1[0].ShowY());
			snake_1[0].SetY(snake_1[0].ShowY() + 1);
			if (snake_1[0].ShowY() > cxHang - 1)
			{
				snake_1[0].SetY(1);
			}
			break;
		default:
			break;
		}

		switch (snake_2[0].ShowWay())
		{
		case Left:
			snake_2[0].SetOldX(snake_2[0].ShowX());
			snake_2[0].SetX(snake_2[0].ShowX() - 1);
			if (snake_2[0].ShowX() < 1)
			{
				snake_2[0].SetX(cyLie - 1);
			}
			break;
		case Up:
			snake_2[0].SetOldY(snake_2[0].ShowY());
			snake_2[0].SetY(snake_2[0].ShowY() - 1);
			if (snake_2[0].ShowY() < 1)
			{
				snake_2[0].SetY(cxHang - 1);
			}
			break;
		case Rigth:
			snake_2[0].SetOldX(snake_2[0].ShowX());
			snake_2[0].SetX(snake_2[0].ShowX() + 1);
			if (snake_2[0].ShowX() > cyLie - 1)
			{
				snake_2[0].SetX(1);
			}
			break;
		case Down:
			snake_2[0].SetOldY(snake_2[0].ShowY());
			snake_2[0].SetY(snake_2[0].ShowY() + 1);
			if (snake_2[0].ShowY() > cxHang - 1)
			{
				snake_2[0].SetY(1);
			}
			break;
		default:
			break;
		}

		if ((WhoEat = EatFood())!=0)
		{
			Add_Body(WhoEat);
			CreateFood();
		}
		for (int index = 1; index < N_1; index++)
		{
			if ((snake_1[index].ShowX() == snake_2[0].ShowX()) && (snake_1[index].ShowY() == snake_2[0].ShowY()))
			{
				N_1--;
				voice(killer_2,killer_1, begin_2, FirstBlood);
			}
		}
		for (int index = 1; index < N_2; index++)
		{
			if ((snake_2[index].ShowX() == snake_1[0].ShowX()) && (snake_2[index].ShowY() == snake_1[0].ShowY()))
			{
				N_2--;
				voice(killer_1,killer_2, begin_1, FirstBlood);
			}
		}
		if (N_1<4)
		{
			KillTimer(hwnd, 1);
			PlaySound(TEXT("Sound//Aced.wav"), NULL, SND_FILENAME | SND_ASYNC);

			MessageBox(NULL, TEXT("P1输了"), TEXT("失败"), MB_ICONERROR);
			SendMessage(hwnd, WM_COMMAND, ID_40003, lParam);
			return 0;
		}
		else if (N_2<4)
		{
			KillTimer(hwnd, 1);
			PlaySound(TEXT("Sound//Aced.wav"), NULL, SND_FILENAME | SND_ASYNC);

			MessageBox(NULL, TEXT("P2输了"), TEXT("失败"), MB_ICONERROR);
			SendMessage(hwnd, WM_COMMAND, ID_40003, lParam);

			return 0;
		}
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_UP:
				if ((snake_2[0].ShowWay() == Left) || (snake_2[0].ShowWay() == Rigth))
				{
					snake_2[0].SetWay(Up);
				}
				return 0;
			case VK_LEFT:
				if ((snake_2[0].ShowWay() == Up) || (snake_2[0].ShowWay() == Down))
				{
					snake_2[0].SetWay(Left);
				}
				return 0;
			case VK_RIGHT:
				if ((snake_2[0].ShowWay() == Up) || (snake_2[0].ShowWay() == Down))
				{
					snake_2[0].SetWay(Rigth);
				}
				return 0;
			case  VK_DOWN:
				if ((snake_2[0].ShowWay() == Left) || (snake_2[0].ShowWay() == Rigth))
				{
					snake_2[0].SetWay(Down);
				}
				return 0;
			case 'W':
				if ((snake_1[0].ShowWay() == Left) || (snake_1[0].ShowWay() == Rigth))
				{
					snake_1[0].SetWay(Up);
				}
				return 0;
			case 'A':
				if ((snake_1[0].ShowWay() == Up) || (snake_1[0].ShowWay() == Down))
				{
					snake_1[0].SetWay(Left);
				}
				return 0;
			case 'D':
				if ((snake_1[0].ShowWay() == Up) || (snake_1[0].ShowWay() == Down))
				{
					snake_1[0].SetWay(Rigth);
				}
				return 0;
			case  'S':
				if ((snake_1[0].ShowWay() == Left) || (snake_1[0].ShowWay() == Rigth))
				{
					snake_1[0].SetWay(Down);
				}
				return 0;
			default:
				break;
			}
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		case WM_DESTROY:
			TerminateProcess(GetCurrentProcess(), 0);
			return 0;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case ID_40001:
				SetTimer(hwnd, 1, 50, NULL);
				return 0;
			case ID_40002:
				KillTimer(hwnd, 1);
				return 0;
			case ID_40003:
				snake_1[0].SetX(8);
				snake_1[1].SetX(7);
				snake_1[2].SetX(6);
				snake_1[3].SetX(5);
				snake_1[0].SetY(6);
				snake_1[1].SetY(6);
				snake_1[2].SetY(6);
				snake_1[3].SetY(6);
				snake_1[0].SetBody(TEXT("+"));
				snake_1[1].SetBody(TEXT("0"));
				snake_1[2].SetBody(TEXT("0"));
				snake_1[3].SetBody(TEXT("0"));
				snake_2[0].SetX(8);
				snake_2[1].SetX(7);
				snake_2[2].SetX(6);
				snake_2[3].SetX(5);
				snake_2[0].SetY(15);
				snake_2[1].SetY(15);
				snake_2[2].SetY(15);
				snake_2[3].SetY(15);
				snake_2[0].SetBody(TEXT("+"));
				snake_2[1].SetBody(TEXT("0"));
				snake_2[2].SetBody(TEXT("0"));
				snake_2[3].SetBody(TEXT("0"));
				N_1 = 4;
				N_2 = 4;
				FirstBlood = FALSE;
				InvalidateRect(hwnd, NULL, TRUE);
				return 0;

			case ID_ABOUT_40004:
				DialogBox(hinstance, TEXT("About"), hwnd, AboutWndProc);
				break;

			case ID_40005:
				DialogBox(hinstance, TEXT("Teach"), hwnd, AboutWndProc);
			}
			return 0;

	default:
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void Go()
{
	for (int index = 0; index <= N_1; index++)
	{
		snake_1[index].SetOldX(snake_1[index].ShowX());
		snake_1[index].SetOldY(snake_1[index].ShowY());
		if (index == 0)
		{
			continue;
		}
		snake_1[index].SetX(snake_1[index - 1].ShowOldx());
		snake_1[index].SetY(snake_1[index - 1].ShowOldy());
	}
	for (int index = 0; index <= N_2; index++)
	{
		snake_2[index].SetOldX(snake_2[index].ShowX());
		snake_2[index].SetOldY(snake_2[index].ShowY());
		if (index == 0)
		{
			continue;
		}
		snake_2[index].SetX(snake_2[index - 1].ShowOldx());
		snake_2[index].SetY(snake_2[index - 1].ShowOldy());
	}
}

void CreateFood()
{
	Foodx = rand() % (cyLie - 30) + 16;
	Foody = rand() % (cxHang - 4) + 1;
}

int EatFood()
{
	if ((snake_1[0].ShowX() == Foodx) && (snake_1[0].ShowY() == Foody))
	{
		return 11;
	}
	else if ((snake_2[0].ShowX() == Foodx) && (snake_2[0].ShowY() == Foody))
	{
		return 21;
	}
	else
	{
		return 0;
	}
}

void Add_Body(int who)
{
	if (who == 11)
	{
		snake_1[N_1].SetX(snake_1[N_1 - 1].ShowX()).SetOldX(snake_1[N_1].ShowX());
		snake_1[N_1].SetY(snake_1[N_1 - 1].ShowY()).SetOldY(snake_1[N_1].ShowY());
		snake_1[N_1].SetBody(TEXT("0"));
		if (N_1 == 299)
		{
			MessageBox(NULL, TEXT("P1蛇的长度已经达到最长"), TEXT("增长失败"), MB_OK);

		}
		else
		{
			N_1++;
		}
	}
	else if (who == 21)
	{
		snake_2[N_2].SetX(snake_2[N_2 - 1].ShowX()).SetOldX(snake_2[N_2].ShowX());
		snake_2[N_2].SetY(snake_2[N_2 - 1].ShowY()).SetOldY(snake_2[N_2].ShowY());
		snake_2[N_2].SetBody(TEXT("0"));
		if (N_2 == 299)
		{
			MessageBox(NULL, TEXT("此蛇的长度已经达到最长"), TEXT("增长失败"), MB_OK);

		}
		else
		{
			N_2++;
		}
	}
}

BOOL CALLBACK  AboutWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hwnd, 0);
			return TRUE;
		default:
			break;
		}
	default:
		break;
	}
	return FALSE;
}

void voice(int &killer, int &killed, time_t &begin, BOOL &firstBlood)
{
	if (!firstBlood)
	{
		PlaySound(TEXT("Sound//firstBlood.wav"), NULL, SND_ASYNC | SND_FILENAME);
	}
	if (killer == 0)
	{
		killer++;
		begin = clock();
	}
	else
	{
		if (clock()-begin>10*CLOCKS_PER_SEC)
		{
			killer = 0;
		}
		else
		{
			killer++;
			switch (killer)
			{
			case 2:
				PlaySound(TEXT("Sound//double kill.wav"), NULL, SND_FILENAME | SND_ASYNC);
				break;
			case 3:
				PlaySound(TEXT("Sound//triple kill.wav"), NULL, SND_ASYNC | SND_FILENAME);
				break;
			case 4:
				PlaySound(TEXT("Sound//quatre kill.wav"), NULL, SND_FILENAME | SND_ASYNC);
				break;
			case 5:
				PlaySound(TEXT("Sound//penta kill.wav"), NULL, SND_ASYNC | SND_FILENAME);
				break;
			default:
				break;
			}
		}
		switch (killer)
		{
		case 3:
			PlaySound(TEXT("Sound//rampage.wav"), NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 4:
			PlaySound(TEXT("Sound//killing spree.wav"), NULL, SND_ASYNC | SND_FILENAME);
			break;
		case 5:
			PlaySound(TEXT("Sound//dominating.wav"), NULL, SND_ASYNC | SND_FILENAME);
			break;
		case 6:
			PlaySound(TEXT("Sound//rampage.wav"), NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 7:
			PlaySound(TEXT("Sound//godlike.wav"), NULL, SND_ASYNC | SND_FILENAME);
			break;
		case 8:
			PlaySound(TEXT("Sound//chaoshen.wav"), NULL, SND_ASYNC | SND_FILENAME);
			break;
		default:
			if (killer>8)
			{
				PlaySound(TEXT("Sound//chaoshen.wav"), NULL, SND_ASYNC | SND_FILENAME);
			}
			break;
		}
	}
	if (killed > 3)
	{
		PlaySound(TEXT("Sound//shutdown.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	killed = 0;
	firstBlood = TRUE;
}