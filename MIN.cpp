#include"MIN.h"
#include<assert.h>
#include<math.h>
#include<time.h>
#include<vector>
extern int NUM_of_VC;

NodeInfo* MIN::forward(Message &s){
	next->node = -1;
	next->buff = NULL;

	//torus is array access,not change
	linkrouter(s.src,s.dst,s.src_init_router,s.dest_init_router);
	return forward((*torus)[s.routpath[0].node], (*torus)[s.dst], s.src,s);
}

bool MIN::checkBuffer(Direction *dir){
	if (dir == NULL || dir->buff->linkused == true)
		return false;
	switch (dir->vc){
	case VC1:
		if (dir->buff->r1 >= MESSLENGTH)
			return true; //VC1 virtual channel is available
		return false;
	case VC2:
		if (dir->buff->r2 >= MESSLENGTH)
			return true; //VC2 virtual channel is available
		return false;
	case VC3:
		if (dir->buff->r3 >= MESSLENGTH)
			return true; //VC3 virtual channel is available
		return false;
	}
	return false;
}


int MIN::hierdragonfly_min(int rID, int source, int dest, Message &s){//基于最短路径的流控负向优先路由
	assert(rID != dest);
	int _grp_num_routers = gA;//组内路由器个数

	int out_port = -1;//组内连接输出端口

	int curcluster, curgroup, curid, curglobalgroup, curglobalid;
	int dstcluster, dstgroup, dstid, dstglobalgroup, dstglobalid;
	curcluster = CLUSTER(rID);
	curgroup = GROUP(rID);
	curglobalgroup = GLOBALGROUP(rID);
	curid = IDINGROUP(rID);
	curglobalid = rID;
	dstcluster = CLUSTER(dest);
	dstgroup = GROUP(dest);
	dstglobalgroup = GLOBALGROUP(dest);
	dstid = IDINGROUP(dest);
	dstglobalid = dest;

	//decide safe or unsafe: safe when not violating MFR of the whole path from cur to dst
	vector<int> pathtodecidesafe;
	if (rID == source && s.pathreserved.size() == 0){
		if (torus->nodeLink[curglobalid][dstglobalid].src != -1){
			pathtodecidesafe.push_back(curglobalid);
			pathtodecidesafe.push_back(dstglobalid);
			s.safe = SAFE;
		}
		else{
			if (torus->groupLink[curglobalgroup][dstglobalgroup].src != -1){//组间直接相连
				if (curglobalid != torus->groupLink[curglobalgroup][dstglobalgroup].src)
					pathtodecidesafe.push_back(curglobalid);
				pathtodecidesafe.push_back(torus->groupLink[curglobalgroup][dstglobalgroup].src);
				pathtodecidesafe.push_back(torus->groupLink[curglobalgroup][dstglobalgroup].dst);
				if (dstglobalid != torus->groupLink[curglobalgroup][dstglobalgroup].dst)
					pathtodecidesafe.push_back(dstglobalid);
			}
			else{//组间不直接相连
				int CLsrc = -1;//簇间连接起点
				int CLdst = -1;//簇间连接终点
				vector<Link> link;
				if (curcluster < dstcluster){
					for (vector<Link>::iterator it = torus->clusterLinks[curcluster][dstcluster].begin(); it != torus->clusterLinks[curcluster][dstcluster].end(); ++it){
						if (dstglobalgroup == GLOBALGROUP(it->dst)){//目的组存在簇间连接终点情况下的簇间连接起点
							link.push_back(*it);
						}
					}
				}
				else{
					for (vector<Link>::iterator it = torus->clusterLinks[curcluster][dstcluster].begin(); it != torus->clusterLinks[curcluster][dstcluster].end(); ++it){
						if (curglobalgroup == GLOBALGROUP(it->src)){//当前组存在的簇间连接起点
							link.push_back(*it);
						}
					}
				}
				int random = rand() % link.size();
				CLsrc = link[random].src;
				CLdst = link[random].dst;
				if (CLsrc == -1){
					int randindex = rand() % betweencluster_num_links;
					CLsrc = torus->clusterLinks[curcluster][dstcluster][randindex].src;//簇间连接起点
					CLdst = torus->clusterLinks[curcluster][dstcluster][randindex].dst;//簇间连接终点
				}
				int GLsrctoCLsrc_src, GLdsttoCLsrc_src;
				if (curglobalgroup != GLOBALGROUP(CLsrc)){
					GLsrctoCLsrc_src = torus->groupLink[curglobalgroup][GLOBALGROUP(CLsrc)].src;//与簇间连接起点所在组相连的当前组的结点
					GLdsttoCLsrc_src = torus->groupLink[curglobalgroup][GLOBALGROUP(CLsrc)].dst;
				}
				else{
					GLsrctoCLsrc_src = -1;
					GLdsttoCLsrc_src = -1;
				}
				int GLsrctoCLdst_dst, GLdsttoCLdst_dst;
				if (dstglobalgroup != GLOBALGROUP(CLdst)){
					GLsrctoCLdst_dst = torus->groupLink[dstglobalgroup][GLOBALGROUP(CLdst)].src;//与簇间连接终点所在组相连的目的组的结点
					GLdsttoCLdst_dst = torus->groupLink[dstglobalgroup][GLOBALGROUP(CLdst)].dst;
				}
				else{
					GLsrctoCLdst_dst = -1;
					GLdsttoCLdst_dst = -1;
				}
				if (curglobalid != GLsrctoCLsrc_src && curglobalid != CLsrc)
					pathtodecidesafe.push_back(curglobalid);
				if (GLsrctoCLsrc_src != -1)
					pathtodecidesafe.push_back(GLsrctoCLsrc_src);
				if (GLdsttoCLsrc_src != CLsrc && GLdsttoCLsrc_src != -1)
					pathtodecidesafe.push_back(GLdsttoCLsrc_src);
				pathtodecidesafe.push_back(CLsrc);
				pathtodecidesafe.push_back(CLdst);
				if (GLdsttoCLdst_dst != CLdst && GLdsttoCLdst_dst != -1)
					pathtodecidesafe.push_back(GLdsttoCLdst_dst);
				if (GLsrctoCLdst_dst != -1)
					pathtodecidesafe.push_back(GLsrctoCLdst_dst);
				if (dstglobalid != GLsrctoCLdst_dst && dstglobalid != CLdst)
					pathtodecidesafe.push_back(dstglobalid);
			}
			bool plusoccur = false;
			for (int i = 1; i < pathtodecidesafe.size(); i++)
			{
				if (plusoccur == true){
					if (pathtodecidesafe[i] - pathtodecidesafe[i - 1] < 0)
						s.safe = UNSAFE;
				}
				if (pathtodecidesafe[i] - pathtodecidesafe[i - 1] > 0){
					plusoccur = true;
				}
			}
		}
		for (int i = 0; i < pathtodecidesafe.size(); i++)
			s.pathreserved.push_back(pathtodecidesafe[i]);
	}
	else{
		bool startcopy = false;
		for (int i = 0; i < s.pathreserved.size(); i++){
			if (s.pathreserved[i] == rID){
				startcopy = true;
			}
			if (startcopy == true){
				pathtodecidesafe.push_back(s.pathreserved[i]);
			}
		}
		s.safe = SAFE;
		bool plusoccur = false;
		for (int i = 1; i < pathtodecidesafe.size(); i++)
		{
			if (plusoccur == true){
				if (pathtodecidesafe[i] - pathtodecidesafe[i - 1] < 0)
					s.safe = UNSAFE;
			}
			if (pathtodecidesafe[i] - pathtodecidesafe[i - 1] > 0){
				plusoccur = true;
			}
		}
	}

	//decide out_port(next node)
	//根据初始路径直接确定端口
	//assert(pathtodecidesafe.size() >= 2);
	int pathsize = pathtodecidesafe.size();
	if (pathsize > 6)
		printf("pathtodecidesafe.size() = %d", pathsize);
	int currentid = pathtodecidesafe[0];
	int nextid = pathtodecidesafe[1];
	assert(currentid == rID);
	out_port = torus->nodeLink[currentid][nextid].srcport;
	//assert(out_port != -1);
	if(out_port == -1)
		printf("out_port == -1");
	if ((*torus)[currentid]->id_next_router[out_port] != nextid)
		printf("(*torus)[%d]->id_next_router[%d] = %d\n", currentid, out_port,(*torus)[currentid]->id_next_router[out_port]);
	return out_port;
}

//packet output port based on the source, destination and current location
int MIN::dragonfly_min(int rID, int source, int dest){
	assert(rID != dest);
	
	int _grp_num_routers= gA;

	int out_port = -1;
	int grp_ID = rID / _grp_num_routers; 
	int dest_grp_ID = dest/_grp_num_routers;
	int grp_output=-1;
	int grp_RID=-1;

	//which router within this group the packet needs to go to
	if (dest_grp_ID == grp_ID) {
		grp_RID = dest;
	} 
	else {
		/*
		if (grp_ID > dest_grp_ID) {
			grp_output = grp_ID - dest_grp_ID - 1;
		} 
		else {
			grp_output = (gA*gP + 1)-(dest_grp_ID - grp_ID)-1;
		}
		grp_RID = grp_output /gP + grp_ID * _grp_num_routers;
		*/
		if (dest_grp_ID == (grp_ID - 1 + net_num_ports) % net_num_ports)
		{
			grp_output = 0;
			goto for_end;
		}
		if (dest_grp_ID == (grp_ID + 1) % net_num_ports)
		{
			grp_output = (net_num_ports - 1) - 1;
			goto for_end;
		}
		if (grp_ID == net_num_ports - 1)
		{
			grp_output = dest_grp_ID;
			goto for_end;
		}
		if (dest_grp_ID == net_num_ports - 1)
		{
			grp_output = grp_ID;
			goto for_end;
		}
		if (grp_ID > dest_grp_ID) 
		{
			grp_output = dest_grp_ID + 1;
		}
		else
		{
			grp_output = (net_num_ports - 1)-(dest_grp_ID - grp_ID);
		}
for_end:
		grp_RID = grp_output /gP + grp_ID * _grp_num_routers;
	}

	if(grp_RID == rID) {
		//At the optical link
		//calculate the route port number
		out_port = gA - 1 + grp_output % gP;
	} 
	else {
		//need to route within a group
		assert(grp_RID!=-1);

		if (rID < grp_RID){
			out_port = (grp_RID % _grp_num_routers) - 1;
		}
		else{
			out_port = (grp_RID % _grp_num_routers);
		}
	} 

	assert(out_port!=-1);
	return out_port;
}
//计算最短路径中两个路由器组相连接的两个路由器的ID.其中之一必定与src或者dest相等
void MIN::linkrouter(int src, int dest,int &src_router,int &dest_router)
{
	assert(src != dest);
	//组内路由器的数量
	int _grp_num_routers= gA;

	//源路由器所在的组标号
	int src_grp_ID = src / _grp_num_routers;
	//目的路由器所在的组标号
	int dest_grp_ID = dest / _grp_num_routers;

	//当前路由器所在组内路由器标号
	int src_rID = src % _grp_num_routers;
	//目的路由器所在组内路由器标号
	int dest_rID = dest % _grp_num_routers;

	//后续计算需要的临时变量
	int grp_output;
	int grp_dest_input;
	//后续计算需要的临时变量
	int src_intm_rID;
	int dest_intm_rID;

	if (src_grp_ID > dest_grp_ID) {
		if (src_grp_ID == net_num_ports - 1 && dest_grp_ID == 0)
		{
			grp_output = (net_num_ports - 1) - 1;
			grp_dest_input = 0;
			goto for_end_1;
		}
		else if (src_grp_ID == dest_grp_ID + 1)
		{
			grp_output = 0;
			grp_dest_input = (net_num_ports - 1) - 1;
			goto for_end_1;
		}
		else if (src_grp_ID == net_num_ports - 1)
		{
			grp_output = dest_grp_ID;
			grp_dest_input = dest_grp_ID;
			goto for_end_1;
		}
		else
		{
			grp_output = dest_grp_ID + 1;
			grp_dest_input = (net_num_ports - 1) - (src_grp_ID - dest_grp_ID);
			goto for_end_1;
		}

for_end_1:
		src_intm_rID = grp_output / gP;
		dest_intm_rID = grp_dest_input / gP;	
		src_router = src_grp_ID * _grp_num_routers + src_intm_rID;
		dest_router = dest_grp_ID * _grp_num_routers + dest_intm_rID;
	}
	if (src_grp_ID < dest_grp_ID)
	{
		if (src_grp_ID == 0 && dest_grp_ID == net_num_ports - 1)
		{
			grp_output = 0;
			grp_dest_input = (net_num_ports - 1) - 1;
			goto for_end_2;
		}
		else if (src_grp_ID == dest_grp_ID - 1)
		{
			grp_output = (net_num_ports - 1) - 1;
			grp_dest_input = 0;
			goto for_end_2;
		}
		else if (dest_grp_ID == net_num_ports - 1)
		{
			grp_output = src_grp_ID;
			grp_dest_input = src_grp_ID;
			goto for_end_2;
		}
		else
		{
			grp_output = (net_num_ports - 1) - (dest_grp_ID - src_grp_ID);
			grp_dest_input = src_grp_ID + 1;
			goto for_end_2;
		}

for_end_2:
		dest_intm_rID = grp_dest_input / gP ;
		src_intm_rID = grp_output / gP;
		src_router = src_grp_ID * _grp_num_routers + src_intm_rID;
		dest_router = dest_grp_ID * _grp_num_routers + dest_intm_rID;
	}
	return;
}
/***************************************************************************
Name :Forward()
Input: The current node,the destionation
Output:The next node to rout.
***************************************************************************/
NodeInfo*	MIN::forward(Q2DTorusNode* cur, Q2DTorusNode* dst,int src,Message &s){
	assert( cur && dst && (cur != dst));

	int vc = 1;

	//int cur_grp = cur->x;
	//int cur_rtr = cur->y;
	////actually gA is group router nums,src maybe is the global id
	//int src_grp = src / gA;
	//int src_rtr = src % gA;
	//int dst_grp = dst->x;
	//int dst_rtr = dst->y;

	/*int cur_cluster = cur->x;
	int cur_group = cur->y;
	int src_cluster = CLUSTER(src);
	int src_group = GROUP(src);
	int dst_cluster = dst->x;
	int dst_group = dst->y;*/

	
	/************************The Routing Function********************************/
	int out_port =-1; 
	out_port = hierdragonfly_min(cur->nodeid, src, dst->nodeid, s);
	if (NUM_of_VC == 2){//dragonfly
		//the routing algorithm
		//if (cur_grp == src_grp)  vc = 1;
		//if (cur_grp == dst_grp)  vc = 2;
		printf("error of VC number\n");
	}
	//cout<<"grp:"<<cur_grp<<"\t router:"<<cur_rtr<<"\tsrc grp:"<<src_grp<<"\t src router:"<<src_rtr<<"\tdst grp:"<<dst_grp<<"\t dst router:"<<dst_rtr<<"\t output:"<<out_port<<endl;
	//system("pause");
	if (NUM_of_VC == 3){//CLHR
		if (out_port < gA - 1){//local link
			vc = s.routedlocallinkcount + 1;
		}
		else{
			vc = s.routedgloballinkcount + 1;
		}
	}

	assert(vc <= 3);
	Direction* flagD = new Direction(cur->buffer_next_router[out_port],vc,cur->id_next_router[out_port]);
	if (checkBuffer(flagD)){
		next->buff = flagD->buff;
		next->channel = flagD->vc;
		next->node = flagD->nextnode;
		next->buff->linkused = true;
		next->buff->bufferMin(next->channel,MESSLENGTH);
		s.routerSequence.push_back(cur->nodeid);
		if (out_port < gA - 1){//local link
			s.routedlocallinkcount++;
		}
		else{
			s.routedgloballinkcount++;
		}

		////时钟周期+4
		//if(cur_grp == src_grp && out_port >= (gA - 1))
		//{
		//	s.count += 4;
		//}
	}

	delete flagD;
	return next;
}
