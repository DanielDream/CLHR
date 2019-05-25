#ifndef LINK_H
#define LINK_H

class Link{
public:
	int src,srcport;
	int dst,dstport;
	int linktype;//0:inner link; 1:group link; 2:cluster link

	Link(int src, int srcport, int dst, int dstport,int linktype){
		this->src = src;
		this->srcport = srcport;
		this->dst = dst;
		this->dstport = dstport;
		this->linktype = linktype;
	}

	Link(){
		this->src = -1;
		this->srcport = -1;
		this->dst = -1;
		this->dstport = -1;
		this->linktype = -1;
	}
};
#endif