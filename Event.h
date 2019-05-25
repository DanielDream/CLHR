#ifndef   EVEN_T
#define   EVEN_T

#include"Message.h"
#include"Allrouting.h"
#include"Q2DTorus.h"
#include"Q2DTorusNode.h"
#include "vector"

class Event{
public:
	int consumed;
	double totalcir;                         // total circle all the messages take;
	int messarrive;                          // how many message arrived
	Allrouting* rout;
	Q2DTorus* tor;
	int k;                                   // k torus
	int cur_circle;                          //�趨��ǰִ�е�ʱ�����ڣ�Ϊ��ͳ��ƽ��ʱ�����ڵ�������
	std::vector<int> circle_accept;               //ͳ��ÿ��ʱ����������������������ı���ʹ��
	
    Message* 	genMes(float rate);          //generate a message
	Message*    genMes(int id, int src, int dst, int size);
    void        forwardMes(Message&);        //forward a message
    //void      consumeMes(Message&);
	Event(Allrouting*);
	//��һ����ʼ��circle_accept���飬������������±�ͳ��,�ṩ��client�Ľӿ�
	void init_circle_array(int all_cir)
	{
		this->circle_accept = std::vector<int>(all_cir,0);
	}
};


















#endif