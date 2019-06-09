#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define JSON_VAL_STRING  1
#define JSON_VAL_INTER   2
#define JSON_VAL_DECIMAL 3
#define JSON_VAL_NODE    4
#define JSON_VAL_LIST    5

typedef unsigned int u_int;

typedef struct p_json_sign {
	char  sign;
	char* start;
	char* end;
} PJsonSign;

typedef struct json_value {
	char* key;
	char* value;
	json_value* child;
	json_value* next;
} JsonValue;

typedef struct json_node {
	char* key;               //键值
	u_int key_addlen;           //键值长度
	union {
		char*      str;      //字符串
		int        inter;    //整数
		double     decimal;  //小数
		json_node* child; //下一个链表
	} value;                 //值
	u_int value_addlen;         //值长度
	u_int type;              //1-字符串, 2-整型, 3-小数, 4-此json链表, 5-列表
	json_node* next;         //下一个链表
} JsonNode;


class Json {
private:
	char*      _string;
	JsonValue* _phead;
	u_int      _addnum;
	u_int      _addlen;    //转成json要分配的长度
	JsonNode*  _addhead;   //链表头
	JsonNode*  _addcurr;   //当前
private:
	char *_getChildValue(char** ptr, int curr, int total, JsonValue* child);
	bool  _add(JsonNode* p, u_int num); //添加链表, num[要分配的长度]
	char* _toJson(char* str, JsonNode* node); //转成json
public:
	Json();
	int parseString(char* string, JsonValue** node = nullptr, bool is_child = false); //解析字符串
	void echoV();
	JsonValue*  getJsonValue(char* key) const;  //根据键值获取JsonValue, 需先调用parseString解析字符串
	char*  valueOfChild(char* key);
	char*  value(char* key);             //根据键值获取值, 需先调用parseString解析字符串
	int valueToInt(char* key);
	float valueToFloat(char* key);
	double valueToDouble(char* key);
	JsonNode* createList(bool add = false); //创建一个json列表
	JsonNode* push(char* key, char* value, bool add = true); //添加json对
	JsonNode* push(char* key, int value, bool add = true); //同上
	JsonNode* push(char* key, double value, bool add = true); //同上
	JsonNode* push(char* key, JsonNode* node, bool add = true); //同上
	JsonNode* push(JsonNode* parent, JsonNode* child); //同上
	JsonNode* push(JsonNode* node); //同上
	char* toJson(); //转成json
	void clear(JsonValue* p = nullptr); //清理json, 转成json后需调用此函数
	void clearAdd(JsonNode* p = nullptr); //清理json, 转成json后需调用此函数
};