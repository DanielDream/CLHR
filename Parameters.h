#ifndef  Parameters_H
#define Paremeters_H
//总的时钟周期
#define TOTALCICLE    5000
//消息的长度，单位微片
#define MESSLENGTH    16
//消息产生到路由过程的等待时间
#define PROCESSTIME   16
//延时时间，这里废弃
#define TIMEOUT		  2000
//网络规模，这里废弃
#define NetworkSize   4
//虚拟通道
#define VC1		1
//虚拟通道
#define VC2		2
//虚拟通道
#define VC3		3

//BUFF的大小
//动态设置，BUFF=1的时候xd算法能够取得最好的效果
#define BUFFSIZE 1
//流量模型的起始位置的定义
//0:ADV+1,1:ADV+2,2:ADV+3,3:ADV+4 4:UNIFORM
#define TRAFFIC_START 0
#define TRAFFIC_END   1
//路由函数的起始位置的定义
//0:MFR3 1:VAL 2:MFR2 3:PB1 4:PB  5:MIN 6:SAMEVC
#define ROUTING_START 0
#define ROUTING_END   1
//using conjestion information
//CONJESTION_AWARE = 1 启用冲突检测
//CONJESTION_AWARE = 0 不启用冲突检测
#define CONJESTION_AWARE 0
//using global information
//LOCAL = 0 启用全局冲突检测
//LOCAL = 1 启用局部冲突检测
#define LOCAL 0

//每个路由器的簇内组间全局link
#define gPGL 3
//每个路由器的簇间全局link
#define gPCL 3
//每个路由器的全局link
#define gP gPGL+gPCL
//每组路由组的路由器数量
#define gA 12//(2*gP)
//每个路由器的端口数量
#define rtr_num_ports    gA - 1 + gP//(2*gP-1 + gP)
//组内路由器数量
#define grp_num_routers  gA//(2*gP)
//每个簇中的组数
#define cluster_num_grps 36
//网络中的簇的数量
#define net_num_clusters 7

//网络中的路由器数量
#define net_num_routers (grp_num_routers * net_num_clusters * cluster_num_grps)
//组中的全局端口数
//#define grp_num_ports    gP * grp_num_routers
//K
#define K 6
#define betweencluster_num_links K*cluster_num_grps


//簇内组间link数
#define grp_num_ports    (gPGL * grp_num_routers +1)
//----------------------组内全局端口总数??????????????----------------------
#define net_num_ports    (gP * grp_num_routers +1)
//网络中的最大组数
#define net_num_grps    net_num_clusters * cluster_num_grps// the biggest groups : (gP * grp_num_routers +1)
//网络中的路由器数量
//#define net_num_routers (grp_num_routers * net_num_grps)

#define CLUSTER(x) (x)/(grp_num_routers*cluster_num_grps)
#define GROUP(x) ((x)%(grp_num_routers*cluster_num_grps))/grp_num_routers
#define GLOBALGROUP(x) (x)/grp_num_routers
#define IDINGROUP(x) ((x)%(grp_num_routers*cluster_num_grps))%grp_num_routers



//不可达
#define not_arrive -1
//消息的类型定义
#define SAFE   0
#define UNSAFE 1

//内存泄露的检测声明
#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
#endif
