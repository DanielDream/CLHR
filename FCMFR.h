#ifndef  FCMFR_ING_H
#define FCMFR_ING_H

#include "Q2DTorusNode.h"
#include "Message.h"
#include "Allrouting.h"
#include"Direction.h"
//#include<vector>

class Q2DTorus;
class Message;
//1条vc避免死锁的路由算法
class FCMFR :public Allrouting
{
public:
	FCMFR(Q2DTorus* torus1)
	{
		torus = torus1;
		k = torus1->getNode();
		next = new NodeInfo();
	}
	~FCMFR()
	{
		delete next;
	}
	NodeInfo*		forward(Message &s);
	NodeInfo*		forward(Q2DTorusNode* cur, Q2DTorusNode* dst, int src, Message &s);
	int				hierdragonfly_fcmfr(int rID, int source, int dest, Message &s);
	int	inline		dragonfly_min(int rID, int src, int dst);
	int	inline		dragonfly_minus(int rID, int src, int dst, Message &s, Q2DTorusNode* cur);
	int inline      dragonfly_intm_grp(int rID, int source, int dest, Message &s, Q2DTorusNode* cur);
	bool inline		checkBuffer(Direction *dir);
	//bool inline		checkBuffer(Buffer *buf);
	int inline		checkBuffer_int(Direction *dir);
	bool inline     checkBufferEmpty(Direction *dir);
	Direction*		Select(vector<Direction *> all_poss_dir);
	bool inline		checkSafe(Link link);

};
#endif