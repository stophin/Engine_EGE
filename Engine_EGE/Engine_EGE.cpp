// Engine_EGE.cpp : 定义控制台应用程序的入口点。
//

#include "flatform/EPaint.h"
#include "extern/World.h"
#include "extern/ResourceManager.h"
#include "extern/Drawable.h"

#include "NanoCGR.h"
extern NanoCGR nanoCGR;

extern World world;
extern int isDrag;
mouse_msg msg = { 0 };
int main(int argc, char* argv[])
{
	//EP_Init(-1, -1);
	EP_Init(800, 600);

	HDC hdc = GetDC(NULL);
	Graphics grapchis(hdc);

	
	Initialize();
	nanoCGR.Init();
	
	//Roles * role = world.focus;

	CHAR c = 0;
	EP_RenderStart();
	INT count = 0;
	Roles * role = NULL;
	do
	{
		//world.drawRole(grapchis);
		// Get the role the mouse pointing at
		role = onPaint(getHWnd());

		while (mousemsg())
		{
			msg = getmouse();
			if (msg.is_right())
			{
				onAStar(msg.x, msg.y);
			}
			else
			{
				if (msg.is_down())
				{
					onDrag(msg.x, msg.y, 1);
				}
				else if (msg.is_move())
				{
					onDrag(msg.x, msg.y, 2);
				}
				else if (msg.is_up())
				{
					if (role && !isDrag) {
						world.focus = role;
					}
					onDrag(msg.x, msg.y, 0);
				}
			}
		}


		if (kbhit())
		{
			c = getch();
			c = toupper(c);
			onKeyDown(c);
		}

		if (count++ > 2) {
			count = 0;
			onTimer();
		}
		//delay_ms(100);
		EP_RenderFlush();
	} while (c != 27);
	closegraph();
}

