#pragma once

#include "proto.h"
#include <list>

class AFNPackage : public Pkg
{
	friend class Connection;
	friend class AFNPackageBuilder;

	//填写ACK包的相关字段
	void SetAckType2ZJQ();
	//填写REQ包的相关字段
	void SetReqType2ZJQ();
	//生产CS校验
	void CreateCS();
	//设置长度
	void SetL();
public:
	AFNPackage(void);
	~AFNPackage(void);

	//分析收到的数据帧生产包
	int ParseProto(BYTE* data,DWORD len);
	//包序列化到帧buf中
	int Serialize(BYTE* buf,DWORD bufLen);

	//校验包合法性
	BOOL valid();
	//填写长度以及CS
	void okPkg();

	//计算CS值
	static BYTE GetCS(BYTE* buf,DWORD len);
	//根据pn(1-2040)获取DA1,DA2值(DA2值从1开始)
	static void GetDA(WORD pn,BYTE& DA1,BYTE& DA2);
	//根据DA1,DA2获取pn值(1-2040)
	static WORD Getpn(BYTE DA1,BYTE DA2);
	//根据Fn(1-248)获取DT1,DT2值(DT2值从0开始)
	static void GetDT(WORD Fn,BYTE& DT1,BYTE& DT2);
	//根据DT1,DT2获取Fn值(1-248)
	static WORD GetFn(BYTE DT1,BYTE DT2);
public:
	//应用层包唯一ID计数
	static DWORD s_pkgID;
	//应用层包唯一ID
	DWORD m_nId;
	//Fn,pn,值为对应的n
	WORD Fn;
	WORD pn;
};

