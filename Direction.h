#ifndef DIRECTION_H
#define DIRECTION_H
#include"Q2DTorusNode.h"

class Direction{
public:

	Buffer *  buff;
	int       vc;
	int       nextnode;

	Direction(Buffer* buff,int vc,int nextnode){
		this->buff=buff;
		this->vc=vc;
		this->nextnode=nextnode;
	}

	Direction(){
		this->buff=NULL;
		this->vc=-1;
		this->nextnode=nextnode;
	}
};
#endif