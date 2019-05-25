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
	int cur_circle;                          //设定当前执行的时钟周期，为了统计平均时钟周期的吞吐量
	std::vector<int> circle_accept;               //统计每个时钟周期吞吐量，配上上面的变量使用
	
    Message* 	genMes(float rate);          //generate a message
	Message*    genMes(int id, int src, int dst, int size);
    void        forwardMes(Message&);        //forward a message
    //void      consumeMes(Message&);
	Event(Allrouting*);
	//第一步初始化circle_accept数组，方便后面利用下标统计,提供给client的接口
	void init_circle_array(int all_cir)
	{
		this->circle_accept = std::vector<int>(all_cir,0);
	}
};


















#endif