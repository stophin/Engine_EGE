//CharLink.cpp
//


#include "CharLink.h"
#include "../flatform/EPaint.h"

CharLink::CharLink():
charpoollen(0),
charpool(NULL),
charcur(NULL),
charsta(NULL),
charlne(NULL),
charlle(NULL),
lines(0)
{
}

CharLink::~CharLink()
{
}

void CharLink::SetCharPool(PCharElement pce,int len)
{
	if (charpool!=NULL)
		return;
	if (pce==NULL)
		return;

	charpool=pce;
	charpoollen=len;
	ClearCharPool();
}

void CharLink::ClearCharPool()
{
	if (charpool==NULL)
		return;

	int len;
	for (len=0;len<charpoollen;len++)
	{
		ClearChar(&charpool[len]);
	}
}

void CharLink::ClearChar(PCharElement pce)
{
	if (pce==NULL)
		return;
	pce->data=CHAR_NULL;
	pce->up=NULL;
	pce->down=NULL;
	pce->left=NULL;
	pce->right=NULL;
	pce->last=NULL;
}

PCharElement CharLink::AllocCharPool()
{
	if (charpool==NULL)
		return NULL;
	int i;
	for (i=0;i<charpoollen;i++)
	{
		if (charpool[i].data!=CHAR_NULL)
			continue;
		if (charpool[i].up||charpool[i].down||charpool[i].left||charpool[i].right)
			continue;
		return &charpool[i];
	}
	return NULL;
}

bool CharLink::LineChanged()
{
	if (charlne==charlle)
		return false;
	charlle=charlne;
	return true;
}

bool CharLink::CharLinkInit()
{
	if (charsta!=NULL)
		return false;
	charsta=AllocCharPool();
	if (charsta==NULL)
		return false;
	charsta->data=CHAR_HEAD;

	PCharElement pce=AllocCharPool();
	if (pce==NULL)
	{
		ClearChar(charsta);
		return false;
	}
	pce->data=CHAR_TERM;

	charsta->left=pce;
	charsta->right=pce;
	charsta->up=pce;
	charsta->down=pce;
	pce->left=charsta;
	pce->right=charsta;
	pce->up=charsta;
	pce->down=charsta;

	//Current cursor pointer
	charcur=pce;
	//Current line pointer
	charlne=charsta;
	return true;
}

void CharLink::DeleteChar()
{
	if (charsta==NULL)
		return;
	if (charlne==NULL)
		return;

	PCharElement pce=charcur;

	if (pce==NULL)
		return;
	if (pce->data==CHAR_HEAD)
		return;
	if (pce->data==CHAR_TERM)
		return;

	//Prepare for deleting element
	pce->left->right=pce->right;
	pce->right->left=pce->left;

	charcur=pce->right;
	
	//If a new line mark is deleted
	if (pce->data==CHAR_LINE)
	{
		pce->up->down=pce->down;
		pce->down->up=pce->up;

		//Reset current line
		charlne=pce->up;

		lines--;
	}

	//Delete element
	ClearChar(pce);
}

bool CharLink::AddChar(PCharElement pce)
{
	if (charsta==NULL)
		return false;
	if (pce==NULL)
		return false;
	if (charcur==NULL)
		return false;
	if (charlne==NULL)
		return false;
	if (pce->data==CHAR_HEAD)
		return false;
	if (pce->data==CHAR_TERM)
		return false;
	
	//store this line's last cursor point
	charlne->last=pce;

	//Add element to current cursor pointer's left
	pce->left=charcur->left;
	pce->right=charcur;
	charcur->left->right=pce;
	charcur->left=pce;

	//If this element is a new line mark
	if (pce->data==CHAR_LINE)
	{
		pce->up=charlne;
		pce->down=charlne->down;
		charlne->down->up=pce;
		charlne->down=pce;

		charlne=pce;

		lines++;
	}
	return true;
}

bool CharLink::AddString(char *pc)
{
	int i;
	bool ret=false;
	for (i=0;pc[i]!='\0';i++)
	{
		if (!(ret=AddChar(pc[i])))
			break;
	}
	return ret;
}

bool CharLink::AddChar(char c)
{
	if (c==CHAR_HEAD)
		return false;
	if (c==CHAR_TERM)
		return false;
	PCharElement pce=AllocCharPool();
	if (pce==NULL)
		return false;
	pce->data=c;
	return AddChar(pce);
}

void CharLink::CurMoveLeft()
{
	if (charsta==NULL)
		return;
	if (charcur==NULL)
		return;
	
	if (charcur->left->data==CHAR_HEAD)
		return;

	charcur=charcur->left;
	if (charcur==charlne)
		charlne=charlne->up;

	//store this line's last cursor point
	charlne->last=charcur;
}

void CharLink::CurMoveRight()
{
	if (charsta==NULL)
		return;
	if (charcur==NULL)
		return;
	
	if (charcur->data==CHAR_TERM)
		return;
	
	if (charcur==charlne->down)
		charlne=charlne->down;
	charcur=charcur->right;
		
	//store this line's last cursor point
	charlne->last=charcur;
}

void CharLink::CurMoveUp()
{
	if (charsta==NULL)
		return;
	if (charcur==NULL)
		return;
	
	if (charlne==charsta)
		return;

	//store this line's last cursor point
	charlne->last=charcur;

	if (charlne->up->last&&charlne->up->last->data!=CHAR_NULL)
	{	
		charlne=charlne->up;

		charcur=charlne->last;
	}
	else
	{
		PCharElement pce=charcur;
		int loop=0;
		do 
		{
			pce=pce->left;
			loop++;
		} while (pce!=charlne&&pce!=charcur);
		charlne=charlne->up;
		charcur=charlne;
		loop--;
		do 
		{
			charcur=charcur->right;
		} while (loop--&&charcur!=charlne->down);
	}
}

void CharLink::CurMoveDown()
{
	if (charsta==NULL)
		return;
	if (charcur==NULL)
		return;
	
	if (charlne->down==charsta->up)
		return;

	//store this line's last cursor point
	charlne->last=charcur;

	if (charlne->down->last&&charlne->down->last->data!=CHAR_NULL)
	{	
		charlne=charlne->down;

		charcur=charlne->last;
	}
	else
	{
		PCharElement pce=charcur;
		int loop=0;
		do 
		{
			pce=pce->left;
			loop++;
		} while (pce!=charlne&&pce!=charcur);
		charlne=charlne->down;
		charcur=charlne;
		loop--;
		do 
		{
			charcur=charcur->right;
		} while (loop--&&charcur!=charlne->down);
	}
}

void CharLink::CurMoveHead()
{
	if (charsta==NULL)
		return;
	if (charcur==NULL)
		return;
	if (charlne==NULL)
		return;
	
	charcur=charlne->right;
	
	//store this line's last cursor point
	charlne->last=charcur;
}

void CharLink::CurMoveEnd()
{
	if (charsta==NULL)
		return;
	if (charcur==NULL)
		return;
	if (charlne==NULL)
		return;
	
	charcur=charlne->down;
	
	//store this line's last cursor point
	charlne->last=charcur;
}

int CharLink::GetLine(int line,int& offset,char *buffer,int& len,int& lens,int& leno,int& lenl,int& lena)
{
	if (charsta==NULL)
		return -1;
	if (line<=0)
		return -1;
	if (offset<0)
		return -1;

	int ret=0;
	buffer[0]=CHAR_TERM;
	PCharElement phead=charsta->up;
	int i=line-1;
	do
	{
		phead=phead->down;
		if (i+1!=line&&phead==charsta)
		{
			ret=-1;
			break;
		}
	}while(i--);
	if (ret!=0)
		return ret;

	phead=phead->right;
	
	int lenlp=0,lenop=0;
	int lenp=len,ofst=offset;
	ret=0;
	line=0;
	lenl=0;lena=0;leno=0;
	int asc=0;
	while(lenp--&&phead->data!=CHAR_LINE&&phead->data!=CHAR_TERM&&phead->data!=CHAR_HEAD)
	{
		if (phead==charcur)
		{
			ret=line+1;
			lenlp=line;
		}
		if (ofst--==0)
		{
			lenop=line;
			if (asc%2)
			{
				lenop--;
				offset--;
			}
		}
		if (line>=len)
		{
			buffer[line-1]=CHAR_TERM;
			len=line;
			return ret;
		}
		buffer[line++]=phead->data;
		if (phead->data&0x80)
			asc++;
		phead=phead->right;
	}
	if (ret==0&&phead==charcur)
	{
		ret=line+1;
		lenlp=line;
	}
	char temp=buffer[lenop];
	buffer[lenop]=CHAR_TERM;
	leno=textwidth(buffer);
	buffer[lenop]=temp;

	char *buf=buffer+lenop;
	for (lenp=1;lenp+lenop<line;lenp++)
	{
		temp=buf[lenp];
		buf[lenp]=CHAR_TERM;
		if (textwidth(buf)>=lens)
		{
			buf[lenp]=temp;
			break;
		}
		buf[lenp]=temp;
	}
	if (lenp+lenop>=line)
		lenp=0;
	lens=lenp-2;

	temp=buffer[lenlp];
	buffer[lenlp]=CHAR_TERM;
	lenl=textwidth(buffer);
	buffer[lenlp]=temp;

	if (ofst>0)
		leno=lenl+1;
	
	temp=buffer[line];
	buffer[line]=CHAR_TERM;
	lena=textwidth(buffer);
	buffer[line]=temp;
	
	if (line>=len)
	{
		len=line;
		buffer[line-1]=CHAR_TERM;
		return ret;
	}
	if (phead->data==CHAR_LINE)
		buffer[line++]='<';

	if (line>=len)
	{
		len=line;
		buffer[line-1]=CHAR_TERM;
		return ret;
	}
	if (phead==charlne->down)
		buffer[line++]='^';

	if (line>=len)
	{
		len=line;
		buffer[line-1]=CHAR_TERM;
		return ret;
	}
	
	len=line;
	buffer[line]=CHAR_TERM;

	return ret;
}