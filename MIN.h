#ifndef  MIN_ING_H
#define MIN_ING_H

#include "Q2DTorusNode.h"
#include "Message.h"
#include "Allrouting.h"
#include"Direction.h"
#include<vector>
class Q2DTorus;
class Message;
// Dally MIN
class MIN:public Allrouting
{
public:
	MIN(Q2DTorus* torus1)
	{
		torus = torus1;
		k = torus1->getNode();
		next = new NodeInfo();
	}
	~MIN() 
	{
		delete next;
	}
	NodeInfo*		forward(Message &s);	
	NodeInfo*		forward(Q2DTorusNode* cur, Q2DTorusNode* dst,int src,Message &s);
	int	inline		dragonfly_min(int rID, int src, int dst);	
	int				hierdragonfly_min(int rID, int source, int dest, Message &s);
	bool inline		checkBuffer(Direction *dir);
	Direction*		Select(vector<Direction *> all_poss_dir);
	void            linkrouter(int src, int dest,int &src_router,int &dest_router);
};														  
#endif