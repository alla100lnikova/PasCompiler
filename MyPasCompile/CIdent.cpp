#include "CIdent.h"

CIdent::CIdent(eUseType UseType)
	: m_UseType(UseType)
{
}

CIdent::~CIdent()
{
}

CType::CType(string TypeName, eCustomType Type) : CIdent(utType)
	, m_Type(Type)
	, TypeName(TypeName)
{
}

CRecordType::CRecordType() : CType("", tRecord)
{
}

CRecordType::CRecordType(map<string, CIdent*> Fields) : CType("", tRecord)
	, RecordFields(Fields) 
{
}

CVar::CVar() : CIdent(utVar)
{
}
