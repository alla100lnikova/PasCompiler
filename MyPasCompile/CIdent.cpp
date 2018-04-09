#include "CIdent.h"

CIdent::CIdent(eUseType UseType)
	: m_UseType(UseType)
{
}

CIdent::~CIdent()
{
}

CType::CType(string TypeName, string BaseType, eCustomType Type) : CIdent(utType)
	, m_Type(Type)
	, TypeName(TypeName)
	, BaseTypeName(BaseType)
{
}

CRecordType::CRecordType() : CType("", "", tRecord)
{
}

CRecordType::CRecordType(map<string, CIdent*> Fields) : CType("", "", tRecord)
	, RecordFields(Fields) 
{
}

CVar::CVar() : CIdent(utVar)
{
}
