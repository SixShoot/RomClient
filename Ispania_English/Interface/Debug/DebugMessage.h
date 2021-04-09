#pragma once

#include "../interface.h"

class CDebugMessage;
extern CDebugMessage*				g_pDebugMessage;

class CDebugMessage : public CInterfaceBase
{
public:
	CDebugMessage(CInterface* object);
	virtual ~CDebugMessage();

	virtual	void				RegisterFunc();
	virtual	void				DebugOutput(int type, const char* format, ...);
protected:
};