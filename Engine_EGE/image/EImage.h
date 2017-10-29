//EImage.h
//A image class
//
//Author: Stophin
//2014.01.08
//Ver: 0.01
#ifndef _EIMAGE_H_
#define _EIMAGE_H_

#include "../flatform/EPaint.h"

#define DEF_GEOM_PAM	10

typedef class EImage
{
public:
	EImage();
	~EImage();

	//image
	EIMAGE image;
	//image's mask
	EIMAGE image_bk;
	//Is image a png
	int imgtype;

	// 
	EPTYPE factor;
	EPTYPE shift;
	EPTYPE tall;
	EPTYPE regX;
	EPTYPE regY;

	EPVOID SetGeom(EPTYPE x,EPTYPE y);
	EPVOID ResetGeom(EPTYPE x,EPTYPE y);
	EPTYPE Height();
	EPTYPE Width();

	EPVOID LoadImage(EPSTRING fimg,EPSTRING fimg_bk);
	EPVOID LoadImageFrnt(EPSTRING fimg);
	EPVOID LoadImageBack(EPSTRING fimg_bk);

	EPoint geom;
}EImage,* PEImage;

#endif//end of _EIMAGE_H_
//end of file
