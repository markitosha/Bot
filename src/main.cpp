#include "../lib/inter.h"

Leksema* LA::HF(int c)
{
	Leksema *l;
	if (len != 0){
		char ch = buf[0];
		len = 0;
		l = GetSym(ch);
		if (l != 0){
			buf[0] = c;
			len = 1;
			return l;
		}else{
			return GetSym(c);
		}
	}else{
		buf[len++] = c;
		switch (SymType(c)){
		case sigma:
			if(c == '\n')
				str_num++;
			len = 0;
			return 0;
		case alfa:
			state = C;
			return 0;
		case beta:
			state = K;
			return 0;
		case delta:
			state = L;
			return 0;
		case quote:
			state = T;
			return 0;
		case rho:
			buf[len++] = ' ';
			return SF(c, H);
		default:
			throw ErrorLA("Wrong lexeme beginning", str_num);
		}
	}
}

Leksema* LA::CF(int c)
{
	if (buf[0] == '0' && SymType(c) == alfa)
		buf[0] = c;
	else{
		buf[len++] = c;
	}
	switch (SymType(c)){
	case alfa:
		return 0;
	case point:
		state = D;
		return 0;
	case sigma: case rho: case delta:
		return SF(c, C);
	default:
		throw ErrorLA("Wrong integer part", str_num);
	}
}

Leksema* LA::DF(int c)
{
	buf[len++] = c;
	switch (SymType(c)){
	case alfa:
		if (c == '0')
			len0++;
		else
			len0 = 0;
		return 0;
	case rho:case sigma:case delta:
		return SF(c, D);
	default:
		throw ErrorLA("Wrong real part", str_num);
	}
}

Leksema* LA::KF(int c)
{
	buf[len++] = c;
	switch (SymType(c)){
	case beta:
		return 0;
	case alfa:
		state = I;
		return 0;
	case delta:case sigma:case rho:
		return SF(c, K);
	default:
		throw ErrorLA("Wrong symbols in name", str_num);
	}
}

Leksema* LA::IF(int c)
{
	buf[len++] = c;
	switch (SymType(c)){
	case beta:case alfa:
		return 0;
	case delta:case sigma:case rho:
		return SF(c, I);
	default:
		throw ErrorLA("Wrong identifier", str_num);
	}
}

Leksema* LA::TF(int c)
{
	buf[len++] = c;
	if(c == ' ' || c =='\t')
		return 0;
	switch (SymType(c)){
	case beta: case alfa: case rho: case delta:
		return 0;
	case quote:
		buf[len++] = ' ';
		return SF(c, T);
	default:
		throw ErrorLA("Unmatched quotes", str_num);
	}
}

Leksema* LA::LF(int c)
{
	buf[len++] = c;
	if(c == '='){
		buf[len++] = ' ';
		return SF(c, H);
	}
	switch (SymType(c)){
	case beta: case alfa: case sigma: case rho:
		return SF(c, L);
	default:
		throw ErrorLA("Wrong operator", str_num);
	}
}

char* LA::MakeBuff(char * buff)
{
	buff = new char[len];
	for(int i = 0; i < len -1; i++)
		buff[i] = buf[i];
	buff[len-1] = '\0';
	if (SymType(buf[len-1]) != sigma && len > 1){
		buf[0] = buf[len-1];
		len = 1;
	}else
		len = 0;
	return buff;
}

int LA::SearchInTable(char *buff)
{
	int i = 0;
	while(KeyTable[i] != 0){
		if(strcmp(buff, KeyTable[i]) == 0)
			return keyword;
		i++;
	}
	i = 0;
	while(FuncTable[i] != 0){
		if(strcmp(buff, FuncTable[i]) == 0)
			return function;
		i++;
	}
	return -1;
}

void LA::NulNum()
{
	buf[len - len0] = buf[len - 1];
	len = len - len0 + 1;
	len0 = 0;
}

Leksema* LA::SF(int c, int st)
{
	int type;
	if (len0 != 0)
		NulNum();
	char *buff = 0;
	buff = MakeBuff(buff);
	switch (st){
	case H:
		type = divider;
		break;
	case C: case D:
		type = number;
		break;
	case K:
		type = SearchInTable(buff);
		if (type == -1)
			type = variable;
		break;
	case I:
		type = variable;
		break;
	case T:
		type = string;
		break;
	case L:
		type = divider;
		break;
	default:
		throw ErrorLA("Output error", str_num);
	}
	state = H;
	Leksema *l = new Leksema(buff, str_num, type);
	if(c == '\n')
		str_num++;
	return l;
}

int LA::SymType(int c)
{
	if (c >= '0' && c<='9')
		return alfa;
	if ((c>='a' && c<='z') || (c>= 'A' && c<= 'Z') || (c == '_'))
		return beta;
	switch (c){
	case '"':
		return quote;
	case '.':
		return point;
	case ' ': case'\t': case'\n':case EOF:
		return sigma;
	case '/':
	case '<': case'>': case'+': case'-': case'!': case'*': case'=':
		return delta;
	case'&': case'|': case'(': case')':case '[':case ']':
	case ';': case',': case '{': case '}':
		return rho;
	default:
		throw ErrorLA("Wrong symbol", str_num);
	}
}

Leksema* LA::GetSym(int c)
{
	switch (state){
	case H: 
		return HF(c);
	case C:
		return CF(c);
	case D:
		return DF(c);
	case K:
		return KF(c);
	case I:
		return IF(c);
	case L:
		return LF(c);
	case T:
		return TF(c);
	default:
		return 0;
	}
}

const char * LA::KeyTable[] = {
	"if",
	"then",
	"else",
	"while",
	"do",
	"print",
	"buy",
	"sell",
	"prod",
	"build",
	"endturn",
	0,
};

const char * LA::FuncTable[] = {
	"my_id",
	"turn",
	"players",
	"active_players",
	"supply",
	"raw_price",
	"demand",
	"production_price",
	"money",
	"raw",
	"production",
	"factories",
	"auto_factories",
	"result_raw_sold",
	"result_raw_price",
	"result_prod_bought",
	"result_prod_price",
	0
};	

void Leksema::Print()
{
	char *t;
	switch (type){
	case divider:
		t = (char *)"divider";
		break;
	case keyword:
		t = (char *)"keyword";
		break;
	case function:
		t = (char *)"function";
		break;
	case number:
		t = (char *)"number";
		break;
	case variable:
		t = (char *)"variable";
		break;
	case string:
		t = (char *)"string";
		break;
	default:
		throw ErrorLA("How you did it?! Haven't type",str_num);
	}
	printf("   %d %s %s\n",str_num, t, leks);
};

void print(LekList *list)
{
	if (list -> next == 0){
		list -> leks -> Print();
		delete list;
	}else{
		list -> leks -> Print();
		print(list -> next);
		delete list;
	}
}

	
void OpenFiles(int argc, char** argv)
{
	int d;
	if (argc != 6){
		d = open(argv[6], O_RDONLY);
		if (d == -1)
			throw ErrorLA("Read file error", 0);
		dup2(d, 0);
		close(d);
		if (argc != 7){
			d = open(argv[7], O_WRONLY|O_CREAT|O_TRUNC, 0666);
			if (d == -1)
				throw ErrorLA("Write file errorn", 0);
			dup2(d, 1);
			close(d);
		}
	}
}

LekList* LekList::Add(Leksema *l)
{
	LekList * li = new LekList, *o_li = this;
	li->leks = new Leksema(*l);
	li->next = 0;
	if (o_li == 0)
		return li;
	else{
		while (o_li -> next != 0)
			o_li = o_li -> next;
		o_li -> next = li;
		return this;
	}
}

void SA::WhereFalse(int type)
{
	if (type == iff){
		Push(&stack_if_false, new RPNItem(new RPNLabel(RPN)), 0);
		Push(&RPN, new RPNItem(new RPNOpGoF()));
	}else{
		Push(&stack_while_false, new RPNItem(new RPNLabel(RPN)), 0);
		Push(&RPN, new RPNItem(new RPNOpGoF()));
	}	
}

void SA::WhereTrue(int type)
{
	if (type == iff){
		Push(&stack_if_true, new RPNItem(new RPNLabel(RPN)), 0);
		Push(&RPN, new RPNItem(new RPNOpGo()));
	}else{
		Push(&stack_while_true, new RPNItem(new RPNLabel(RPN)), 0);
		Push(&RPN, new RPNItem(new RPNOpGo()));
	}
}

void SA::WhatFalse(int type)
{
	if (type == iff)
		Push(&stack_if_false, new RPNItem(new RPNLabel(RPN)), 1);
	else
		Push(&stack_while_false, new RPNItem(new RPNLabel(RPN)), 1);
}

void SA::WhatTrue(int type)
{
	if (type == iff)
		Push(&stack_if_true, new RPNItem(new RPNLabel(RPN)), 1);
	else
		Push(&stack_while_true, new RPNItem(new RPNLabel(RPN)), 1);
}

void SA::FreeTo()
{
	RPNItem *el;
	RPNElem *el1;
	RPNBegin* beg;
	while(1){
		el = Pop(&stack);
		el1 = el -> elem;
		beg = dynamic_cast<RPNBegin*>(el1);
		if (beg)
			break;
		Push(&RPN, el);
	}
}

void SA::Bracket()
{
	char *s = (char *)"(";
	Push(&stack, new Leksema(s, 0, 0));
}

RPNItem* SA::Start()
{
	RPNItem *RPNList;
	Push(&RPN, new RPNItem(new RPNBeginProg()));
	RPNList = RPN;
	while(1){
		Bracket();
		O();
		if(strcmp(leks,";") != 0)
			throw ErrorSA("';'", str_num, leks);
		FreeTo();
		if (list->next != 0)
			Next();
		else{
			Push(&RPN, new RPNItem(new RPNEndProg()));
			Push(&RPN, (RPNItem*)0);
			break;
		}
	}
	if (stack_if_true != 0)
		UnwindStack(&stack_if_true);
	if (stack_while_false != 0)
		UnwindStack(&stack_while_false);
	if (stack_while_true != 0)
		UnwindStack(&stack_while_true);
	if (stack_if_false != 0)
		UnwindStack(&stack_if_false);
	return RPNList;
}

void SA::O()
{
	if (type == keyword)
		K();
	else if (type == function){
		Push(&stack, new Leksema(leks, 0, function));
		Next();
		G();
	}else if (type == variable){
		Push(&RPN, new Leksema(leks, 0, variable));
		Next();
		V();
		P();
	}else if (strcmp(leks,";") != 0 && strcmp(leks, "}") != 0)
		throw ErrorSA("Operator", str_num, leks);
	FreeTo();
	Bracket();
}

void SA::G()
{
	if (strcmp(leks, "(") == 0){
		Bracket();
		Next();
		L();
	}else{
		throw ErrorSA("'('", str_num, leks);
	}
	if (strcmp(leks,")") != 0)
		throw ErrorSA("')'", str_num, leks);
	FreeTo();
	Next();
}

void SA::V()
{
	if (strcmp(leks, "[") == 0){
		Bracket();
		Next();
		W();
		if (strcmp(leks, "]") != 0)
			throw ErrorSA("']'", str_num, leks);
		else{
			FreeTo();
			Push(&RPN, new RPNItem(new RPNArray));
			Next();
		}
	}
}

void SA::L()
{
	if (strcmp(leks, ")") == 0)
		return;
	if (type == function){
		Push(&stack, new Leksema(leks, 0, function));
		Next();
		G();
	}else
		W();
	R();
}

void SA::R()
{
	if (strcmp(leks, ",") == 0){
		Next();
		L();
	}
}

void SA::W()
{
	if (type == variable){
		Push(&RPN, new Leksema(leks, 0, variable));
		Next();
		V();
		Push(&RPN, new RPNItem(new RPNVal));
		F();
	}else if (type == number){
		Push(&RPN, new Leksema(leks, 0, number));
		Next();
		F();
	}else if (strcmp(leks, "(") == 0){
		Bracket();
		Next();
		W();
		if (strcmp(leks, ")") != 0)
			throw ErrorSA("')'", str_num, leks);
		else{
			FreeTo();
			Next();
			F();
		}
	}else if (type == function){
		Push(&stack, new Leksema(leks, 0, function));
		Next();
		G();
		F();
	}else{
		throw ErrorSA("Expression", str_num, leks);
	}
}

void SA::F()
{
	if (strcmp(leks, "+")==0 || strcmp(leks, "-")==0 ||
	strcmp(leks,"*")==0 || strcmp(leks, "/") == 0){
		Push(&stack, new Leksema(leks, 0, 0));
		Next();
		W();
	}	
}

void SA::T()
{
	if (strcmp(leks,"+=")==0 || strcmp(leks,"-=")==0 ||
	strcmp(leks,"*=") == 0 || strcmp(leks, "/=") == 0){
		Push(&stack, new Leksema(leks, 0, 0));
		Next();
		W();
	}else{
		throw ErrorSA("Operator", str_num, leks);
	}
}

void SA::P()
{
	if (strcmp(leks, "=") == 0){
		Push(&stack, new Leksema(leks, 0, 0));
		Next();
		W();
	}else{
		T();
	}
}

void SA::K()
{
	if (strcmp(leks, "if") == 0){
		Next();
		A();
	}else if (strcmp(leks, "while") == 0){
		WhatTrue(whilee);
		Next();
		C();
	}else if (strcmp(leks, "print") == 0){
		Push(&stack, new RPNItem(new RPNPrint()));
		Next();
		D();
	}else if (strcmp(leks, "buy") == 0){
		Push(&stack, new RPNItem(new RPNBuy));
		Next();
		E();
	}else if (strcmp(leks, "sell") == 0){
		Push(&stack, new RPNItem(new RPNSell));
		Next();
		E();
	}else if (strcmp(leks, "prod") == 0){
		Push(&stack, new RPNItem(new RPNProd));
		Next();
		W();
	}else if (strcmp(leks, "build") == 0){
		Push(&stack, new RPNItem(new RPNBuild));
		Next();
		W();
	}else if (strcmp(leks, "endturn") == 0){
		Push(&stack, new RPNItem(new RPNEnd));
		Next();
	}else
		throw ErrorLA("ERROR IN SECOND STAGE", str_num);
}

void SA::A()
{
	if (strcmp(leks, "(") != 0)
		throw ErrorSA("'('", str_num, leks);
	else{
		Bracket();
		Next();
		Z();
		if (strcmp(leks, ")") != 0)
			throw ErrorSA("')'", str_num, leks);
		else{
			FreeTo();
			Next();
			if (strcmp(leks, "then") != 0)
				throw ErrorSA("'then'", str_num, leks);
			else{
				WhereFalse(iff);
				Next();
				if (strcmp(leks, "{") != 0)
					throw ErrorSA("'{'", str_num, leks);
				Bracket();
				Next();
				N();
				if (strcmp(leks, "}") != 0)
					throw ErrorSA("'}'", str_num, leks);
				FreeTo();
				Next();
				if (strcmp(leks, "else") != 0)
					throw ErrorSA("'else'", str_num, leks);
				else{
					Next();
					WhereTrue(iff);
					WhatFalse(iff);
					if (strcmp(leks, "{") != 0)
						throw ErrorSA("'{'", str_num, leks);
					Bracket();
					Next();
					N();
					if (strcmp(leks, "}") != 0)
						throw ErrorSA("'}'", str_num, leks);
					FreeTo();
					Next();
					WhatTrue(iff);
				}
			}
		}
	}		
}

void SA::Z()
{
	Q();
	Y();
}

void SA::Y()
{
	if (strcmp(leks, "&") == 0 || strcmp(leks, "|")==0 || strcmp(leks, "!")==0){
		Push(&stack, new Leksema(leks, 0, 0));
		Next();
		Z();
	}
}

void SA::C()
{
	if (strcmp(leks, "(") != 0)
		throw ErrorSA("'('", str_num, leks);
	else{
		Bracket();
		Next();
		Z();
		if (strcmp(leks, ")") != 0)
			throw ErrorSA("')'", str_num, leks);
		else{
			FreeTo();
			Next();
			if (strcmp(leks, "do") != 0)
				throw ErrorSA("'do'", str_num, leks);
			else{
				WhereFalse(whilee);
				Next();
				if (strcmp(leks, "{") != 0)
					throw ErrorSA("'{'", str_num, leks);
				Bracket();
				Next();
				N();
				if (strcmp(leks, "}") != 0)
					throw ErrorSA("'}'", str_num, leks);
				FreeTo();
				Next();
				WhereTrue(whilee);
				WhatFalse(whilee);
			}
		}
	}
}

void SA::N()
{
	O();
	M();
}

void SA::M()
{
	if (strcmp(leks, ",") == 0){
		Next();
		N();
	}	
}
void SA::D()
{
	if (type == string){
		Push(&RPN, new RPNItem(new RPNString(leks)));
		Next();
	}else{
		W();
	}
}

void SA::E()
{
	Bracket();
	W();
	if (strcmp(leks, ",") != 0)
		throw ErrorSA("','", str_num, leks);
	else{
		FreeTo();
		Next();
		Bracket();
		W();
		FreeTo();
	}
}

void SA::Q()
{
	W();
	B();
	W();
}

void SA::B()
{
	if (strcmp(leks, "==")!=0 && strcmp(leks, ">")!=0 &&
	strcmp(leks, "<")!=0 && strcmp(leks, "!=")!=0 &&
	strcmp(leks, "<=") != 0 && strcmp(leks, ">=") != 0)
		throw ErrorSA("Comparison", str_num, leks);
	Push(&stack, new Leksema(leks, 0, 0));
	Next();
}

void SA::Next()
{
	if (list -> next != 0){
		list = list -> next;
		type = list -> leks -> type;
		str_num = list -> leks -> str_num;
		leks = list -> leks -> leks;
	}else{
		throw ErrorSA("Leksema", list -> leks ->str_num, "end of file");
	}
}

VarTable* RPNVar::table = 0;

void RPNConst::Evaluate(RPNItem **stack, RPNItem **cur_cmd, Context &info)
const
{
	Push(stack, Copy());
	*cur_cmd = (*cur_cmd) -> next;
}

void RPNFunc::Evaluate(RPNItem **stack, RPNItem **cur_cmd, Context &info)
const
{
	RPNElem *res = EvaluateFun(stack, info);
	if (res) 
		Push(stack, res);
	*cur_cmd = (*cur_cmd)->next;
}

RPNElem* RPNString::Copy() const
{
	return new RPNString(str);
}

RPNElem* RPNFloat::Copy() const
{
	return new RPNFloat(value);
}

RPNElem* RPNVar::Copy() const
{
	return new RPNVar(addr);
}

RPNElem* RPNBool::Copy() const
{
	return new RPNBool(value);
}

RPNElem* RPNPlus::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	float res = i1->Get() + i2->Get();
	delete op1;
	delete op2;
	return new RPNFloat(res);
}

VarTable *RPNVar::GetArray(int i)
{
	NewRPNVar(this, i);
	VarTable *cur = addr;
	for(int j = 0; j<i; j++)
		cur = cur -> array;
	return cur;
}


RPNElem* RPNArray::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNVar *i2 = dynamic_cast<RPNVar*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to variable");
	VarTable* res = i2 -> GetArray(i1->Get());
	delete op1;
	delete op2;
	return new RPNVar(res);
}

RPNElem* RPNMulti::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	float res = i1->Get()*i2->Get();
	delete op1;
	delete op2;
	return new RPNFloat(res);
}

RPNElem* RPNDiv::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	float res = i2->Get() / i1->Get();
	delete op1;
	delete op2;
	return new RPNFloat(res);
}

RPNElem* RPNMinus::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	float res = i2->Get() - i1->Get();
	delete op1;
	delete op2;
	return new RPNFloat(res);
}

RPNElem* RPNNotEqu::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	int res = 0;
	if (i1->Get() != i2->Get())
		res = 1;
	else
		res = 0;
	delete op1;
	delete op2;
	return new RPNBool(res);
}

RPNElem* RPNOr::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNBool *i1 = dynamic_cast<RPNBool*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to boolean");
	RPNBool *i2 = dynamic_cast<RPNBool*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to boolean");
	int res = 0;
	if (i1->Get() == 1 || i2->Get() == 1)
		res = 1;
	else
		res = 0;
	delete op1;
	delete op2;
	return new RPNBool(res);
}

RPNElem* RPNAnd::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNBool *i1 = dynamic_cast<RPNBool*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to boolean");
	RPNBool *i2 = dynamic_cast<RPNBool*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to boolean");
	int res = 0;
	if (i1->Get() == 1 && i2->Get() == 1)
		res = 1;
	else
		res = 0;
	delete op1;
	delete op2;
	return new RPNBool(res);
}

RPNElem* RPNEqu::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	int res = 0;
	if (i1->Get() == i2->Get())
		res = 1;
	else
		res = 0;
	delete op1;
	delete op2;
	return new RPNBool(res);
}

RPNElem* RPNNot::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNBool *i1 = dynamic_cast<RPNBool*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to boolean");
	int res = 0;
	if (i1->Get() == 0)
		res = 1;
	else
		res = 0;
	delete op1;
	return new RPNBool(res);
}

RPNElem* RPNLeq::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	int res = 0;
	if (i2->Get() <= i1->Get())
		res = 1;
	else
		res = 0;
	delete op1;
	delete op2;
	return new RPNBool(res);
}

RPNElem* RPNLe::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	int res = 0;
	if (i2->Get() < i1->Get())
		res = 1;
	else
		res = 0;
	delete op1;
	delete op2;
	return new RPNBool(res);
}

RPNElem* RPNGeq::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	int res = 0;
	if (i2->Get() >= i1->Get())
		res = 1;
	else
		res = 0;
	delete op1;
	delete op2;
	return new RPNBool(res);
}

RPNElem* RPNGe::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	int res = 0;
	if (i2->Get() > i1->Get())
		res = 1;
	else
		res = 0;
	delete op1;
	delete op2;
	return new RPNBool(res);
}

void RPNVar::Change(float val)
{
	addr -> value = val;
}

RPNElem* RPNMultiEq::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNVar *i2 = dynamic_cast<RPNVar*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to variable");
	i2 -> Change(i2->Get()*i1->Get());
	delete op1;
	delete op2;
	return 0;
}

RPNElem* RPNDivEq::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNVar *i2 = dynamic_cast<RPNVar*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to variable");
	i2 -> Change(i2->Get() / i1->Get());
	delete op1;
	delete op2;
	return 0;
}

RPNElem* RPNMinusEq::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNVar *i2 = dynamic_cast<RPNVar*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to variable");
	i2 -> Change(i2->Get() - i1->Get());
	delete op1;
	delete op2;
	return 0;
}

RPNElem* RPNPlusEq::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNVar *i2 = dynamic_cast<RPNVar*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to variable");
	i2 -> Change(i1->Get() + i2->Get());
	delete op1;
	delete op2;
	return 0;
}

RPNElem* RPNSell::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	info.my_turn.sell_price += i1->Get();
	info.my_turn.sell_amount += i2->Get();
	delete op1;
	delete op2;
	return 0;
}

RPNElem* RPNBuy::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNFloat *i2 = dynamic_cast<RPNFloat*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to float");
	info.my_turn.buy_price += i1->Get();
	info.my_turn.buy_amount += i2->Get();
	delete op1;
	delete op2;
	return 0;
}

RPNElem* RPNResProdPrice::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	int i = i1->Get();
	return new RPNFloat(info.players[i].result_prod_price);
}

RPNElem* RPNResProdBought::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	int i = i1->Get();
	return new RPNFloat(info.players[i].result_prod_bought);
}

RPNElem* RPNResRawPrice::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	int i = i1->Get();
	return new RPNFloat(info.players[i].result_raw_price);
}

RPNElem* RPNResRawSold::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	int i = i1->Get();
	return new RPNFloat(info.players[i].result_raw_sold);
}

RPNElem* RPNAutoFact::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	int i = i1->Get();
	return new RPNFloat(info.players[i].auto_factories);
}

RPNElem* RPNFact::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	int i = i1->Get();
	return new RPNFloat(info.players[i].factories);
}

RPNElem* RPNProduct::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	int i = i1->Get();
	return new RPNFloat(info.players[i].production);
}

RPNElem* RPNRaw::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	int i = i1->Get();
	return new RPNFloat(info.players[i].raw);
}

RPNElem* RPNMoney::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	int i = i1->Get();
	return new RPNFloat(info.players[i].money);
}

RPNElem* RPNProdPrice::EvaluateFun(RPNItem **stack, Context &info) const
{
	return new RPNFloat(info.game_info.production_price);
}

RPNElem* RPNDemand::EvaluateFun(RPNItem **stack, Context &info) const
{
	return new RPNFloat(info.game_info.demand);
}

RPNElem* RPNRawPrice::EvaluateFun(RPNItem **stack, Context &info) const
{
	return new RPNFloat(info.game_info.raw_price);
}

RPNElem* RPNSupply::EvaluateFun(RPNItem **stack, Context &info) const
{
	return new RPNFloat(info.game_info.supply);
}

RPNElem* RPNActivePl::EvaluateFun(RPNItem **stack, Context &info) const
{
	return new RPNFloat(info.game_info.active_players);
}

RPNElem* RPNPlayers::EvaluateFun(RPNItem **stack, Context &info) const
{
	return new RPNFloat(info.game_info.players);
}

RPNElem* RPNTurn::EvaluateFun(RPNItem **stack, Context &info) const
{
	return new RPNFloat(info.game_info.turn);
}

RPNElem* RPNMyId::EvaluateFun(RPNItem **stack, Context &info) const
{
	return new RPNFloat(info.game_info.my_id);
}

RPNElem* RPNEnd::EvaluateFun(RPNItem **stack, Context &info) const
{
	info.EndTurn();
	return 0;
}

RPNElem* RPNBuild::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	info.my_turn.build = i1->Get();
	delete op1;
	return 0;
}

RPNElem* RPNProd::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	info.my_turn.prod = i1->Get();
	delete op1;
	return 0;
}

RPNElem* RPNPrint::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNString *i1 = dynamic_cast<RPNString*>(op1);
	if (!i1){
		RPNFloat *i2 = dynamic_cast<RPNFloat*>(op1);
		if (!i2)
			throw ErrorRPN("Can't convert");
		printf("%f\n", i2->Get());
	}else{
		printf("%s\n", i1->Get());
	}
	delete op1;
	return 0;
}

RPNElem* RPNAssig::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNFloat *i1 = dynamic_cast<RPNFloat*>(op1);
	if (!i1)
		throw ErrorRPN("Can't convert to float");
	RPNVar *i2 = dynamic_cast<RPNVar*>(op2);
	if (!i2)
		throw ErrorRPN("Can't convert to variable");
	i2 -> Change(i1->Get());
	delete op1;
	delete op2;
	return 0;
}

RPNElem* RPNVal::EvaluateFun(RPNItem **stack, Context &info) const
{
	RPNElem *op1 = Pop(stack);
	RPNVar *var = dynamic_cast<RPNVar*>(op1);
	if (!var)
		throw ErrorRPN("Can't convert to variable");
	float res = var->Get();
	delete op1;
	return new RPNFloat(res);
}

void RPNElem::Push(RPNItem **stack, RPNElem *el)
{
	RPNItem *inner = new RPNItem(el);
	inner -> next = *stack;
	*stack = inner;
}

RPNElem* RPNElem::Pop(RPNItem **stack)
{
	if (*stack){
		RPNElem *res = (*stack) -> elem;
		RPNItem *now = *stack;
		*stack = (*stack) -> next;
		now -> elem = 0;
		delete now;
		return res;
	}else{
		printf("STACK IS EMPTY\n");
		return 0;
	}
}

float RPNVar::Get() const
{
	return  addr-> value;
}

int RPNBool::Get() const
{
	return value;
}

void RPNOpGo::Evaluate(RPNItem **stack, RPNItem **cur_cmd, Context &info)
const
{
	RPNElem *op1 = Pop(stack);
	RPNLabel *lab = dynamic_cast<RPNLabel*>(op1);
	if(!lab)
		throw ErrorRPN("Can't convert to label");
	RPNItem *addr = lab->Get();
	*cur_cmd = addr;
	delete op1;
}

void RPNOpGoF::Evaluate(RPNItem **stack,RPNItem **cur_cmd,Context &info)
const
{
	RPNElem *op1 = Pop(stack);
	RPNElem *op2 = Pop(stack);
	RPNLabel *lab = dynamic_cast<RPNLabel*>(op1);
	if(!lab)
		throw ErrorRPN("Can't convert to label");
	RPNBool *log = dynamic_cast<RPNBool*>(op2);
	if(!log)
		throw ErrorRPN("Can't convert to boolean");
	if (log -> Get() == 0){
		RPNItem *addr = lab->Get();
		*cur_cmd = addr;
	}else{
		*cur_cmd = (*cur_cmd) -> next;
	}
	delete op1;
	delete op2;
}

void SA::Push(RPNItem **st, const Leksema *lek)
{
	RPNItem *n = new RPNItem(lek);
	if (*st == RPN && *st != 0)
		(*st)->next = n;
	else
		n -> next = *st;
	*st = n;
}

void SA::Push(RPNItem **st, RPNItem *n)
{
	if (*st == RPN && *st != 0)
		(*st)->next = n;
	else
		n -> next = *st;
	*st = n;
}

RPNItem* SA::Pop(RPNItem **st)
{
	RPNItem *n = *st;
	*st = (*st) -> next;
	n -> next = 0;
	return n;
}

void SA::Push(LabStack **st, RPNItem *lek, int k)
{
	LabStack *n = new LabStack(lek, k);
	n -> next = *st;
	*st = n;
}

RPNItem* SA::Pop(LabStack **st, int *k)
{
	RPNItem *n = (*st) -> elem;
	*k = (*st)->kind;
	*st = (*st) -> next;
	n -> next = 0;
	return n;
}
void SA::UnwindStack(LabStack **st)
{
	LabStack *help = 0;
	int k, k1;
	RPNItem* el= Pop(st, &k), *el1, *el2;
	RPNLabel *i1, *i2, *i;
	Push(&help, el, k);
	while (*st){
		el = Pop(st, &k1);
		if (k1 != k && k != -1){
			el1 = Pop(&help, &k);
			i1 = dynamic_cast<RPNLabel*>(el->elem);
			if (!i1)
				throw ErrorRPN("Can't convert to label");
			i2 = dynamic_cast<RPNLabel*>(el1->elem);
			if (!i2)
				throw ErrorRPN("Can't convert to label");
			if(k == 0){
				i = new RPNLabel(i1->Get()->next);
				el2 = new RPNItem(i);
				el2->next = i2->Get()->next;
				i2->Get()->next = el2;
			}else{
				i = new RPNLabel(i2->Get()->next);
				el2 = new RPNItem(i);
				el2->next = i1->Get()->next;
				i1->Get()->next = el2;
			}
			if (!help)
				k = -1;
		}else{
			Push(&help, el, k1);
			k = k1;
		}
	}
}

RPNElem * MakeRPN(const Leksema *leks)
{
	if (leks->type == number)
		return new RPNFloat(atof(leks->leks));
	if (leks->type == variable)
		return new RPNVar(leks->leks);
	if (leks->type == divider || leks->type == function){
		if (strcmp(leks->leks,"(")==0)
			return new RPNBegin();
		if (strcmp(leks->leks,"+")==0){
			SA::FreeStackPr(2);
			return new RPNPlus();
		}if (strcmp(leks->leks,"-")==0){
			SA::FreeStackPr(2);
			return new RPNMinus();
		}if (strcmp(leks->leks,"&")==0){
			SA::FreeStackPr(4);
			return new RPNAnd();
		}if (strcmp(leks->leks,"|")==0){
			SA::FreeStackPr(4);
			return new RPNOr();
		}if (strcmp(leks->leks,"+=")==0){
			SA::FreeStackPr(3);
			return new RPNPlusEq();
		}if (strcmp(leks->leks,"==")==0){
			SA::FreeStackPr(3);
			return new RPNEqu();
		}if (strcmp(leks->leks,"-=")==0){
			SA::FreeStackPr(3);
			return new RPNMinusEq();
		}if (strcmp(leks->leks,">")==0){
			SA::FreeStackPr(3);
			return new RPNGe();
		}if (strcmp(leks->leks,">=")==0){
			SA::FreeStackPr(3);
			return new RPNGeq();
		}if (strcmp(leks->leks,"=")==0){
			SA::FreeStackPr(3);
			return new RPNAssig();
		}if (strcmp(leks->leks,"!=")==0){
			SA::FreeStackPr(3);
			return new RPNNotEqu();
		}if (strcmp(leks->leks,"/")==0){
			SA::FreeStackPr(1);
			return new RPNDiv();
		}if (strcmp(leks->leks,"*")==0){
			SA::FreeStackPr(1);
			return new RPNMulti();
		}if (strcmp(leks->leks,"*=")==0){
			SA::FreeStackPr(3);
			return new RPNMultiEq();
		}if (strcmp(leks->leks,"/=")==0){
			SA::FreeStackPr(3);
			return new RPNDivEq();
		}if (strcmp(leks->leks,"<")==0){
			SA::FreeStackPr(3);
			return new RPNLe();
		}if (strcmp(leks->leks,"<=")==0){
			SA::FreeStackPr(3);
			return new RPNLeq();
		}if (strcmp(leks->leks,"!")==0){
			SA::FreeStackPr(4);
			return new RPNNot();
		}if (strcmp(leks->leks,"my_id")==0){
			SA::FreeStackPr(0);
			return new RPNMyId();
		}if (strcmp(leks->leks,"turn")==0){
			SA::FreeStackPr(0);
			return new RPNTurn();
		}if (strcmp(leks->leks,"players")==0){
			SA::FreeStackPr(0);
			return new RPNPlayers();
		}if (strcmp(leks->leks,"active_players")==0){
			SA::FreeStackPr(0);
			return new RPNActivePl();
		}if (strcmp(leks->leks,"supply")==0){
			SA::FreeStackPr(0);
			return new RPNSupply();
		}if (strcmp(leks->leks,"raw_price")==0){
			SA::FreeStackPr(0);
			return new RPNRawPrice();
		}if (strcmp(leks->leks,"demand")==0){
			SA::FreeStackPr(0);
			return new RPNDemand();
		}if (strcmp(leks->leks,"production_price")==0){
			SA::FreeStackPr(0);
			return new RPNProdPrice();
		}if (strcmp(leks->leks,"money")==0){
			SA::FreeStackPr(0);
			return new RPNMoney();
		}if (strcmp(leks->leks,"raw")==0){
			SA::FreeStackPr(0);
			return new RPNRaw();
		}if (strcmp(leks->leks,"production")==0){
			SA::FreeStackPr(0);
			return new RPNProduct();
		}if (strcmp(leks->leks,"factories")==0){
			SA::FreeStackPr(0);
			return new RPNFact();
		}if (strcmp(leks->leks,"auto_factories")==0){
			SA::FreeStackPr(0);
			return new RPNAutoFact();
		}if (strcmp(leks->leks,"result_raw_sold")==0){
			SA::FreeStackPr(0);
			return new RPNResRawSold();
		}if (strcmp(leks->leks,"result_raw_price")==0){
			SA::FreeStackPr(0);
			return new RPNResRawPrice();
		}if (strcmp(leks->leks,"result_prod_bought")==0){
			SA::FreeStackPr(0);
			return new RPNResProdBought();
		}if (strcmp(leks->leks,"result_prod_price")==0){
			SA::FreeStackPr(0);
			return new RPNResProdPrice();
		}if (strcmp(leks->leks,"buy")==0){
			SA::FreeStackPr(3);
			return new RPNBuy();
		}if (strcmp(leks->leks,"sell")==0){
			SA::FreeStackPr(3);
			return new RPNSell();
		}if (strcmp(leks->leks,"prod")==0){
			SA::FreeStackPr(3);
			return new RPNProd();
		}if (strcmp(leks->leks,"build")==0){
			SA::FreeStackPr(3);
			return new RPNBuild();
		}if (strcmp(leks->leks,"endturn")==0){
			SA::FreeStackPr(3);
			return new RPNEnd();
		}
	}else{
		throw ErrorRPN("Can't make RPN element");
	}
	return 0;
}


void SA::FreeStackPr(int i)
{
	RPNItem *elem;
	RPNOper *op;
	while(1){
		elem = Pop(&stack);
		op = dynamic_cast<RPNOper *>(elem->elem);
		if (!op)
			ErrorRPN("Not an operator");
		if (op->Priority() <= i){
			Push(&RPN, elem);
		}else{
			Push(&stack, elem);
			break;
		}
	}
}

LekList *MakeList()
{
	LA A;
	int c = ' ';
	Leksema *l;
	LekList *list = 0;
	int ok = 0;
	while (ok == 0){
		if(c == EOF)
			ok = 1;
		c = getchar();
		l = A.GetSym(c);
		if (l != 0)
			list = list -> Add(l);
		delete l;
	}
	return list;
}

void Context::BeginGame()
{
	serv.Put(my_name);
	serv.Put("\n");
	if (strcmp(arg[4],"create")==0){
		serv.Put(".create\n");
		int n = atoi(arg[5]);
		for(int i = 0; i<n; i++){
			serv.Find("@+ JOIN");
		}
		serv.Put("start\n");
	}else{
		serv.Put(".join ");
		serv.Put(arg[5]);
		serv.Put("\n");
	}
	serv.Find("& START");
	serv.Put("info\n");
	serv.Find("& PLAYERS");
	game_info.players = serv.ReadNum();
	serv.Put("info\n");
	players = new Player[game_info.players];
	for(int i = 0; i < game_info.players; i++){
		serv.Find("& INFO");
		players[i].name = serv.ReadName();
		if (strcmp(players[i].name,my_name)==0){
			game_info.my_id = i;
		}
		players[i].id = i;
	}
	game_info.active_players = game_info.players;
	BeginTurn();
}

void Context::BeginTurn()
{
	char *name;
	int j;
	serv.Put("info\n");
	serv.Find("& PLAYERS");
	game_info.active_players = serv.ReadNum();
	if (game_info.active_players > 1){
		serv.Put("info\n");
		for(int i = 0; i < game_info.active_players; i++){
			serv.Find("& INFO");
			name = serv.ReadName();
			for(j = 0; j < game_info.players; j++)
				if(strcmp(name,players[j].name)==0)
					break;
			players[j].raw = serv.ReadNum();
			players[j].production = serv.ReadNum();
			players[j].money = serv.ReadNum();
			players[j].factories = serv.ReadNum();
			players[j].auto_factories = serv.ReadNum();
		}
		serv.Put("market\n");
		serv.Find("& MARKET");
		(game_info.turn)++;
		game_info.supply = serv.ReadNum();
		game_info.raw_price = serv.ReadNum();
		game_info.demand = serv.ReadNum();
		game_info.production_price = serv.ReadNum();
		my_turn.buy_price = 0;
		my_turn.sell_price = 0;
		my_turn.buy_amount = 0;
		my_turn.sell_amount = 0;
		my_turn.prod = 0;
		my_turn.build = 0;
	}
}

void Context::EndTurn()
{
	serv.Put("buy ");
	serv.Put(my_turn.buy_amount);
	serv.Put(" ");
	serv.Put(my_turn.buy_price);
	serv.Put("\n");
	serv.Put("sell ");
	serv.Put(my_turn.sell_amount);
	serv.Put(" ");
	serv.Put(my_turn.sell_price);
	serv.Put("\n");
	serv.Put("prod ");
	serv.Put(my_turn.prod);
	serv.Put("\n");
	serv.Put("turn\n");
	serv.Find("price\n");
	for(int i = 0; i<game_info.players; i++){
		players[i].result_raw_sold = 0;
		players[i].result_raw_price = 0;
		players[i].result_prod_bought = 0;
		players[i].result_prod_price = 0;
	}
	int j;
	char *next = serv.ReadName(), *what, *name;
	while (strcmp(next, "&") == 0){
		what = serv.ReadName();
		if (strcmp(what, "BOUGHT")==0){
			name = serv.ReadName();
			for(j = 0; j < game_info.players; j++)
				if(strcmp(name,players[j].name)==0)
					break;
			players[j].result_raw_sold = serv.ReadNum();
			players[j].result_raw_price = serv.ReadNum();
		}else if (strcmp(what, "SOLD")==0){
			name = serv.ReadName();
			for(j = 0; j < game_info.players; j++)
				if(strcmp(name,players[j].name)==0)
					break;
			players[j].result_prod_bought = serv.ReadNum();
			players[j].result_prod_price = serv.ReadNum();
		}else{
			name = serv.ReadName();
		}
		next = serv.ReadName();
	}
	BeginTurn();
}

void Server::Find(const char *str)
{
	char buf[2];
	int r, i = 0;
	while(str[i] != '\0'){
		r = read(sockfd, buf, 1);
		if(!r)
			throw ErrorServer("Can't read game data");
		printf("%c",buf[0]);
		if (buf[0] == str[i]){
			i++;
		}else if (i != 0){
			i = 0;
		}
	}
}

char * Server::ReadName()
{
	char buf[1024], *val;
	int i = 0, r;
	while(1){
		r = read(sockfd, buf, 1);
		if(!r)
			throw ErrorServer("Can't read game data");
		printf("%c",buf[0]);
		if (buf[0] != ' ' && buf[0] != '\n' && buf[0] != '\t')
			break;
	}
	while(1){
		i++;
		r = read(sockfd, &buf[i], 1);
		if(!r)
			throw ErrorServer("Can't read game data");
		printf("%c",buf[i]);
		if (buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\t'){
			buf[i] = '\0';
			break;
		}
	}
	val = new char[i+1];
	strcpy(val, buf);
	return val;
}

int Server::ReadNum()
{
	char *name = ReadName();
	return atoi(name);
}

void Server::Put(const char *str)
{
	write(sockfd, str, strlen(str));
}

void Server::Put(int n)
{
	char buf[1024];
	sprintf(buf, "%d",n);
	int i = 0;
	while (buf[i] != '\0' && buf[i] != '.' && buf[i] != ',')
		i++;
	buf[i] = '\0';
	Put(buf);
}

int main(int argc, char **argv)
{
	try{
		LA A;
		RPNItem *RPNlist, *current, *stack;
		LekList *list = 0;
		Context info(argc, argv);
		OpenFiles(argc, argv);
		list = MakeList();
		SA S(list);
		RPNlist = S.Start();
		current = RPNlist;
		while(current)
			current->elem->Evaluate(&stack, &current, info);
	}
	catch(const ErrorSA &E){
		printf("In string #%d: %s expected, but '%s' found\n", 
			E.str_num, E.er_name, E.leks);
		exit(1);
	}
	catch(const ErrorLA &E){
		printf("In string #%d: %s\n", E.str_num, E.er_name); exit(1);
	}
	catch(const ErrorRPN &E){
		printf("IN RPN: %s\n", E.er_name); exit(1);
	}
	catch(const ErrorServer &E){
		printf("SERVER ERROR: %s\n", E.er_name); exit(1);
	}
	return 0;
}
