#pragma once

#include "Constants.h"

#include "../flatform/EPaint.h"

extern class Roles;

Relation::type getRelation(Roles * tango, Roles * item);
INT isRelationD(Relation::type relation);
CHAR * stringRelation(Relation::type relation, CHAR * string);

