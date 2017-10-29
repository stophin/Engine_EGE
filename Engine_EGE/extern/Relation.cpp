
#include "Relation.h"

#include "Roles.h"

Relation::type getRelation(Roles * tango, Roles * item)
{
	if (tango == NULL || item == NULL)
	{
		return Relation::A;
	}
	FLOAT item_posX = item->flatting.X;
	FLOAT item_posY = item->flatting.Y;
	FLOAT item_geomW = item->flatting.Width;
	FLOAT item_geomH = item->flatting.Height;
	FLOAT tango_posX = tango->flatting.X;
	FLOAT tango_posY = tango->flatting.Y;
	FLOAT tango_geomW = tango->flatting.Width;
	FLOAT tango_geomH = tango->flatting.Height;

	item_posY += item_geomH;
	tango_posY += tango_geomH;

	if (item_posX >= tango_posX && item_posX <= tango_posX + tango_geomW)
	{
		if (item_posY <= tango_posY && item_posY - item_geomH >= tango_posY - tango_geomH)
		{
			return Relation::D;
		}
		if (item_posY - item_geomH <= tango_posY && item_posY >= tango_posY - tango_geomH)
		{
			return Relation::D;
		}
	}
	if (item_posX + item_geomW >= tango_posX && item_posX + item_geomW <= tango_posX + tango_geomW)
	{
		if (item_posY <= tango_posY && item_posY - item_geomH >= tango_posY - tango_geomH)
		{
			return Relation::D;
		}
		if (item_posY - item_geomH <= tango_posY && item_posY >= tango_posY - tango_geomH)
		{
			return Relation::D;
		}
	}
	if (item_posX + item_geomW < tango_posX + tango_geomW && item_posY - item_geomH < tango_posY)
	{
		return Relation::A;
	}
	if (item_posX < tango_posX + tango_geomW && item_posY < tango_posY - tango_geomH)
	{
		return Relation::A;
	}
	if (item_posX >= tango_posX + tango_geomW && item_posY - item_geomH >= tango_posY)
	{
		return Relation::B;
	}
	if (item_posX + item_geomW >= tango_posX && item_posY - item_geomH >= tango_posY - tango_geomH)
	{
		return Relation::B;
	}
	if (item_posX + item_geomW < tango_posX && item_posY >= tango_posY - tango_geomH)
	{
		return Relation::C;
	}
	if (item_posX >= tango_posX + tango_geomW && item_posY - item_geomH < tango_posY)
	{
		return Relation::C;
	}

	return Relation::A;
}

INT isRelationD(Relation::type relation)
{
	return (relation > Relation::C);
}

CHAR * stringRelation(Relation::type relation, CHAR * string)
{
	switch (relation)
	{
	case Relation::A:
		sprintf_s(string, 10, "A");
		break;
	case Relation::B:
		sprintf_s(string, 10, "B");
		break;
	case Relation::C:
		sprintf_s(string, 10, "C");
		break;
	case Relation::D:
		sprintf_s(string, 10, "D");
		break;
	case Relation::D_L:
		sprintf_s(string, 10, "D_L");
		break;
	case Relation::D_R:
		sprintf_s(string, 10, "D_R");
		break;
	case Relation::D_T:
		sprintf_s(string, 10, "D_T");
		break;
	case Relation::D_B:
		sprintf_s(string, 10, "D_B");
		break;
	}
	return string;
}