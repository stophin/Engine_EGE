//CharLink.h
//
#ifndef _CHARLINK_H_
#define _CHARLINK_H_

#include <stdlib.h>

#define CHAR_HEAD	-1
#define CHAR_TERM	'\0'
#define CHAR_NULL	'\0'
#define CHAR_LINE	'\n'

typedef struct CharElement
{
	char data;

	struct CharElement * left;
	struct CharElement * right;
	struct CharElement * up;
	struct CharElement * down;
	struct CharElement * last;
}CharElement;
typedef struct CharElement* PCharElement;

typedef class CharLink
{
public:
	CharLink();
	~CharLink();

	PCharElement charpool;	//Must be an array
	int charpoollen;		//Array length

	PCharElement charcur;	//Cursor pointer
	PCharElement charsta;	//Link start
	PCharElement charlne;	//Current line
	PCharElement charlle;	//Last line, used to judge if line is changed

	int lines;	//Total length

	bool LineChanged();

	bool CharLinkInit();	//Initialization of link

	void SetCharPool(PCharElement pce,int len);
	void ClearCharPool();
	void ClearChar(PCharElement pce);
	PCharElement AllocCharPool();

	void DeleteChar();
	bool AddString(char *pc);
	bool AddChar(PCharElement e);
	bool AddChar(char c);

	int GetLine(int line,int& offset,char *buffer,int& len,int& lens,int& leno,int& lenl,int& lena);

	void CurMoveLeft();
	void CurMoveRight();
	void CurMoveUp();
	void CurMoveDown();
	void CurMoveHead();
	void CurMoveEnd();

	void LinkBackspace();
	void LinkDelete();
	

}CharLink,* PCharLink;

#endif	//end of _CHARLINK_H_
