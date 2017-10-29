//GetContour.cpp
//

#include "cv.h"
#include "highgui.h"
#include "stdio.h"

int main(int argc,char *argv[])
{
	int round=10;

	if (argc>1)
	{
		//If error occurs, atoi returns 0, 
		//and it's no problem if round is zero
		round=atoi(argv[1]);
	}


	IplImage* image=cvLoadImage("test.jpg",-1);

	if(image==NULL)
	{
		printf("无法读取图像数据!\n");
		return -1;
	}

	IplImage *gray=NULL,*edge=NULL;
	edge=cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_8U, 1);
	edge->origin=image->origin;

	gray=cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_8U, 1);
	gray->origin=image->origin;

	cvCvtColor(image,gray,CV_RGB2GRAY);

	cvCanny(gray,edge,150,50,3);

	if (round)
	{

		int i,j,k;
		int p,q;
		for(i=0;i != edge->height; ++ i)  
		{ 
			for(j=0;j != edge->width; ++ j) 
			{ 
				for(k=0;k != edge->nChannels; ++ k) 
				{ 
					if (edge->imageData[i*edge->widthStep+j*edge->nChannels+k]!=0x00)
					{
						//round is >= 0
						p=i-round;q=j-round;
						if (p<0)	p=0;
						if (q<0)	q=0;
						for (;p<=i+round&&p<image->height;p++)
						{
							for (;q<=j+round&&q<image->width;q++)
							{
								image->imageData[p*image->widthStep+q*image->nChannels+0]=0x00;
								image->imageData[p*image->widthStep+q*image->nChannels+1]=0x00;
								image->imageData[p*image->widthStep+q*image->nChannels+2]=0xFF;
							}
							q=j-round;
						}
					}
				} 
			} 
		}   
	}

	cvNamedWindow("图像显示",CV_WINDOW_AUTOSIZE);
	cvShowImage("图像显示",image);

	cvNamedWindow("转换后图像",CV_WINDOW_AUTOSIZE);
	cvShowImage("转换后图像",edge);

	cvSaveImage("test-bk.bmp",image); 

	cvWaitKey(0);
	cvReleaseImage(&image);
	cvDestroyWindow("图像显示");

	return 0;
}