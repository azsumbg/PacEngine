#include "pch.h"
#include "PacEngine.h"

//RAND ENGINE ***********

gamedll::RANDENGINE::RANDENGINE()
{
	for (int i = 0; i < 650; i++) vSeeds.push_back(rd());
	seed_seq_ptr = new std::seed_seq(vSeeds.begin(), vSeeds.end());
	twister = new std::mt19937(*seed_seq_ptr);
}
gamedll::RANDENGINE::~RANDENGINE()
{
	delete seed_seq_ptr;
	delete twister;
}
int gamedll::RANDENGINE::operator()(int min, int max)
{
	std::uniform_int_distribution rand_engine{ min, max };
	return rand_engine(*twister);
}

//////////////

//ATOM *************

gamedll::ATOM::ATOM(float _x, float _y, float _width = 1.0f, float _height = 1.0f)
{
	x = _x;
	y = _y;
	width = _width;
	height = _height;
}
float gamedll::ATOM::GetWidth() const
{
	return width;
}
float gamedll::ATOM::GetHeight() const
{
	return height;
}
void gamedll::ATOM::SetWidth(float new_width)
{
	width = new_width;
	ex = x + width;
}
void gamedll::ATOM::SetHeight(float new_height)
{
	height = new_height;
	ey = y + height;
}
void gamedll::ATOM::SetEdges()
{
	ex = x + width;
	ey = y + height;
}
void gamedll::ATOM::NewDims(float new_width, float new_height)
{
	width = new_width;
	height = new_height;
	ex = x + width;
	ey = y + height;
}

//////////////

//ATOMPACK *************

void gamedll::ATOMPACK::push_back(ATOM& object)
{
	if (next_pushback_pos <= end_pos)
	{
		*(mPtr + next_pushback_pos) = object;
		next_pushback_pos++;
	}
}
void gamedll::ATOMPACK::push_front(ATOM& object)
{
	*mPtr = object;
}
int gamedll::ATOMPACK::size() const
{
	return max_size;
}
int gamedll::ATOMPACK::end() const
{
	return end_pos;
}
gamedll::ATOM& gamedll::ATOMPACK::operator[](int position) const
{
	if (position <= end_pos)return *(mPtr + position);
	return *(mPtr + end_pos);
}

//CREATURES ***************

bool gamedll::CREATURES::GetFlag(char which_flag) const
{
	return(flags && which_flag);
}
void gamedll::CREATURES::SetFlag(char which_flag)
{
	flags = flags | which_flag;
}
void gamedll::CREATURES::ResetFlag(char which_flag)
{	
	flags = flags ^ which_flag;
}
void gamedll::CREATURES::InitObstaclesPack(int maximum_size)
{
	ObstaclesPack = new ATOMPACK{ maximum_size };
}

///////////////////////////