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
	switch (which_flag)
	{
	case left_flag:
		flags &= ~(which_flag << 2);
		break;

	case right_flag:
		flags &= ~(which_flag << 3);
		break;

	case up_flag: 
		flags &= ~which_flag;
		break;

	case down_flag:
		flags &= ~(which_flag << 1);
		break;
	}
}

///////////////////////////

//EVILS ******************

class PAC_API EVILS :public gamedll::CREATURES
{
	public:

		EVILS(creatures what_type, float _x, float _y) :CREATURES(_x, _y) 
		{
			type = what_type;
			NewDims(41.0f, 50.0f);

			switch (type)
			{
			case creatures::blue:
				speed = 0.9f;
				break;

			case creatures::red:
				speed = 0.5f;
				break;

			case creatures::pink:
				speed = 0.6f;
				break;

			case creatures::orange:
				speed = 0.7;
				break;

			case creatures::hurt:
				speed = 0.8f;
				break;


			}

		};

		void Release() override
		{
			delete this;
		}

		void Hurt() override
		{
			if (type != creatures::hurt)
			{
				previous_type = type;
				type = creatures::hurt;
				
				max_frames = 10;
				frame_delay = 3;

				hurt_delay = 1000;
				return;
			}
			else
			{
				type = previous_type;
				previous_type = creatures::no_type;
				
				max_frames = 2;
				frame_delay = 5;
				return;
			}
		}
		int GetFrame() override
		{
			frame_delay--;

			if (frame_delay < 0)
			{
				if (type != creatures::hurt)frame_delay = 3;
				else frame_delay = 5;
				++current_frame;
				if (current_frame > max_frames)current_frame = 0;
			}
			return current_frame;
		}
		creatures GetType() const
		{
			return type;
		}
		
		void Move(float gear, dirs to_where, dirs alternative_dir, gamedll::ATOMPACK& Obstacles) override
		{
			float now_speed = speed + gear / 10;

			for (int i = 0; i < Obstacles.size(); i++)
			{
				switch (dir)
				{
				case dirs::right:
					{
						float dummy_x = x + gear;
						float dummy_ex = ex + gear;

						if (!(dummy_x >= Obstacles[i].ex || dummy_ex <= Obstacles[i].x
							|| y >= Obstacles[i].ey || ey <= Obstacles[i].y))
						{
							SetFlag(right_flag);
							break;
						}
					}
					break;

				case dirs::left:
				{
					float dummy_x = x - gear;
					float dummy_ex = ex - gear;

					if (!(dummy_x >= Obstacles[i].ex || dummy_ex <= Obstacles[i].x
						|| y >= Obstacles[i].ey || ey <= Obstacles[i].y))
					{
						SetFlag(left_flag);
						break;
					}
				}
					break;

				case dirs::down:
				{
					float dummy_y = y + gear;
					float dummy_ey = ey + gear;

					if (!(x >= Obstacles[i].ex || ex <= Obstacles[i].x
						|| dummy_y >= Obstacles[i].ey || dummy_ey <= Obstacles[i].y))
					{
						SetFlag(down_flag);
						break;
					}
				}
					break;

				case dirs::up:
				{
					float dummy_y = y - gear;
					float dummy_ey = ey - gear;

					if (!(x >= Obstacles[i].ex || ex <= Obstacles[i].x
						|| dummy_y >= Obstacles[i].ey || dummy_ey <= Obstacles[i].y))
					{
						SetFlag(up_flag);
						break;
					}
				}
					break;
				}
			}
			switch (dir)
			{
			case dirs::right:
				if (GetFlag(right_flag))
				{
					dir = alternative_dir;
					break;
				}
				else ResetFlag(right_flag);
				if (ex + gear >= scr_width)SetFlag(right_flag);
				else
				{
					x += gear;
					SetEdges();
				}
				break;

			case dirs::left:
				if (GetFlag(left_flag))
				{
					dir = alternative_dir;
					break;
				}
				else ResetFlag(left_flag);
				if (x - gear <= 0)SetFlag(left_flag);
				else
				{
					x -= gear;
					SetEdges();
				}
				break;

			case dirs::down:
				if (GetFlag(down_flag))
				{
					dir = alternative_dir;
					break;
				}
				else ResetFlag(down_flag);
				if (ey + gear >= ground)SetFlag(down_flag);
				else
				{
					y += gear;
					SetEdges();
				}
				break;

			case dirs::up:
				if (GetFlag(up_flag))
				{
					dir = alternative_dir;
					break;
				}
				else ResetFlag(up_flag);
				if (y - gear <= 0)SetFlag(up_flag);
				else
				{
					y -= gear;
					SetEdges();
				}
				break;
			}
		}
};







//FUNCTIONS **********************


gamedll::ATOMPACK& gamedll::InitObstaclesPack(int maximum_size)
{
	ATOMPACK ret{ maximum_size };
	return ret;
}