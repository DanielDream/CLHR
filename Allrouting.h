#ifndef ALLROUTING_H
#define ALLROUTING_H

#include "Q2Dtorus.h"
#include "Message.h"
//消息的声明
class Message;
//网络
class Q2DTorus;
//路由器节点信息
class NodeInfo;
//路由算法的基类
class Allrouting{
public:
	NodeInfo* next;
	Q2DTorus* torus;
	int k;
	virtual NodeInfo*	forward(Message& s){return NULL;}; // implemented by child

};
#endif