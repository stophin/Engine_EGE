// Loading.h
//

#ifndef _LOADING_H_
#define _LOADING_H_

#include "Drawable.h"

#define MAX_STR	1024
#define MAX_PAR	5

extern VOID trigger(void * lParam, void * wParam);

class Loading {
public:
	static void loadResource(ResourceManager& resm) {
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


	static Roles * loadRole(ResourceManager& resm, FLOAT scale, const char * fileName)
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

	static void loadMap(ResourceManager& resm, World& world, FLOAT scale)
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
						RectF flatting = RectF(j * 80, i * 45, 80, 45);
						FLOAT tall = 47;
						flatting.Width *= scale;
						flatting.Height *= scale;
						flatting.X *= scale;
						flatting.Y *= scale;
						role->scale = scale;
						tall *= scale;
						role->setFlatting(flatting, tall);

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


	static INT parseParameter(CHAR buffer[], CHAR command[], CHAR parameters[][MAX_STR])
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

	static CONSTANTS_TYPE getDirection(char * base, char * direction) {
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

};


#endif