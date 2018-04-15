#include "CScope.h"

CScope::CScope(CScope* Parent) : m_Parent(Parent)
{
}

CScope::~CScope()
{
	delete m_Parent;
	for (auto ident : m_IdentMap)
	{
		delete ident.second;
	}
}

CIdent* CScope::GetIdent(string Key)
{
	if (m_IdentMap.find(Key) == m_IdentMap.end())
	{
		if (m_Parent)
		{
			return m_Parent->GetIdent(Key);
		}
		else
		{
			return nullptr;
		}
	}

	return m_IdentMap[Key];
}

void CScope::AddIdent(string Name, CIdent* Ident, int StrNum)
{
	if (m_IdentMap.find(Name) == m_IdentMap.end())
	{
		m_IdentMap.insert({ Name, Ident});
	}
	else
	{
		Constants::AddError(101, { StrNum, 0 });
		delete Ident;
	}
}

CIdent* CScope::CheckDescription(string Name, eUseType UseType, int StrNum)
{
	CIdent* Id;
	if (m_IdentMap.find(Name) == m_IdentMap.end())
	{
		if (m_Parent)
		{
			return m_Parent->CheckDescription(Name, UseType, StrNum);
		}
		Constants::AddError(104, { StrNum, 0 });
		m_IdentMap.insert({ Name, nullptr });
		return nullptr;
	}
	else
	{
		if (Id = m_IdentMap[Name])
		{
			if (Id->GetUseType() != UseType)
			{
				Constants::AddError(100, { StrNum, 0 });
				return nullptr;
			}
		}
	}

	return Id;
}
