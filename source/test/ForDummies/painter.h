#pragma once

#include "turtle.h"

class Painter
{
public:
	Painter(Turtle* turtle);

	bool DrawCircle(int x, int y, int r);
private:
	Turtle* turtle_;
};
