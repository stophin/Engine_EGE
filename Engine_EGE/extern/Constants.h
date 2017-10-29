// Constants.h
//

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

typedef int CONSTANTS_TYPE;

namespace Animation_Mode {
	enum  type{
		Still = 0,
		Step = 1,
		Auto = 2,
		Loop = 3
	};
}

namespace Animation_Base {
	enum type {
		Still = 0,
		Move = 100,
		Run = 200,
		Attack = 300,
		Dead = 400
	};
}

namespace Animation_Direction {
	enum type{
		Undefined = 0,
		Left = 1,
		Left_Up = 2,
		Up = 3,
		Right_Up = 4,
		Right = 5,
		Right_Down = 6,
		Down = 7,
		Left_Down = 8
	};
}

namespace Role_Type {
	enum type {
		Disabled = 0,
		Normal = 1,
		Back = 2,
		Cursor = 3,
		Pitch = 4,
		Player = 5
	};
}

namespace Role_Link  {
	enum type{
		Role = 0,
		Sort = 1,
		Object = 2,
		Back = 3,
		Front = 4,
		Player = 5,
		Collision = 6,
		SortBacks = 7,
		QuadTree = 8,
		AStars = 9,
		Temp1,
		Temp2,
		Max
	};
}

namespace Path_Link {
	enum type{
		Path = 0,
		Open = 1,
		Close = 2,
		Max = 3
	};
}

namespace Relation {
	enum type{
		A = 0,
		B = 1,
		C = 2,
		D = 3,
		D_L = 4,
		D_T = 5,
		D_R = 6,
		D_B = 7
	};
}

namespace RB_Color {
	enum type{
		Red = 0,
		Black = 1
	};
}

#define DEF_ASTAR_LIMIT		100

#define DEF_FETCH_SPEED		10
#define DEF_OCCUPY			100

#define DEF_QUAD_W			10000
#define DEF_QUAD_H			10000

#define DEF_RANGE_W			2
#define DEF_RANGE_H			2

#define DEF_HEALTH			300

#define Astar_Infinity		99999999

// already defined in ege
//#define PI					3.141592654

#define DEF_ZERO			1e-10

#define DEF_ISZERO(x)		(x > -DEF_ZERO && x < DEF_ZERO)


#endif //__CONSTANTS_H__