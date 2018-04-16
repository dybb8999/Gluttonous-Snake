#pragma once
#include <windows.h>

	enum Way
	{
		Left, Up, Rigth, Down
	};

	class Snake
	{
	private:
		int x, y, Oldx, Oldy;
		TCHAR *body;
		BOOLEAN Next = FALSE;
		//1вС2ио3ср4об
		int way = Rigth;
	public:
	//	TCHAR body[] = TEXT("*");
		Snake();
		Snake(int _x, int _y);
		Snake(Snake &snake);
		int ShowX();
		int ShowY();
		int ShowOldx();
		int ShowOldy();
		void SetWay(int _way);
		int ShowWay();
		Snake SetBody(TCHAR *_body);
		TCHAR *ShowBody();
		Snake SetX(int _x);
		Snake SetY(int _y);
		Snake SetOldX(int Old_x);
		Snake SetOldY(int Old_y);
		void SetNext(BOOLEAN _bool);
		~Snake();
	};
