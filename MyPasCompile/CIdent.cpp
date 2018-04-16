#include "CIdent.h"

CIdent::CIdent(eUseType UseType, string Name)
	: m_UseType(UseType)
	, m_Name(Name)
{
}

CIdent::~CIdent()
{
}

CType::CType(string TypeName, eCustomType Type) : CTypedIdent(utType, TypeName)
	, m_Type(Type)
{
}

CRecordType::CRecordType() : 
	CType("", tRecord),
	FlagField(nullptr),
	RecordFields({}),
	RecordVarFields({})
{
}

CRecordType::~CRecordType()
{
	delete FlagField;
	for (auto var : RecordFields)
	{
		delete var.second;
	}

	for (auto var : RecordVarFields)
	{
		delete var.first;
	}
}

CVar::CVar(string Name) : CTypedIdent(utVar, Name)
{
}
