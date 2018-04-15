#pragma once
#include "CIdent.h"

class CScope
{
private:
	CScope* m_Parent;
	map<string, CIdent*> m_IdentMap;
public:
	CScope(CScope* Parent);
	~CScope();

	CIdent* GetIdent(string Key);
	CScope* GetParent() { return m_Parent; }
	void AddIdent(string Name, CIdent* Ident, int StrNum);
	CIdent* CheckDescription(string Name, eUseType UseType, int StrNum);
};

