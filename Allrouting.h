#ifndef ALLROUTING_H
#define ALLROUTING_H

#include "Q2Dtorus.h"
#include "Message.h"
//��Ϣ������
class Message;
//����
class Q2DTorus;
//·�����ڵ���Ϣ
class NodeInfo;
//·���㷨�Ļ���
class Allrouting{
public:
	NodeInfo* next;
	Q2DTorus* torus;
	int k;
	virtual NodeInfo*	forward(Message& s){return NULL;}; // implemented by child

};
#endif