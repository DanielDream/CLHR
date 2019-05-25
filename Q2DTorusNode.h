#ifndef Q2DTorusNode_H
#define Q2DTorusNode_H

#include<iostream>
#include<string>
#include<assert.h>
#include"Parameters.h"
#include "Q2DTorus.h"

using namespace std;
class Q2DTorus;

class Buffer{		// VC1 and VC2 reserve some buffers separately,they can also use sharebuffer.
public:
	bool linkused;   // in a physical circle ,check whether physical link is used , only one flit is allowed to pass the link in a circle
	int r1;
	int r2;
	int r3;
	void bufferMin(int chn , int n)	;	
	void bufferPlus (int chn ,int n);

	int c; // flow control buffer
	int s; // flow control special buffer

	//if buffer is safe
	int safe[4];

	int safe_count[4];

	Buffer()
	{
		linkused = false;
		r1 = 0;
		r2 = 0;
		r3 = 0;
		c = 0;
		s = 0;
		for (int i = 0;i < 4;i++)
		{
			safe[i] = 0;
			safe_count[i] = 0;
		}
	}
};


/**this class define the info of a flit in the torus node including nodeid, buffer**/
class NodeInfo{  
public:				
	int node;   	//nextnode is the node to be routed
	int channel;	// which is used : VC1 or VC2
	Buffer* buff;	// point to the buffer occupy ,such as bufferxneg......
	NodeInfo():node(-1){		
	}
};


class Q2DTorusNode{

public:
	//the buffer in the # prot of current router 
	Buffer* buffer[rtr_num_ports];

	//the # router, the current router connect to with the ## port 
	int id_next_router[rtr_num_ports];

	Q2DTorus* torus;

	//the buffer of the router connect with current router with the # prot 
	Buffer* buffer_next_router[rtr_num_ports];

	int nodeid; //the # router in the net
	int x;      //the cluster No.
	int y; 		//grp No. in the cluster
	int z;		//router No. in the grp

	//路由器一个时钟周期的吸收能力
	int limit;
	//设定路由器一个时钟周期吸收能力的函数
	void    set_router_limit(int limit);
	int     get_router_limit()
	{
		return limit;
	}
	void    minus_router_limit()
	{
		(this->limit)--;
		return;
	}
	
	void 	setCoordinate(int _nodeid ,int _x, int _y, int _z);
	void 	setbuffer(int VC1_buff, int VC2_buff, int VC3_buff ); //num is the share buffer number
	void 	setLinkBuffer(int s[rtr_num_ports][2]);
	void    setLinkBuffer(int _port_cur_router, int _id_next_router, int _port_next_router);
	void    setTorus(Q2DTorus * torus);	
	void 	bufferPlus(Buffer* buff, int chn , int n);	 
	void	bufferMin(Buffer* buff, int chn,int n);	  
	void 	clearBuffer();

	~Q2DTorusNode(){
		for (int i=0; i< rtr_num_ports; i++){
			delete  buffer[i];
		}
	}

};
#endif