
#include "StdAfx.h"

#include <sqplus.h>



SQWindowHost::SQWindowHost(SquirrelObject &so):m_Instance(so)
{
	SquirrelObject msgtab=SquirrelVM::CreateTable();
	
	msgtab.SetValue(_T("sType"),"NULL");

	msgtab.SetValue(_T("pSender"),0);

	msgtab.SetValue(_T("dwTimestamp"),0);
	msgtab.SetValue(_T("wParam"),0);
	msgtab.SetValue(_T("lParam"),0);



	SquirrelObject ptMouse=SquirrelVM::CreateTable();
	msgtab.SetValue(_T("ptMouse"),ptMouse);
	ptMouse.SetValue(_T("x"),0);
	ptMouse.SetValue(_T("y"),0);
	m_Notify=msgtab;

}
SQWindowHost::~SQWindowHost()
{

}

UINT SQWindowHost::GetClassStyle() 
{

	HSQUIRRELVM v=SquirrelVM::GetVMPtr();
	UINT ret=0;
	SquirrelObject   func   = m_Instance.GetValue(_T("GetClassStyle"));
	if (func.GetType()==OT_CLOSURE)
	{

		sq_pushobject(v,func.GetObjectHandle());
		sq_pushobject(v,m_Instance.GetObjectHandle());
		SQPLUS_CHECK_FNCALL(sq_call(v,1,SQTrue,SQ_CALL_RAISE_ERROR));
		ret=SqPlus::GetRet(	SqPlus::TypeWrapper<UINT>(),v,-1);	


	}
	return ret;
}

void SQWindowHost::OnFinalMessage(HWND /*hWnd*/)
{

	HSQUIRRELVM v=SquirrelVM::GetVMPtr();
	LPCTSTR ret=NULL;
	SquirrelObject   func   = m_Instance.GetValue(_T("OnFinalMessage"));
	if (func.GetType()==OT_CLOSURE)
	{

		sq_pushobject(v,func.GetObjectHandle());
		sq_pushobject(v,m_Instance.GetObjectHandle());
		SQPLUS_CHECK_FNCALL(sq_call(v,1,SQTrue,SQ_CALL_RAISE_ERROR));

		

	}
base::Debug::printf(_T("OnFinalMessage()%s"),GetWindowClassName());
	delete this;

}
LPCTSTR SQWindowHost::GetWindowClassName() 
{
	HSQUIRRELVM v=SquirrelVM::GetVMPtr();
	LPCTSTR ret=NULL;
	SquirrelObject   func   = m_Instance.GetValue(_T("GetWindowClassName"));
	if (func.GetType()==OT_CLOSURE)
	{

		sq_pushobject(v,func.GetObjectHandle());
		sq_pushobject(v,m_Instance.GetObjectHandle());
		SQPLUS_CHECK_FNCALL(sq_call(v,1,SQTrue,SQ_CALL_RAISE_ERROR));

		ret=	SqPlus::GetRet(	SqPlus::TypeWrapper<LPCTSTR>(),v,-1);	

	}
	return ret;
}
void SQWindowHost::Notify(TNotifyUI& msg)
{

	HSQUIRRELVM v=SquirrelVM::GetVMPtr();
	SquirrelObject   func   = m_Instance.GetValue(_T("Notify"));
	if (func.GetType()==OT_CLOSURE)
	{
		sq_pushobject(v,func.GetObjectHandle());
		sq_pushobject(v,m_Instance.GetObjectHandle());

		SquirrelObject msgtab=m_Notify;
		
		msgtab.SetValue(_T("sType"),msg.sType.c_str());

		SquirrelObject pSender=SquirrelVM::CreateInstance(SquirrelVM::GetRootTable().GetValue(_T("Control")));

		pSender.SetInstanceUP(msg.pSender);
		msgtab.SetValue(_T("pSender"),pSender);

		msgtab.SetValue(_T("dwTimestamp"),(int)msg.dwTimestamp);
		msgtab.SetValue(_T("wParam"),(int)msg.wParam);
		msgtab.SetValue(_T("lParam"),msg.lParam);
	
		
		SquirrelObject ptMouse=msgtab.GetValue(_T("ptMouse"));
		//msgtab.SetValue(_T("ptMouse"),ptMouse);
		ptMouse.SetValue(_T("x"),msg.ptMouse.x);
		ptMouse.SetValue(_T("y"),msg.ptMouse.y);
	
		SqPlus::Push(v,msgtab);

		SQPLUS_CHECK_FNCALL(sq_call(v,2,SQTrue,SQ_CALL_RAISE_ERROR));


 

		SquirrelObject ret=	SqPlus::GetRet(	SqPlus::TypeWrapper<SquirrelObject>(),v,-1);

		CControlUI* pS=(CControlUI*)ret.GetInstanceUP(0);


	}

	
}

LRESULT SQWindowHost::SQHandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool handle=false;
	int retValue=0;
	{
		if (uMsg==WM_NCHITTEST)
			uMsg=WM_NCHITTEST;

		HSQUIRRELVM v=SquirrelVM::GetVMPtr();
		SquirrelObject   func   = m_Instance.GetValue(_T("MessageHandle"));
		if (func.GetType()==OT_CLOSURE)
		{



			sq_pushobject(v,func.GetObjectHandle());
			sq_pushobject(v,m_Instance.GetObjectHandle());
			SqPlus::Push(v,uMsg);
			SqPlus::Push(v,wParam);
			SqPlus::Push(v,lParam);

			SQPLUS_CHECK_FNCALL(sq_call(v,4,SQTrue,SQ_CALL_RAISE_ERROR));




			SquirrelObject ret=	SqPlus::GetRet(	SqPlus::TypeWrapper<SquirrelObject>(),v,-1);

			handle=ret.GetBool(1);
			retValue=ret.GetInt(0);




		}
	}

	if (!handle)
		return CWindowHost::HandleMessage(uMsg, wParam, lParam);
	else
		return retValue;

}





