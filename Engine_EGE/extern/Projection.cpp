
#include "Projection.h"


#include <math.h>
FLOAT DEF_OFFSETX = 100;
FLOAT e = 0.7;
FLOAT degree = 30 * PI / 180;
FLOAT degree1 = 45 * PI / 180;
FLOAT cos45 = -cos(degree1);
FLOAT sin45 = sin(degree1);
FLOAT tan45 = sin45 / cos45;
FLOAT parameter = sqrt(1 - e * e) / sqrt(1 - e * e * cos45 * cos45);
FLOAT offset = DEF_OFFSETX;

PointF projectShow(FLOAT x, FLOAT y)
{
	if (x == 0)
	{
		x = DEF_ZERO;
	}
	FLOAT h = atan(y / x);

	if (y > 0)
	{
		if (x < 0)
		{
			h += PI;
		}
	}
	else
	{
		if (x >= 0)
		{
			h += 2 * PI;
		}
		else if (x < 0)
		{
			h += PI;
		}
	}

	FLOAT r = sqrt(x * x + y * y);

	FLOAT sx = r * cos(h + degree);
	FLOAT sy = r * sin(h + degree);

	FLOAT dx = offset + sx + sy * parameter * cos45;
	FLOAT dy = sy * parameter * sin45;
	return PointF(dx, dy);
}

PointF projectFlat(FLOAT x, FLOAT y)
{
	FLOAT sy = y / (sin45 * parameter);
	FLOAT sx = x - offset - sy * parameter * cos45;

	if (sx == 0)
	{
		sx = DEF_ZERO;
	}
	FLOAT gh = atan(sy / sx);

	if (sy > 0)
	{
		if (sx < 0)
		{
			gh += PI;
		}
	}
	else
	{
		if (sx > 0)
		{
			gh += 2 * PI;
		}
		else if (sx < 0)
		{
			gh += PI;
		}
	}

	FLOAT r = sqrt(sx * sx + sy * sy);

	FLOAT dx = r * cos(gh - degree);
	FLOAT dy = r * sin(gh - degree);

	return PointF(dx, dy);
}
