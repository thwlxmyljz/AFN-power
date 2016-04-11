#pragma once

#ifndef _WIN32
#include <sys/types.h>
#include <netinet/in.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "event2/util.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/event.h"

#include "proto.h"
#include "Mutex.h"
#include <list>
#include <map>
#include <string>

using namespace std;

class AFNPackage;

class Jzq{
public:
	class JzqA1A2{	
	public:
		//��������,0��Ч
		WORD m_areacode;
		//��ַ����,0��Ч
		WORD m_number;
		void Invalid(){
			m_areacode = 0;
			m_number = 0;
		}
		BOOL isOK(){
			return m_areacode != 0 && m_number != 0;
		}
		bool operator==(const JzqA1A2& o){
			return m_areacode == o.m_areacode && m_number == o.m_number;
		}
	} m_a1a2;

	//����
	string m_name;
	//��λ��ʾ����
	/*
	bit0:1�Ѵ������ݿ�����,0δ����
	bit1:1�ѵ�¼��0δ��¼
	*/
	BYTE m_tag;	
	//��Ӧ֡���
	BYTE m_RSEQ;
	//����֡���
	BYTE m_PSEQ;
	//����֡֡��ż�����PFC
	BYTE m_PFC;
	//���������ʱ��
	DWORD m_heart;
	//��վMSAֵ
	static BYTE s_MSA;
public:
	Jzq();
	Jzq(string _name,WORD _areaCode,WORD _number,BYTE _tag);
	~Jzq();
	BOOL operator==(const Jzq& o);
};
//------------------------------------------------------------------------------------
/*����������*/
class Connection
{
	friend class ZjqList;
public:
	Connection(struct event_base *base,struct bufferevent *_bev,evutil_socket_t _fd,struct sockaddr *sa);
	virtual ~Connection(void);
	//���������
	Jzq::JzqA1A2 m_jzq;

public:
	//��ȡ�������ݰ�
	int RecBuf();

	//�������ݰ�
	int SendBuf(const void* cmd,unsigned int cmdlen);
	int SendPkg(const AFNPackage* pkg);
	int SendPkg(std::list<AFNPackage*>& pkgLst);

	//�ж��Ƿ������
	BOOL Compare(struct bufferevent *_bev);

protected:
	void ClearPkgList(std::list<AFNPackage*>& lst);

private:
	//libevent����ͨ��
	struct bufferevent *bev;	
	//fd
	evutil_socket_t fd;
	//�Զ˵�ַ
	sockaddr_in m_remoteAddr;
	//���յ������б�(��֡)
	std::list<AFNPackage*> m_pkgList;
	typedef std::list<AFNPackage*>::iterator Iter;
};
//------------------------------------------------------------------------------------
/*
�����������б�,��������+����������
*/
class JzqList : public list<Connection*>
{
private:
	//�������б�
	list<Jzq*> m_jzqList;
public:	
	typedef list<Connection*>::iterator conIter;
	typedef list<Jzq*>::iterator jzqIter;
public:
	virtual ~JzqList();
	//�����ݿ���ؼ�����
	void LoadJzq();
	//�������б���ӡ
	std::string printJzq();
	Jzq* getJzq(WORD _areacode,WORD _number);
	Jzq* getJzq(const std::string& _name);
	//�µļ���������
	int newConnection(struct event_base *base,evutil_socket_t fd, struct sockaddr *sa);
	//ɾ������������
	void delConnection(struct bufferevent *bev);
	//��ȡ���Ӷ���
	Connection* getConnection(struct bufferevent *bev);
	//��ȡ���������Ӷ���
	Connection* getConnection(WORD _areacode,WORD _number);
	Connection* getConnection(const std::string& _name) ;
	//��¼���
	void ReportLoginState(WORD _areacode,WORD _number,WORD _Fn,BYTE _pseq);
	//��ȡ����������ӦRSEQ
	BYTE GetRSEQ(WORD _areacode,WORD _number,BOOL _increase=TRUE);
	//��ȡ������������PSEQ
	BYTE GetPSEQ(WORD _areacode,WORD _number,BOOL _increase=TRUE);
public:
	/*libevent�¼�����*/
	static void conn_writecb(struct bufferevent *bev, void *user_data);
	static void conn_readcb(struct bufferevent *bev, void *user_data);
	static void conn_eventcb(struct bufferevent *bev, short events, void *user_data);
};
extern Mutex g_JzqConListMutex;
extern JzqList* g_JzqConList;