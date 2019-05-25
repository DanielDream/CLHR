#include <assert.h>
#include "Q2DTorus.h"

Q2DTorus::~Q2DTorus(){
	delete[] head;

	for (int i = 0; i < net_num_routers; i++){
		delete[]  nodeLink[i];
	}
	delete[]  nodeLink;

	for (int i = 0; i < net_num_grps; i++){
		delete[] groupLink[i];
	}
	delete[] groupLink;

	for (int i = 0; i < net_num_clusters; i++){
		delete[] clusterLinks[i];
	}
	delete[] clusterLinks;
}

Q2DTorus::Q2DTorus(int VC1_buff, int VC2_buff, int VC3_buff){
	
	head = new Q2DTorusNode[net_num_routers];
	assert(head);

	nodeLink = (Link **)malloc(net_num_routers*sizeof(Link *));
	for (int i = 0; i < net_num_routers; i++){
		nodeLink[i] = new Link[net_num_routers];
	}
	groupLink = (Link **)malloc(net_num_grps*sizeof(Link *));
	for (int i = 0; i < net_num_grps; i++){
		groupLink[i] = new Link[net_num_grps];
	}
	clusterLinks = (vector<Link> **)malloc(net_num_clusters*sizeof(vector<Link> *));
	for (int i = 0; i < net_num_clusters; i++){
		clusterLinks[i] = new vector<Link>[net_num_clusters];
	}

	int xx;
	int yy;
	int zz;

	//x: the cluster NO. ,y: the grp NO. in the cluster, z: the Router No. in the grp, id: the router NO 
	for (xx = 0; xx < net_num_clusters; xx++){
		for (yy = 0; yy < cluster_num_grps; yy++){
			for (zz = 0; zz < grp_num_routers; zz++){
				int id = (xx * cluster_num_grps + yy)*grp_num_routers + zz;
				(head + id)->setTorus(this);
				(head + id)->setCoordinate(id, xx, yy, zz);
				(head + id)->setbuffer(VC1_buff, VC2_buff, VC3_buff);
			}
		}
	}

	
	for (xx = 0; xx < net_num_clusters; xx++){
		for (yy = 0; yy < cluster_num_grps; yy++){
			for (zz = 0; zz < grp_num_routers; zz++){
				int id = (xx * cluster_num_grps + yy)*grp_num_routers + zz;
				int next_router_port_id[rtr_num_ports][2];
				for (int ii = 0; ii < rtr_num_ports; ii++){
					next_router_port_id[ii][0] = -1;
					next_router_port_id[ii][1] = -1;
				}
				int next_grp = -1;
				int next_rtr = -1;
				int next_port = -1;
				
				//local port (inner is all connected)
				for (int i = 0; i < gA - 1; i++){
					next_grp = yy;
					if (i >= zz){
						next_rtr = i + 1;
						next_port = zz;
					}
					else{
						next_rtr = i;
						next_port = zz - 1;
					}
					next_router_port_id[i][0] = (xx * cluster_num_grps + next_grp) * grp_num_routers + next_rtr;
					next_router_port_id[i][1] = next_port;

					//cout<<"id:"<<id<<"\t grp:"<<yy<<"\t router:"<<zz<<"\t port:"<<i<<"\t next router:"<<next_router_port_id[i][0]<<"\t next port:"<<next_router_port_id[i][1]<<endl;
					//system("pause");
				}
				//global port of grouplink in the  cluster
				//change the global connecting style ok!
				for (int i = gA - 1; i < gA-1+gPGL; i++)
				{
					//this is the k & i is equal to xx,so now we know k & i,to obtain x&z,so we can check all the possible x.
					int j = zz * gPGL + i - (gA - 1);
					/***first check out first port connect to last port situation***/
					// j = 0 the first port of group xx
					if (j == 0)
					{
						next_grp = ((yy - 1) + grp_num_ports) % grp_num_ports;
						next_port = (grp_num_ports - 1) - 1;
						goto for_end;
					}
					// j is the last port of group xx
					if (j == (grp_num_ports - 1) - 1)
					{
						next_grp = (yy + 1) % grp_num_ports;
						next_port = 0;
						goto for_end;
					}
					/***xx or next_grp is the last group & this is the special situation & j = xx or j = next_grp,it depends***/
					// xx is the last group
					if (yy == grp_num_ports - 1)
					{
						next_grp = j;
						next_port = j;
						goto for_end;
					}
					// j = xx , so the next_grp is the last group
					if (j == yy)
					{
						next_grp = grp_num_ports - 1;
						next_port = j;
						goto for_end;
					}
					/***now disscuss the xx & next_grp***/
					int tmpyy, tmp_i;
					for (tmpyy = 0; tmpyy < grp_num_ports; tmpyy++)
					{
						if (yy < tmpyy)
						{
							tmp_i = (grp_num_ports - 1) - (tmpyy - yy);
							if (tmp_i == j)
							{
								next_grp = tmpyy;
								next_port = yy + 1;
								break;
							}
						}
						if (yy > tmpyy)
						{
							tmp_i = tmpyy + 1;
							if (tmp_i == j)
							{
								next_grp = tmpyy;
								next_port = (grp_num_ports - 1) - (yy - tmpyy);
								break;
							}
						}
					}
				for_end:
					next_rtr = next_port / gPGL;
					next_router_port_id[i][0] = (xx * cluster_num_grps + next_grp) * grp_num_routers + next_rtr;
					next_router_port_id[i][0] = (next_router_port_id[i][0]>(xx + 1) * grp_num_routers * cluster_num_grps - 1 || next_router_port_id[i][0] < xx * grp_num_routers * cluster_num_grps) ? -1 : next_router_port_id[i][0];
					next_port = next_port % gPGL + gA - 1;
					next_router_port_id[i][1] = next_port;

					/***now we get the bigest network,add the another changing var***/
					//cout << "id:" << id << "\t grp:" << yy << "\t router:" << zz << "\t port:" << i << "\t next router:" << next_router_port_id[i][0] << "\t next port:" << next_router_port_id[i][1] << endl;
					//system("pause");
				}


				(head + id)->setLinkBuffer(next_router_port_id);
				//初始化进行设定，每个时钟周期循环同样需要进行设定
				(head + id)->set_router_limit(gP);



				for (int i = 0; i< gA - 1 + gPGL; i++){
					if (next_router_port_id[i][0] != -1)
					{
						int globalgrpA = xx * cluster_num_grps + yy;
						int globalgrpB = next_router_port_id[i][0] / grp_num_routers;
						int idA = id;
						int idB = next_router_port_id[i][0];
						Link linkAtoB;
						linkAtoB.src = idA;
						linkAtoB.dst = idB;
						linkAtoB.srcport = i;
						linkAtoB.dstport = next_router_port_id[i][1];
						if (i < grp_num_routers - 1)
							linkAtoB.linktype = 0;
						else if (i < grp_num_routers - 1 + gPGL)
							linkAtoB.linktype = 1;
						else
							linkAtoB.linktype = 2;
						nodeLink[idA][idB] = linkAtoB;
						groupLink[globalgrpA][globalgrpB] = linkAtoB;
						//no clusterLinks here

						Link linkBtoA;
						linkBtoA.src = idB;
						linkBtoA.dst = idA;
						linkBtoA.srcport = next_router_port_id[i][1];
						linkBtoA.dstport = i;
						if (i < grp_num_routers - 1)
							linkBtoA.linktype = 0;
						else if (i < grp_num_routers - 1 + gPGL)
							linkBtoA.linktype = 1;
						else
							linkBtoA.linktype = 2;

						nodeLink[idB][idA] = linkBtoA;
						groupLink[globalgrpB][globalgrpA] = linkBtoA;
						//no clusterLinks here
						if (globalgrpA / cluster_num_grps != globalgrpB / cluster_num_grps)
							printf("簇内组间连接不同簇\n");
					}
				}
			}
		}
	}

	//global port  
	
	//首尾相连connect(i,j,v,w,i',j',v',w',linktype)
	int i, j, v, w;
	int i_, j_, v_, w_;
	for (i = 0; i < net_num_clusters - 1; i++){
		for (i_ = i + 1; i_ < net_num_clusters; i_++){
			for (j_ = 0; j_ < cluster_num_grps; j_++){
				for (int l = 0; l < K; l++){
					//printf("%d", i);
					int m = grp_num_routers;
					int n = cluster_num_grps;
					int p = gPCL;
					j = (n - 1) - ((i_ - i - 1)*n + j_) / (p*m - i*K)*K - l;
					v = m - 1 - (((i_ - i - 1)*n + j_) % (p*m - i*K)) / p;
					w = (m*p - ((i_ - i - 1)*n + j_) % (p*m - i*K)) % p + m - 1 + gPGL;
					if (j_ == 0 && i_ - i == 1 && l == 0){
						v_ = 0;
						w_ = p - 1 + m - 1 + gPGL;//0
					}
					else if (j_ == 0 && i_ - i != 1){
						v_ = (i*K + l + 1) / p;
						w_ = p - 1 - (i*K + l + 1) % p + m - 1 + gPGL;
					}
					else{
						v_ = (i*K + l) / p;
						w_ = p - 1 - (i*K + l) % p + m - 1 + gPGL;
					}
					connect(i, j, v, w, i_, j_, v_, w_, 2);
					//cout << "i:" << i << "\t j:" << j << "\t v:" << v << "\t w:"<< w << "\t i':" << i_ << "\t j':" << j_ << "\t v':" << v_ << "\t w':" << w_ << endl;
				}
			}
		}
	}

	/*for (xx = 0; xx < net_num_clusters; xx++){
	connect(xx, 0, 3, 3, xx, 1, 0, 3, 1);
	connect(xx, 0, 2, 3, xx, 2, 1, 3, 1);
	connect(xx, 0, 1, 3, xx, 3, 1, 3, 1);
	connect(xx, 1, 3, 3, xx, 2, 0, 3, 1);
	connect(xx, 1, 2, 3, xx, 3, 2, 3, 1);
	connect(xx, 2, 3, 3, xx, 3, 0, 3, 1);
	}
	connect(0, 3, 3, 4, 1, 0, 0, 4, 2);//cluster1~cluster2
	connect(1, 3, 3, 4, 2, 0, 0, 4, 2);//cluster2~cluster3
	//cluster1~cluster2
	connect(0, 2, 3, 4, 1, 0, 1, 4, 2);
	connect(0, 3, 2, 4, 1, 1, 0, 4, 2);
	connect(0, 2, 2, 4, 1, 1, 1, 4, 2);
	connect(0, 3, 1, 4, 1, 2, 0, 4, 2);
	connect(0, 2, 1, 4, 1, 2, 1, 4, 2);
	connect(0, 3, 0, 4, 1, 3, 0, 4, 2);
	connect(0, 2, 0, 4, 1, 3, 1, 4, 2);
	//cluster1~cluster3
	connect(0, 1, 3, 4, 2, 0, 1, 4, 2);
	connect(0, 0, 3, 4, 2, 0, 2, 4, 2);
	connect(0, 1, 2, 4, 2, 1, 0, 4, 2);
	connect(0, 0, 2, 4, 2, 1, 1, 4, 2);
	connect(0, 1, 1, 4, 2, 2, 0, 4, 2);
	connect(0, 0, 1, 4, 2, 2, 1, 4, 2);
	connect(0, 1, 0, 4, 2, 3, 0, 4, 2);
	connect(0, 0, 0, 4, 2, 3, 1, 4, 2);
	//cluster2~cluster3
	connect(1, 2, 3, 4, 2, 0, 3, 4, 2);
	connect(1, 3, 2, 4, 2, 1, 2, 4, 2);
	connect(1, 2, 2, 4, 2, 1, 3, 4, 2);
	connect(1, 1, 3, 4, 2, 2, 2, 4, 2);
	connect(1, 0, 3, 4, 2, 2, 3, 4, 2);
	connect(1, 1, 2, 4, 2, 3, 2, 4, 2);
	connect(1, 0, 2, 4, 2, 3, 3, 4, 2);
	*/
}


//Q2DTorus::Q2DTorus(int VC1_buff, int VC2_buff, int VC3_buff){
//	head = new Q2DTorusNode[net_num_routers];
//	assert(head);
//
//	nodeLink = (Link **)malloc(net_num_routers*sizeof(Link *));
//	for (int i = 0; i < net_num_routers; i++){
//		nodeLink[i] = new Link[net_num_routers];
//	}
//	groupLink = (Link **)malloc(net_num_grps*sizeof(Link *));
//	for (int i = 0; i < net_num_grps; i++){
//		groupLink[i] = new Link[net_num_grps];
//	}
//	clusterLinks = (vector<Link> **)malloc(net_num_clusters*sizeof(vector<Link> *));
//	for (int i = 0; i < net_num_clusters; i++){
//		clusterLinks[i] = new vector<Link>[net_num_clusters];
//	}
//
//	int xx;
//	int yy;
//	int id;
//
//	//x: the grp NO., y: the Router No. in the grp, id: the router NO 
//	for (xx = 0; xx < net_num_grps; xx++){
//		for (yy = 0; yy < grp_num_routers; yy++){
//			id = xx * grp_num_routers + yy;
//			(head + id)->setTorus(this);
//			(head + id)->setCoordinate(id, xx, yy,0);
//			(head + id)->setbuffer(VC1_buff, VC2_buff, VC3_buff);
//		}
//	}
//
//	for (xx = 0; xx < net_num_grps; xx++){
//		for (yy = 0; yy < grp_num_routers; yy++){
//			int id = xx * grp_num_routers + yy;
//			int next_router_port_id[rtr_num_ports][2];
//			int next_grp = -1;
//			int next_rtr = -1;
//			int next_port = -1;
//
//			//local port (inner is all connected)
//			for (int i = 0; i < gA - 1; i++){
//				next_grp = xx;
//				if (i >= yy){
//					next_rtr = i + 1;
//					next_port = yy;
//				}
//				else{
//					next_rtr = i;
//					next_port = yy - 1;
//				}
//				next_router_port_id[i][0] = next_grp * grp_num_routers + next_rtr;
//				next_router_port_id[i][1] = next_port;
//
//				//cout<<"grp:"<<xx<<"\t router:"<<yy<<"\t port:"<<i<<"\tnext router:"<<next_router_port_id[i][0]<<"\t next port:"<<next_router_port_id[i][1]<<endl;
//				//system("pause");
//			}
//
//			//global port   
//			//change the global connecting style ok!
//			for (int i = gA - 1; i < rtr_num_ports; i++)
//			{
//				//this is the k & i is equal to xx,so now we know k & i,to obtain x&z,so we can check all the possible x.
//				int j = yy * gP + i - (gA - 1);
//				/***first check out first port connect to last port situation***/
//				// j = 0 the first port of group xx
//				if (j == 0)
//				{
//					next_grp = ((xx - 1) + net_num_ports) % net_num_ports;
//					next_port = (net_num_ports - 1) - 1;
//					goto for_end;
//				}
//				// j is the last port of group xx
//				if (j == (net_num_ports - 1) - 1)
//				{
//					next_grp = (xx + 1) % net_num_ports;
//					next_port = 0;
//					goto for_end;
//				}
//				/***xx or next_grp is the last group & this is the special situation & j = xx or j = next_grp,it depends***/
//				// xx is the last group
//				if (xx == net_num_ports - 1)
//				{
//					next_grp = j;
//					next_port = j;
//					goto for_end;
//				}
//				// j = xx , so the next_grp is the last group
//				if (j == xx)
//				{
//					next_grp = net_num_ports - 1;
//					next_port = j;
//					goto for_end;
//				}
//				/***now disscuss the xx & next_grp***/
//				int tmpxx, tmp_i;
//				for (tmpxx = 0; tmpxx < net_num_ports; tmpxx++)
//				{
//					if (xx < tmpxx)
//					{
//						tmp_i = (net_num_ports - 1) - (tmpxx - xx);
//						if (tmp_i == j)
//						{
//							next_grp = tmpxx;
//							next_port = xx + 1;
//							break;
//						}
//					}
//					if (xx > tmpxx)
//					{
//						tmp_i = tmpxx + 1;
//						if (tmp_i == j)
//						{
//							next_grp = tmpxx;
//							next_port = (net_num_ports - 1) - (xx - tmpxx);
//							break;
//						}
//					}
//				}
//
//			for_end:
//				next_rtr = next_port / gP;
//				next_router_port_id[i][0] = next_grp * grp_num_routers + next_rtr;
//				next_router_port_id[i][0] = next_router_port_id[i][0]>net_num_routers - 1 ? -1 : next_router_port_id[i][0];
//				next_port = next_port % gP + gA - 1;
//				next_router_port_id[i][1] = next_port;
//
//				/***now we get the bigest network,add the another changing var***/
//				//cout<<"grp:"<<xx<<"\t router:"<<yy<<"\t port:"<<i<<"\tnext router:"<<(next_router_port_id[i][0]>net_num_routers - 1?-1:next_router_port_id[i][0])<<"\t next port:"<<next_router_port_id[i][1]<<endl;
//				//system("pause");
//			}
//			//waiting for check if the topo is right?
//			/*
//			for (int i = 2*gP - 1; i < rtr_num_ports; i++){
//
//			//the # global link in the grp
//			int j = yy * gP + i - (2*gP - 1) + 1;
//
//			if (j > xx){
//			next_grp = net_num_grps - (j - xx);
//			next_rtr = grp_num_routers - 1 - yy;
//			next_port = (net_num_grps - j -1) % gP + (2*gP - 1);
//			}
//			else{
//			next_grp = xx - j ;
//			next_rtr = grp_num_routers - 1 - yy;
//			next_port = (net_num_grps - j -1) % gP + (2*gP - 1) ;
//			}
//
//			next_router_port_id[i][0] = next_grp * grp_num_routers + next_rtr;
//			next_router_port_id[i][1] = next_port;
//			cout<<"grp:"<<xx<<"\t router:"<<yy<<"\t port:"<<i<<"\tnext router:"<<next_router_port_id[i][0]<<"\t next port:"<<next_router_port_id[i][1]<<endl;
//			system("pause");
//			}
//			*/
//			(head + id)->setLinkBuffer(next_router_port_id);
//			//初始化进行设定，每个时钟周期循环同样需要进行设定
//			(head + id)->set_router_limit(gP);
//
//			for (int i = 0; i< rtr_num_ports; i++){
//				if (next_router_port_id[i][0] != -1)
//				{
//					int globalgrpA = id/grp_num_routers;
//					int globalgrpB = next_router_port_id[i][0] / grp_num_routers;
//					int idA = id;
//					int idB = next_router_port_id[i][0];
//					Link linkAtoB;
//					linkAtoB.src = idA;
//					linkAtoB.dst = idB;
//					linkAtoB.srcport = i;
//					linkAtoB.dstport = next_router_port_id[i][1];
//					if (i < grp_num_routers - 1)
//						linkAtoB.linktype = 0;
//					else if (i == grp_num_routers - 1)
//						linkAtoB.linktype = 1;
//					else
//						linkAtoB.linktype = 2;
//					nodeLink[idA][idB] = linkAtoB;
//					groupLink[globalgrpA][globalgrpB] = linkAtoB;
//					//no clusterLinks here
//
//					Link linkBtoA;
//					linkBtoA.src = idB;
//					linkBtoA.dst = idA;
//					linkBtoA.srcport = next_router_port_id[i][1];
//					linkBtoA.dstport = i;
//					if (i < grp_num_routers - 1)
//						linkBtoA.linktype = 0;
//					else if (i == grp_num_routers - 1)
//						linkBtoA.linktype = 1;
//					else
//						linkBtoA.linktype = 2;
//
//					nodeLink[idB][idA] = linkBtoA;
//					groupLink[globalgrpB][globalgrpA] = linkBtoA;
//					//no clusterLinks here
//				}
//			}
//		}
//	}
//}

Q2DTorusNode* Q2DTorus::operator [](int n){
	return (head+n); 
}

void	Q2DTorus::clearAll(){
	for(int i = 0; i < k * k ; i++){
		(head + i)->clearBuffer();
	}
}

void Q2DTorus::connect(int clusterA, int groupA, int routerA, int portA, int clusterB, int groupB, int routerB, int portB, int linktype){
	int globalgrpA = clusterA * cluster_num_grps + groupA;
	int globalgrpB = clusterB * cluster_num_grps + groupB;
	int idA = globalgrpA * grp_num_routers + routerA;
	int idB = globalgrpB * grp_num_routers + routerB;
	
	(head + idA)->setLinkBuffer(portA, idB, portB);
	(head + idB)->setLinkBuffer(portB, idA, portA);

	Link linkAtoB;
	linkAtoB.src = idA;
	linkAtoB.dst = idB;
	linkAtoB.srcport = portA;
	linkAtoB.dstport = portB;
	linkAtoB.linktype = linktype;

	nodeLink[idA][idB] = linkAtoB;
	groupLink[globalgrpA][globalgrpB] = linkAtoB;
	clusterLinks[clusterA][clusterB].push_back(linkAtoB);

	Link linkBtoA;
	linkBtoA.src = idB;
	linkBtoA.dst = idA;
	linkBtoA.srcport = portB;
	linkBtoA.dstport = portA;
	linkBtoA.linktype = linktype;

	nodeLink[idB][idA] = linkBtoA;
	groupLink[globalgrpB][globalgrpA] = linkBtoA;
	clusterLinks[clusterB][clusterA].push_back(linkBtoA);
}