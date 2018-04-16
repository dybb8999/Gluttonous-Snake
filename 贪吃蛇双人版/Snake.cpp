#include "Snake.h"

Snake::Snake()
{
	x = 0;
	y = 0;
	Oldx = 0;
	Oldy = 0;
}
Snake::Snake(int _x,int _y)
{
	x = _x;
	y = _y;
}

Snake::Snake(Snake &snake)
{
	body = TEXT("*");
	x = snake.ShowX();
	y = snake.ShowY();
	Oldx = snake.ShowOldx();
	Oldy = snake.ShowOldy();
	Next = snake.Next;
}

int Snake::ShowX()
{
	return x;
}

int Snake::ShowY()
{
	return y;
}

int Snake::ShowOldx()
{
	return Oldx;
}

int Snake::ShowOldy()
{
	return Oldy;
}

void Snake::SetWay(int _way)
{
	way = _way;
}

int Snake::ShowWay()
{
	return way;
}
Snake Snake::SetX(int _x)
{
	x = _x;
	return *this;
}

Snake Snake::SetBody(TCHAR *_body)
{
	body = _body;
	return *this;
}

TCHAR *Snake::ShowBody()
{
	return body;
}

Snake Snake::SetY(int _y)
{
	y = _y;
	return *this;
}

Snake  Snake::SetOldX(int Old_x)
{
	Oldx = Old_x;
	return *this;
}

Snake Snake::SetOldY(int Old_y)
{
	Oldy = Old_y;
	return *this;
}

void Snake::SetNext(BOOLEAN _bool)
{
	Next = _bool;
}
Snake::~Snake()
{
}
