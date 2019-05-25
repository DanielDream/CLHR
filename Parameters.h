#ifndef  Parameters_H
#define Paremeters_H
//�ܵ�ʱ������
#define TOTALCICLE    5000
//��Ϣ�ĳ��ȣ���λ΢Ƭ
#define MESSLENGTH    16
//��Ϣ������·�ɹ��̵ĵȴ�ʱ��
#define PROCESSTIME   16
//��ʱʱ�䣬�������
#define TIMEOUT		  2000
//�����ģ���������
#define NetworkSize   4
//����ͨ��
#define VC1		1
//����ͨ��
#define VC2		2
//����ͨ��
#define VC3		3

//BUFF�Ĵ�С
//��̬���ã�BUFF=1��ʱ��xd�㷨�ܹ�ȡ����õ�Ч��
#define BUFFSIZE 1
//����ģ�͵���ʼλ�õĶ���
//0:ADV+1,1:ADV+2,2:ADV+3,3:ADV+4 4:UNIFORM
#define TRAFFIC_START 0
#define TRAFFIC_END   1
//·�ɺ�������ʼλ�õĶ���
//0:MFR3 1:VAL 2:MFR2 3:PB1 4:PB  5:MIN 6:SAMEVC
#define ROUTING_START 0
#define ROUTING_END   1
//using conjestion information
//CONJESTION_AWARE = 1 ���ó�ͻ���
//CONJESTION_AWARE = 0 �����ó�ͻ���
#define CONJESTION_AWARE 0
//using global information
//LOCAL = 0 ����ȫ�ֳ�ͻ���
//LOCAL = 1 ���þֲ���ͻ���
#define LOCAL 0

//ÿ��·�����Ĵ������ȫ��link
#define gPGL 3
//ÿ��·�����Ĵؼ�ȫ��link
#define gPCL 3
//ÿ��·������ȫ��link
#define gP gPGL+gPCL
//ÿ��·�����·��������
#define gA 12//(2*gP)
//ÿ��·�����Ķ˿�����
#define rtr_num_ports    gA - 1 + gP//(2*gP-1 + gP)
//����·��������
#define grp_num_routers  gA//(2*gP)
//ÿ�����е�����
#define cluster_num_grps 36
//�����еĴص�����
#define net_num_clusters 7

//�����е�·��������
#define net_num_routers (grp_num_routers * net_num_clusters * cluster_num_grps)
//���е�ȫ�ֶ˿���
//#define grp_num_ports    gP * grp_num_routers
//K
#define K 6
#define betweencluster_num_links K*cluster_num_grps


//�������link��
#define grp_num_ports    (gPGL * grp_num_routers +1)
//----------------------����ȫ�ֶ˿�����??????????????----------------------
#define net_num_ports    (gP * grp_num_routers +1)
//�����е��������
#define net_num_grps    net_num_clusters * cluster_num_grps// the biggest groups : (gP * grp_num_routers +1)
//�����е�·��������
//#define net_num_routers (grp_num_routers * net_num_grps)

#define CLUSTER(x) (x)/(grp_num_routers*cluster_num_grps)
#define GROUP(x) ((x)%(grp_num_routers*cluster_num_grps))/grp_num_routers
#define GLOBALGROUP(x) (x)/grp_num_routers
#define IDINGROUP(x) ((x)%(grp_num_routers*cluster_num_grps))%grp_num_routers



//���ɴ�
#define not_arrive -1
//��Ϣ�����Ͷ���
#define SAFE   0
#define UNSAFE 1

//�ڴ�й¶�ļ������
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
