#include "common.h"
#include"Parameters.h"
#include <limits>
#include <time.h>
#include <algorithm>
#include <sstream>
extern "C"{
#include "queue.h"
}

//流量模型，全局变量
int TRAFFIC;
enum GENERATETYPE{ Uniform, ADV }generatetype;
//总的时钟周期
int totalcircle;
//变量意义不明确
int ICLUE1;
//变量意义不明确
int ICLUE2;
//变量意义不明确
int NUM_of_VC;
//变量意义不明确
int _router_delay;
//变量意义不明确
int knode;
//函数意义不明确
int getsize(vector<Message*>* mess);
//看起来像是一个比较的函数
bool myfunction (Message *i,Message *j) 
{ 
	return (i->count > j->count); 
}

void split(const string& s, vector<string>& sv, const char flag = ' ') {
	sv.clear();
	istringstream iss(s);
	string temp;

	while (getline(iss, temp, flag)) {
		sv.push_back(temp);
	}
	return;
}

typedef struct nt_packet {
	unsigned long long int cycle;
	unsigned int id;
	unsigned int length;
	unsigned char type;
	unsigned char src;
	unsigned char dst;
}nt_packet_t;

typedef struct queue_node {
	nt_packet_t* packet;
	unsigned long long int cycle;
}queue_node_t;


int messageid;
nt_packet_t* read_packet(std::ifstream &fin){
	string line;
	getline(fin, line);
	while (line.size() < 80)
		getline(fin, line);
	//std::cout << line << endl;
	vector<string> sv;
	split(line, sv, ' ');//cycle:sv[0]前4位截断,src:sv[4],dst:sv[9]
	int messagecycle = atoi(sv[0].substr(4).c_str()) - 35057398;
	int src=0, dst=0,length=0;
	for (int i = 0; i < sv.size(); i++){
		if (sv[i] == "master_node" || sv[i] == "src_node"){
			if (sv[i] == "master_node")
				src = 0;
			else{
				src = atoi(sv[i + 1].c_str());
			}
			break;
		}
	}
	for (int i = 5; i < sv.size(); i++){
		if (sv[i] == "master_node" || sv[i] == "dst_node"){
			if (sv[i] == "master_node")
				dst = 0;
			else{
				dst = atoi(sv[i+1].c_str());
			}
			break;
		}
	}
	for (int i = 9; i < sv.size(); i++){
		if (sv[i] == "length"){
			length = atoi(sv[i + 1].c_str());
		}
		break;
	}
	messageid++;
	nt_packet_t *trace_packet = (nt_packet_t*)malloc(sizeof(nt_packet_t));
	trace_packet->cycle = messagecycle;
	trace_packet->src = src;
	trace_packet->dst = dst;
	trace_packet->id = messageid;
	trace_packet->length = length;
	return trace_packet;
}

int totalcycle;
int cycle;

//主函数
int main(){	
	//内存溢出vs的检测工具
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
	//随机种子产生
	srand((unsigned) time(NULL));
	//根据网络配置流量模型生成文件名称
	string networksizestr("networksize_");
	stringstream ss;
	string tmp;
	ss<<gA;
	ss>>tmp;
	networksizestr += tmp;
	networksizestr += "_";
	ss.clear();
	ss<<net_num_grps;
	ss>>tmp;
	networksizestr += tmp;
	//cout<<networksizestr<<endl;
	string trafficstr("traffic_");
	ss.clear();
	ss<<TRAFFIC_START;
	ss>>tmp;
	trafficstr += tmp;
	trafficstr += "-";
	ss.clear();
	ss<<TRAFFIC_END;
	ss>>tmp;
	trafficstr += tmp;
	//cout<<trafficstr<<endl;
	string routingstr("route_");
	ss.clear();
	ss<<ROUTING_START;
	ss>>tmp;
	routingstr += tmp;
	routingstr += "-";
	ss.clear();
	ss<<ROUTING_END;
	ss>>tmp;
	routingstr += tmp;
	//cout<<routingstr<<endl;
	string conjestionstr("conjestion_");
	ss.clear();
	ss<<CONJESTION_AWARE;
	ss>>tmp;
	conjestionstr += tmp;
	conjestionstr += "LOCAL_";
	ss.clear();
	ss<<LOCAL;
	ss>>tmp;
	conjestionstr += tmp;
	//cout<<conjestionstr<<endl;
	string fileprefix = networksizestr + "_" + trafficstr + "_" + routingstr + "_" + conjestionstr;
	ofstream resultout(fileprefix + ".txt");
	ofstream resultout_no(fileprefix+"_no.txt");
	ofstream resultlatency(fileprefix+"_latency.txt");
	ofstream resultaccept(fileprefix+"_accept.txt");
	
	//different traffic pattern
	for(int traffic =  1; traffic < 2; traffic++){
		
		int threshold = numeric_limits<long int>::max();
		Allrouting* rout=NULL;
		generatetype = (GENERATETYPE)traffic;
		NUM_of_VC = 3;
		_router_delay=1;
		totalcircle = TOTALCICLE;
		knode = NetworkSize;
		Q2DTorus* tor = NULL;
		Event* s = NULL;
		string gen[5] = {"0","1", "2", "3", "4"};
		string filename[8] = {
			"D://data//gene4//WCMIN",
			"D://data//gene4//WCFULLY",
			"D://data//gene4//PB3",
			"D://data//gene4//PB",
			"D://data//gene4//PB2",
			"D://data//gene4//VAL",
			"D://data//gene4//WCMIS",
			"D://data//gene4//PB1"
		};

		for(int lop=0; lop < 8; lop ++){
			filename[lop].replace(14, 1, gen[TRAFFIC]);				 
		}

		int VC1buff[8] = {BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE};
		int VC2buff[8] = {BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE};
		int VC3buff[8] = {BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE};

		string gentypestring;
		if (generatetype == Uniform)
			gentypestring = "Uniform";
		else if (generatetype == ADV)
			gentypestring = "ADV";

		string filenames = (string)"CLHR(" + to_string(grp_num_routers) + "X" + to_string(cluster_num_grps) + "X" + to_string(net_num_clusters) + ")_" + gentypestring + "_" + to_string(totalcircle) + "_L" + to_string(MESSLENGTH) + "_VC" + to_string(NUM_of_VC) /*+ "_test"*/ + ".xls";
		ofstream out(filenames.c_str());

		/*func = 0 : minimal
		func = 1 : minus
		   func = 2 : val
		   func = 3 : icluedn
		   func = 4 : icluend
		   func = 5 : icluenn*/


		//different routing algorithms
		for(int func = 0;func < 2; func++){
			//can't create folder,only create file
			//ofstream out = ofstream(filename[func].c_str());	

			switch (func){
			case 0:out << "FCMFR" << endl;
				break;
			case 1:out << "MIN" << endl;
				break;
			}
			out<<"injection rate\t"<<"throughput\t"<<"latency "<<endl;
			double max = 0;
			/************************************************************************************
			start simulate
			***********************************************************************************/
			//different injection rate
			for(double rate = 0.02; rate <= 0.96;)
			{
				tor = new Q2DTorus(VC1buff[func] * MESSLENGTH, VC2buff[func] * MESSLENGTH, VC3buff[func] * MESSLENGTH);
				
				switch (func){
					case 0:rout = new FCMFR(tor);
						   break;
					case 1:rout = new MIN(tor);
						   break;
					case 2:rout = new WC_SAME2(tor);
						   break;
					case 3:rout = new PB1(tor);
						   break;
					case 4:rout = new PB(tor);
						   break;
					case 5:rout = new MIN(tor);
						   break;
					case 6:rout = new SAMEVC(tor);
						   break;
					case 7:rout = new PB1(tor);
						   break;
				}
				
				
				s = new Event(rout);
				//初始化计算每个时钟周期吸收的数组，方便调试
				s->init_circle_array(TOTALCICLE);
				float msgpercir = (float)(rate *((cluster_num_grps*K*(net_num_clusters / 2)*((net_num_clusters / 2) + (net_num_clusters % 2)*0.5)) * 2 * 2 / MESSLENGTH)*3/4);
				//float msgpercir = (float)(rate * net_num_routers *(gA / 2) / (MESSLENGTH)*2*K / cluster_num_grps);// *10));*K / cluster_num_grps
				vector<Message*> allvecmess[10];
				float k = 0;
				int allmess = 0;

				//限定路由器消息吸收能力数组，与路由器的数量相等
				vector<int> router_message_limit(net_num_routers,gP);

				/************************************************************************************
				genarate message
				***********************************************************************************/
				
				for(int i = 0; i < totalcircle && getsize(allvecmess) < threshold; i++)
				{
					//开始的时候设定一下event中的当前时钟周期
					s->cur_circle = i;

					vector<Message*>& vecmess = allvecmess[0];// [i % 10];
					if (i < TOTALCICLE){
						for (k += msgpercir; k > 0; k--)
						{
							allmess++;
							vecmess.push_back(s->genMes(rate));
						}
					}
					//这里属于新添加的内容，因为需要对每个路由器的消息每个时钟周期吸收消息的能力进行限制，
					//所以这里针对每个新的时钟周期将每个路由器的吸收能力重新置为gP
					for (int router = 0;router < net_num_routers;++router)
					{
						(*(s->tor))[router]->set_router_limit(gP);
					}
					/**********************************************************************************
					release link        
					***********************************************************************************/
					/*i can't understand this step , and i don't know the routpath?*/

					for(vector<Message*>::iterator it = vecmess.begin(); it != vecmess.end(); it++)
					{
						// if the tail of a message shifts , the physical link the message  occupied should release.
						if((*it)->releaselink == true)
						{
							assert((*it)->routpath[MESSLENGTH - 1].buff->linkused);
							(*it)->routpath[MESSLENGTH - 1].buff->linkused = false;
							(*it)->releaselink = false;
						}
					}

					/************************************************************************************
					forward message
					***********************************************************************************/
					for(vector<Message*>::iterator it = vecmess.begin(); it != vecmess.end();){
						if((*it)->active == false){	  // when a message arrive at its destination, it is not active
							/*int temp = ((*it)->pathreserved.size() < (*it)->routerSequence.size()) ? (*it)->pathreserved.size() : (*it)->routerSequence.size();
							for (int i = 0; i < temp-1; i++){
								if ((*it)->pathreserved[i] != (*it)->routerSequence[i])
									printf("a");
							}*/
							delete (*it);

							it = vecmess.erase(it);
						}
						else
							s->forwardMes(*(*it++));
					}		
				}

				/*****************************************************************************
				output results
				****************************************************************************/

				int size = getsize(allvecmess);

				cout <<endl 
					 <<"injection rate:"<<rate<<"\t arrive:" << s->messarrive << "\t in the network:" <<size <<endl
					 << " average latency:  " <<(s->totalcir/s->messarrive) << " nomalized accepted traffic: "
					 <<rate*((float) s->messarrive/allmess)<<endl;
				resultout <<endl 
					<<"injection rate:"<<rate<<"\t arrive:" << s->messarrive << "\t in the network:" <<size <<endl
					<< " average latency:  " <<(s->totalcir/s->messarrive) << " nomalized accepted traffic: "
					<<rate*((float) s->messarrive/allmess)<<endl;

				resultlatency<<(s->totalcir/s->messarrive)<<endl;
				resultaccept<<rate*((float) s->messarrive/allmess)<<endl;

				out<< rate<<"\t"<< rate * ((float) s->messarrive / allmess)  
					<< "\t" << (s->totalcir/s->messarrive) << endl;

				/************************************************************************************
				whether arrive at saturation point
				***********************************************************************************/
				
				if (true/*rate * ((float)s->messarrive/allmess)>max  && ((rate*((float) s->messarrive/allmess)-max)/max)>0.005 && getsize(allvecmess)<threshold*/) 
					max = rate * ((float) s->messarrive / allmess);

				else {
						cout << "Saturation point, drain......." << endl;
						resultout << "Saturation point, drain......." << endl;
						resultlatency<<"================================================"<<endl;
						resultaccept<<"================================================"<<endl;
						drain(allvecmess,tor,s);
						int size = 0;
						for(int j = 0; j < 10; j++){
							if(!allvecmess[j].empty()){
								size += allvecmess[j].size();
							}
						}
						cout << "in the network:\t"  << size << endl;
						resultout << "in the network:\t"  << size << endl;
						outtotest(allvecmess,tor);
						bufferleft(tor, knode);
						cout << "max:" << max << endl;
						resultout << "max:" << max << endl;
						
						/************************************************************************************
				        clean
				        *******************************************************************************************/

				        for(int m = 0; m < 10; m++)
						{
					        for(vector<Message*>::iterator it = allvecmess[m].begin();
						        it != allvecmess[m].end(); it++)
						            delete (*it);
				        }
				        delete rout;
				        delete tor;
				        delete s;

						break;
				}
				
				//这里希望在total circle完成之后输出网络中仍然存在的并且路由成功的消息

//#ifdef _DEBUG
//				int linenum = 1;
//				for (int m = 0; m < 10;m++)
//				{
//					for (vector<Message*>::iterator it = allvecmess[m].begin();it != allvecmess[m].end();it++)
//					{
//						//路由成功的消息，输出
//						if ((*it)->routpath[0].channel != 0)
//						{
//							//输出源，目的信息
//							cout<<linenum++<<":"<<"src router:"<<(*it)->src<<"\t mid router"<<(*it)->init_r_num<<"\tdest router:"<<(*it)->dst<<"\tMessage count:"<<(*it)->count;
//							//输出路由路径信息
//							for (int i = 0;i < 20;++i)
//							{
//								cout<<"\t";
//								cout<<(*it)->routpath[i].node<<","<<(*it)->routpath[i].channel;
//							}
//							cout<<endl;
//						}
//					}
//				}
//				//这里输出平均每个时钟周期的吞吐量
//				int sum_accept = 0;
//				for (int m = 0; m < TOTALCICLE;m++)
//				{
//					sum_accept += s->circle_accept[m];
//				}
//				double avg_accept = ((double)sum_accept)/TOTALCICLE;
//				cout<<"avg_accept:"<<avg_accept<<endl;
//				resultout<<"avg_accept:"<<avg_accept<<endl;
//				//在clean网络中存留的消息之前可以对网络中的消息进行统计工作
//				vector<Message*> sortedMessage;
//				if (getsize(allvecmess) >0)
//				{
//					for (int m = 0; m < 10;m++)
//					{
//						for(vector<Message*>::iterator it = allvecmess[m].begin();it != allvecmess[m].end();it++)
//						{
//							sortedMessage.push_back((*it));
//						}
//					}
//					sort(sortedMessage.begin(),sortedMessage.end(),myfunction);
//				}
//				
//				int leavemessage[6] = {0,0,0,0,0,0};
//				for (int m = 0; m < 10;m++)
//				{
//					/*
//					for(vector<Message*>::iterator it = allvecmess[m].begin();it != allvecmess[m].end();it++)
//					{
//						if ((*it)->routpath[0].node == (*it)->dest_init_router)
//						{
//							leavemessage[0]++;
//						}
//						else if ((*it)->routpath[0].node == (*it)->mid_init_router_2)
//						{
//							leavemessage[1]++;
//						}
//						else if ((*it)->routpath[0].node == (*it)->mid_init_router_1)
//						{
//							leavemessage[2]++;
//						}
//						else if ((*it)->routpath[0].node == (*it)->src_init_router)
//						{
//							leavemessage[3]++;
//						}
//						else if ((*it)->routpath[0].node == (*it)->src)
//						{
//							leavemessage[4]++;
//						}
//						else if ((*it)->routpath[0].node == (*it)->dst)
//						{
//							leavemessage[5]++;
//						}
//					}
//					*/
//					for(vector<Message*>::iterator it = allvecmess[m].begin();it != allvecmess[m].end();it++)
//					{
//						if ((*it)->count > 1000)
//						{
//							if ((*it)->routpath[0].node == (*it)->src)
//							{
//								leavemessage[0]++;
//							}
//							else if ((*it)->routpath[0].node == (*it)->src_init_router && (*it)->src_init_router == (*it)->src)
//							{
//								leavemessage[0]++;
//							}
//							else if ((*it)->routpath[0].node == (*it)->src_init_router)
//							{
//								leavemessage[1]++;
//							}
//							else if ((*it)->routpath[0].node == (*it)->dest_init_router && (*it)->src_init_router == (*it)->src)
//							{
//								leavemessage[1]++;
//							}
//							else if ((*it)->routpath[0].node == (*it)->dest_init_router)
//							{
//								leavemessage[2]++;
//							}
//							else if ((*it)->routpath[0].node == (*it)->dst)
//							{
//								leavemessage[3]++;
//							}
//						}
//					}
//				}
//				/*
//				cout<<"共消息 ："<<size<<endl;
//				cout<<"剩余1步："<<leavemessage[0]<<endl;
//				cout<<"剩余2步："<<leavemessage[1]<<endl;
//				cout<<"剩余3步："<<leavemessage[2]<<endl;
//				cout<<"剩余4步："<<leavemessage[3]<<endl;
//				cout<<"剩余5步："<<leavemessage[4]<<endl;
//				cout<<" 未吸收："<<leavemessage[5]<<endl;
//				*/
//				/*
//				cout<<"共消息 ："<<size<<endl;
//				cout<<"0步："<<leavemessage[0]<<endl;
//				cout<<"1步："<<leavemessage[1]<<endl;
//				cout<<"2步："<<leavemessage[2]<<endl;
//				cout<<"3步："<<leavemessage[3]<<endl;
//				*/
//#endif // DEBUG
				
				/************************************************************************************
				clean
				*******************************************************************************************/

				for(int m = 0; m < 10; m++){
					for(vector<Message*>::iterator it = allvecmess[m].begin();
						it != allvecmess[m].end(); it++)
						delete (*it);
					allvecmess[m].clear();
				}
				delete rout;
				delete tor;
				delete s;

				if(rate < 0.3)
					rate += 0.02;
				else
					rate += 0.02;

				if(fabs(rate - 0.55) < 0.000001)
				{
					//cout<<"debug step"<<endl;
				}

				//system("pause");

			}// end of rate
		} //end of func
	}
	//system("pause");
	//system("shutdown -s");
	return 1;
}

//int main(){
//	//内存溢出vs的检测工具
//	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
//	//随机种子产生
//	srand((unsigned)time(NULL));
//	//根据网络配置流量模型生成文件名称
//	string networksizestr("networksize_");
//	stringstream ss;
//	string tmp;
//	ss << gA;
//	ss >> tmp;
//	networksizestr += tmp;
//	networksizestr += "_";
//	ss.clear();
//	ss << net_num_grps;
//	ss >> tmp;
//	networksizestr += tmp;
//	//cout<<networksizestr<<endl;
//	string trafficstr("traffic_");
//	ss.clear();
//	ss << TRAFFIC_START;
//	ss >> tmp;
//	trafficstr += tmp;
//	trafficstr += "-";
//	ss.clear();
//	ss << TRAFFIC_END;
//	ss >> tmp;
//	trafficstr += tmp;
//	//cout<<trafficstr<<endl;
//	string routingstr("route_");
//	ss.clear();
//	ss << ROUTING_START;
//	ss >> tmp;
//	routingstr += tmp;
//	routingstr += "-";
//	ss.clear();
//	ss << ROUTING_END;
//	ss >> tmp;
//	routingstr += tmp;
//	//cout<<routingstr<<endl;
//	string conjestionstr("conjestion_");
//	ss.clear();
//	ss << CONJESTION_AWARE;
//	ss >> tmp;
//	conjestionstr += tmp;
//	conjestionstr += "LOCAL_";
//	ss.clear();
//	ss << LOCAL;
//	ss >> tmp;
//	conjestionstr += tmp;
//	//cout<<conjestionstr<<endl;
//	string fileprefix = networksizestr + "_" + trafficstr + "_" + routingstr + "_" + conjestionstr;
//	ofstream resultout(fileprefix + ".txt");
//	ofstream resultout_no(fileprefix + "_no.txt");
//	ofstream resultlatency(fileprefix + "_latency.txt");
//	ofstream resultaccept(fileprefix + "_accept.txt");
//
//	//different traffic pattern
//	int threshold = numeric_limits<long int>::max();
//	Allrouting* rout = NULL;
//	NUM_of_VC = 2;
//	_router_delay = 1;
//	totalcircle = TOTALCICLE;
//	knode = NetworkSize;
//	Q2DTorus* tor = NULL;
//	Event* s = NULL;
//	string gen[5] = { "0", "1", "2", "3", "4" };
//	string filename[8] = {
//		"D://data//gene4//WCMIN",
//		"D://data//gene4//WCFULLY",
//		"D://data//gene4//PB3",
//		"D://data//gene4//PB",
//		"D://data//gene4//PB2",
//		"D://data//gene4//VAL",
//		"D://data//gene4//WCMIS",
//		"D://data//gene4//PB1"
//	};
//
//	for (int lop = 0; lop < 8; lop++){
//		filename[lop].replace(14, 1, gen[TRAFFIC]);
//	}
//
//	int VC1buff[8] = { BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE };
//	int VC2buff[8] = { BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE };
//	int VC3buff[8] = { BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE, BUFFSIZE };
//
//	string filenames = (string)"hier-dragonfly_3_4_4_traces" + ".xls";
//	ofstream out(filenames.c_str());
//
//	/*func = 0 : minimal
//	func = 1 : minus
//	func = 2 : val
//	func = 3 : icluedn
//	func = 4 : icluend
//	func = 5 : icluenn*/
//
//	//different routing algorithms
//	for (int func = 0; func < 2; func++){
//		//can't create folder,only create file
//		//ofstream out = ofstream(filename[func].c_str());	
//
//		switch (func){
//		case 0:out << "FCMFR" << endl;
//			break;
//		case 1:out << "MIN" << endl;
//			break;
//		}
//		out << "injection rate\t" << "throughput\t" << "latency " << endl;
//		double max = 0;
//		/************************************************************************************
//		start simulate
//		***********************************************************************************/
//		messageid = 0;
//		std::ifstream fin("..\\..\\Netraces\\res_0110\\res_0110.txt", std::ios::in);
//		//nt_packet_t *trace_packet = read_packet(fin);
//
//		tor = new Q2DTorus(VC1buff[func] * MESSLENGTH, VC2buff[func] * MESSLENGTH, VC3buff[func] * MESSLENGTH);
//
//		switch (func){
//		case 0:rout = new FCMFR(tor);
//			break;
//		case 1:rout = new MIN(tor);
//			break;
//		case 2:rout = new WC_SAME2(tor);
//			break;
//		case 3:rout = new PB1(tor);
//			break;
//		case 4:rout = new PB(tor);
//			break;
//		case 5:rout = new MIN(tor);
//			break;
//		case 6:rout = new SAMEVC(tor);
//			break;
//		case 7:rout = new PB1(tor);
//			break;
//		}
//
//		s = new Event(rout);
//		//初始化计算每个时钟周期吸收的数组，方便调试
//		s->init_circle_array(TOTALCICLE);
//
//		float k = 0;
//		int allmess = 0;
//		nt_packet_t* trace_packet = NULL;
//		nt_packet_t* packet = NULL;
//
//		//限定路由器消息吸收能力数组，与路由器的数量相等
//		vector<int> router_message_limit(net_num_routers, gP);
//
//		int packets_left = 0;
//		cycle = 0;
//		totalcycle = 0;
//		//queue_t** waiting;
//		queue_t** inject;
//		//queue_t** traverse;
//
//		int* injecting_remainingcycle_node = NULL;//record the injecting message per node
//
//		//waiting = (queue_t**)malloc(header->num_nodes * sizeof(queue_t*));
//		inject = (queue_t**)malloc(net_num_routers * sizeof(queue_t*));
//		//traverse = (queue_t**)malloc(header->num_nodes * sizeof(queue_t*));
//
//		injecting_remainingcycle_node = (int*)malloc(net_num_routers * sizeof(int));
//
//		//if ((waiting == NULL) || (inject == NULL) || (traverse == NULL)) {
//		if (inject == NULL) {
//			printf("malloc fail queues\n");
//			exit(0);
//		}
//		for (int i = 0; i < net_num_routers; ++i) {
//			//waiting[i] = queue_new();
//			inject[i] = queue_new();
//			//traverse[i] = queue_new();
//			injecting_remainingcycle_node[i] = 0;
//		}
//
//		//packet = nt_read_packet();
//		//cycle = packet->cycle;
//		//nt_clear_dependencies_free_packet(packet);
//		//while (cycle < 1000000){
//		//	//nt_print_packet(packet);
//		//	if (packet->dst == -1)
//		//		printf("X");
//		//	packet = nt_read_packet();
//		//	nt_clear_dependencies_free_packet(packet);
//		//	//if (packet == NULL) break;
//		//	//cycle = packet->cycle;
//		//	cycle++;
//		//}
//
//		vector<Message*> allvecmess;
//
//		int dropoutcount = 0;
//		int packetcount = 0;
//		int longpacketcount = 0;
//
//		trace_packet = read_packet(fin);
//
//		int temp_i = 0;
//		while (temp_i++ < 0){
//			trace_packet = read_packet(fin);
//			//nt_clear_dependencies_free_packet(trace_packet);
//			//nt_print_packet(trace_packet);
//			cycle = trace_packet->cycle;
//		}
//
//		while ((cycle <= 200000) || packets_left) {
//			// Reset packets remaining check
//			packets_left = 0;
//
//			// Get packets for this cycle
//
//			while ((trace_packet != NULL) && (trace_packet->cycle == cycle)) {
//				if (trace_packet->src == trace_packet->dst){
//					//printf("Dropout: %llu ", cycle);
//					//nt_print_packet(trace_packet);
//					dropoutcount++;
//					//nt_clear_dependencies_free_packet(trace_packet);
//				}
//				else{
//					// Place in appropriate queue
//					queue_node_t* new_node = (queue_node_t*)malloc(sizeof(queue_node_t));
//					new_node->packet = trace_packet;
//					new_node->cycle = (trace_packet->cycle > cycle) ? trace_packet->cycle : cycle;
//					// Add to inject queue
//					//if (trace_packet->length > 2796)
//						for (int i = 0; i < 10; i++){
//							queue_push(inject[trace_packet->src], new_node, new_node->cycle);
//						}
//					/*else
//						queue_push(inject[trace_packet->src], new_node, new_node->cycle);*/
//				}
//				// Get another packet from trace
//				trace_packet = read_packet(fin);
//				packetcount++;
//			}
//			if ((trace_packet != NULL) && (trace_packet->cycle < cycle)) {
//				// Error check: Crash and burn
//				printf("Invalid trace_packet cycle time: %llu, current cycle: %llu\n", trace_packet->cycle, cycle);
//				exit(-1);
//			}
//
//			int injectcount = 0;
//			// Inject where possible (max one per node)
//			for (int i = 0; i < net_num_routers; ++i) {
//				packets_left |= !queue_empty(inject[i]);
//				if (injecting_remainingcycle_node[i] <= 0){
//					queue_node_t* temp_node = (queue_node_t*)queue_peek_front(inject[i]);
//					if (temp_node != NULL) {
//						packet = temp_node->packet;
//						if ((packet != NULL) && (packet->cycle <= cycle)) {
//							//printf("Inject: %llu ", cycle);
//							//nt_print_packet(packet);
//							temp_node = (queue_node_t*)queue_pop_front(inject[i]);
//							int packetsize = MESSLENGTH;
//							//if (packetsize == 9)
//							//	packetsize = 5;
//							if (packetsize > 1)
//								longpacketcount++;
//							if (packetsize < 0)
//								printf("packetsize = -1");
//							if (packetsize > 0){
//								injectcount++;
//								/*printf("Inject: %llu ", cycle);
//								printf("Length: %d", packetsize);
//								nt_print_packet(packet);*/
//								allvecmess.push_back(s->genMes(temp_node->packet->id, temp_node->packet->src, temp_node->packet->dst, packetsize));
//								injecting_remainingcycle_node[temp_node->packet->src] = 0;//packetsize / 2;
//								assert(temp_node->packet->src == i);
//							}
//							/*else
//							temp_node = (queue_node_t*)queue_pop_front(inject[i]);*/
//						}
//					}
//				}
//			}
//
//
//			/***************************************************
//
//			release link
//
//			***************************************************/
//			for (vector<Message*>::iterator it = allvecmess.begin(); it != allvecmess.end(); it++){
//				//if the tail of a message shifts, teh physical link the message occupied should release.
//				if ((*it)->releaselink == true){
//					/*if ((*it)->messagelength == 1){
//					assert((*it)->releaselinknode.buff->linkused);
//					(*it)->releaselinknode.buff->linkused = false;
//					(*it)->releaselink = false;
//					}
//					else{*/
//					assert((*it)->routpath[MESSLENGTH - 1].buff->linkused);
//					(*it)->routpath[MESSLENGTH - 1].buff->linkused = false;
//					(*it)->releaselink = false;
//					//}
//				}
//			}
//			/***************************************************
//
//			forward messages
//
//			***************************************************/
//			for (vector<Message*>::iterator it = allvecmess.begin(); it != allvecmess.end();){
//				if ((*it)->active == false){//when a message arrive at its destination, it is not active
//					//packet = (*it)->packet;
//					//printf("Eject: %llu ", cycle);
//					//nt_print_packet(packet);
//					delete (*it);
//					it = allvecmess.erase(it);
//				}
//				else{
//					s->forwardMes(*(*it++));
//				}
//			}
//
//
//			////inform the end of the injecting packet
//			//for (vector<Message*>::iterator it = allvecmess.begin(); it != allvecmess.end(); it++){
//			//	if ((*it)->routepath[(*it)->messagelength - 1].nodeid != (*it)->src){
//			//		if (injecting_messageid_node[(*it)->src] == (*it)->messageid){
//			//			(queue_node_t*)queue_pop_front(inject[(*it)->src]);
//			//			injecting_messageid_node[(*it)->src] = -1;
//			//		}
//			//	}
//			//}
//			for (int i = 0; i < net_num_routers; ++i) {
//				injecting_remainingcycle_node[i]--;
//			}
//
//			/*if (cycle % 50000 == 1 && s->messarrive > 0){
//				int size = allvecmess.size();
//				cout << endl << endl << "arrive:" << s->messarrive << "   in the network:"
//					<< size << endl << "average latency:"
//					<< ((double)(s->totalcir) / s->messarrive) << endl;
//			}*/
//			cycle++;
//		}
//		/***************************************************
//
//		output results
//
//		***************************************************/
//		int size = allvecmess.size();
//		cout << endl << endl << "arrive:" << s->messarrive << "   in the network:"
//			<< size << endl << "average latency:"
//			<< ((double)(s->totalcir) / s->messarrive) << endl;
//		//for (int m = 1; m < 9; m++){
//		//cout << "type" << m << ":" << e->messagetypecount[m] << endl;
//		//}
//		cout << endl;
//
//		if (out.is_open()){
//			//out << "messarrivedcount" << "\t" << "in the network" << "\t" << "average latency" << "\t" << endl;
//			out << ((double)(s->totalcir) / s->messarrive) << "\t";
//		}
//		for (int i = 0; i < net_num_routers; ++i) {
//			//queue_delete(waiting[i]);
//			queue_delete(inject[i]);
//			//queue_delete(traverse[i]);
//		}
//		//free(waiting);
//		free(inject);
//		//free(traverse);
//		fin.close();
//	}
//	if (out.is_open()){
//		out << endl;
//	}
//
//	return 1;
//}