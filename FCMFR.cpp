#include"FCMFR.h"
#include<assert.h>
#include<math.h>
#include<time.h>
//#include<vector>

extern int NUM_of_VC;

NodeInfo* FCMFR::forward(Message &s){
	next->node = -1;
	next->buff = NULL;

	return forward((*torus)[s.routpath[0].node], (*torus)[s.dst], s.src, s);
}

bool FCMFR::checkBuffer(Direction *dir){
	if (dir == NULL || dir->buff->linkused == true) return false;
	switch (dir->vc){
	case VC1:  if (dir->buff->r1 >= MESSLENGTH)  return true; //VC1 virtual channel is available
		return false;
	case VC2:  if (dir->buff->r2 >= MESSLENGTH)  return true; //VC2 virtual channel is available
		return false;
	case VC3:  if (dir->buff->r3 >= MESSLENGTH)  return true; //VC3 virtual channel is available
		return false;
	}
	return false;
}

//bool FCMFR::checkBuffer(Buffer *buf){
//	if (buf == NULL || buf->linkused == true) return false;
//	if (buf->r1 >= MESSLENGTH || buf->r2 >= MESSLENGTH || buf->r3 >= MESSLENGTH)
//		return true; 
//	return false;
//}

bool FCMFR::checkBufferEmpty(Direction *dir){
	if (dir == NULL) return false;
	switch (dir->vc){
	case VC1:  if (dir->buff->r1 >= MESSLENGTH)  return true; //VC1 virtual channel is available
		return false;
	case VC2:  if (dir->buff->r2 >= MESSLENGTH)  return true; //VC2 virtual channel is available
		return false;
	case VC3:  if (dir->buff->r3 >= MESSLENGTH)  return true; //VC3 virtual channel is available
		return false;
	}
	return false;
}

int FCMFR::checkBuffer_int(Direction *dir) {
	if (dir == NULL || dir->buff->linkused == true) return 0;
	switch (dir->vc){
	case VC1:  if (dir->buff->r1 >= MESSLENGTH)  return 1; //VC1 virtual channel is available
		return 0;
	case VC2:  if (dir->buff->r2 >= MESSLENGTH)  return 2; //VC2 virtual channel is available
		return 0;
	case VC3:  if (dir->buff->r3 >= MESSLENGTH)  return 3; //VC3 virtual channel is available
		return 0;
	}
	return 0;
}

bool FCMFR::checkSafe(Link link){
	vector<Direction*> all_poss_dir;
	int emptycount = 0;
	int unsafecount = 0;
	
	all_poss_dir.push_back(new Direction((*torus)[link.src]->buffer_next_router[link.srcport], 1, (*torus)[link.src]->id_next_router[link.srcport]));
	all_poss_dir.push_back(new Direction((*torus)[link.src]->buffer_next_router[link.srcport], 2, (*torus)[link.src]->id_next_router[link.srcport]));
	if (NUM_of_VC == 3)
		all_poss_dir.push_back(new Direction((*torus)[link.src]->buffer_next_router[link.srcport], 3, (*torus)[link.src]->id_next_router[link.srcport]));
	for (vector<Direction *>::iterator it = all_poss_dir.begin(); it != all_poss_dir.end(); ++it){
		if (checkBufferEmpty(*it))
			emptycount++;
		else
			if ((*it)->buff->safe[(*it)->vc] == UNSAFE)	unsafecount++;
	}
	if (NUM_of_VC == 2){
		if (emptycount == 0 || (emptycount == 1 && unsafecount == 1)){
			/*******************************Delete**********************************/
			for (vector<Direction *>::iterator it = all_poss_dir.begin(); it != all_poss_dir.end(); ++it){
				delete  (*it);
				(*it) = NULL;
			}
			return false;
		}
	}
	else if (NUM_of_VC == 3){
		if (emptycount == 0 || (emptycount == 1 && unsafecount == 2)){
			/*******************************Delete**********************************/
			for (vector<Direction *>::iterator it = all_poss_dir.begin(); it != all_poss_dir.end(); ++it){
				delete  (*it);
			}
			return false;
		}
	}
	/*******************************Delete**********************************/
	for (vector<Direction *>::iterator it = all_poss_dir.begin(); it != all_poss_dir.end(); ++it){
		delete  (*it);
		(*it) = NULL;
	}
	return true;
}

/***************************************************************************
Name :Select()
Input: The vector of all available directions
Output:The selected direction
***************************************************************************/
Direction* FCMFR::Select(vector<Direction *> all_poss_dir){
	vector<Direction *>  all_avail_dir;       //All available directions can be select
	Direction *		     last_dir = NULL;       //The last direction we decide.

	for (vector<Direction *>::iterator it = all_poss_dir.begin(); it != all_poss_dir.end(); ++it){
		if (checkBuffer(*it)) all_avail_dir.push_back(*it);
	}

	int      size = all_avail_dir.size();
	if (size) last_dir = all_avail_dir[rand() % size];
	return   last_dir;
}

//给定当前结点、源和目的结点
int FCMFR::hierdragonfly_fcmfr(int rID, int source, int dest, Message &s){//基于最短路径的流控负向优先路由
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
				vector<Link> all_avail_link;
				for (vector<Link>::iterator it = link.begin(); it != link.end(); ++it){
					if (checkSafe(*it)) 
						all_avail_link.push_back(*it);
				}
				if (all_avail_link.size() == 0){
					int random = rand() % link.size();
					CLsrc = link[random].src;
					CLdst = link[random].dst;
				}
				else{
					int random = rand() % all_avail_link.size();
					CLsrc = all_avail_link[random].src;
					CLdst = all_avail_link[random].dst;
				}
				
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
	if (pathtodecidesafe.size() < 2)
		printf("???");
	int currentid = pathtodecidesafe[0];
	int nextid = pathtodecidesafe[1];
	assert(currentid == rID);
	out_port = torus->nodeLink[currentid][nextid].srcport;
	if ((*torus)[currentid]->id_next_router[out_port] != nextid)
		printf("%d\n",(*torus)[currentid]->id_next_router[out_port]);

	//if (torus->nodeLink[curglobalid][dstglobalid].src != -1){
	//	out_port = torus->nodeLink[curglobalid][dstglobalid].srcport;
	//}
	//else if (curglobalgroup == dstglobalgroup){//同组
	//	if (curglobalid > dstglobalid)
	//		out_port = dstid;
	//	else
	//		out_port = dstid - 1;
	//}
	//else if (torus->groupLink[curglobalgroup][dstglobalgroup].src != -1){//组间直接相连
	//	if (curglobalid == torus->groupLink[curglobalgroup][dstglobalgroup].src)//组间连接点在当前点
	//		out_port = torus->groupLink[curglobalgroup][dstglobalgroup].srcport;
	//	else{//组间连接点不在当前点，则需要先转发到组间连接点起点，组内转发规则：组内标号大->组内标号小时，端口号=小的组内标号；组内标号小->组内标号大时，端口号=大的组内标号-1
	//		if (curglobalid > torus->groupLink[curglobalgroup][dstglobalgroup].src)
	//			out_port = IDINGROUP(torus->groupLink[curglobalgroup][dstglobalgroup].src);
	//		else
	//			out_port = IDINGROUP(torus->groupLink[curglobalgroup][dstglobalgroup].src) - 1;
	//	}
	//}
	//else{//组间无连接，此时只能走簇间连接，需要先走到与簇间连接起点所在组相连的当前组的结点
	//	int tempid = -1, tempport;//簇间连接起点
	//	assert(curcluster != dstcluster);
	//	for (vector<Link>::iterator it = torus->clusterLinks[curcluster][dstcluster].begin(); it != torus->clusterLinks[curcluster][dstcluster].end(); ++it){
	//		if (curglobalgroup == GLOBALGROUP(it->src)){
	//			tempid = it->src;//当前组存在的簇间连接起点
	//			tempport = it->srcport;
	//			break;
	//		}
	//	}
	//	if (tempid == -1){
	//		for (vector<Link>::iterator it = torus->clusterLinks[curcluster][dstcluster].begin(); it != torus->clusterLinks[curcluster][dstcluster].end(); ++it){
	//			if (dstglobalgroup == GLOBALGROUP(it->dst)){//目的组存在簇间连接终点的情况
	//				tempid = it->src;
	//				tempport = it->srcport;
	//				break;
	//			}
	//		}
	//	}
	//	if (tempid == -1){
	//		int randindex = rand() % 8;
	//		tempid = torus->clusterLinks[curcluster][dstcluster][randindex].src;//簇间连接起点
	//		tempport = torus->clusterLinks[curcluster][dstcluster][randindex].srcport;
	//	}
	//	if (curglobalgroup != GLOBALGROUP(tempid)){
	//		int tempid2 = torus->groupLink[curglobalgroup][GLOBALGROUP(tempid)].src;//与簇间连接起点所在组相连的当前组的结点
	//		if (curglobalid == tempid2)
	//			out_port = torus->groupLink[curglobalgroup][GLOBALGROUP(tempid)].srcport;
	//		else{
	//			if (curglobalid > tempid2)
	//				out_port = IDINGROUP(tempid2);
	//			else
	//				out_port = IDINGROUP(tempid2) - 1;
	//		}
	//	}
	//	else{
	//		if (curglobalid == tempid)
	//			out_port = tempport;
	//		else{
	//			if (curglobalid > tempid)
	//				out_port = IDINGROUP(tempid);
	//			else
	//				out_port = IDINGROUP(tempid) - 1;
	//		}
	//	}
	//}
	assert(out_port != -1);
	return out_port;
}

//packet output port based on the source, destination and current location
int FCMFR::dragonfly_min(int rID, int source, int dest){
	assert(rID != dest);

	int _grp_num_routers = gA;

	int out_port = -1;
	int grp_ID = rID / _grp_num_routers;
	int dest_grp_ID = dest / _grp_num_routers;
	int grp_output = -1;
	int grp_RID = -1;

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
			grp_output = (net_num_ports - 1) - (dest_grp_ID - grp_ID);
		}
	for_end:
		grp_RID = grp_output / gP + grp_ID * _grp_num_routers;
	}

	if (grp_RID == rID) {
		//At the optical link
		//calculate the route port number
		out_port = gA - 1 + grp_output % gP;
	}
	else {
		//need to route within a group
		assert(grp_RID != -1);

		if (rID < grp_RID){
			out_port = (grp_RID % _grp_num_routers) - 1;
		}
		else{
			out_port = (grp_RID % _grp_num_routers);
		}
	}

	assert(out_port != -1);
	return out_port;
}

//packet output port based on the source, destination and current location
int FCMFR::dragonfly_intm_grp(int rID, int source, int dest, Message &s, Q2DTorusNode* cur){
	assert(rID != dest);

	int _grp_num_routers = gA;

	int grp_ID = rID / _grp_num_routers;
	int rtr_ID = rID % _grp_num_routers;

	int dest_grp_ID = dest / _grp_num_routers;
	int dest_rtr_ID = dest % _grp_num_routers;

	int src_intm_rID = -1;
	int dest_intm_rID = -1;

	int src_intm_port = -1;
	int intm_intm_port = -1;

	int intm = -1;
	int delta = -1;

	int temp_outport = -1;
	int temp_queue = -1;
	int min = 1000000;
	int intm_grp = -1;

	if (grp_ID > dest_grp_ID) {

		//if the message can not directly go to the destination group, do need intermedia group					    
		//choose the intermedia group only when the 1st time  					
		delta = (rtr_ID + 1) *gP;
		int i = 1;
		do{
			intm = grp_ID - i;
			src_intm_rID = (int)((grp_ID - intm - 1) / gP);
			dest_intm_rID = _grp_num_routers - 1 - src_intm_rID;

			if (src_intm_rID == rtr_ID){
				src_intm_port = grp_ID - intm - 1;
				intm_intm_port = (gA*gP + 1) - src_intm_port - 2;

				//global port
				temp_outport = (2 * gP - 1) + src_intm_port % gP;
				temp_queue = cur->buffer[temp_outport]->r1 + cur->buffer[temp_outport]->r2 + cur->buffer[temp_outport]->r3;
				if (min > temp_queue){
					min = temp_queue;
					intm_grp = intm;
				}
			}
			else if (src_intm_rID < rtr_ID){
				//local port
				temp_outport = src_intm_rID;
				temp_queue = cur->buffer[temp_outport]->r1 + cur->buffer[temp_outport]->r2 + cur->buffer[temp_outport]->r3;
				if (min > temp_queue){
					min = temp_queue;
					intm_grp = intm;
				}
			}
			else{
				//local port
				temp_outport = src_intm_rID - 1;
				temp_queue = cur->buffer[temp_outport]->r1 + cur->buffer[temp_outport]->r2 + cur->buffer[temp_outport]->r3;
				if (min > temp_queue){
					min = temp_queue;
					intm_grp = intm;
				}
			}
			i++;
		} while (i <= delta);
	}
	if (grp_ID < dest_grp_ID) {

		delta = (dest_rtr_ID + 1)*gP;
		int i = 1;

		do{
			intm = dest_grp_ID - i;
			dest_intm_rID = (int)((intm - grp_ID - 1) / gP);
			src_intm_rID = _grp_num_routers - 1 - dest_intm_rID;

			if (src_intm_rID == rtr_ID){
				intm_intm_port = intm - grp_ID - 1;
				src_intm_port = (gA*gP + 1) - intm_intm_port - 2;

				//global port
				temp_outport = (2 * gP - 1) + src_intm_port % gP;
				temp_queue = cur->buffer[temp_outport]->r1 + cur->buffer[temp_outport]->r2 + cur->buffer[temp_outport]->r3;
				if (min > temp_queue){
					min = temp_queue;
					intm_grp = intm;
				}
			}
			else if (src_intm_rID < rtr_ID){
				//local port
				temp_outport = src_intm_rID;
				temp_queue = cur->buffer[temp_outport]->r1 + cur->buffer[temp_outport]->r2 + cur->buffer[temp_outport]->r3;
				if (min > temp_queue){
					min = temp_queue;
					intm_grp = intm;
				}
			}
			else{
				//local port
				temp_outport = src_intm_rID - 1;
				temp_queue = cur->buffer[temp_outport]->r1 + cur->buffer[temp_outport]->r2 + cur->buffer[temp_outport]->r3;
				if (min > temp_queue){
					min = temp_queue;
					intm_grp = intm;
				}
			}
			i++;

		} while (i <= delta);
	}
	return intm_grp;
}

/************************************************************************/
/* output: output_port?                                                 */
/************************************************************************/
int FCMFR::dragonfly_minus(int rID, int src, int dest, Message &s, Q2DTorusNode* cur){
	assert(rID != dest);

	int _grp_num_routers = gA;
	int out_port = -1;

	int grp_ID = rID / _grp_num_routers;
	int src_grp_ID = src / _grp_num_routers;
	int dest_grp_ID = dest / _grp_num_routers;

	int cur_rID = rID % _grp_num_routers;
	int dest_rID = dest % _grp_num_routers;

	int grp_output;
	int grp_dest_input;

	int src_intm_rID;
	int dest_intm_rID;
	int intm_intm_rID;

	//do not need misrouting, equal to the minimal routing
	out_port = dragonfly_min(rID, src, dest);
	//system("pause");

	//else  
	//misrouting
	if (rID == src && grp_ID > dest_grp_ID) {
		if (grp_ID == net_num_ports - 1 && dest_grp_ID == 0)
		{
			grp_output = (net_num_ports - 1) - 1;
			grp_dest_input = 0;
			goto for_end_1;
		}
		else if (grp_ID == dest_grp_ID + 1)
		{
			grp_output = 0;
			grp_dest_input = (net_num_ports - 1) - 1;
			goto for_end_1;
		}
		else if (grp_ID == net_num_ports - 1)
		{
			grp_output = dest_grp_ID;
			grp_dest_input = dest_grp_ID;
			goto for_end_1;
		}
		else
		{
			grp_output = dest_grp_ID + 1;
			grp_dest_input = (net_num_ports - 1) - (grp_ID - dest_grp_ID);
			goto for_end_1;
		}

	for_end_1:
		src_intm_rID = grp_output / gP;
		dest_intm_rID = grp_dest_input / gP;
		if (cur_rID < src_intm_rID){
			s.type = 1;
			srand((unsigned)time(NULL));
			s.intm = grp_ID - 1;//(rand()%(( src % gP + 1)*gP) + 1);
			//s.intm = dragonfly_intm_grp(rID, src, dest, s, cur);
			//cout<<"misrouting1"<<" grp_id:"<<grp_ID<<" destid:"<<dest_grp_ID<<" src_intm_rID:"<<src_intm_rID<<" cur_rID:"<<cur_rID<<endl;
			//system("pause");
		}
	}
	/*
	if (rID == src && grp_ID > dest_grp_ID) {
	src_intm_rID = (grp_ID - dest_grp_ID - 1) / gP;
	dest_intm_rID = _grp_num_routers -1 - src_intm_rID;
	if ( cur_rID < src_intm_rID){
	s.type = 1 ;
	srand((unsigned)time(NULL));
	s.intm = grp_ID - 1;//(rand()%(( src % gP + 1)*gP) + 1);
	//s.intm = dragonfly_intm_grp(rID, src, dest, s, cur);
	}
	}
	*/
	// misrouting
	if (rID == src && grp_ID < dest_grp_ID){
		if (grp_ID == 0 && dest_grp_ID == net_num_ports - 1)
		{
			grp_output = 0;
			grp_dest_input = (net_num_ports - 1) - 1;
			goto for_end_2;
		}
		else if (grp_ID == dest_grp_ID - 1)
		{
			grp_output = (net_num_ports - 1) - 1;
			grp_dest_input = 0;
			goto for_end_2;
		}
		else if (dest_grp_ID == net_num_ports - 1)
		{
			grp_output = grp_ID;
			grp_dest_input = grp_ID;
			goto for_end_2;
		}
		else
		{
			grp_output = (net_num_ports - 1) - (dest_grp_ID - grp_ID);
			grp_dest_input = grp_ID + 1;
			goto for_end_2;
		}

	for_end_2:
		dest_intm_rID = grp_dest_input / gP;
		src_intm_rID = grp_output / gP;
		if (dest_rID < dest_intm_rID){
			s.type = 2;
			srand((unsigned)time(NULL));
			s.intm = dest_grp_ID - 1;//(rand()%(( dest%gP + 1)*gP) +1);
			//s.intm = dragonfly_intm_grp(rID, src, dest, s, cur);
			//cout<<"misrouting2"<<" grp_id:"<<grp_ID<<" destid:"<<dest_grp_ID<<" dest_intm_rID:"<<dest_intm_rID<<" dest_rID:"<<dest_rID<<endl;
			//system("pause");
		}
	}
	/*
	if (rID == src && grp_ID < dest_grp_ID){
	dest_intm_rID = (dest_grp_ID - grp_ID - 1) / gP ;
	src_intm_rID = _grp_num_routers -1 - dest_intm_rID;
	if ( dest_rID < dest_intm_rID){
	s.type = 2;
	srand((unsigned)time(NULL));
	s.intm = dest_grp_ID - 1;//(rand()%(( dest%gP + 1)*gP) +1);
	//s.intm = dragonfly_intm_grp(rID, src, dest, s, cur);
	}
	}
	*/

	if (s.type == 1){
		intm_intm_rID = (_grp_num_routers - 1 - (src_grp_ID - s.intm - 1) / gP) + s.intm * _grp_num_routers;
		if (grp_ID == src_grp_ID){
			out_port = dragonfly_min(rID, src, intm_intm_rID);
		}
		//cout<<"misrouting1"<<endl;
	}
	if (s.type == 2){

		if (s.intm == grp_ID + 1)
		{
			intm_intm_rID = s.intm * _grp_num_routers;
		}
		else
		{
			intm_intm_rID = (grp_ID + 1) / gP + s.intm * _grp_num_routers;
		}

		if (grp_ID == src_grp_ID){
			out_port = dragonfly_min(rID, src, intm_intm_rID);
		}
		//cout<<"misrouting2"<<endl;
	}

	/*
	if(s.type == 1){
	intm_intm_rID = (_grp_num_routers -1- (src_grp_ID - s.intm -1)/gP) + s.intm * _grp_num_routers;
	if (grp_ID == src_grp_ID){
	out_port = dragonfly_min(rID, src, intm_intm_rID);
	}
	}
	if(s.type == 2){
	intm_intm_rID = (s.intm - src_grp_ID -1)/gP + s.intm * _grp_num_routers;
	if (grp_ID == src_grp_ID){
	out_port = dragonfly_min(rID, src, intm_intm_rID);
	}
	}
	*/
	return out_port;
}


/***************************************************************************
Name :Forward()
Input: The current node,the destionation
Output:The next node to rout.
***************************************************************************/
NodeInfo*	FCMFR::forward(Q2DTorusNode* cur, Q2DTorusNode* dst, int src, Message &s){
	assert(cur && dst && (cur != dst));
	assert(cur->nodeid != dst->nodeid);

	int vc = 1;
	vector<Direction*> all_poss_dir;
	Direction* flagD = NULL;

	/************************The Routing Function********************************/
	int out_port = -1;
	out_port = hierdragonfly_fcmfr(cur->nodeid, src, dst->nodeid,s);

	//cout<<"grp:"<<cur_grp<<"\t router:"<<cur_rtr<<"\tsrc grp:"<<src_grp<<"\t src router:"<<src_rtr<<"\tdst grp:"<<dst_grp<<"\t dst router:"<<dst_rtr<<"\t output:"<<out_port<<endl;
	//system("pause");

	all_poss_dir.push_back(new Direction(cur->buffer_next_router[out_port], 1, cur->id_next_router[out_port]));
	all_poss_dir.push_back(new Direction(cur->buffer_next_router[out_port], 2, cur->id_next_router[out_port]));
	if (NUM_of_VC==3)
		all_poss_dir.push_back(new Direction(cur->buffer_next_router[out_port], 3, cur->id_next_router[out_port]));


	int emptycount = 0;
	int unsafecount = 0;
	if (s.safe == UNSAFE){
		for (vector<Direction *>::iterator it = all_poss_dir.begin(); it != all_poss_dir.end(); ++it){
			if (checkBufferEmpty(*it))
				emptycount++;
			else
				if ((*it)->buff->safe[(*it)->vc] == UNSAFE)	unsafecount++;
		}
		if (NUM_of_VC == 2){
			if (emptycount == 0 || (emptycount == 1 && unsafecount == 1)){
				/*******************************Delete**********************************/
				for (vector<Direction *>::iterator it = all_poss_dir.begin(); it != all_poss_dir.end(); ++it){
					delete  (*it);
					(*it) = NULL;
				}
				return next;
			}
		}
		else if(NUM_of_VC == 3){
			if (emptycount == 0 || (emptycount == 1 && unsafecount == 2)){
				/*******************************Delete**********************************/
				for (vector<Direction *>::iterator it = all_poss_dir.begin(); it != all_poss_dir.end(); ++it){
					delete  (*it);
					(*it) = NULL;
				}
				return next;
			}
		}
	}

	flagD = Select(all_poss_dir);
	if (flagD != NULL){
		next->buff = flagD->buff;
		next->channel = flagD->vc;
		next->node = flagD->nextnode;
		next->buff->linkused = true;
		if (next->channel != 1 && next->channel != 2 && next->channel != 3)
			cout << next->channel << endl;
		next->buff->bufferMin(next->channel, MESSLENGTH);
		next->buff->safe[next->channel] = s.safe;
		s.routerSequence.push_back(cur->nodeid);
		if (std::count(s.routerSequence.begin(), s.routerSequence.end(), cur->nodeid) > 1)
			printf("errorcycle");
	}
	/*******************************Delete**********************************/
	for (vector<Direction *>::iterator it = all_poss_dir.begin(); it != all_poss_dir.end(); ++it){
		delete  (*it);
		(*it) = NULL;
	}
	return next;
}
