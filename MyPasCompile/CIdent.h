#pragma once
#include "CSymbol.h"
#include <algorithm>
#include "Constants.h"
#include <vector>
using namespace std;

class CIdent
{
protected:
	string m_Name;
	eUseType m_UseType;

public:
	CIdent(eUseType UseType, string Name);
	eUseType GetUseType() { return m_UseType; }
	string GetName() { return m_Name; }
	void SetName(string Name) { m_Name = Name; }
	virtual ~CIdent();
};

class CType;
class CTypedIdent : public CIdent
{
protected:
	CType* m_Type;

public:
	CTypedIdent(eUseType UseType, string TypeName) : CIdent(UseType, TypeName) {}
	CType* GetBaseType() { return m_Type; }
	void SetBaseType(CType* Type) { m_Type = Type; }
	virtual ~CTypedIdent() {}
};

class CType : public CTypedIdent
{
protected:
	eCustomType m_Type;

public:
	eCustomType GetCustType() { return m_Type; }
	CType(string TypeName, eCustomType Type = tScalar);
	virtual ~CType() {}
};

class CVar : public CTypedIdent
{
public:
	CVar(string Name);
};

class CRecordType : public CType
{
public:
	map<string, CVar*> RecordFields;
	CVar* FlagField;
	//вар. поля. Название, значение и тип
	map<CRecordType*, vector<CValue*>> RecordVarFields;
	CRecordType();
	~CRecordType();
};
