#include "CSemantic.h"

CSemantic::CSemantic()
{
	IdentMap = 
	{
		{ "integer", new CType("integer") },
		{ "real", new CType("real") },
		{ "boolean", new CType("boolean") },
		{ "char", new CType("char") },
		{ "string", new CType("string") }
	};
}

CSemantic::~CSemantic()
{
}

void CSemantic::SetProgName(CSymbol* Symbol, int StrNum)
{
	if (!Symbol) return;
	if (IdentMap.find(Symbol->GetSymbol()) == IdentMap.end())
	{
		IdentMap.insert({ Symbol->GetSymbol(), new CIdent(utProg) });
	}
	else
	{
		Constants::AddError(101, { StrNum, 0 });
	}
}

CType* CSemantic::GetType(CSymbol* Symbol)
{
	if (!Symbol) return nullptr;

	switch (Symbol->SymbolCode)
	{
		case ident: return GetIdentType(Symbol);
		case intc: return (CType*)IdentMap["integer"];
		case floatc: return (CType*)IdentMap["real"];
		case stringc: return (CType*)IdentMap["string"];
		case boolc: return (CType*)IdentMap["boolean"];
		case charc: return (CType*)IdentMap["char"];
		default: return nullptr;
	}
}

CType * CSemantic::GetBaseType(CType* Type)
{
	if (!Type) return nullptr;
	CType* BaseType; 
	string TypeName = Type->TypeName;
	do
	{
		BaseType = static_cast<CType*> (IdentMap[TypeName]);
		TypeName = BaseType ? BaseType->TypeName : "";
	} while (TypeName != "integer"
		&& TypeName != "real"
		&& TypeName != "char"
		&& TypeName != "string"
		&& TypeName != "boolean"
		&& TypeName != "");

	return BaseType;
}

CType * CSemantic::Cast(CType* One, CType* Two, bool IsAssign)
{
	One = GetBaseType(One);
	Two = GetBaseType(Two);
	if (!One && Two) return Two;
	if (One && !Two) return One;
	if (One->TypeName == "integer" && Two->TypeName == "integer"
		|| One->TypeName == "real" && Two->TypeName == "real"
		|| One->TypeName == "boolean" && Two->TypeName == "boolean"
		|| One->TypeName == "char" && Two->TypeName == "char"
		|| One->TypeName == "string" && Two->TypeName == "char"
		|| One->TypeName == "string" && Two->TypeName == "string"
		|| One->TypeName == "real" && Two->TypeName == "integer") 
		return One;

	if (!IsAssign)
	{
		if (One->TypeName == "integer" && Two->TypeName == "real"
			|| One->TypeName == "char" && Two->TypeName == "string") 
			return Two;
	}

	return nullptr;
}

CIdent* CSemantic::CheckDescription(CSymbol* Symbol, int StrNum, eUseType UseType)
{
	if (!Symbol) return nullptr;
	if (IdentMap.find(Symbol->GetSymbol()) == IdentMap.end())
	{
		Constants::AddError(104, { StrNum, 0 });
		IdentMap.insert({ Symbol->GetSymbol(), nullptr });
		return nullptr;
	}
	else
	{
		if (IdentMap[Symbol->GetSymbol()]->GetUseType() != UseType)
		{
			Constants::AddError(100, { StrNum, 0 });
			return nullptr;
		}
	}
	return IdentMap[Symbol->GetSymbol()];
}
//
//void CSemantic::CheckAddOperandType(CSymbol* Symbol, int StrNum)
//{
//	if (!Symbol) return;
//	for (auto op : m_OperandExpression)
//	{
//		if (!Constant(op->SymbolCode) || BoolOperation(op->SymbolCode) || RelationshipOperation(op->SymbolCode))
//		{
//			Constants::AddError(211, { StrNum, 0 });
//			break;
//		}
//		//if(GetIdentType(op)->
//	}
//}

void CSemantic::CheckAssignType(CSymbol* Symbol, int StrNum)
{
	//if (!Symbol || !m_OldSym) return;
	////не забыть, что может быть имя поля записи!
	//if (IdentMap.find(m_OldSym->GetSymbol()) == IdentMap.end()) return;
	////CType* Type = IdentMap[m_OldSym->GetSymbol()];



	/*for (auto op : m_OperandExpression)
	{
		if (!Constant(op->SymbolCode) || BoolOperation(op->SymbolCode) || RelationshipOperation(op->SymbolCode))
		{
			Constants::AddError(145, { StrNum, 0 });
		}
	}*/
}

void CSemantic::CheckBoolExpType(CSymbol* Symbol, int StrNum)
{
	if (!Symbol) return;
}

void CSemantic::CheckDivModOperandType(CSymbol* Symbol, int StrNum)
{
	if (!Symbol) return;
}

void CSemantic::CheckMultOperandType(CSymbol* Symbol, int StrNum)
{
	if (!Symbol) return;
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
	if (IdentMap.find(Ident->GetSymbol()) == IdentMap.end()) return nullptr;
	if (IdentMap[Ident->GetSymbol()]->GetUseType() != utVar) return nullptr;

	return static_cast<CType*> (IdentMap[Ident->GetSymbol()]);
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

void CSemantic::CheckRelatOperandType(CSymbol* Symbol, int StrNum)
{
	if (!Symbol) return;
}

void CSemantic::CheckScalarType(CSymbol* Symbol, int StrNum)
{
	if (!Symbol) return;
	if (IdentMap.find(Symbol->GetSymbol()) != IdentMap.end())
	{
		Constants::AddError(101, { StrNum, 0 });
	}
	else
	{
		CType* Type = new CType(Symbol->GetSymbol());
		IdentMap.insert({ Symbol->GetSymbol(), Type });	
	}
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
	if (IdentMap.find(Symdol->GetSymbol()) == IdentMap.end()) /*ошибка о несущ типе*/ return;

	if (Rec->RecordFields.find(Rec->FlagField.first) != Rec->RecordFields.end())
	{
		Constants::AddError(101, { StrNum, 0 });
		Rec->FlagField = {};
		return;
	}

	CType* Type; string TypeName = Symdol->GetSymbol();
	do 
	{
		Type = static_cast<CType*> (IdentMap[TypeName]);
		TypeName = Type ? Type->TypeName : "";
	} while (TypeName != "integer" 
			 && TypeName != "real" 
			 && TypeName != "char" 
			 && TypeName != "string" 
			 && TypeName != "boolean"
			 && TypeName != "");
	if (Type->TypeName == "" || Type->TypeName != "string" || Type->TypeName != "real")
	{
		Constants::AddError(118, { StrNum, 0 });
		Rec->FlagField = {};
		return;
	}

	Rec->FlagField.second = static_cast<CType*> (IdentMap[Symdol->GetSymbol()]);
}

void CSemantic::AddOperand(CSymbol* Symbol)
{
	if (!Symbol) return;
	//m_OperandExpression.push_back(Symbol->SymbolCode);
}

void CSemantic::SetVarsType(CSymbol* Symbol, int StrNum, bool IsRecordFields)
{
	if (!Symbol) return;
	CType* Type;

	//if (Symbol->SymbolCode == ident)
	{
		if (!CheckDescription(Symbol, StrNum, utType))
		{
			Type = nullptr;
		}
		else
		{
			Type = static_cast<CType*> (IdentMap[Symbol->GetSymbol()]);
		}
	}
	//else
	//{
	//	Type = m_CurrentRecord;
	//}
	for (auto var : m_TempVar)
	{
		CVar* Var = new CVar();
		Var->SetType(Type);
		if(!IsRecordFields)
			if (IdentMap.find(var) == IdentMap.end()) IdentMap.insert({ var, Var });
			else Constants::AddError(101, { StrNum, 0 });
		//else
		//{
		//	if (m_CurrentRecord->RecordFields.find(var) == m_CurrentRecord->RecordFields.end()) 
		//		m_CurrentRecord->RecordFields.insert({ var, Var });
		//	else Constants::AddError(101, { StrNum, 0 });
		//}
	}

	m_TempVar.clear();
}

void CSemantic::SetRecFieldType(CType* Type, vector<string> Vars, CRecordType* Rec, int StrNum)
{
	for (auto var : Vars)
	{
		CVar* Var = new CVar();
		Var->SetType(Type);
		if (Rec->RecordFields.find(var) == Rec->RecordFields.end())
			Rec->RecordFields.insert({ var, Var });
		else Constants::AddError(101, { StrNum, 0 });
	}
	Vars.clear();
}

//void CSemantic::SetCurrentRecord(CSymbol* Symbol, bool IsCheckField)
//{
//	if (!Symbol) return;
//	if(!IsCheckField)
//		m_CurrentRecord = new CRecordType();	
//	else
//	{
//		if (IdentMap.find(Symbol->GetSymbol()) != IdentMap.end())
//		{
//			m_CurrentRecord = static_cast<CRecordType*> (IdentMap[Symbol->GetSymbol()]);
//		}
//	}
//}
