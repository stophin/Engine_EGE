
#include "Drawable.h"
#include "Loading.h"

int DEBUG_MODE = DEBUG_GRADE_2;

World preview;
World world;
ResourceManager resm;

#include "../NanoCGR.h"
NanoCGR nanoCGR;

INT isresize = -1;
PAINTSTRUCT ps;
HDC hdc;
HDC memHdc;
INT nWidth;
INT nHeight;
HBITMAP hbmp;
Graphics * graphics;
INT isDrag = 0;

VOID onResize(FLOAT width, FLOAT height)
{
	world.resize(width, height);
	isresize = 1;
}

Roles * onPaint(HWND hWnd)
{
	//EndPaint(hWnd, &ps);
	//hdc = BeginPaint(hWnd, &ps);
	if (isresize)
	{
		if (isresize != -1)
		{
			DeleteDC(memHdc);
			DeleteObject(hbmp);
			//ReleaseDC(hWnd, hdc);
			//EndPaint(hWnd, &ps);
		}
		isresize = 0;

		hdc = GetDC(hWnd);
		//hdc = BeginPaint(hWnd, &ps);
		memHdc = CreateCompatibleDC(hdc);
		nWidth = GetDeviceCaps(hdc, HORZRES);
		nHeight = GetDeviceCaps(hdc, VERTRES);
		hbmp = CreateCompatibleBitmap(hdc, nWidth, nHeight);
		if (!SelectObject(memHdc, hbmp))
		{
			isresize = 1;
			return NULL;
		}
		graphics = Graphics::FromHDC(memHdc);
	}
	Roles * role = world.focus;

	//if (!role || role->paths.linkcount == 0 )
	{
		//graphics->Clear(Color(0, 0, 0));
		//cleardevice();
	}
	
	//world.refreshRole();

	EP_FillRectangle(world.display.X, world.display.Y, world.display.X + world.geometry.Width, world.display.Y + world.geometry.Height);
	Roles* rselected = world.drawRole(*graphics);
	EP_FillRectangle(preview.display.X, preview.display.Y, preview.display.X + preview.geometry.Width, preview.display.Y + preview.geometry.Height);
	preview.drawRole(*graphics);

	if (DEBUG_MODE & DEBUG_GRADE_3)
	{
		CHAR fmt[100];

		EP_SetColor(WHITE);
		sprintf_s(fmt, "(%.2f, %.2f, %.2f, %.2f)", role->flatting.X, role->flatting.Y, role->flatting.Width, role->flatting.Height);
		EP_Text(10, 10, fmt);
		EP_SetColor(GREEN);
		sprintf_s(fmt, "(%.2f, %.2f, %.2f, %.2f)", role->position.X, role->position.Y, role->position.Width, role->position.Height);
		EP_Text(10, 30, fmt);
		EP_SetColor(BLUE);
		sprintf_s(fmt, "tall: %.2f", role->tall);
		EP_Text(10, 50, fmt);
		EP_SetColor(YELLOW);
		sprintf_s(fmt, "offset:(%.2f, %.2f) W*H:(%.2f x %.2f)", world.geometry.X, world.geometry.Y, world.geometry.Width, world.geometry.Height);
		EP_Text(10, 70, fmt);

		sprintf_s(fmt, "%d", world.count);
		EP_Text(10, 80, fmt);

		if (0)
		{
			EP_SetColor(RED);
			PointF points[] = { world.leftTop, world.leftTop + world.vRightTop, world.leftTop + world.vRightBottom, world.leftTop + world.vLeftBottom, world.leftTop };
			EP_Lines(points, 5);

			EP_SetColor(BROWN);
			EP_Rectangle(world.geometry);
		}
	}
	EP_SetColor(BROWN);
	EP_Rectangle(preview.display.X, preview.display.Y, preview.display.X + preview.geometry.Width, preview.display.Y + preview.geometry.Height);

	Roles * next;
	role = world.astar.link;
	if (role) {
		do {
			next = world.astar.next(role);

			role->fetchPath();

			role = next;
		} while (role && role != world.astar.link);
	}
	role = preview.astar.link;
	if (role) {
		do {
			next = preview.astar.next(role);

			role->fetchPath();

			role = next;
		} while (role && role != preview.astar.link);
	}

	return rselected;

	//BitBlt(hdc, 0, 0, nWidth, nHeight, memHdc, 0, 0, SRCCOPY);
}

VOID onAStar(FLOAT x, FLOAT y)
{
	World * pworld = &world;
	EPoint pt(x, y), lq, gm;
	lq.Set(preview.display.X, preview.display.Y);
	gm.Set(preview.geometry.Width, preview.geometry.Height);
	if (pt.IsIn(lq, gm, E_RectLtGm)) {
		pworld = &preview;
	}
	World &rworld = *pworld;

	Roles * role = rworld.focus;
	if (role)
	{
		PointF pt = projectFlat(x, y);
		Paths * dest = new Paths((pt.X - role->flatting.X) / role->flatting.Width, (pt.Y - role->flatting.Y) / role->flatting.Height);
		role->astarPath(dest, 1000);

		if (rworld.cursor)
		{
			rworld.cursor->moveFlat(pt.X - rworld.cursor->flatting.X, pt.Y - rworld.cursor->flatting.Y);
		}
	}
}

PointF start;
PointF prest;

VOID onDrag(FLOAT x, FLOAT y, INT mode)
{
	World * pworld = &world;
	EPoint pt(x, y), lq, gm;
	lq.Set(preview.display.X, preview.display.Y);
	gm.Set(preview.geometry.Width, preview.geometry.Height);
	if (pt.IsIn(lq, gm, E_RectLtGm)) {
		pworld = &preview;
	}
	World &rworld = *pworld;
	if (mode == 1)
	{
		start.X = x - rworld.leftTop.X;
		start.Y = y - rworld.leftTop.Y;
	}
	else if (mode == 2)
	{
		if (start.X != 0 && start.Y != 0)
		{
			rworld.offset(x - rworld.leftTop.X - start.X, y - rworld.leftTop.Y - start.Y);
			isDrag = 1;
		}
	}
	else
	{
		start.X = 0;
		start.Y = 0;
		isDrag = 0;
	}
}

VOID trigger(void * lParam, void * wParam)
{
	Roles * role = ((Roles *)lParam);
	if (!role) {
		return;
	}

	// It's necessary beacause there's only one client
	// that can handle emit
	if (!role->equal(world.focus))
	{
		return;
	}
	if (role->irole)
	{
	}
	else
	{
		Animation_Base::type oldbase = role->base;
		role->base = Animation_Base::Still;
	}
	role->selectAnimation(role->base + role->direction);
}

VOID Initialize()
{
	preview.resize(10, 10);
	world.resize(getwidth(), getheight());

	Loading::loadResource(resm);

	Roles * cursor = new Roles();
	Animations * animation = new Animations();
	animation->delay = 1;
	animation->mode = Animation_Mode::Step;
	cursor->animations.addAnimation(animation, 233)\
		->addSequence(new ObjectManager(), 3455)\
		->addStep(new Objects(resm.getResource(22), RectF(0, 0, 10, 14), RectF(0, 0, 10, 14)), 202);
	cursor->setFlatting(RectF(0, 0, 10, 14), 14);
	cursor->moveRole(100, 100);
	world.addRole(cursor, Role_Type::Cursor);

	Roles * role;
	INT i, j;

	role = Loading::loadRole(resm, 0.5, "./scene/player.txt");
	role->setFlatting(RectF(100, 100, 40, 25), role->tall);
	world.addRole(role, Role_Type::Player);
	world.focus = role;

	char temp[100];
	for (i = 0; i < 300; i++)
	{
		int id = rand() % 4 + 1;
		if (!(id >= 1 && id <= 4)) {
			continue;
		}
		sprintf_s(temp, "./scene/tree%d.txt", id);
		role = Loading::loadRole(resm, 2, temp);
		role->setFlatting(RectF(::rand() % 5000, ::rand() % 5000, role->flatting.Width, role->flatting.Height), role->tall);
		world.addRole(role);
	}


	cursor = new Roles();
	animation = new Animations();
	animation->delay = 1;
	animation->mode = Animation_Mode::Step;
	cursor->animations.addAnimation(animation, 233)\
		->addSequence(new ObjectManager(), 3455)\
		->addStep(new Objects(resm.getResource(22), RectF(0, 0, 10, 14), RectF(0, 0, 10, 14)), 202);
	cursor->setFlatting(RectF(0, 0, 10, 14), 14);
	preview.addRole(cursor, Role_Type::Cursor);

	role = Loading::loadRole(resm, 0.25, "./scene/player.txt");
	role->setFlatting(RectF(100, 100, 40, 25), role->tall);
	preview.addRole(role, Role_Type::Player);
	preview.focus = role;
	for (i = 0; i < 2; i++)
	{
		int id = rand() % 4 + 1;
		if (!(id >= 1 && id <= 4)) {
			continue;
		}
		sprintf_s(temp, "./scene/tree%d.txt", id);
		role = Loading::loadRole(resm, 1, temp);
		role->setFlatting(RectF(::rand() % 300, ::rand() % 300, role->flatting.Width, role->flatting.Height), role->tall);
		preview.addRole(role);
	}
	INT count = 100;
	INT index = 0;
	for (i = 0; i < 300; i++)
	{
		int id = rand() % 4 + 1;
		if (!(id >= 1 && id <= 4)) {
			continue;
		}
		sprintf_s(temp, "./scene/tree%d.txt", id);
		role = Loading::loadRole(resm, 1, temp);
		if (count-- == 0) {
			index = role->uniqueID;
		}
		role->setFlatting(RectF(::rand() % 5000, ::rand() % 5000, role->flatting.Width, role->flatting.Height), role->tall);
		preview.addRole(role);
	}

	preview.display.X = 50;
	preview.display.Y = 50;
	preview.offset(100, 100);

	Loading::loadMap(resm, world, 1);
	Loading::loadMap(resm, preview, 0.5);

	world.focus = world.binTree.getRole(index);
}

VOID onTimer()
{
	world.refreshRole();
}

VOID onKeyDown(WPARAM wParam)
{
	Roles * role = world.focus;
	Roles * temp;
	if (1)
	{
		switch (wParam)
		{
		case VK_LEFT:
			wParam = 'J';
			break;
		case VK_RIGHT:
			wParam = 'L';
			break;
		case VK_UP:
			wParam = 'I';
			break;
		case VK_DOWN:
			wParam = 'K';
			break;
		case 'J':
			wParam = VK_LEFT;
			break;
		case 'L':
			wParam = VK_RIGHT;
			break;
		case 'I':
			wParam = VK_UP;
			break;
		case 'K':
			wParam = VK_DOWN;
			break;
		}
	}
	switch (wParam)
	{
	case VK_LEFT:
		role->selectAnimation(115);
		role->moveRole(-10, 0, 1);
		//world.offset(10, 0);
		break;
	case VK_RIGHT:
		role->selectAnimation(116);
		role->moveRole(10, 0, 1);
		//world.offset(-10, 0);
		break;
	case VK_UP:
		role->selectAnimation(117);
		role->moveRole(0, -10, 1);
		//world.offset(0, 10);
		break;
	case VK_DOWN:
		role->selectAnimation(114);
		role->moveRole(0, 10, 1);
		//world.offset(0, -10);
		break;
	case 'J':
		role->selectAnimation(112);
		role->moveFlat(-10, 0, 1);
		break;
	case 'L':
		role->selectAnimation(112);
		role->moveFlat(10, 0, 1);
		break;
	case 'I':
		role->selectAnimation(112);
		role->moveFlat(0, -10, 1);
		break;
	case 'K':
		role->selectAnimation(112);
		role->moveFlat(0, 10, 1);
		break;
	case 'A':
		role->flatting.Width -= 1;
		role->updateFlatting();
		break;
	case 'D':
		role->flatting.Width += 1;
		role->updateFlatting();
		break;
	case 'S':
		role->flatting.Height += 1;
		role->updateFlatting();
		break;
	case 'W':
		role->flatting.Height -= 1;
		role->updateFlatting();
		break;
	case 'Q':
		preview.display.X++;
		temp = preview.roles.link;
		if (temp) {
			do {
				temp->moveRole(1, 0, 0);

				temp = preview.roles.next(temp);
			} while (temp && temp != preview.roles.link);
		}
		//role->tall -= 1;
		//role->updateFlatting();
		break;
	case 'E':
		preview.display.Y++;
		temp = preview.roles.link;
		if (temp) {
			do {
				temp->moveRole(0, 1, 0);

				temp = preview.roles.next(temp);
			} while (temp && temp != preview.roles.link);
		}
		//role->tall += 1;
		//role->updateFlatting();
		break;
	case 'T':
		role->animations.animation.link->objects.link->object.link->destination.Y -= 1;
		break;
	case 'G':
		role->animations.animation.link->objects.link->object.link->destination.Y += 1;
		break;
	case 'F':
		role->animations.animation.link->objects.link->object.link->destination.X -= 1;
		break;
	case 'H':
		role->animations.animation.link->objects.link->object.link->destination.X += 1;
		break;
	case 'V':
		DEBUG_MODE = DEBUG_MODE >> 1;
		if (DEBUG_MODE == 0)
		{
			DEBUG_MODE = 0x0B;
		}
		break;
	}
	world.refreshDepth(role);
}