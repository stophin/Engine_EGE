#pragma once

#include "Constants.h"

#include "../flatform/EPaint.h"
#include "../image/EImage.h"

#include <wingdi.h>
#include <comdef.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

class Resources
{
public:
	Resources(const WCHAR * filename, INT resourceID, Resources * mask = NULL, INT cutmode = 0);
	~Resources();

	EImage * image;
	FLOAT width;
	FLOAT height;

	INT uniqueID;

	INT cutmode;

	Resources * prev;
	Resources * next;
};

