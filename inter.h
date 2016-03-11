#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


enum type{
    divider, keyword, number, function, variable, string
};

enum type_stack{
    iff, whilee
};

class Error{
public:
    const char *er_name;
    int str_num;
};

class ErrorServer: public Error{
public:
    ErrorServer(const char *err){er_name = err; str_num = 0;};
    ~ErrorServer(){delete[] er_name;}
};


class ErrorLA: public Error{
public:
    ErrorLA(){er_name = new char[6]; er_name = "Error"; str_num = -1;};
    ErrorLA(const char *err, int num) {er_name = err; str_num = num;};
    ~ErrorLA(){delete[] er_name;};
};

class ErrorSA: public Error{
public:
	const char *leks;
	ErrorSA(){
		er_name = new char[6];
		er_name = "Error";
		str_num = -1;
		leks = new char[3];
		leks = "No";
	};
	ErrorSA(const char *err, int num, const char *le){
		er_name = err;
		str_num = num;
		leks = le;
	}; 
	~ErrorSA(){delete[] er_name; delete[] leks;};
};

class ErrorRPN: public Error{
public:
	ErrorRPN(){er_name = new char[7]; er_name = "Error4"; str_num = -1;};
	ErrorRPN(const char *err, int num = 0) {er_name = err; str_num = num;};
	~ErrorRPN(){delete[] er_name;};
};

class Server{
	int sockfd;
public:
	void Find(const char *str);
	int ReadNum();
	char * ReadName();
	void Put(const char *str);
	void Put(int n);
	Server(int argc, char **argv){
		if (argc < 5)
			throw ErrorServer("Need more parameters");
		int port = atoi(argv[2]);
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
			throw ErrorServer("Can't make socket");
		if (!inet_aton(argv[1], &(addr.sin_addr)))
			throw ErrorServer("Invalid IP-adress");
		if (0 != connect(sockfd, (struct sockaddr*)&addr, sizeof(addr))){
			throw ErrorServer("Connect error");
		}
	}
};

class Leksema{
public:
    int type;
    char *leks;
    int str_num;
    Leksema(const Leksema &l){
        leks = new char[strlen(l.leks) + 1];
        type = l.type;
        str_num = l.str_num;
        for(unsigned int i = 0; i < strlen(l.leks) + 1; i++)
            leks[i] = l.leks[i];
    };
    Leksema(char *name, int num, int t){
        leks = name;
        str_num = num;
        type = t;
    };
    ~Leksema(){delete [] leks;};
	void Print();
};

struct LekList{
	LekList* Add(Leksema *l);
    Leksema *leks;
    LekList *next;
	~LekList(){delete leks;};
};

class LA{
	enum autom_status{
    	H, C, D, K, I, L, T
	};
	enum sym_type{
    	alfa, beta, sigma, rho, delta, quote, point
	};
    static const char *KeyTable[];
    static const char *FuncTable[];
    int str_num;
    char *buf;
    int len;
    int len0;
    int state;
    Leksema* HF(int c);
	Leksema* CF(int c);
	Leksema* DF(int c);
	Leksema* KF(int c);
	Leksema* IF(int c);
	Leksema* TF(int c);
	Leksema* LF(int c);
	Leksema* SF(int c, int st);
	int SymType(int c);
	int SearchInTable(char *buff);
	char* MakeBuff(char *buff);
	void NulNum();
public:
	LA(){
		str_num = 1;
	    buf = new char[1024];
        len = 0;
        len0 = 0;
        state = H;
    };
	~LA(){if (buf !=0) delete[] buf;};
	Leksema* GetSym(int c);
};

struct RPNItem;

struct Player{
	char *name;
	int id;
	int money;
	int raw;
	int production;
	int factories;
	int auto_factories;
	int result_raw_sold;
	int result_raw_price;
	int result_prod_bought;
	int result_prod_price;
	Player(){
		name = 0;
		id = 0;
		money = 10000;
		raw = 2;
		production = 2;
		factories = 2;
		auto_factories = 0;
		result_raw_sold = 0;
		result_raw_price = 0;
		result_prod_bought = 0;
		result_prod_price = 0;
	}
};
	
struct GameInfo{
	int my_id;
	int turn;
	int players;
	int active_players;
	int supply;
	int raw_price;
	int demand;
	int production_price;
	GameInfo(){
		my_id = 0;
		turn = 0;
		players = 0;
		active_players = 0;
		supply = 0;
		raw_price = 0;
		demand = 0;
		production_price = 0;
	}
};

struct MyTurn{
	int buy_price;
	int sell_price;
	int buy_amount;
	int sell_amount;
	int prod;
	int build;
	MyTurn(){
		buy_price = 0;
		sell_price = 0;
		buy_amount = 0;
		sell_amount = 0;
		prod = 0;
		build = 0;
	}
};
		
struct Context{
	Player *players;
	GameInfo game_info;
	MyTurn my_turn;
	Server serv;
	int info;
	char** arg;
	char *my_name;
	void BeginGame();
	void BeginTurn();
	void EndTurn();
	Context(int argc, char **argv):serv(argc,argv){
		my_name = argv[3];
		arg = argv;
		info = 0;
	}
};

class RPNElem{
public:
	virtual void Evaluate(RPNItem **stack,RPNItem **cur_cmd,Context &info)
		const = 0;
	virtual ~RPNElem(){};
	static void Push(RPNItem **stack, RPNElem *el);
	static RPNElem* Pop(RPNItem **stack);
};

RPNElem * MakeRPN(const Leksema *leks);

struct RPNItem{
	RPNElem *elem;
	RPNItem *next;
	RPNItem(RPNElem *el){elem = el;};
	RPNItem(const Leksema *leks){
		elem = MakeRPN(leks);
		next = 0;
	}
};

class RPNConst: public RPNElem{
public:
	virtual RPNElem* Copy() const = 0;
	void Evaluate(RPNItem **stack, RPNItem **cur_cmd, Context &info) 
		const;
	virtual ~RPNConst(){};
};

class RPNFunc: public RPNElem{
public:
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info)
		const = 0;
	void Evaluate(RPNItem **stack, RPNItem **cur_cmd, Context &info)
		const;
	virtual ~RPNFunc(){};
};

class RPNOper: public RPNFunc{
protected:
	int pr;
public:
	int Priority() const {return pr;};
	virtual ~RPNOper(){}
	RPNOper():pr(3){}
};

class RPNFuncFunc: public RPNOper{
public:
	RPNFuncFunc(){pr = 0;}
	virtual ~RPNFuncFunc(){}
};

class RPNKey: public RPNOper{
public:
	RPNKey(){pr = 3;}
	virtual ~RPNKey(){}
};

class RPNAlgStrong: public RPNOper{
public:
	RPNAlgStrong(){pr = 1;};
	virtual ~RPNAlgStrong(){};
};

class RPNAlgLess: public RPNOper{
public:
	RPNAlgLess(){pr = 2;};
	virtual ~RPNAlgLess(){};
};

class RPNLogFun: public RPNOper{
public:
	RPNLogFun(){pr = 4;};
	virtual ~RPNLogFun(){};
};

class RPNString: public RPNConst{
	char * str;
public:
	RPNString(char *s){str = s;};
	virtual ~RPNString(){delete[] str;};
	virtual RPNElem* Copy() const;
	const char *Get() const {return str;};
};

class RPNBeginProg: public RPNFunc{
public:
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const{
		info.BeginGame();
		return 0;
	}
	virtual ~RPNBeginProg(){};
	RPNBeginProg(){};
};

class RPNEndProg: public RPNFunc{
public:
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const{
		return 0;
	};
	virtual ~RPNEndProg(){};
	RPNEndProg(){};
};

class RPNBegin: public RPNOper{
public:
	virtual ~RPNBegin(){};
	RPNBegin(){pr = 5;};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const{
		return 0;
	};
};

class RPNFloat: public RPNConst{
	float value;
public:
	RPNFloat(float a = 0){value = a;};
	virtual ~RPNFloat(){};
	virtual RPNElem* Copy() const;
	float Get() const {return value;};
};

class RPNMultiEq: public RPNOper{
public:
	RPNMultiEq() {};
	virtual ~RPNMultiEq() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNMulti: public RPNAlgStrong{
public:
	RPNMulti() {};
	virtual ~RPNMulti() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNPlusEq: public RPNOper{
public:
	RPNPlusEq() {};
	virtual ~RPNPlusEq() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNPlus: public RPNAlgLess{
public:
	RPNPlus() {};
	virtual ~RPNPlus() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNDivEq: public RPNOper{
public:
	RPNDivEq() {};
	virtual ~RPNDivEq() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNDiv: public RPNAlgStrong{
public:
	RPNDiv() {};
	virtual ~RPNDiv() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNOr: public RPNLogFun{
public:
	RPNOr() {};
	virtual ~RPNOr() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNAnd: public RPNLogFun{
public:
	RPNAnd() {};
	virtual ~RPNAnd() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNMinusEq: public RPNOper{
public:
	RPNMinusEq() {};
	virtual ~RPNMinusEq() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNMinus: public RPNAlgLess{
public:
	RPNMinus() {};
	virtual ~RPNMinus() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNAssig: public RPNOper{
public:
	RPNAssig() {};
	virtual ~RPNAssig() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNVal: public RPNFunc{
public:
	RPNVal() {};
	virtual ~RPNVal() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNEqu: public RPNOper{
public:
	RPNEqu() {};
	virtual ~RPNEqu() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNNot: public RPNLogFun{
public:
	RPNNot() {};
	virtual ~RPNNot() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNLeq: public RPNOper{
public:
	RPNLeq() {};
	virtual ~RPNLeq() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNLe: public RPNOper{
public:
	RPNLe() {};
	virtual ~RPNLe() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNGeq: public RPNOper{
public:
	RPNGeq() {};
	virtual ~RPNGeq() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNGe: public RPNOper{
public:
	RPNGe() {};
	virtual ~RPNGe() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNNotEqu: public RPNOper{
public:
	RPNNotEqu() {};
	virtual ~RPNNotEqu() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNLabel: public RPNConst{
	RPNItem* value;
public:
	RPNLabel(RPNItem *a){value = a;};
	virtual ~RPNLabel() {};
	virtual RPNElem* Copy() const
		{ return new RPNLabel(value);}
	RPNItem* Get() const { return value; }
};

class RPNArray: public RPNFunc{
public:
	RPNArray() {};
	virtual ~RPNArray() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNPrint: public RPNKey{
public:
	RPNPrint() {};
	virtual ~RPNPrint() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNBuy: public RPNKey{
public:
	RPNBuy() {};
	virtual ~RPNBuy() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNSell: public RPNKey{
public:
	RPNSell() {};
	virtual ~RPNSell() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNProd: public RPNKey{
public:
	RPNProd() {};
	virtual ~RPNProd() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNBuild: public RPNKey{
public:
	RPNBuild() {};
	virtual ~RPNBuild() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNEnd: public RPNKey{
public:
	RPNEnd() {};
	virtual ~RPNEnd() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNMyId: public RPNFuncFunc{
public:
	RPNMyId() {};
	virtual ~RPNMyId() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNTurn: public RPNFuncFunc{
public:
	RPNTurn() {};
	virtual ~RPNTurn() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNPlayers: public RPNFuncFunc{
public:
	RPNPlayers() {};
	virtual ~RPNPlayers() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNActivePl: public RPNFuncFunc{
public:
	RPNActivePl() {};
	virtual ~RPNActivePl() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNSupply: public RPNFuncFunc{
public:
	RPNSupply() {};
	virtual ~RPNSupply() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNRawPrice: public RPNFuncFunc{
public:
	RPNRawPrice() {};
	virtual ~RPNRawPrice() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNDemand: public RPNFuncFunc{
public:
	RPNDemand() {};
	virtual ~RPNDemand() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNProdPrice: public RPNFuncFunc{
public:
	RPNProdPrice() {};
	virtual ~RPNProdPrice() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNMoney: public RPNFuncFunc{
public:
	RPNMoney() {};
	virtual ~RPNMoney() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNRaw: public RPNFuncFunc{
public:
	RPNRaw() {};
	virtual ~RPNRaw() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNProduct: public RPNFuncFunc{
public:
	RPNProduct() {};
	virtual ~RPNProduct() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNFact: public RPNFuncFunc{
public:
	RPNFact() {};
	virtual ~RPNFact() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNAutoFact: public RPNFuncFunc{
public:
	RPNAutoFact() {};
	virtual ~RPNAutoFact() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNResRawSold: public RPNFuncFunc{
public:
	RPNResRawSold() {};
	virtual ~RPNResRawSold() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNResRawPrice: public RPNFuncFunc{
public:
	RPNResRawPrice() {};
	virtual ~RPNResRawPrice() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNResProdBought: public RPNFuncFunc{
public:
	RPNResProdBought() {};
	virtual ~RPNResProdBought() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

class RPNResProdPrice: public RPNFuncFunc{
public:
	RPNResProdPrice() {};
	virtual ~RPNResProdPrice() {};
	virtual RPNElem* EvaluateFun(RPNItem **stack, Context &info) const;
};

struct VarTable{
	float value;
	int index;
	const char *name;
	VarTable *next;
	VarTable *array;
	VarTable(const char *n, float val = 0, int id = 0){
		name = n;
		value = val;
		next = 0;
		index = id;
	}
};

class RPNVar: public RPNConst{
	static	VarTable* table;
	VarTable* addr;
public:
	RPNVar(const char *n, float value = 0){
		VarTable *cur = table;
		addr = 0;
		while (cur){
			if (strcmp(n, cur->name)==0)
				addr = cur;
			cur = cur->next;
		}
		if (!addr){
			addr = new VarTable(n, value);
			addr -> next = table;
			table = addr;
		}
	};
	void NewRPNVar(RPNVar *a, int id){
		VarTable *cur = a -> addr, *ad = a -> addr;
		for(int i = 1; i <= id; i++){
			if (!(cur -> array)){
				ad = new VarTable(a->addr->name, i);
				cur -> array = ad;
			}
			cur = cur->array;
		}
	};
	RPNVar(VarTable* adr){
		addr = adr;
	};
	virtual ~RPNVar(){};
	float Get() const;
	void Change(float val);
	VarTable *GetArray(int i);
	RPNElem* Copy() const;
};

class RPNOpGo: public RPNElem{
public:
	RPNOpGo(){};
	virtual ~RPNOpGo() {};
	void Evaluate(RPNItem **stack, RPNItem **cur_cmd, Context &info)
		const;
};
	
class RPNOpGoF: public RPNElem{
public:
	RPNOpGoF(){};
	virtual ~RPNOpGoF() {};
	void Evaluate(RPNItem **stack, RPNItem **cur_cmd, Context &info)
		const;
};

class RPNBool: public RPNConst{
	enum{
		False, True
	};
	int value;
public:
	RPNBool(int val){
		if (val == 0)
			value = False;
		else
			value = True;
	};
	virtual ~RPNBool() {};
	int Get() const;
	RPNElem* Copy() const;
};


void print(LekList *list);

struct LabStack{
	RPNItem *elem;
	int kind;
	LabStack *next;
	LabStack(RPNItem *el, int k = 0){
		elem = el;
		kind = k;
		next = 0;
	}
};

class SA{
	friend RPNElem * MakeRPN(const Leksema *leks);
	char *leks;
	int type;
	int str_num;
	LekList *list;
	static RPNItem* stack;
	static LabStack* stack_if_false;
	static LabStack* stack_while_true;
	static LabStack* stack_if_true;
	static LabStack* stack_while_false;
	static RPNItem* RPN;
	void O();
	void K();
	void L();
	void V();
	void P();
	void W();
	void R();
	void F();
	void T();
	void Q();
	void B();
	void A();
	void C();
	void D();
	void E();
	void G();
	void N();
	void M();
	void Z();
	void Y();
	static void Push(RPNItem **st, const Leksema *lek);
	static void Push(RPNItem **st, RPNItem *lek);
	static RPNItem* Pop(RPNItem **st);
	static void Push(LabStack **st, RPNItem *lek, int k);
	static RPNItem* Pop(LabStack **st, int *k);
	void FreeTo();
	void UnwindStack(LabStack **st);
	static void FreeStackPr(int i);
	void WhereFalse(int type);
	void WhereTrue(int type);
	void WhatFalse(int type);
	void WhatTrue(int type);
	void Bracket();
public:
	RPNItem* Start();
	void Next();
	SA(LekList *li){
		list = li;
		type = list -> leks -> type;
		str_num = list -> leks -> str_num;
		leks = list -> leks -> leks;
		stack = 0;
		RPN = 0;
	};
};

RPNItem* SA::stack = 0;
LabStack* SA::stack_if_true = 0;
LabStack* SA::stack_if_false = 0;
LabStack* SA::stack_while_true = 0;
LabStack* SA::stack_while_false = 0;
RPNItem* SA::RPN = 0;

