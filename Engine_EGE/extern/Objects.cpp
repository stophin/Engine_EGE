
#include "Objects.h"

#include "Drawable.h"


Objects::Objects(Resources * resource, RectF& destination, RectF& truncation, FLOAT scale)
{
	this->resource = resource;
	this->destination = destination;
	this->truncation = truncation;

	this->destination.Width *= scale;
	this->destination.Height *= scale;
	this->destination.X *= scale;
	this->destination.Y *= scale;

	this->reverse = 0;
}

Objects::Objects() : prev(NULL), next(NULL)
{
}

Objects::~Objects()
{
}

EPoint mouse;
INT Objects::drawObject(Graphics & graphics)
{
		if (!this->resource || !this->resource->image)
		{
			return 0;
		}
		if (!this->world) {
			return 0;
		}
		PointF geom(this->world->geometry.Width, this->world->geometry.Height);
		INT in = 0;
		mouse.Set(msg.x, msg.y);
	//graphics.DrawImage(this->resource->image, this->destination, this->truncation.X, this->truncation.Y, this->truncation.Width, this->truncation.Height, UnitPixel);
	/*EP_TruncImage(this->destination.X, this->destination.Y, this->resource->image->image, this->resource->image->image_bk, BLACK, false, 0, 1, 0, \
		0,0, this->truncation.X, this->truncation.Y, this->truncation.X + this->truncation.Width, this->truncation.X + this->truncation.Y );*/
		if (this->reverse) {
			in = EP_TruncateImage(this->destination.X, this->destination.Y, \
				-this->destination.Width, this->destination.Height, this->resource->image->image, \
				this->truncation.X, this->truncation.Y, this->truncation.Width, this->truncation.Height, \
				this->resource->image->image_bk, this->resource->cutmode, BLACK, &mouse, &this->world->display, &geom);
		}
		else {
			in = EP_TruncateImage(this->destination.X, this->destination.Y, \
				this->destination.Width, this->destination.Height, this->resource->image->image, \
				this->truncation.X, this->truncation.Y, this->truncation.Width, this->truncation.Height, \
				this->resource->image->image_bk, this->resource->cutmode, BLACK, &mouse, &this->world->display, &geom);

			if (in) {
				EP_TruncateImage(this->destination.X, this->destination.Y, \
					this->destination.Width, this->destination.Height, this->resource->image->image_bk, \
					this->truncation.X, this->truncation.Y, this->truncation.Width, this->truncation.Height, \
					NULL, -1, DEF_CTCOLOR, 0, &this->world->display, &geom);
			}
		}
		if (DEBUG_MODE & DEBUG_GRADE_4) {
			CHAR fmt[100];
			EP_SetColor(WHITE);
			sprintf_s(fmt, "(%d, %d) [%d]", msg.x, msg.y, in);
			EP_Text(10, 100, fmt);
		}
	
	/*ege::putimage(this->destination.X, this->destination.Y, this->destination.Width, this->destination.Height, \
		this->resource->image->image, \
		//this->truncation.X, this->truncation.Y, this->truncation.Width, this->truncation.Height);
		0, 0, EP_GetImageWidth(this->resource->image->image), EP_GetImageHeight(this->resource->image->image));*/

		return in;
}

VOID Objects::moveObject(FLOAT directionX, FLOAT directionY)
{
	this->destination.X += directionX;
	this->destination.Y += directionY;
}