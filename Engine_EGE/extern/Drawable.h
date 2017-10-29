#pragma once

#include "World.h"
#include "ResourceManager.h"

#define DEBUG_GRADE_1	0x01
#define DEBUG_GRADE_2	0x02
#define DEBUG_GRADE_3	0x03
#define DEBUG_GRADE_4	0x04

VOID Initialize();
Roles * onPaint(HWND hWnd);
VOID onTimer();
VOID onKeyDown(WPARAM wParam);
VOID onResize(FLOAT width, FLOAT height);
PointF projectShow(FLOAT x, FLOAT y);
PointF projectFlat(FLOAT x, FLOAT y);
VOID onAStar(FLOAT x, FLOAT y);
VOID onDrag(FLOAT x, FLOAT y, INT mode);
Roles * createRole(FLOAT scale = 1);