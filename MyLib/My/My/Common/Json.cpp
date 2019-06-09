#include "Json.h"
#include "mystring.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _malloc(p) malloc(p)
#define _free(p) free(p)

Json::Json() : _addnum(0), _addlen(0), _addhead(NULL) {
	_phead = nullptr;
}

void Json::echoV() {
	JsonValue* p = _phead;
	while (p) {
		printf("key: %s, value: %s\n", p->key, p->value);
		p = p->next;
	}
}

int Json::parseString(char* string, JsonValue** node, bool is_child) {
	if (node == nullptr) {
		node = &(this->_phead);
	}
	//printf("JSON字符串:%s, %d\n", string, strlen(string));
	int i = 0;
	this->_string = string;
	char* m = string;
	//printf("m start ptr: 5%d\n", m);
	while (*m++) {
		i++;
		//printf("start char: %c!\n", *m);
		if (*m == '}' && is_child) {
			//printf("break.\n");
			break;
		}
		PJsonSign key = { 0, nullptr, nullptr };
		int key_len = 0;
		while (*m == ' ') m++;
		if (*m == '\'' || *m == '"') {
			//printf("key sign\n");
			key.sign = *m;
			m++;
		}
		key.start = m;

		//printf("m ptr: %d\n", m);
		while (*m) {
			if (*m == key.sign) {
				key.end = m;
				while (*m++) {
					while (*m == ' ') m++;
					//printf("mx:%c\n", *m);
					if (*m != ':') {
						printf("语法错误3.\n");
						goto error;
					}
					break;
				}
				break;
			}
			if (key.sign == 0 && *m == ':') {
				key.end = m;
				break;
			}
			if (*m == ' ') {
				if (key.sign) {
					printf("语法错误1.\n");
					goto error;
				}
				key.end = m;
				while (*m == ' ') m++;
				//printf("x:%c\n", *m);
				if (*m != ':') {
					printf("语法错误2(%c, %d).\n", *m, m);
					goto error;
				}
				break;
			}
			key_len++;
			m++;
		}
		//printf("key start:%d, key end:%d\n", key.start, key.end);
		if (key.end <= key.start) {
			goto error;
		}

		JsonValue* p = (JsonValue*)_malloc(sizeof(JsonValue));
		p->key = copy_str(key.start, key.end);
		p->value = NULL;
		p->child = NULL;
		p->next = NULL;

		*node = p;
		node = &(p->next);

		m++;
		while (*m == ' ') m++;
		PJsonSign value = { 0, NULL, NULL };
		if (*m == '{') {
			//printf("%s->child\n", p->key);
			m += this->parseString(m, &(p->child), true);
			m++;
			//printf("!m:%c, !next:%c\n", *m, *(m+1));
			/*if (*m != '}') {
				printf("!m:%c\n", *m);
				goto error;
			}
			m++;*/
			//while (*m++ == ' ');
			//if (*m == ',') m++;
			//while (*m == '}') m++;
		}
		else {
			if (*m == '\'' || *m == '"') {
				value.sign = *m;
				m++;
			}
			value.start = m;
			if (value.sign == 0) {
				while (*m == ' ') m++;
				if (*m == '-') m++;
			}
			while (*m) {
				if (value.sign == *m) {
					if (*(m - 1) != '\\') {
						value.end = m;
						m++;
						while (*m == ' ') m++;
						//printf("c:%c\n", *m);
						if (*m != ',' && *m != '}') {
							printf("语法错误value1.\n");
							goto error;
						}
						break;
					}
				}
				if (value.sign == 0) {
					if (strneq(m, "true", 4)) {
						m += 3;
						value.end = m + 1;
					}
					else if (strneq(m, "false", 5)) {
						m += 4;
						value.end = m + 1;
					}
					else if (*m < '0' || *m > '9') {
						if (*m == ' ') {
							value.end = m;
							while (*m == ' ') m++;
							if (*m != ',' && *m != '}') {
								printf("语法错误value2.\n");
								goto error;
							}
							break;
						}
						else if (*m == ',' || *m == '}') {
							value.end = m;
							break;
						}
						else {
							if (*m != '.') {
								printf("语法错误value3(%c, %c)|%s.\n", *m, *(m - 1), m);
								goto error;
							}
						}
					}
				}
				m++;
			}
			//printf("value start:%c, value end:%d\n", *value.start, value.end);
			if (value.end > value.start) {
				//printf("in value, next char:%c!\n", *m);
				p->value = copy_str(value.start, value.end);
			}
		}
		//printf("loop:%c, next:%c!\n", *m, *(m + 1));
		if (*m == '}' && is_child) {
			break;
		}
	}
	//printf("i: %d, j: %d, m: %c\n", i, m - string, *m);
error:

	return m - string;
}

JsonValue*  Json::getJsonValue(char* key) const {
	JsonValue* p = _phead;
	while (p) {
		if (strcmp(p->key, key) == 0) {
			return p;
		}
		p = p->next;
	}
	return nullptr;
}

char* Json::value(char* key) {
	JsonValue* p = this->getJsonValue(key);
	return p ? p->value : nullptr;
}

char* Json::valueOfChild(char* key) {
	int dim = 0, len = 0;
	char* tmp = key;
	while (*tmp) {
		if (*tmp == '.') dim++;
		tmp++;
	}
	len = tmp - key;

	dim = 0, tmp = key;
	char** ptr = (char**)_malloc(sizeof(char*) * (dim + 1));
	while (*tmp) {
		if (*tmp == '.') {
			ptr[dim] = tmp;
			dim++;
		}
		tmp++;
	}
	ptr[dim] = tmp;
	for (int i = 0; i <= dim; i++) {
		//printf("dim(%d):%c\n", i, *(ptr[i]));
	}

	JsonValue* p = _phead;
	while (p) {
		if (strneq(p->key, key, ptr[0] - key)) {
			if (dim == 0) {
				//printf("p key:%s, key: %s\n", p->key, key);
				return p->value;
			}
			else {
				return this->_getChildValue(ptr, 1, dim, p->child);
			}
		}
		p = p->next;
	}
	_free(ptr);
	return NULL;
}

inline int Json::valueToInt(char* key) {
	char* value = this->value(key);
	return value ? atoi(this->value(key)) : 0;
}

inline float Json::valueToFloat(char* key) {
	return atof(this->value(key));
}

inline double Json::valueToDouble(char* key) {
	return atof(this->value(key));
}

char* Json::_getChildValue(char** ptr, int curr, int total, JsonValue* child) {
	if (child == NULL) {
		return NULL;
	}
	//printf("%c=%c, (%d), p key: %s\n", *(ptr[curr - 1] + 1), *(child->key), ptr[curr] - ptr[curr - 1] - 1, child->key);
	u_int key_len = strlen(child->key);
	u_int eq_len = ptr[curr] - ptr[curr - 1] - 1;
	if (eq_len < key_len) {
		eq_len = key_len;
	}

	//printf("next child: %s\n", child->next->key);
	char* value = NULL;
	JsonValue* p = child;
	while (p) {
		//printf("p key: %s\n", p->key);
		if (strneq(p->key, ptr[curr - 1] + 1, eq_len)) {
			//printf("is eq: %s\n", p->key);
			if (curr >= total) {
				//printf("return\n");
				return p->value;
			}
			else {
				//printf("zl\n");
				return this->_getChildValue(ptr, curr + 1, total, p->child);
			}
		}
		p = p->next;
	}
	//printf("value: %s\n", value);
	return NULL;
}

JsonNode* Json::createList(bool add) {
	JsonNode* p = (JsonNode*)_malloc(sizeof(JsonNode));
	p->key_addlen = 0;
	p->value_addlen = 0;
	p->value.child = NULL;
	p->type = JSON_VAL_LIST;
	p->next = NULL;

	return p;
}

JsonNode* Json::push(char* key, char* value, bool add) {
	JsonNode* p = (JsonNode*)_malloc(sizeof(JsonNode));
	p->key_addlen = strlen(key);
	p->value_addlen = strlen(value);
	p->key = key;
	p->value.str = value;
	p->type = JSON_VAL_STRING;
	p->next = NULL;

	if (add) {
		this->_add(p, 6); //为什么加6, 因为键值和值双引号为4个, 冒号占一个, 默认还多分配一个逗号
	}
	return p;
}

JsonNode* Json::push(char* key, int value, bool add) {
	char tmp[12];
	sprintf(tmp, "%d", value);
	JsonNode* p = (JsonNode*)_malloc(sizeof(JsonNode));
	p->key_addlen = strlen(key);
	p->value_addlen = strlen(tmp);
	p->key = key;
	p->value.inter = value;
	p->type = JSON_VAL_INTER;
	p->next = NULL;

	if (add) {
		this->_add(p, 4); //为什么加4, 因为键值双引号为2个, 整型值不加引号, 冒号占一个, 默认还多分配一个逗号
	}
	return p;
}

JsonNode* Json::push(char* key, double value, bool add) {
	char tmp[16];
	sprintf(tmp, "%.3f", value);
	JsonNode* p = (JsonNode*)_malloc(sizeof(JsonNode));
	p->key_addlen = strlen(key);
	p->value_addlen = strlen(tmp);
	p->key = key;
	p->value.decimal = value;
	p->type = JSON_VAL_DECIMAL;
	p->next = NULL;

	if (add) {
		this->_add(p, 4); //为什么加4, 因为键值双引号为2个, 小数不加引号, 冒号占一个, 默认还多分配一个逗号
	}
	return p;
}

JsonNode* Json::push(char* key, JsonNode* node, bool add) {
	JsonNode* p = (JsonNode*)_malloc(sizeof(JsonNode));
	p->key_addlen = strlen(key);
	p->value_addlen = node->value_addlen;
	p->key = key;
	p->value.child = node;
	p->type = JSON_VAL_NODE;
	p->next = NULL;

	//printf("lenxx: %d\n", node->type == 1 ? 6 : 4);
	u_int num = 4;
	if (node->type == 1) num = 6;
	if (node->type == 5) num = 2;
	if (add) {
		this->_add(p, num);
	}
	else {
		_addlen += num;
	}
	
	_addlen += node->key_addlen;
	_addlen += 5;
	return p;
}

JsonNode* Json::push(JsonNode* parent, JsonNode* child) {
	JsonNode** ptr;
	if (parent->type == JSON_VAL_LIST) {
		ptr = &(parent->value.child);
		//parent->value.child = child;
	}
	else {
		ptr = &(parent->next);
		//parent->next = child;
	}
	while (true) {
		if (*ptr == NULL) {
			break;
		}
		ptr = &(*ptr)->next;
	}

	*ptr = child;

	_addnum++;
	_addlen += child->key_addlen;
	_addlen += child->value_addlen;

	//printf("lenxx2: %d\n", child->type == 1 ? 6 : 4);
	u_int num = 4;
	if (child->type == 1) num = 6;
	if (child->type == 5) num = 2;
	_addlen += num;
	//_addlen += 2;
	return child;
}

JsonNode* Json::push(JsonNode* node) {
	u_int num = 4;
	if (node->type == JSON_VAL_STRING) num = 6;
	if (node->type == JSON_VAL_LIST) num = 2;
	this->_add(node, num);

	return NULL;
}

bool Json::_add(JsonNode* p, u_int num) {
	if (_addhead == NULL) {
		_addhead = p;
		_addcurr = _addhead;
	}
	else {
		_addcurr->next = p;
		_addcurr = p;
	}
	_addnum++;
	_addlen += p->key_addlen;
	_addlen += p->value_addlen;
	_addlen += num;

	return true;
}

char* Json::toJson() {
	_addlen += 2; //为什么加2, {}占两个, 字符串结尾一个[但是已经多分配了一个逗号]
	//printf("len:%d\n", _addlen);
	char* json_str = (char*)_malloc(_addlen);
	char* str = json_str;
	*str = '{';
	str = this->_toJson(str, _addhead);
	*str = '}';
	*(str + 1) = 0;

	//printf("len:%d, %d\n", _addlen, strlen(json_str));
	return json_str;
}

char* Json::_toJson(char* str, JsonNode* p) {
	while (p) {
		//printf("key: %s\n", p->key);
		//printf("value: %s\n", p->value.str);
		str++;
		if (p->key_addlen > 0) {
			*str = '"';
			str++;
			memcpy(str, p->key, p->key_addlen);
			str += p->key_addlen;
			*str = '"';
			*(str + 1) = ':';
			str += 2;
		}
		

		if (p->type == JSON_VAL_STRING) {
			*str = '"';
			str++;
			memcpy(str, p->value.str, p->value_addlen);
			str += p->value_addlen;
			*str = '"';
		}
		else if (p->type == JSON_VAL_INTER) {
			char tmp[12];
			//printf("value len: %d\n", p->value_addlen);
			sprintf(tmp, "%d", p->value.inter);
			memcpy(str, tmp, p->value_addlen);
			str += p->value_addlen - 1;
		}
		else if (p->type == JSON_VAL_DECIMAL) {
			char tmp[16];
			//printf("value len: %d\n", p->value_addlen);
			sprintf(tmp, "%.3f", p->value.decimal);
			memcpy(str, tmp, p->value_addlen);
			str += p->value_addlen - 1;
		}
		else if(p->type == JSON_VAL_NODE){
			if (p->value.child && p->value.child->type != JSON_VAL_LIST) {
				*str = '{';
			}
			if (p->value.child && p->value.child->type == JSON_VAL_LIST) {
				str--;
			}
			str = this->_toJson(str, p->value.child) - 1;
			if (p->value.child && p->value.child->type != JSON_VAL_LIST) {
				str++;
				*str = '}';
			}
		}
		else {
			if (p->value.child) {
				//printf("str2:%d\n", str);
				*str = '[';
				str++;
				*str = '{';
				str = this->_toJson(str, p->value.child);
				*str = '}';
				str++;
				*str = ']';
			}
		}

		str++;

		p = p->next;
		if (p) {
			*str = ',';
		}
	}

	return str;
}

void Json::clear(JsonValue* p) {
	if (p == nullptr) {
		p = _phead;
	}
	while (p) {
		JsonValue* dp = p;
		if (p->child) {
			this->clear(p->child);
		}
		if (p->key) {
			_free(p->key);
		}
		if (p->value) {
			_free(p->value);
		}

		//printf("free json: %p, %s\n", dp, dp->key);
		p = p->next;
		_free(dp);
		
	}
	_phead = nullptr;
}

void Json::clearAdd(JsonNode* p) {
	if (p == NULL) {
		p = _addhead;
	}
	while (p) {
		JsonNode* dp = p;
		if (p->type == JSON_VAL_NODE || p->type == JSON_VAL_LIST) {
			this->clearAdd(p->value.child);
		}

		p = p->next;
		_free(dp);
		//printf("free add json: %p\n", dp);
	}
	_addlen = 0;
	_addhead = NULL;
}