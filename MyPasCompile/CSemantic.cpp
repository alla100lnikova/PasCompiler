#include "CSemantic.h"

CSemantic::CSemantic()
{
	m_BaseScope = new CScope(nullptr);
	m_BaseScope->AddIdent("integer", dynamic_cast<CIdent*>(new CType("integer")), 0);
	m_BaseScope->AddIdent("real", dynamic_cast<CIdent*>(new CType("real")), 0);
	m_BaseScope->AddIdent("boolean", dynamic_cast<CIdent*>(new CType("boolean")), 0);
	m_BaseScope->AddIdent("char", dynamic_cast<CIdent*>(new CType("char")), 0);
	m_BaseScope->AddIdent("string", dynamic_cast<CIdent*>(new CType("string")), 0);

	m_CurrentScope = new CScope(m_BaseScope);
}

CSemantic::~CSemantic()
{
	delete m_CurrentScope;
}

void CSemantic::SetProgName(CSymbol* Symbol, int StrNum)
{
	if (!Symbol) return;
	string Name = Symbol->GetSymbol();
	m_CurrentScope->AddIdent(Name, new CIdent(utProg, Name), StrNum);
}

CType* CSemantic::GetType(CSymbol* Symbol)
{
	if (!Symbol) return nullptr;

	switch (Symbol->SymbolCode)
	{
		case ident: return GetIdentType(Symbol);
		case intc: return (CType*)m_BaseScope->GetIdent("integer");
		case floatc: return (CType*)m_BaseScope->GetIdent("real");
		case stringc: return (CType*)m_BaseScope->GetIdent("string");
		case boolc: return (CType*)m_BaseScope->GetIdent("boolean");
		case charc: return (CType*)m_BaseScope->GetIdent("char");
		default: return nullptr;
	}
}

CType* CSemantic::GetBaseType(CType* Type)
{
	if (!Type) return nullptr;

	if (Type->GetName() == "") return Type;
	return Type->GetBaseType(); 
}

CType * CSemantic::Cast(CType* One, CType* Two, bool IsAssign)
{
	One = GetBaseType(One);
	Two = GetBaseType(Two);
	if (!One && !Two) return nullptr;
	if (!One && Two) return Two;
	if (One && !Two) return One;
	if (One->GetName() == "integer" && Two->GetName() == "integer"
		|| One->GetName() == "real" && Two->GetName() == "real"
		|| One->GetName() == "boolean" && Two->GetName() == "boolean"
		|| One->GetName() == "char" && Two->GetName() == "char"
		|| One->GetName() == "string" && Two->GetName() == "char"
		|| One->GetName() == "string" && Two->GetName() == "string"
		|| One->GetName() == "real" && Two->GetName() == "integer") 
		return One;

	if (!IsAssign)
	{
		if (One->GetName() == "integer" && Two->GetName() == "real"
			|| One->GetName() == "char" && Two->GetName() == "string") 
			return Two;
	}

	return nullptr;
}

CIdent* CSemantic::CheckDescription(CSymbol* Symbol, int StrNum, eUseType UseType)
{
	if (!Symbol) return nullptr;	
	return m_CurrentScope->CheckDescription(Symbol->GetSymbol(), UseType, StrNum);
}

//void CSemantic::AddCheckRecord(CSymbol* Symbol, int StrNum)
//{
//	if (!Symbol) return;
//	if (!m_CurrentRecord)
//	{
//		CRecordType* Rec = static_cast<CRecordType*> (IdentMap[Symbol->GetSymbol()]);
//		m_CurrentRecord = Rec;
//	}
//	else
//	{
//		CRecordType* Rec = static_cast<CRecordType*> (m_CurrentRecord->RecordFields[Symbol->GetSymbol()]);
//		m_CurrentRecord = Rec;
//	}
//}


CType* CSemantic::GetIdentType(CSymbol* Ident)
{
	return (CType*) m_CurrentScope->GetIdent(Ident->GetSymbol());
}

bool CSemantic::CheckRecordFields(CRecordType* Rec, CSymbol* Symbol, int StrNum)
{
	if (!Symbol) return false;
	
	if (Rec != nullptr) 
	{
		if (Rec->RecordFields.find(Symbol->GetSymbol()) == Rec->RecordFields.end())
		{
			bool IsFind = false;
			for (auto rec : Rec->RecordFields)
			{
				CType* Type = (CType*)rec.second;
				if (Type->GetCustType() == tRecord) IsFind = CheckRecordFields((CRecordType*)Type, Symbol, StrNum);
				if (IsFind) return true;
			}

			Constants::AddError(152, { StrNum, 0 });
			return false;
		}
	}
	else
	{
		Constants::AddError(140, { StrNum, 0 });
		return false;
	}
}

void CSemantic::CheckScalarType(CSymbol* Symbol, CType* BaseType, int StrNum)
{
	if (!Symbol) return;

	CType* Type = new CType(Symbol->GetSymbol());
	Type->SetBaseType(BaseType);
	m_CurrentScope->AddIdent(Symbol->GetSymbol(), dynamic_cast<CIdent*>(Type), StrNum);
}

void CSemantic::AddVar(CSymbol* Symbol, int StrNum)
{
	if (!Symbol) return;
	m_TempVar.push_back(Symbol->GetSymbol());
}

void CSemantic::CheckVarRecordPartType(CRecordType* Rec, CRecordType* ChildRec, vector<CValue*> Val, int StrNum)
{
	if (!Rec) return;
	bool IsValidVal = true;
	for (auto val : Val)
	{
		if (val->GetValueType() == vtReal || val->GetValueType() == vtString)
		{
			Constants::AddError(147, { StrNum, 0 });
			IsValidVal = false;
			break;
		}
	}

	if (IsValidVal)
	{
		if (Rec->RecordVarFields.find(ChildRec) == Rec->RecordVarFields.end())
			Rec->RecordVarFields.insert({ ChildRec,  Val });
	}
}

void CSemantic::AddFlagVarPart(CRecordType* Rec, CSymbol* Symdol, int StrNum)
{
	if (!Symdol) return;

	CType* Type = nullptr;
	if (!(Type = (CType*)(m_CurrentScope->CheckDescription(Symdol->GetSymbol(), utType, StrNum)))) return;

	if (Rec->RecordFields.find(Rec->FlagField->GetName()) != Rec->RecordFields.end())
	{
		Constants::AddError(101, { StrNum, 0 });
		Rec->FlagField = nullptr;
		return;
	}

	string TypeName = Symdol->GetSymbol();
	if (TypeName != "")
	{
		Type = Type->GetBaseType();
	}

	if (Type->GetName() == "" || Type->GetName() != "string" || Type->GetName() != "real")
	{
		Constants::AddError(118, { StrNum, 0 });
		Rec->FlagField = {};
		return;
	}

	Rec->FlagField->SetBaseType(Type);
}

void CSemantic::SetVarsType(CType* Type, int StrNum)
{
	for (auto var : m_TempVar)
	{
		CVar* Var = new CVar(var);
		Var->SetBaseType(Type);
		m_CurrentScope->AddIdent(var, dynamic_cast<CIdent*>(Var), StrNum);
	}

	m_TempVar.clear();
}

void CSemantic::SetRecFieldType(CType* Type, vector<string>& Vars, CRecordType* Rec, int StrNum)
{
	for (auto var : Vars)
	{
		CVar* Var = new CVar(var);
		Var->SetBaseType(Type);
		if (Rec->RecordFields.find(var) == Rec->RecordFields.end())
			Rec->RecordFields.insert({ var, Var });
		else Constants::AddError(101, { StrNum, 0 });
	}
	Vars.clear();
}
