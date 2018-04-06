#pragma once
#include "CSymbol.h"
#include <algorithm>
#include "Constants.h"
#include <vector>

using namespace std;

class CIdent
{
protected:
	eUseType m_UseType;

public:
	CIdent(eUseType UseType);
	eUseType GetUseType() { return m_UseType; }
	~CIdent();
};

class CType : public CIdent
{
protected:
	eCustomType m_Type;

public:
	eCustomType GetCustType() { return m_Type; }
	CType(string TypeName, eCustomType Type = tScalar);
	string TypeName;
	~CType() {}

};

class CVar : public CIdent
{
private:
	CType* m_VarType;

public:
	CVar();

	CType* GetType() { return m_VarType; }
	void SetType(CType* Value) { m_VarType = Value; }
};


class CRecordType : public CType
{
public:
	map<string, CIdent*> RecordFields;
	pair<string, CIdent*> FlagField;
	//вар. поля. Название, значение и тип
	map<CRecordType*, vector<CValue*>> RecordVarFields;
	CRecordType();
	CRecordType(map<string, CIdent*> Fields);
};