#ifndef Q2DTORUS_H
#define Q2DTORUS_H

#include <vector>
#include "Q2DTorusNode.h"
#include "Link.h"


class Q2DTorusNode;
class Link;

class Q2DTorus{
public:
	Q2DTorus(int VC1_buff, int VC2_buff,int VC3_buff);
	Q2DTorusNode* operator [](int n);
	void clearAll();
	~Q2DTorus();
	int getNode(){return k;}  
	void connect(int clusterA, int groupA, int routerA, int portA, int clusterB, int groupB, int routerB, int portB, int linktype);

	int k;
	std::vector<Link> **clusterLinks;
	Q2DTorusNode* head;
	Link **nodeLink;
	Link **groupLink;
	
};
#endif