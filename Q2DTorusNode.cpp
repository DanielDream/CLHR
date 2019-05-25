#include "Q2DTorusNode.h"

//fill the buffer from vc1 to vc3,chn represent which vc & n represent size
void Buffer::bufferMin(int chn , int n) {   //share buffers are used first,when no share buffers ,		
	assert( chn == VC1 || chn == VC2 ||chn == VC3);		
	if(VC1 == chn)							//	the separate buffers are used.So first we check the	separate buffers whether used.														
		r1 -= n;													
	else if (VC2 == chn)
		r2 -= n;
	else
		r3 -= n;
}
//release the buffer from vc1 to vc3,chn represent which vc & n represent size
void Buffer::bufferPlus (int chn ,int n) {   
	assert( chn == VC1 || chn == VC2 ||chn == VC3);		
	if(chn ==VC1)
		r1 += n;																		
	else if (chn ==VC2)			
		r2 += n;	
	else
		r3 += n;
}


void Q2DTorusNode::setCoordinate(int _nodeid ,int _x, int _y, int _z){
	nodeid = _nodeid;
	x = _x;
	y = _y;
	z = _z;
}


void Q2DTorusNode::setbuffer( int VC1_buff, int VC2_buff, int VC3_buff){  
	
	for (int i = 0; i< rtr_num_ports; i++){
		buffer[i] = new Buffer();	
	}

	clearBuffer();
	
	for (int i = 0; i< rtr_num_ports; i++){
		buffer[i]->r1  = VC1_buff;
		buffer[i]->r2  = VC2_buff;
		buffer[i]->r3  = VC3_buff;
		buffer[i]->s  = 0;
	}
}
//next router id & next port in router?not sure
void Q2DTorusNode::setLinkBuffer(int _id_next_router[rtr_num_ports][2]){

	for (int i = 0; i< rtr_num_ports; i++){
		if (_id_next_router[i][0] != -1)
		{
		    buffer_next_router[i] = (*torus)[_id_next_router[i][0]]->buffer[_id_next_router[i][1]];
		    id_next_router[i] = _id_next_router[i][0];
		}
	}
}


void Q2DTorusNode::setLinkBuffer(int _port_cur_router, int _id_next_router, int _port_next_router){
	if (_id_next_router != -1)
	{
		buffer_next_router[_port_cur_router] = (*torus)[_id_next_router]->buffer[_port_next_router];
		id_next_router[_port_cur_router] = _id_next_router;
	}
}

//设定吸收能力
void Q2DTorusNode::set_router_limit(int limit)
{
	this->limit = limit;
}

void Q2DTorusNode::bufferPlus(Buffer* buff, int chn , int n){
	buff->bufferPlus(chn,n);
}

void Q2DTorusNode::bufferMin(Buffer* buff, int chn,int n){
	buff->bufferMin(chn,n);
}

void Q2DTorusNode::clearBuffer(){
	for (int i = 0; i< rtr_num_ports; i++){
		buffer[i]->linkused  = false;
	}
}

void    Q2DTorusNode::setTorus(Q2DTorus * torus){
	this->torus = torus;
}