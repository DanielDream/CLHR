#include"Event.h"
#include<math.h>
#include"Message.h"

extern int TRAFFIC;
extern enum GENERATETYPE{ Uniform, ADV };
extern enum GENERATETYPE generatetype;
extern int _router_delay;

Event::Event(Allrouting * rout1){	
	consumed = 0;
	totalcir =0 ;
	messarrive = 0;
	rout = rout1;
	tor = rout1->torus;
	k = rout1->k;
}
Message*  Event::genMes(int id,int src,int dst,int size){   //generate a message
	return new Message(src, dst, 0);
}

Message*  Event::genMes(float rate){   //generate a message
	Q2DTorus* torus;
	assert(rout != NULL);
	torus = rout->torus;
	//Uniform 
	if (generatetype == Uniform){
		int tempRand;
		int  src, dest;
		tempRand = rand() % net_num_routers;
		src = tempRand;
		while (1){
			tempRand = rand() % net_num_routers;
			if ( tempRand != src){
				dest = tempRand;
				break;
			}
		}
		return new Message(src, dest,rate);
	}
	//ADV
	if (generatetype == ADV){
		int tempRand1, tempRand2, temp_grp;
		int src, dest;
		int src_cluster, dest_cluster;
		tempRand1 = rand() % net_num_routers;
		src = tempRand1;
		src_cluster = tempRand1 / (cluster_num_grps*grp_num_routers);
		/*if (rand()%2==1)
			dest_cluster = (src_cluster - 1 + net_num_clusters) % net_num_clusters;
		else*/
			dest_cluster = (src_cluster + 1) % net_num_clusters;
			
		tempRand2 = rand() % (cluster_num_grps*grp_num_routers);
		dest = dest_cluster * (cluster_num_grps*grp_num_routers) + tempRand2;
		return new Message(src, dest, rate);
			//int tempRand1,tempRand2,temp_grp;
			//int src,dest;
			//int src_grp,dest_grp;
			//tempRand1 = rand() % net_num_routers;
			//src = tempRand1;
			//src_grp = tempRand1 / grp_num_routers;

			//dest_grp = (src_grp + 1)%net_num_grps;
			//tempRand2 = rand() % grp_num_routers;
			//dest = dest_grp * grp_num_routers + tempRand2;

			//return new Message(src,dest,rate);
	}
	////Transpose
	//if (generatetype == Transpose1){
	//	int tempRand;
	//	int  src, dest;
	//	int  x;
	//	int  y;

	//	while (1){
	//		tempRand = abs(rand()) % (k * k );
	//		x = (*torus)[tempRand]->x;
	//		y = (*torus)[tempRand]->y;

	//		if ( x != (k - 1 - y) || y != (k - 1 - x) )
	//			break;
	//	}
	//	dest = (k - 1 - y)  + (k - 1 - x) * k;
	//	src = tempRand;

	//	return new Message(src, dest,rate);
	//}

	//if (generatetype == Transpose2){
	//	int tempRand;
	//	int  src, dest;
	//	int  x;
	//	int  y;

	//	while (1){
	//		tempRand = abs(rand()) % (k * k);
	//		x = (*torus)[tempRand]->x;
	//		y = (*torus)[tempRand]->y;

	//		if ( x != y  )
	//			break;
	//	}
	//	dest = y   + x * k ;
	//	src = tempRand;

	//	return new Message(src, dest,rate);
	//}

	////Hotspot***************************************************************************************************************************
	//if(TRAFFIC == 4){
	//	int tempRand;
	//	int  src, dest;
	//	tempRand = abs(rand()) % 10;
	//	if(tempRand != 0){
	//		tempRand = abs(rand()) % (k * k );
	//		src = tempRand;
	//		while (1){
	//			tempRand = abs(rand()) % (k * k);
	//			if ( tempRand != src){
	//				dest = tempRand;
	//				break;
	//			}
	//		}
	//	}
	//	else{
	//		int temprand2 = abs(rand()) % 2;
	//		switch(temprand2){
	//		case 0:
	//			dest = (int) ((k - 1 - k/4) * k + k - 1 - k/4); break;
	//		case 1:
	//			dest = (int) (k/4 * k + k/4); break;
	//		}
	//		while (1){
	//			tempRand = abs(rand()) % (k * k);
	//			if ( tempRand != dest){
	//				src = tempRand;
	//				break;
	//			}
	//		}
	//	}
	//	return new Message(src, dest,rate);
	//}
	
	////Worst case 
	//if(TRAFFIC == 0){
	//	/*
	//	int src, dst;
	//	int srcx,srcy;
	//	int dstx,dsty;

	//	src = abs(rand()) % (k * k);
	//	srcx= src%k;
	//	srcy= src/k;
	//	dstx= (srcx+k/2+1)%k;
	//	dsty= (srcy+k/2+1)/k;
	//	dst = dstx+dsty*k;

	//	return new Message(src, dst,rate);
	//	*/
	//	int tempRand1,tempRand2,temp_grp;
	//	int src,dest;
	//	int src_grp,dest_grp;
	//	tempRand1 = rand() % net_num_routers;
	//	src = tempRand1;
	//	src_grp = tempRand1 / grp_num_routers;

	//	dest_grp = (src_grp + 1)%net_num_grps;
	//	tempRand2 = rand() % grp_num_routers;
	//	dest = dest_grp * grp_num_routers + tempRand2;

	//	return new Message(src,dest,rate);
	//}

	////Worst case 
	//if(TRAFFIC == 1){
	//	/*
	//	int src, dst;
	//	int srcx,srcy;
	//	int dstx,dsty;

	//	src = abs(rand()) % (k * k);
	//	srcx= src%k;
	//	srcy= src/k;
	//	dstx= (srcx+k/2+1)%k;
	//	dsty= (srcy+k/2+1)/k;
	//	dst = dstx+dsty*k;

	//	return new Message(src, dst);
	//	*/
	//	int tempRand1,tempRand2,temp_grp;
	//	int src,dest;
	//	int src_grp,dest_grp;
	//	tempRand1 = rand() % net_num_routers;
	//	src = tempRand1;
	//	src_grp = tempRand1 / grp_num_routers;

	//	dest_grp = (src_grp + 2)%net_num_grps;
	//	tempRand2 = rand() % grp_num_routers;
	//	dest = dest_grp * grp_num_routers + tempRand2;

	//	return new Message(src,dest,rate);
	//}

	////Worst case 
	//if(TRAFFIC == 2){
	//	/*
	//	int src, dst;
	//	int srcx,srcy;
	//	int dstx,dsty;

	//	src = abs(rand()) % (k * k);
	//	srcx= src%k;
	//	srcy= src/k;
	//	dstx= (srcx+k/2+1)%k;
	//	dsty= (srcy+k/2+1)/k;
	//	dst = dstx+dsty*k;

	//	return new Message(src, dst);
	//	*/
	//	int tempRand1,tempRand2,temp_grp;
	//	int src,dest;
	//	int src_grp,dest_grp;
	//	tempRand1 = rand() % net_num_routers;
	//	src = tempRand1;
	//	src_grp = tempRand1 / grp_num_routers;

	//	dest_grp = (src_grp + 3)%net_num_grps;
	//	tempRand2 = rand() % grp_num_routers;
	//	dest = dest_grp * grp_num_routers + tempRand2;

	//	return new Message(src,dest,rate);
	//}

	////Worst case 
	//if(TRAFFIC == 3){
	//	/*
	//	int src, dst;
	//	int srcx,srcy;
	//	int dstx,dsty;

	//	src = abs(rand()) % (k * k);
	//	srcx= src%k;
	//	srcy= src/k;
	//	dstx= (srcx+k/2+1)%k;
	//	dsty= (srcy+k/2+1)/k;
	//	dst = dstx+dsty*k;

	//	return new Message(src, dst);
	//	*/
	//	int tempRand1,tempRand2,temp_grp;
	//	int src,dest;
	//	int src_grp,dest_grp;
	//	tempRand1 = rand() % net_num_routers;
	//	src = tempRand1;
	//	src_grp = tempRand1 / grp_num_routers;

	//	dest_grp = (src_grp + 4)%net_num_grps;
	//	tempRand2 = rand() % grp_num_routers;
	//	dest = dest_grp * grp_num_routers + tempRand2;

	//	return new Message(src,dest,rate);
	//}
}

void       Event::forwardMes(Message& s){
	//router delay == 1 cycle  
	if(s.begintrans <= 0) 
		s.count++;

	//at the sourse node
	if( s.routpath[0].node == s.src) {
		if( s.begintrans > 0)
			s.begintrans --; // check whether process time  
		else{					
			s.begintrans--;
			NodeInfo* next;
			assert(rout != NULL);
			//尝试的对消息s进行一次路由，路由后的下一个节点next
			next = rout->forward(s);
			//routing flase
			if (next->node == -1){
				s.timeout++;
				//if(s.timeout == TIMEOUT) consumeMes(s);
			}
			//routing complement
			else{
				s.timeout = 0;
				//cout<<"head flit position:"<<s.routpath[0].node<<"  "<<"next position:"<<next->node<<"  "<<"src:    "<<s.src<<""<<"   dst:"<<s.dst<<endl;
				assert(s.routpath[0].node != next->node);
				//just routing , not forward
				s.routpath[0] = *next; 
			}
		}
	}
	//not at the sourse node
	else{
		//not at the destination
		if( s.routpath[0].node != s.dst){
			NodeInfo* next=NULL;
			if(rout != NULL)		
				next = rout->forward(s);
			//routing flase
			if (next->node == -1){
				int i ;
				for( i = 1; i < MESSLENGTH && s.routpath[i].node == s.routpath[0].node; i++); //if blocked insert the flits until all are inserted
				if(i < MESSLENGTH){
					NodeInfo temp1 ,temp2; // all the flits forward one step
					temp2 = s.routpath[i - 1];
					while(i < MESSLENGTH){
						temp1 = s.routpath[i] ;
						s.routpath[i] = temp2;
						temp2 = temp1;
						i++;
					}
					if(temp2.node != s.routpath[MESSLENGTH - 1].node){
						assert(s.routpath[MESSLENGTH - 1].buff->linkused); 
						s.releaselink = true;
					}
					if(temp2.buff != NULL && temp2.node != s.routpath[MESSLENGTH - 1].node)
					{
						temp2.buff->bufferPlus(temp2.channel, MESSLENGTH);

						temp2.buff->safe[temp2.channel] = SAFE;
						////////////////////////////这里利用safecount进行非安全消息的统计
						//if (s.messagesafe.empty())
						//{

						//}
						//else
						//{
						//	if (s.messagesafe.front() == UNSAFE)
						//	{
						//		temp2.buff->safe_count[temp2.channel]--;
						//		//cout<<temp2.buff->safe_count[temp2.channel]<<endl;
						//		assert(temp2.buff->safe_count[temp2.channel] >= 0 && temp2.buff->safe_count[temp2.channel] <= BUFFSIZE);
						//	}
						//	else
						//	{
						//		//cout<<temp2.buff->safe_count[temp2.channel]<<endl;
						//		assert(temp2.buff->safe_count[temp2.channel] >= 0 && temp2.buff->safe_count[temp2.channel] <= BUFFSIZE);
						//	}
						//	s.messagesafe.pop_front();
						//}
						////////////////////////////
					}

				}
				s.timeout++;
				//	if(s.timeout == TIMEOUT) consumeMes(s);
			}
			//routing completement
			else{
				s.timeout = 0;												
				NodeInfo temp1 ,temp2; // all the flits forward one step
				temp2 = s.routpath[0];
				int i = 1;
				while(i < MESSLENGTH)
				{
					temp1 = s.routpath[i] ;
					s.routpath[i] = temp2;
					temp2 = temp1;
					i++;
				}
				if(temp2.node != s.routpath[MESSLENGTH - 1].node){
					assert(s.routpath[MESSLENGTH - 1].buff->linkused || s.routpath[MESSLENGTH - 1].buff->linkused ); 
					s.releaselink = true;   // the tail shift, release the physical link
				}
				if(temp2.buff != NULL && temp2.node != s.routpath[MESSLENGTH - 1].node) // maybe the last flit do not insert into the network
				{
					temp2.buff->bufferPlus(temp2.channel, MESSLENGTH);
					
					temp2.buff->safe[temp2.channel] = SAFE;
					////////////////////////////这里利用safecount进行非安全消息的统计
					//if (s.messagesafe.empty())
					//{

					//}
					//else
					//{
					//	if (s.messagesafe.front() == UNSAFE)
					//	{
					//		temp2.buff->safe_count[temp2.channel]--;
					//		//cout<<temp2.buff->safe_count[temp2.channel]<<endl;
					//		assert(temp2.buff->safe_count[temp2.channel] >= 0 && temp2.buff->safe_count[temp2.channel] <= BUFFSIZE);
					//	}
					//	else
					//	{
					//		//cout<<temp2.buff->safe_count[temp2.channel]<<endl;
					//		assert(temp2.buff->safe_count[temp2.channel] >= 0 && temp2.buff->safe_count[temp2.channel] <= BUFFSIZE);
					//	}
					//	s.messagesafe.pop_front();
					//}
					////////////////////////////
				}

				s.routpath[0] = *next ;
			}
		}
		//head flit has arrive destination node
		else{
			NodeInfo temp1 ,temp2; // all the flits forward one step
			temp2 = s.routpath[0];
			int i;
			for( i = 1; i < MESSLENGTH && s.routpath[i].node == s.routpath[0].node; i++);
			//all filts arrive destination node
			if( i == MESSLENGTH) {		
				//这里最初的设计属于无限的吸收能力，这里实际的吸收能力应该等同于gP的设置，即完成了最后一步
				//一个时钟周期也只能吸收gP个消息对于每个路由器而言
				//所以这里进行一定的限制
                //这里重新设计的思路即可限定为一个时钟周期只有固定gP个数的消息能够进入判定语句；
				//进行buffer的释放，消息的统计，消息的active释放等
				//gP数量之外的消息跳过此判断，等待下一个时钟周期的吸收
				/*if ((*(this->tor))[s.routpath[i-1].node]->get_router_limit() > 0)
				{
					(*(this->tor))[s.routpath[i-1].node]->minus_router_limit();*/
					s.routpath[i-1].buff->bufferPlus(s.routpath[i-1].channel,MESSLENGTH); // add buffer

					s.routpath[i - 1].buff->safe[s.routpath[i - 1].channel] = SAFE;
					////////////////////////////这里利用safecount进行非安全消息的统计
					//if (s.messagesafe.empty())
					//{

					//}
					//else
					//{
					//	if (s.messagesafe.front() == UNSAFE)
					//	{
					//		temp2.buff->safe_count[temp2.channel]--;
					//		//cout<<temp2.buff->safe_count[temp2.channel]<<endl;
					//		assert(temp2.buff->safe_count[temp2.channel] >= 0 && temp2.buff->safe_count[temp2.channel] <= BUFFSIZE);
					//	}
					//	else
					//	{
					//		//cout<<temp2.buff->safe_count[temp2.channel]<<endl;
					//		assert(temp2.buff->safe_count[temp2.channel] >= 0 && temp2.buff->safe_count[temp2.channel] <= BUFFSIZE);
					//	}
					//	s.messagesafe.pop_front();
					//}
					////////////////////////////
					s.active = false;
					//recode the delay of the message
					totalcir += s.count;
					messarrive++;
					//这里吸收一个，计算一下属于哪个时钟周期吸收的消息，统计在数组中
					//this->circle_accept[cur_circle]++;
					//后面可以计算平均值
					return;
				//}
				////此时钟周期已经吸收完成了，无法进行更多的消息吸收
				//else
				//{
			 //       //等待下一个时钟周期进行吸收，所以这里不进行网络环境的还原，直接返回即可
				//	return;
				//}
			}
			//not all meeage have arrive destination
			while(i < MESSLENGTH){
				temp1 = s.routpath[i];
				s.routpath[i] = temp2;
				temp2 = temp1;
				i++;
			}
			if(temp2.node != s.routpath[MESSLENGTH-1].node)
				s.releaselink = true;   // the tail shift, release the physical link

			if(temp2.buff != NULL && temp2.node != s.routpath[MESSLENGTH - 1].node) // maybe the last flit do not insert
			{
				temp2.buff->bufferPlus(temp2.channel, MESSLENGTH);	

				temp2.buff->safe[temp2.channel] = SAFE;
				////////////////////////////这里利用safecount进行非安全消息的统计
				//if (s.messagesafe.empty())
				//{

				//}
				//else
				//{
				//	if (s.messagesafe.front() == UNSAFE)
				//	{
				//		temp2.buff->safe_count[temp2.channel]--;
				//		//cout<<temp2.buff->safe_count[temp2.channel]<<endl;
				//		assert(temp2.buff->safe_count[temp2.channel] >= 0 && temp2.buff->safe_count[temp2.channel] <= BUFFSIZE);
				//	}
				//	else
				//	{
				//		//cout<<temp2.buff->safe_count[temp2.channel]<<endl;
				//		assert(temp2.buff->safe_count[temp2.channel] >= 0 && temp2.buff->safe_count[temp2.channel] <= BUFFSIZE);
				//	}
				//	s.messagesafe.pop_front();
				//}
				////////////////////////////
			}
		}	
	}
	//cout << s.src << "dst: " << s.dst << endl ;
}
