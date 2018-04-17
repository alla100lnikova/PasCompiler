#include "CParser.h"

void CParser::AddErrorAndSkip(int ErrorCode, vector<eOperator> SkipSym)
{
	if (ErrorCode != 0) Constants::AddError(ErrorCode, { Lexer->GetCurrentStr(), 0 });
	if (SkipSym.size() != 0) SkipTo(SkipSym);
}

bool CParser::Accept(eOperator Next, bool GetNext /* = true */)
{
	if (GetNext) LexerGiveMeSymbolBistra();
	return m_pCurrentSymbol && m_pCurrentSymbol->SymbolCode == Next;
}

void CParser::SkipTo(vector<eOperator> SkipSym)
{
	while (m_pCurrentSymbol && find(SkipSym.begin(), SkipSym.end(), m_pCurrentSymbol->SymbolCode) == SkipSym.end())
	{
		LexerGiveMeSymbolBistra();
	}
}

void CParser::Program()
{
	if (Accept(programsy, false))
	{
		LexerGiveMeSymbolBistra();
		if (!Accept(ident, false))
		{
			AddErrorAndSkip(2, {});
			LexerGiveMeSymbolBistra();
		}
		else
		{
			Sem->SetProgName(m_pCurrentSymbol, Lexer->GetCurrentStr());
		}
		if (LexerGiveMeSymbolBistra() && FileList()) LexerGiveMeSymbolBistra();
	}
	else
	{
		AddErrorAndSkip(3, { point });
		Lexer->Print();
		return;
	}

	ProgramBlock();
	if (!Accept(point, false))
	{
		AddErrorAndSkip(61, {});
	}
	Lexer->Print();
}

bool CParser::FileList()
{
	if (Accept(semicolon, false)) return true;
	if (Accept(leftpar, false))
	{
		while (Accept(ident))
		{
			Sem->SetProgName(m_pCurrentSymbol, Lexer->GetCurrentStr());
			if (Accept(comma)) continue;
			else
			{
				if (!Accept(rightpar, false))
					AddErrorAndSkip(20, { rightpar, semicolon, typesy, varsy, beginsy, ifsy, whilesy, withsy, endsy, point });
				break;
			}
		}

		if (Accept(rightpar, false))
		{
			if (Accept(semicolon)) return true;
			else
			{
				AddErrorAndSkip(14, { typesy, varsy, beginsy, ifsy, whilesy, withsy, endsy, point });
				return false;
			}
		}
		else
		{
			AddErrorAndSkip(4, { semicolon, typesy, varsy, beginsy, ifsy, whilesy, withsy, endsy, point });
			return false;
		}
	}

	AddErrorAndSkip(14, { semicolon, typesy, varsy, beginsy, ifsy, whilesy, withsy, endsy, point });
	return false;
}

void CParser::ProgramBlock()
{
	if (m_pCurrentSymbol)
	{
		switch (m_pCurrentSymbol->SymbolCode)
		{
		case typesy:
			if (!TypesBlock())
			{
				AddErrorAndSkip(18, {});
			}
			break;
		case varsy:
			if (!VarBlock()) AddErrorAndSkip(18, {});
			break;
		case beginsy:
			if (LexerGiveMeSymbolBistra()) OperatorBlock();
			else AddErrorAndSkip(13, {});
			break;
		default:
			AddErrorAndSkip(17, {beginsy, point});
			OperatorBlock();
			break;
		}
	}
}

bool CParser::TypesBlock()
{
	LexerGiveMeSymbolBistra();
	while ((!Accept(endsy, false)
			&& !Accept(varsy, false)
			&& !Accept(beginsy, false)
			&& !Accept(ifsy, false)
			&& !Accept(whilesy, false)
			&& !Accept(withsy, false)))
	{
		if (TypeDescription())
		{
			if (!Accept(semicolon))
			{
				AddErrorAndSkip(14, { varsy, beginsy, ifsy, whilesy, withsy, endsy, point });
				break;
			}
			LexerGiveMeSymbolBistra();
		}
	}

	if (m_pCurrentSymbol)
	{
		switch (m_pCurrentSymbol->SymbolCode)
		{
		case varsy:
			if (!VarBlock())
			{
				AddErrorAndSkip(17, {});
				return false;
			}
			break;
		case beginsy:
			if (LexerGiveMeSymbolBistra()) OperatorBlock();
			else
			{
				AddErrorAndSkip(17, { beginsy, point });
				return false;
			}
			break;
		default:
			AddErrorAndSkip(17, { beginsy, point });
			OperatorBlock();
			break;
		}
	}
	else
	{
		AddErrorAndSkip(17, { beginsy, point });
		return false;
	}
	return true;
}

bool CParser::TypeDescription()
{
	if (Accept(ident, false))
	{
		string TypeSymbol = m_pCurrentSymbol->GetSymbol();
		if (Accept(equalsy))
		{
			if (LexerGiveMeSymbolBistra())
			{
				CType* BaseType = Type();
				if (BaseType && BaseType->GetCustType() != tRecord) BaseType = BaseType->GetBaseType();
				Sem->AddCustomType(TypeSymbol, BaseType, Lexer->GetCurrentStr());
			}
			else
			{
				AddErrorAndSkip(18, {});
			}
		}
		else
		{
			AddErrorAndSkip(16, { varsy, beginsy, ifsy, whilesy, withsy });
			return false;
		}
	}
	else
	{
		AddErrorAndSkip(2, { varsy, beginsy, ifsy, whilesy, withsy });
		return false;
	}
	return true;
}

CType* CParser::Type()
{
	switch (m_pCurrentSymbol->SymbolCode)
	{
	case ident:
		return static_cast<CType*>(Sem->CheckDescription(m_pCurrentSymbol, Lexer->GetCurrentStr(), utType));
	case recordsy:
		return RecordType();
	default:
		AddErrorAndSkip(2, { varsy, beginsy, ifsy, whilesy, withsy, endsy, point });
		return nullptr;
	}
}

CRecordType* CParser::RecordType()
{
	CRecordType* Rec = nullptr;
	if (LexerGiveMeSymbolBistra() && !(Rec = RecordFields())) return nullptr;
	if (!Accept(endsy, false))
	{
		AddErrorAndSkip(13, { varsy, beginsy, ifsy, whilesy, withsy, endsy, point });
		return nullptr;
	}
	return Rec;
}

CRecordType* CParser::RecordFields(bool IsRightpar)
{
	CRecordType* Rec = new CRecordType();
	switch (m_pCurrentSymbol->SymbolCode)
	{
	case ident:
		if (FixRecordPart(Rec))
		{
			if (Accept(casesy, false))
			{
				if (VariantRecordPart(Rec)) return Rec;
				else return nullptr;
			}
			return Rec;
		}
		else
			if (IsRightpar && Accept(rightpar, false)) return Rec;
			else
			{
				AddErrorAndSkip(18, { varsy, beginsy, ifsy, whilesy, withsy });
				return nullptr;
			}
	case casesy:
		if (LexerGiveMeSymbolBistra())
		{
			if (VariantRecordPart(Rec)) return Rec;
			else return nullptr;
		}
		else
		{
			AddErrorAndSkip(18, {});
			return nullptr;
		}
	default:
		AddErrorAndSkip(18, { varsy, beginsy, ifsy, whilesy, withsy });
		return nullptr;
	}
}

bool CParser::FixRecordPart(CRecordType* Rec)
{
	CType* Type;
	vector<string> Vars;
	while (Type = RecordSection(Vars))
	{
		Sem->SetRecFieldType(Type, Vars, Rec, Lexer->GetCurrentStr());
		if (Accept(semicolon)) LexerGiveMeSymbolBistra();
		else
		{
			if (Accept(endsy, false))
			{
				return true;
			}
			else
			{
				AddErrorAndSkip(14, {});
				if (m_pCurrentSymbol)
				{
					AddErrorAndSkip(0, { varsy, beginsy, ifsy, whilesy, withsy });
				}
				return false;
			}
		}
	}

	if (Accept(casesy, false) || Accept(endsy, false)) return true;
	return false;
}

CType* CParser::RecordSection(vector<string>& Vars)
{
	while (Accept(ident, false))
	{
		Vars.push_back(m_pCurrentSymbol->GetSymbol());
		if (Accept(comma)) LexerGiveMeSymbolBistra();
		else
		{
			if (Accept(colon, false))
			{
				if (LexerGiveMeSymbolBistra()) return Type();
				else
				{
					AddErrorAndSkip(2, { varsy, beginsy, ifsy, whilesy, withsy });
					return nullptr;
				}
			}
			else
			{
				AddErrorAndSkip(5, { varsy, beginsy, ifsy, whilesy, withsy });
				return nullptr;
			}
		}
	}
	return nullptr;
}

bool CParser::VariantRecordPart(CRecordType* Rec)
{
	if (Accept(ident))
	{
		CVar* Var = new CVar(m_pCurrentSymbol->GetSymbol());
		Rec->FlagField = Var;
		if (Accept(colon))
			if (Accept(ident))
			{
				Sem->AddFlagVarPart(Rec, m_pCurrentSymbol, Lexer->GetCurrentStr());
				if (Accept(ofsy))
					return VariantList(Rec);
				else
				{
					AddErrorAndSkip(8, { varsy, beginsy, ifsy, whilesy, withsy });
					return false;
				}
			}
			else
			{
				AddErrorAndSkip(2, { varsy, beginsy, ifsy, whilesy, withsy });
				return false;
			}
		else
		{
			if (Accept(ofsy, false))
			{
				return VariantList(Rec);
			}
			else
			{
				AddErrorAndSkip(8, { varsy, beginsy, ifsy, whilesy, withsy });
				return false;
			}
		}
	}
	else
	{
		AddErrorAndSkip(2, { varsy, beginsy, ifsy, whilesy, withsy });
		return false;
	}
	return false;
}

bool CParser::VariantList(CRecordType* Rec)
{
	while (Variant(Rec)
		&& Accept(semicolon)
		&& !Accept(endsy, false)
		&& !Accept(varsy, false)
		&& !Accept(beginsy, false));
	if (Accept(endsy, false))
	{
		return true;
	}
	if (!Accept(semicolon, false))
	{
		AddErrorAndSkip(14, { varsy, beginsy, ifsy, whilesy, withsy });
	}
	else if (Accept(endsy)) return true;
	else AddErrorAndSkip(13, { varsy, beginsy, ifsy, whilesy, withsy });

	return false;
}

bool CParser::Variant(CRecordType* Rec)
{
	vector<CValue*> Val;
	if (Rec && Rec->FlagField)
		Val = VariantLableList(Rec->FlagField->GetBaseType());
	else
		Val = VariantLableList(nullptr);
	if (Val.size() != 0)
		if (Accept(leftpar))
		{
			CRecordType* ChildRec;
			if (LexerGiveMeSymbolBistra() && (ChildRec = RecordFields(true)))
			{
				Sem->CheckVarRecordPartType(Rec, ChildRec, Val, Lexer->GetCurrentStr());
				return true;
			}
			else
			{
				if (!Accept(rightpar, false))
					AddErrorAndSkip(4, { varsy, beginsy, ifsy, whilesy, withsy });
				return false;
			}
		}
		else
		{
			AddErrorAndSkip(9, { varsy, beginsy, ifsy, whilesy, withsy });
			return false;
		}
	else
	{
		if (!Accept(endsy, false)) AddErrorAndSkip(0, { varsy, beginsy, ifsy, whilesy, withsy });
		return false;
	}
}

vector<CValue*> CParser::VariantLableList(CType* FlagType)
{
	eValueType ValType = Sem->GetValType(FlagType);
	vector<CValue*> Op;
	while (LexerGiveMeSymbolBistra()
		&& !Accept(colon, false)
		&& !Accept(endsy, false)
		&& !Accept(varsy, false)
		&& !Accept(beginsy, false))
	{
		if (!m_pCurrentSymbol || !VariantLable()) break;
		else
		{
			CValueSymbol* CurSym = static_cast<CValueSymbol*> (m_pCurrentSymbol);
			if (CurSym->GetValue()->GetValueType() != ValType)
			{
				AddErrorAndSkip(147, {});
			}
			Op.push_back(CurSym->GetValue());
		}
		if (!Accept(comma)) break;
	}

	if (Accept(colon, false)) return Op;
	else if (!Accept(endsy, false)) AddErrorAndSkip(5, { varsy, beginsy, ifsy, whilesy, withsy });

	Op.clear();
	return Op;
}

bool CParser::VariantLable()
{
	if ((UnsignedNumber()
		|| Accept(charc, false)
		|| SignedNumber()))
		return true;

	if (!Accept(endsy, false)) AddErrorAndSkip(18, { varsy, beginsy, ifsy, whilesy, withsy });
	return false;
}

CType* CParser::UnsignedNumber()
{
	if (m_pCurrentSymbol->SymbolCode == intc || m_pCurrentSymbol->SymbolCode == floatc)
		return Sem->GetType(m_pCurrentSymbol);
	return nullptr;
}

bool CParser::Sign()
{
	return Accept(plussy, false) || Accept(minussy, false);
}

CType* CParser::SignedNumber()
{
	if (Sign() && LexerGiveMeSymbolBistra()) return UnsignedNumber();
	return nullptr;
}

bool CParser::VarBlock()
{
	while (LexerGiveMeSymbolBistra()
		&& (!Accept(endsy, false)
			&& !Accept(beginsy, false)
			&& !Accept(ifsy, false)
			&& !Accept(whilesy, false)
			&& !Accept(withsy, false)))
	{
		if (!OneTypeVar())
		{
			AddErrorAndSkip(0, { semicolon, beginsy, ifsy, whilesy, withsy, endsy, point });
			break;
		}
	}
	if (m_pCurrentSymbol)
	{
		switch (m_pCurrentSymbol->SymbolCode)
		{
		case beginsy:
			if (LexerGiveMeSymbolBistra()) OperatorBlock();
			break;
		default:
			AddErrorAndSkip(17, { beginsy, ifsy, whilesy, withsy, endsy, point });
			OperatorBlock();
			break;
		}
	}
	else
	{
		AddErrorAndSkip(17, {});
		return false;
	}
	return true;
}

bool CParser::OneTypeVar()
{
	while (!Accept(colon, false) && Accept(ident, false))
	{
		Sem->AddVar(m_pCurrentSymbol, Lexer->GetCurrentStr());
		if (Accept(comma)) LexerGiveMeSymbolBistra();
		else break;
	}

	if (Accept(colon, false))
	{
		if (LexerGiveMeSymbolBistra())
		{
			CType* BaseType = Type();
			Sem->SetVarsType(BaseType, Lexer->GetCurrentStr());
			if (Accept(semicolon)) return true;
			else AddErrorAndSkip(14, { beginsy, ifsy, whilesy, withsy, endsy, point });
		}
		else AddErrorAndSkip(2, { beginsy, ifsy, whilesy, withsy, endsy, point });

	}
	else AddErrorAndSkip(5, { beginsy, ifsy, whilesy, withsy, endsy, point });
	return false;
}

void CParser::OperatorBlock()
{
	CompositeOperator();
}

void CParser::CompositeOperator()
{
	while (m_pCurrentSymbol && !Accept(endsy, false))
	{
		Operator();
		if (!Accept(semicolon, false))
		{
			if (Accept(endsy, false)) continue;
			if (Accept(point, false)) break;
			AddErrorAndSkip(14, { semicolon, beginsy, ifsy, whilesy, withsy, endsy, point });
		}
		else
		{
			LexerGiveMeSymbolBistra();
		}
	}

	if (Accept(endsy, false))
	{
		if (Accept(semicolon)) return;
		else
		{
			if (Accept(elsesy, false) || Accept(point, false) || Accept(endsy, false)) return;
		}
	}
	else
	{

		AddErrorAndSkip(13, { beginsy, ifsy, whilesy, withsy, endsy, point });
	}
}

void CParser::ComplexOperator()
{
	if (!m_pCurrentSymbol) return;

	switch (m_pCurrentSymbol->SymbolCode)
	{
	case beginsy:
		LexerGiveMeSymbolBistra();
		CompositeOperator();
		break;
	case ifsy: ConditionOperator(); break;
	case whilesy: CycleOperator(); break;
	case withsy: WithOperator(); break;
	default:
		break;
	}
}

void CParser::ConditionOperator()
{
	CType* Type;
	Type = Expression();
	Type = Sem->GetBaseType(Type);
	if (Type && Type->GetName() != "boolean")
		AddErrorAndSkip(144, {});

	if (Accept(thensy, false))
	{
		LexerGiveMeSymbolBistra();
	}
	else
	{
		AddErrorAndSkip(52, { beginsy, elsesy, whilesy, withsy, endsy, point });
	}

	Operator();
	if (Accept(elsesy, false))
	{
		LexerGiveMeSymbolBistra();
		Operator();
	}
}

void CParser::CycleOperator()
{
	CType* Type;
	Type = Expression();
	if (!(Type = Sem->GetBaseType(Type)))
		AddErrorAndSkip(144, {});
	else
		if (Type->GetName() != "boolean")
			AddErrorAndSkip(144, {});

	if (Accept(dosy, false))
	{
		LexerGiveMeSymbolBistra();
		Operator();
	}
	else
	{
		AddErrorAndSkip(54, {});
		Operator();
	}
}

void CParser::WithOperator()
{
	if (!RecordVarList())
		AddErrorAndSkip(0, { beginsy, ifsy, dosy, whilesy, withsy, endsy, point });

	InWithOperator.push_back(true);
	if (Accept(dosy, false))
	{
		LexerGiveMeSymbolBistra();
		Operator();
	}
	else
	{
		AddErrorAndSkip(54, {});
		Operator();
	}

	int i = InWithOperator.size();
	for (int j = WithRec.size() - 1; j >= 0; j--)
	{
		if (WithRec[j].first == i)
			WithRec.erase(std::remove(WithRec.begin(), WithRec.end(), WithRec[j]), WithRec.end());
	}

	InWithOperator.pop_back();
}

bool CParser::RecordVarList()
{
	CRecordType* Rec;
	while (LexerGiveMeSymbolBistra())
	{
		Rec = static_cast<CRecordType*>(Variable());
		if (Rec && Rec->GetCustType() == tScalar) AddErrorAndSkip(140, {});
		else
			WithRec.push_back({ InWithOperator.size() + 1, Rec });
		if (!Accept(comma, false)) break;
	}

	if (!Accept(comma, false) && !Accept(dosy, false))
	{
		AddErrorAndSkip(20, {});
		return false;
	}
	return true;
}

void CParser::Operator()
{
	if (Accept(ident, false)) SimpleOperator();
	else ComplexOperator();
}

void CParser::SimpleOperator()
{
	AssignOperator();
}

void CParser::AssignOperator()
{
	CType* TypeVar;
	TypeVar = Variable();
	if (Accept(assign, false))
	{
		CType* TypeExp = Expression();
		if (!Sem->Cast(TypeVar, TypeExp, true))
			AddErrorAndSkip(145, {});
	}
	else
		AddErrorAndSkip(51, { beginsy, ifsy, whilesy, withsy, endsy, point });
}

bool CParser::MultOperation()
{
	return Accept(star, false)
		|| Accept(slash, false);
}

bool CParser::MultIntOperation()
{
	return Accept(divsy, false)
		|| Accept(modsy, false);
}

bool CParser::MultBoolOperation()
{
	return Accept(andsy, false);
}

bool CParser::RelationshipOperation()
{
	return Accept(equalsy, false)
		|| Accept(greater, false)
		|| Accept(greaterequal, false)
		|| Accept(later, false)
		|| Accept(laterequal, false)
		|| Accept(insy, false)
		|| Accept(latergreater, false);
}

bool CParser::AddOperation()
{
	return Sign();
}

bool CParser::AddBoolOperation()
{
	return Accept(orsy, false);
}

CType* CParser::Constant()
{
	if (m_pCurrentSymbol->SymbolCode != ident)
		return Sem->GetType(m_pCurrentSymbol);
	return nullptr;
}

CType* CParser::FullVariable(CRecordType* Rec)
{
	CType* Type = nullptr;
	if (Accept(ident, false))
	{
		if (InWithOperator.size() > 0)
		{
			Type = Sem->CheckWithRecords(WithRec, m_pCurrentSymbol, Lexer->GetCurrentStr());
			if (Type) return Type;
		}

		if (!Rec)
		{
			if (CVar* Var = static_cast<CVar*> (Sem->CheckDescription(m_pCurrentSymbol, Lexer->GetCurrentStr(), utVar)))
				Type = Var->GetBaseType();
			else Type = nullptr;
		}
		else
		{
			if (Rec && Rec->GetCustType() == tRecord)
			{
				Type = Sem->CheckRecordFields(Rec, m_pCurrentSymbol, Lexer->GetCurrentStr());
				if (!Type) AddErrorAndSkip(152, {});
			}
			else
			{
				AddErrorAndSkip(140, {});
			}
		}

		return Type;
	}
	return nullptr;
}

CType* CParser::Variable(CRecordType* Rec)
{
	CType* Type;
	Type = FullVariable(Rec);
	if (Accept(point))
	{
		Type = VariableRecordField(Rec = static_cast<CRecordType*>(Type));
	}
	return Type;
}

CType* CParser::VariableRecordField(CRecordType* Rec)
{
	if (!Rec)
	{
		AddErrorAndSkip(140, {});
		return nullptr;
	}

	CType* Type = nullptr;
	while (Accept(point, false) && LexerGiveMeSymbolBistra() && (Type = Variable(Rec)));
	return Type;
}

CType* CParser::Term()
{
	CType* Type = nullptr;
	while (m_pCurrentSymbol)
	{
		Type = Multiplier();
		CType* BaseType = Sem->GetBaseType(Type);
		string BaseTypeName = BaseType ? BaseType->GetName() : "";
		if (MultBoolOperation())
		{
			if (BaseTypeName != "boolean") AddErrorAndSkip(210, {});
			LexerGiveMeSymbolBistra();
			continue;
		}
		if (MultIntOperation())
		{
			if (BaseTypeName != "integer") AddErrorAndSkip(212, {});
			LexerGiveMeSymbolBistra();
			continue;
		}
		if (MultOperation())
		{
			if (BaseTypeName == "char"
				|| BaseTypeName == "boolean"
				|| BaseTypeName == "string"
				|| BaseTypeName == "")
				AddErrorAndSkip(213, {});
			LexerGiveMeSymbolBistra();
			continue;
		}
		break;
	}
	return Type;
}

CType* CParser::Multiplier()
{
	CType* Type;
	if (Accept(leftpar, false))
	{
		Type = Expression();
		if (Accept(rightpar, false))
		{
			LexerGiveMeSymbolBistra();
			return Type;
		}
		else
		{
			AddErrorAndSkip(4, { semicolon, beginsy, ifsy, whilesy, withsy, endsy, point });
			return false;
		}
	}
	if (Accept(notsy, false))
	{
		LexerGiveMeSymbolBistra();
		return Multiplier();
	}
	if (Accept(ident, false)) return Variable();

	Type = Constant();
	LexerGiveMeSymbolBistra();
	return Type;
}

CType* CParser::Expression()
{
	CType* OneType;
	if (OneType = SimpleExpression())
		if (RelationshipOperation())
		{
			OneType = Sem->Cast(OneType, SimpleExpression());
			if (!OneType) AddErrorAndSkip(186, {});
			return Sem->GetTypeFromMap("boolean");
		}
		else return OneType;
	else return nullptr;
}

CType* CParser::SimpleExpression()
{
	CType* Type;
	bool IsSign = false;
	LexerGiveMeSymbolBistra();
	if (IsSign = Sign()) LexerGiveMeSymbolBistra();
	Type = Term();
	Type = Sem->GetBaseType(Type);
	if (IsSign && Type && Type->GetName() != "real" && Type->GetName() != "integer")
		AddErrorAndSkip(184, {});

	bool IsBoolOp = false;
	while (AddOperation() || (IsBoolOp = AddBoolOperation()))
	{
		LexerGiveMeSymbolBistra();
		Type = Sem->Cast(Type, Term());
		if (IsBoolOp && Type && Type->GetName() != "boolean")
		{
			AddErrorAndSkip(210, {});
			continue;
		}

		if (!Type)
		{
			AddErrorAndSkip(211, {});
		}
	}
	return Type;
}