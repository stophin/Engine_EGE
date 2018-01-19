//StringMap.h
//

#ifndef _STRING_MAP_H_
#define _STRING_MAP_H_

#include "LinkList.h"

#define MAX_KEY_LEN 100
#define MAX_VALUE_LEN	
class StringElement {
public:
	StringElement(const char* str) : prev(NULL), next(NULL), str(NULL){
		int len = strlen(str);
		this->str = new char[len + 1];
		memcpy(this->str, str, len);
		this->str[len] = 0;
	}

	~StringElement() {
		if (this->str) {
			delete[] this->str;
		}
		this->str = NULL;
	}

	int compare(const char * str) {
		if (str == NULL) {
			return 1;
		}
		int ret = 0;
		int i;
		for (i = 0; this->str[i] && str[i]; i++) {
			if (this->str[i] > str[i]) {
				ret = 1;
				break;
			}
			else if (this->str[i] < str[i]) {
				ret = -1;
				break;
			}
		}
		if (ret == 0) {
			if (this->str[i] > str[i]) {
				ret = 1;
			}
			else if (this->str[i] < str[i]) {
				ret = -1;
			}
		}
		return ret;
	}

	char * str;

	INT link_uniqueID;

	INT uniqueID;
	StringElement * prev;
	StringElement * next;
};

class StringMap {
public:
	StringMap() {
	}

	~StringMap() {
	}

	void put(const char * key, const char * value) {
		if (NULL != get(key)) {
			return;
		}
		StringElement* _key = new StringElement(key);
		StringElement* _value = new StringElement(value);

		//link
		_key->uniqueID = this->key.linkcount + 1;
		_value->uniqueID = this->value.linkcount + 1;
		_key->link_uniqueID = _value->uniqueID;
		_value->link_uniqueID = _key->uniqueID;

		this->key.insertLink(_key);
		this->value.insertLink(_value);
	}

	void remove(const char * key) {
		StringElement * _key = get(key);
		if (_key) {
			this->key.removeLink(_key);
		}
	}

	StringElement * get(const char * key) {
		StringElement * _key = this->key.link;
		if (_key) {
			do {
				if (_key->compare(key) == 0){
					return _key;
				}

				_key = _key->next;
			} while (_key && _key != this->key.link);
		}
		return NULL;
	}

	char * getValue(const char * key) {
		StringElement * _key = this->get(key);
		if (NULL == _key) {
			return NULL;
		}
		StringElement * _value = this->value.link;
		if (_value) {
			do {
				if (_value->uniqueID == _key->link_uniqueID) {
					return _value->str;
				}

				_value = _value->next;
			} while (_value && _value != this->value.link);
		}
		return NULL;
		
	}

private:
	LinkList<StringElement> key;
	LinkList<StringElement> value;
};



#endif