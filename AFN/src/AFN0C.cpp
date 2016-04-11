#include "AFN0C.h"
#include "AFNPackage.h"
#include "YQErrCode.h"
#include "AFNPackageBuilder.h"
#include <sstream>

#define GO() p += len;\
		left -= len;

AFN0CAck_Data_GetClock::AFN0CAck_Data_GetClock()
	:Pkg_Afn_Data()
{
}
AFN0CAck_Data_GetClock::AFN0CAck_Data_GetClock(Pkg_Afn_Data* _origin)
	:Pkg_Afn_Data(*_origin)
{
}
AFN0CAck_Data_GetClock::~AFN0CAck_Data_GetClock(void)
{
}
AFN0CAck_Data_GetClock* AFN0CAck_Data_GetClock::New()
{
	return new AFN0CAck_Data_GetClock();
}
int AFN0CAck_Data_GetClock::HandleData()
{
	return AFNData::parseDateTime6(m_pData,m_nLen,dt);
}
std::string AFN0CAck_Data_GetClock::toString()
{
	return dt;
}
//---------------------------------------------------------------------------------
int AFN0CAck_Data_GetRunStatus::Block::parse(const BYTE*p, int len)
{
	if (len >= 19){
		port = p[0];
		memcpy(&num,p+1,2);
		status = p[3];
		memcpy(&success,p+4,2);
		success_import = p[6];
		AFNData::parseDateTime6(p+7,6,sdt);
		AFNData::parseDateTime6(p+13,6,edt);
		return 19;
	}
	return 0;
}
std::string AFN0CAck_Data_GetRunStatus::Block::toString()
{
	std::ostringstream os;
	os << "  �ն�ͨ�Ŷ˿ں�:" << (int)port << "\r\n";
	os << "  Ҫ���������:" << num << "\r\n";
	os << "  ��ǰ��������״̬��־:" << ((status&0x01)?"���ڳ���,":"δ����,") << ((status&0x02)?"ʱ�������":"ʱ����δ���") << "\r\n";
	os << "  �����ɹ�����:" << success << "\r\n";
	os << "  ���ص���ɹ�����:" << (int)success_import << "\r\n";
	os << "  ������ʼʱ��:" << sdt << "\r\n";
	os << "  ��������ʱ��:" << edt << "\r\n";
	return os.str();
}
AFN0CAck_Data_GetRunStatus::AFN0CAck_Data_GetRunStatus()
	:Pkg_Afn_Data(),pBlock(NULL)
{
}
AFN0CAck_Data_GetRunStatus::AFN0CAck_Data_GetRunStatus(Pkg_Afn_Data* _origin)
	:Pkg_Afn_Data(*_origin),pBlock(NULL)
{
}
AFN0CAck_Data_GetRunStatus::~AFN0CAck_Data_GetRunStatus(void)
{
	delete[] pBlock;
}
AFN0CAck_Data_GetRunStatus* AFN0CAck_Data_GetRunStatus::New()
{
	return new AFN0CAck_Data_GetRunStatus();
}
int AFN0CAck_Data_GetRunStatus::HandleData()
{
	int left = m_nLen;
	BYTE* p = m_pData;
		
	if (left < 1){return YQER_PKG_DATA_Err(2);}
	n = p[0];
	int len = 1;
	if (n <1 ||n > 31){return YQER_PKG_DATA_Err(3);}
	GO()

	pBlock = new Block[n];
	for (int i = 0; i < n; i++){
		len = pBlock[i].parse(p,left);
		if (len == 0)
			return YQER_PKG_DATA_Err(3);
		GO()
	}
	return YQER_OK;
}
std::string AFN0CAck_Data_GetRunStatus::toString()
{
	std::ostringstream os;
	os << "�������ݿ����:" << n << "\r\n";
	for (int i = 0;  i < n ; i++){
		os << "��"<<i+1 <<"���ݿ�" << "\r\n";
		os << pBlock[i].toString();
	}
	return os.str();
}
//---------------------------------------------------------------------------------
AFN0CAck_Data_AllKwh::AFN0CAck_Data_AllKwh()
	:Pkg_Afn_Data(),userkwh_fee(NULL),devkwh_fee(NULL),onekwh_fee(NULL),fourkwh_fee(NULL)
{
}
AFN0CAck_Data_AllKwh::AFN0CAck_Data_AllKwh(Pkg_Afn_Data* _origin)
	:Pkg_Afn_Data(*_origin),userkwh_fee(NULL),devkwh_fee(NULL),onekwh_fee(NULL),fourkwh_fee(NULL)
{
}
AFN0CAck_Data_AllKwh::~AFN0CAck_Data_AllKwh(void)
{
	delete[] userkwh_fee;
	delete[] devkwh_fee;
	delete[] onekwh_fee;
	delete[] fourkwh_fee;
}
AFN0CAck_Data_AllKwh* AFN0CAck_Data_AllKwh::New()
{
	return new AFN0CAck_Data_AllKwh();
}
int AFN0CAck_Data_AllKwh::HandleData()
{
	int left = m_nLen;
	BYTE* p = m_pData;
		
	int len = AFNData::parseDateTime5(p,left,dt);
	if (len <= 0){return YQER_PKG_DATA_Err(2);}
	GO()

	if (left < 1){return YQER_PKG_DATA_Err(2);}
	m = p[0];
	len = 1;
	if (m <1 ||m > 12){return YQER_PKG_DATA_Err(3);}
	GO()

	len = AFNData::parseTable14(p,left,userkwh);
	if (len <= 0){return YQER_PKG_DATA_Err(2);}
	GO()

	userkwh_fee = new float[m];
	for (int i = 0; i < m; i++){
		len = AFNData::parseTable14(p,left,userkwh_fee[i]);
		if (len <= 0){return YQER_PKG_DATA_Err(2);}
		GO()
	}

	len = AFNData::parseTable11(p,left,devkwh);
	GO()

	devkwh_fee = new float[m];
	for (int i = 0; i < m; i++){
		len = AFNData::parseTable11(p,left,devkwh_fee[i]);
		if (len <= 0){return YQER_PKG_DATA_Err(2);}
		GO()
	}

	len = AFNData::parseTable11(p,left,onekwh);
	if (len <= 0){return YQER_PKG_DATA_Err(2);}
	GO()

	onekwh_fee = new float[m];
	for (int i = 0; i < m; i++){
		len = AFNData::parseTable11(p,left,onekwh_fee[i]);
		if (len <= 0){return YQER_PKG_DATA_Err(2);}
		GO()
	}

	len = AFNData::parseTable11(p,left,fourkwh);
	if (len <= 0){return YQER_PKG_DATA_Err(2);}
	GO()

	fourkwh_fee = new float[m];
	for (int i = 0; i < m; i++){
		len = AFNData::parseTable11(p,left,fourkwh_fee[i]);
		if (len <= 0){return YQER_PKG_DATA_Err(2);}
		GO()
	}
	
	return YQER_OK;
}
std::string AFN0CAck_Data_AllKwh::toString()
{
	std::ostringstream os;
	os << "�ն˳���ʱ��:" << dt << "\r\n";
	os << "��ǰ�����й��ܵ���ʾֵ:"<< userkwh << "kWh\r\n";
	for (int i = 0;  i < m ; i++){
		os << "  ��ǰ����" << i+1 << "�����й��ܵ���ʾֵ:"<<userkwh_fee[i]<<"kWh\r\n";
	}
	os << "��ǰ�����޹�������޹�1���ܵ���ʾֵ:"<< devkwh << "kvarh\r\n";
	for (int i = 0;  i < m ; i++){
		os << "  ��ǰ����" << i+1 << "�����޹�������޹�1���ܵ���ʾֵ:"<<devkwh_fee[i]<<"kvarh\r\n";
	}
	os << "��ǰһ�����޹��ܵ���ʾֵ:"<< onekwh << "kvarh\r\n";
	for (int i = 0;  i < m ; i++){
		os << "  ��ǰһ���޷���" << i+1 << "�޹�����ʾֵ:"<<onekwh_fee[i]<<"kvarh\r\n";
	}
	os << "��ǰ�������޹��ܵ���ʾֵ:"<< fourkwh << "kvarh\r\n";
	for (int i = 0;  i < m ; i++){
		os << "  ��ǰ�����޷���" << i+1 << "�޹�����ʾֵ:"<<fourkwh_fee[i]<<"kvarh\r\n";
	}
	return os.str();
}
//---------------------------------------------------------------------------------

AFN0C::AFN0C(void)
{
}
AFN0C::~AFN0C(void)
{
}
int AFN0C::HandleAck(std::list<AFNPackage*>& ackLst)
{	
	if (ackLst.size() == 0){
		return YQER_OK;
	}
	//AFN0C��Ӧ֡
	AFNPackage* ackPkg = *(ackLst.begin());	
	
	if (ackPkg->userHeader.C._C.FUN == Pkg_User_Header::UH_FUNC_SUB8){
		//�û�����
		Pkg_Afn_Data* pData = NULL;
		switch (ackPkg->Fn)
		{
			case 2://F2
				pData = new AFN0CAck_Data_GetClock(ackPkg->pAfn->pAfnData);
				break;
			case 11://F11
				pData = new AFN0CAck_Data_GetRunStatus(ackPkg->pAfn->pAfnData);
				break;
			case 33://F33
				//��ǰ������/�޹�����ʾֵ��һ/�������޹�����ʾֵ���ܡ�����1��M��1��M��12��
				pData = new AFN0CAck_Data_AllKwh(ackPkg->pAfn->pAfnData);
				break;
			default:
				break;
		} 
		if (pData){
			pData->HandleData();
		}
		AppCall call;
		call.AFN = ackPkg->pAfn->afnHeader.AFN;
		call.m_areacode = ackPkg->userHeader.A1;
		call.m_number = ackPkg->userHeader.A2;
		AFNPackageBuilder::Instance().Notify(call,pData);
	}
	else if(ackPkg->userHeader.C._C.FUN == Pkg_User_Header::UH_FUNC_SUB9){
		//���ϣ������ٻ�������
		AppCall call;
		call.AFN = ackPkg->pAfn->afnHeader.AFN;
		call.m_areacode = ackPkg->userHeader.A1;
		call.m_number = ackPkg->userHeader.A2;
		AFNPackageBuilder::Instance().Notify(call,(Pkg_Afn_Data*)0);
	}
	return -1;
}

int AFN0C::Create(WORD pn,WORD Fn)
{
	afnHeader.AFN = Pkg_Afn_Header::AFN0C;
	afnHeader.SEQ._SEQ.CON = Pkg_Afn_Header::SEQ_CON_MBANSWER;
	afnHeader.SEQ._SEQ.FIN = 1;
	afnHeader.SEQ._SEQ.FIR = 1;
	afnHeader.SEQ._SEQ.TPV = Pkg_Afn_Header::SEQ_TPV_NO;

	pAfnData = new Pkg_Afn_Data();
	AFNPackage::GetDA(pn,pAfnData->m_Tag.DA1,pAfnData->m_Tag.DA2);
	AFNPackage::GetDT(Fn,pAfnData->m_Tag.DT1,pAfnData->m_Tag.DT2);
	
	Pkg_Afn_Aux_Down* p = new Pkg_Afn_Aux_Down(FALSE,TRUE);
	pAux = p;
	return YQER_OK;
}
int AFN0C::CreateClock()
{
	return Create(0/*p0*/,2);
}
int AFN0C::CreateRunStatus()
{
	return Create(0/*p0*/,11);
}
int AFN0C::CreateCurTotalKwh()
{
	return Create(0/*�ܼ����?*/,17);
}		
int AFN0C::CreateCurTotalKvarh()
{
	return Create(0/*�ܼ����?*/,18);
}
int AFN0C::CreateCurABCTotal(WORD pn)
{
	return Create(pn,25);
}
int AFN0C::CreateCurABCTotal(WORD* pN,int len)
{
	return 0;
}
int AFN0C::CreateAllKwh(WORD pn)
{
	return Create(pn,33);
}
int AFN0C::CreateAllKwh(WORD* pN,int len)
{
	return 0;
}