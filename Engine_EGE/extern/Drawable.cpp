
#include "Drawable.h"

int DEBUG_MODE = DEBUG_GRADE_2;

World preview;
World world;
ResourceManager resm;

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

#define MAX_STR	1024
#define MAX_PAR	5

INT parseParameter(CHAR buffer[], CHAR command[], CHAR parameters[][MAX_STR])
{
	int i, j;
	int cp = 0;
	int pp = 0;
	int pc = 0;
	int flag = 0;
	int lspc = 0;
	for (i = 0; buffer[i] != '\0' && (buffer[i] == '\t' || buffer[i] == ' '); i++);
	for (j = i; buffer[j] != '\0'; j++)
	{
		if (buffer[j] == '\n')
		{
			buffer[j] = '\0';
			break;
		}
	}
	for (; buffer[i] != '\0'; i++)
	{
		if (buffer[i] == '\t' || buffer[i] == ' ')
		{
			if (lspc == 0)
			{
				if (flag)
				{
					parameters[pc][pp] = '\0';
					pc++;
					pp = 0;
				}
				else
				{
					command[cp] = '\0';
					flag = 1;
				}
			}
			lspc++;
			continue;
		}
		else
		{
			lspc = 0;
		}
		if (flag)
		{
			parameters[pc][pp++] = buffer[i];
			if (pp >= MAX_STR)
			{
				break;
			}
		}
		else
		{
			command[cp++] = buffer[i];
			if (cp >= MAX_STR)
			{
				break;
			}
		}
	}
	if (flag)
	{
		parameters[pc][pp] = '\0';
	}
	else
	{
		command[cp] = '\0';
		pc = pc - 1;
	}
	return pc + 1;
}

CONSTANTS_TYPE getDirection(char * base, char * direction) {
	Animation_Base::type index_base = Animation_Base::Still;
	Animation_Direction::type index = Animation_Direction::Undefined;
	if (base) {
		if (!strcmp(base, "still")) {
			index_base = Animation_Base::Still;
		}
		else if (!strcmp(base, "move")) {
			index_base = Animation_Base::Move;
		}
		else if (!strcmp(base, "run")) {
			index_base = Animation_Base::Run;
		}
		else if (!strcmp(base, "attack")) {
			index_base = Animation_Base::Attack;
		}
		else if (!strcmp(base, "dead")) {
			index_base = Animation_Base::Dead;
		}
	}
	if (direction) {
		if (!strcmp(direction, "down")) {
			index = Animation_Direction::Down;
		}
		else if (!strcmp(direction, "left_down")) {
			index = Animation_Direction::Left_Down;
		}
		else if (!strcmp(direction, "left")) {
			index = Animation_Direction::Left;
		}
		else if (!strcmp(direction, "left_up")) {
			index = Animation_Direction::Left_Up;
		}
		else if (!strcmp(direction, "up")) {
			index = Animation_Direction::Up;
		}
		else if (!strcmp(direction, "right_up")) {
			index = Animation_Direction::Right_Up;
		}
		else if (!strcmp(direction, "right")) {
			index = Animation_Direction::Right;
		}
		else if (!strcmp(direction, "right_down")) {
			index = Animation_Direction::Right_Down;
		}
		else if (!strcmp(direction, "still")) {
			index = Animation_Direction::Undefined;
		}
	}

	return (CONSTANTS_TYPE)(index_base + index);
}

Roles * createRole(const char * fileName, FLOAT scale)
{
	if (!scale) {
		scale = 1;
	}
	FILE * fp;
	fopen_s(&fp, fileName, "r");
	CHAR buffer[MAX_STR];
	CHAR command[MAX_STR];
	CHAR parameters[MAX_PAR][MAX_STR];
	INT paramCount;
	Roles * role = new Roles();
	if (fp)
	{
		Animations * animation = NULL;
		ObjectManager * objectManager = NULL;
		Resources * resource = NULL;
		INT index;
		INT tall = 0;
		INT reverse = 0;
		RectF *destination = NULL;
		RectF *truncation = NULL;
		RectF *flatting = NULL;

		while (!feof(fp))
		{
			fgets(buffer, MAX_STR, fp);

			paramCount = parseParameter(buffer, command, parameters);

			if (!strcmp(command, "animation"))
			{
				if (role && animation)
				{
					role->animations.addAnimation(animation, animation->uniqueID);
					animation = NULL;
				}
				if (paramCount >= 1)
				{
					index = getDirection(parameters[0], parameters[1]);
					if (index >= 0)
					{
						animation = new Animations();
						animation->mode = Animation_Mode::Still;
						animation->uniqueID = index;
					}
				}
			}
			else if (!strcmp(command, "following")) {
				if (role && paramCount >= 1) {
					index = atoi(parameters[0]);
					if (index >= 0) {
						role->following = index;
					}
				}
			}
			else if (!strcmp(command, "step"))
			{
				if (animation && objectManager)
				{
					animation->addSequence(objectManager, objectManager->uniqueID);
					objectManager = NULL;
				}
				if (paramCount >= 1)
				{
					index = atoi(parameters[0]);
					if (index >= 0)
					{
						objectManager = new ObjectManager();
						objectManager->uniqueID = index;

						if (animation && objectManager)
						{
							if (animation->mode == Animation_Mode::Step || animation->mode == Animation_Mode::Auto)
							objectManager->callbackFunction = trigger;
						}
					}
				}
			}
			else if (!strcmp(command, "resource"))
			{
				if (objectManager && resource)
				{
					Objects * object = new Objects();
					object->reverse = reverse;
					objectManager->addStep(object, resource, *destination, *truncation, resource->uniqueID, scale);
					resource = NULL;
				}
				if (paramCount >= 1)
				{
					index = atoi(parameters[0]);
					if (index > 0)
					{
						resource = resm.getResource(index);
					}
					reverse = 0;
				}
			}
			else if (!strcmp(command, "reverse"))
			{
				if (paramCount >= 1 && resource)
				{
					reverse = atoi(parameters[0]);
				}
			}
			else if (!strcmp(command, "position"))
			{
				if (paramCount >= 4)
				{
					destination = new RectF();
					destination->X = atoi(parameters[0]);
					destination->Y = atoi(parameters[1]);
					destination->Width = atoi(parameters[2]);
					destination->Height = atoi(parameters[3]);
				}
			}
			else if (!strcmp(command, "truncate"))
			{
				if (paramCount >= 4)
				{
					truncation = new RectF();
					truncation->X = atoi(parameters[0]);
					truncation->Y = atoi(parameters[1]);
					truncation->Width = atoi(parameters[2]);
					truncation->Height = atoi(parameters[3]);
				}
			}
			else if (!strcmp(command, "flatting"))
			{
				if (paramCount >= 4)
				{
					flatting = new RectF();
					flatting->X = atoi(parameters[0]);
					flatting->Y = atoi(parameters[1]);
					flatting->Width = atoi(parameters[2]);
					flatting->Height = atoi(parameters[3]);
				}
			}
			else if (!strcmp(command, "tall"))
			{
				if (paramCount >= 1)
				{
					index = atoi(parameters[0]);
					if (index >= 0)
					{
						tall = index;
					}
				}
			}
			else if (!strcmp(command, "scale"))
			{
				if (paramCount >= 1)
				{
					scale = (FLOAT)(atoi(parameters[0])) / 100.0;
					if (scale < 0)
					{
						scale = 1;
					}
				}
			}
			else if (!strcmp(command, "delay"))
			{
				if (paramCount >= 1 && animation)
				{
					index = atoi(parameters[0]);
					if (animation && index > 0)
					{
						animation->delay = index;
					}
				}
			}
			else if (!strcmp(command, "mode"))
			{
				if (paramCount >= 1 && animation)
				{
					if (!strcmp(parameters[0], "Animation_Step"))
					{
						animation->mode = Animation_Mode::Step;
					}
					else if (!strcmp(parameters[0], "Animation_Auto"))
					{
						animation->mode = Animation_Mode::Auto;
					}
					else if (!strcmp(parameters[0], "Animation_Loop"))
					{
						animation->mode = Animation_Mode::Loop;
					}
				}
			}
		}
		fclose(fp);

		if (flatting) {
			role->scale = scale;
			tall *= scale;
			flatting->Width *= scale;
			flatting->Height *= scale;
			flatting->X *= scale;
			flatting->Y *= scale;

			role->setFlatting(*flatting, tall);
		}
	}

	return role;
}

VOID getResource()
{
	FILE * fp;
	fopen_s(&fp, "./scene/resources.txt", "r");
	CHAR buffer[MAX_STR];
	CHAR command[MAX_STR];
	CHAR parameters[MAX_PAR][MAX_STR];
	WCHAR commandW[MAX_STR];
	INT paramCount;
	if (fp)
	{
		while (!feof(fp))
		{
			fgets(buffer, MAX_STR, fp);

			paramCount = parseParameter(buffer, command, parameters);
			wsprintf(commandW, L"%S", command);

			if (paramCount >= 1 && paramCount < 2)
			{
				resm.addResource(resm.createResource(commandW, atoi(parameters[0])));
			}
			else if (paramCount < 3)
			{
				resm.addResource(resm.createResource(commandW, atoi(parameters[0]), NULL, atoi(parameters[1])));
			}
			else if (paramCount < 4)
			{
				resm.addResource(resm.createResource(commandW, atoi(parameters[0]), resm.getResource(atoi(parameters[1])), atoi(parameters[2])));
			}
		}
		fclose(fp);
	}
}

void createMap()
{
	FILE * fp = NULL;
	Roles * role = NULL;
	Animations * animation = NULL;
	fopen_s(&fp, "./scene/map.txt", "r");
	if (fp)
	{
		CHAR buffer[1024];
		INT i = 0;
		INT j = 0;
		CHAR * pbuffer, *tbuffer;
		MultiLinkList<Roles> temp1(Role_Link::Temp1), temp2(Role_Link::Temp2), *temp = &temp1;
		Roles * tprole = NULL, *lprole = NULL;
		while (!feof(fp))
		{
			fgets(buffer, 1024, fp);

			pbuffer = tbuffer = buffer;
			while (*pbuffer)
			{
				while (*pbuffer)
				{
					if (*pbuffer == '\t')
					{
						*pbuffer = '\0';
						break;
					}
					pbuffer++;
				}
				INT index = atoi(tbuffer);
				if (index > 100 && index < 106)
				{
					role = new Roles();
					animation = new Animations();
					animation->delay = 1;
					role->animations.addAnimation(animation, 112)\
						->addSequence(new ObjectManager(), 120)\
						->addStep(new Objects(resm.getResource(index), RectF(0, 0, 96, 47), RectF(0, 0, 96, 47)), 102);
					role->setFlatting(RectF(j * 80, i * 45, 80, 45), 47);
					world.addRole(role, Role_Type::Back);

					if (lprole) {
						role->left = lprole;
						lprole->right = role;
					}
					lprole = role;

					if (temp == &temp1) {
						temp2.insertLink(role);
					}
					else {
						temp1.insertLink(role);
					}
					if (i > 0) {
						if (tprole) {
							role->up = tprole;
							tprole->down = role;
							tprole = temp->next(tprole);
						}
					}
					j++;
				}
				pbuffer++;
				tbuffer = pbuffer;
			}
			i++;
			j = 0;
			lprole = NULL;
			if (i > 0) {
				temp->clearLink();
			}
			if (temp == &temp1) {
				temp = &temp2;
			}
			else {
				temp = &temp1;
			}
			tprole = temp->link;
		}
		temp1.clearLink();
		temp2.clearLink();
		fclose(fp);
	}
}

VOID Initialize()
{
	preview.resize(300, 300);
	world.resize(getwidth(), getheight());

	getResource();

	Roles * cursor = new Roles();
	Animations * animation = new Animations();
	animation->delay = 1;
	animation->mode = Animation_Mode::Step;
	cursor->animations.addAnimation(animation, 233)\
		->addSequence(new ObjectManager(), 3455)\
		->addStep(new Objects(resm.getResource(22), RectF(0, 0, 10, 14), RectF(0, 0, 10, 14)), 202);
	cursor->setFlatting(RectF(0, 0, 10, 14), 14);
	world.addRole(cursor, Role_Type::Cursor);

	Roles * role;
	INT i, j;

	role = createRole("./scene/player.txt", 0.5);
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
		role = createRole(temp, 2);
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

	role = createRole("./scene/player.txt", 0.5);
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
		role = createRole(temp, 2);
		role->setFlatting(RectF(::rand() % 300, ::rand() % 300, role->flatting.Width, role->flatting.Height), role->tall);
		preview.addRole(role);
	}

	preview.display.X = 100;
	preview.display.Y = 100;
	preview.offset(100, 100);

	createMap();
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