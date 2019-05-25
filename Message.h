#ifndef  MESS_AGE
#define  MESS_AGE
#include<iostream>
#include<string>
#include <deque>
#include<assert.h>
#include"Q2DTorusNode.h"
#include"Parameters.h"

using namespace std;
class Buffer;
class NodeInfo;


/************************ Message info ******************************/
class Message{
public:
	// the length of the message measured by flits;
	int length ;      
	//The source and destination of the message
	int src;         
	int dst;
	//measured by circle ,the time limit of message waits for a channel 
	int timeout; 
	//when a message is generated ,it needs some time until transmitting,begintrans record this.
	int begintrans;
	//how many steps this message has walked;
	int step;  
	//check this message whether consumed or arrive at dst
	bool active; 
	//the ith flit now at routpath[i].node and take routpath[i].buffer 
	NodeInfo* routpath;
	//the total time a message  consumed
	int count;
	//if the tail of a message shifts , the physical link the message occupied should release.
	bool releaselink;  
	//the flag to identify the dimension of last hop, 0: source node; 1: x dimension; 2: y dimension
	int  dim;
	//the flag to identify the virtual channle of last hop, 0: source node; 1: vc1; 2: vc2....
	int  vc;

	//for minus-first routing
	int intm;
	int type;
	int minimal;
	int minusorplus;//record minus or plus of last forward

	//wait circle
	int wait_circle;

	//previous vc number
	int pre_vc_number;

	//MIN
	int routedlocallinkcount;
	int routedgloballinkcount;

	//if message is safe
	int safe;

	//misrouting or min
	int routing_style;

	//init port
	int init_r_num;
	//init src port
	int src_init_r_num;

	//mis one two
	int misstep;

	//injection rate
	float rate;

	//5步路由路径中的中间点
	int src_init_router;
	int mid_init_router_1;
	int mid_init_router_2;
	int dest_init_router;

	deque<int> messagesafe;
	vector<int> routerSequence;//record the whole routed path
	vector<int> pathreserved;//record the whole reserved path

	Message(){
		src = -1;
		dst = -1;
	}

	Message(int src1, int dst1, float rate){
		begintrans = PROCESSTIME;
		src = src1;
		dst = dst1;
		routpath = new NodeInfo[MESSLENGTH];
		assert(routpath);
		for(int i = 0 ; i < MESSLENGTH; i++){
			routpath[i].node = src;
			routpath[i].channel = 0;
			routpath[i].buff = NULL;
		}
		step = 0;
		active = true;
		length = MESSLENGTH;
		count = 0;
		releaselink = false;
		timeout = 0;
		dim=0;
		vc=0;

		wait_circle = 0;

		intm =-1;
		type = -1;
		minimal=1;

		routedlocallinkcount = 0;
		routedgloballinkcount = 0;

		safe = SAFE;

		routing_style = 0;

		init_r_num = -1;

		misstep = -1;

		this->rate = rate;

		src_init_router = -1;
		mid_init_router_1 = -1;
		mid_init_router_2 = -1;
		dest_init_router = -1;
	}

	~Message(){
		delete [] routpath;
	}

	void setLength(int n){
		length = n;
	}
};



#endif