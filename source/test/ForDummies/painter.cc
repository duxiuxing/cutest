#include "painter.h"

Painter::Painter(Turtle* turtle)
	: turtle_(turtle)
{}

bool Painter::DrawCircle(int x, int y, int radius)
{
	turtle_->PenDown();
	//turtle->PenUp();
	return true;
}
