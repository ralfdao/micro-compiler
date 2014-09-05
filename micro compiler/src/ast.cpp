/*
 *         File: ast.cpp
 *         Author: Yu Liu(liu-island@purdue.edu)
 *         
 */
#include "ast.h"

int cnt_tmp = 0; //keeps the number of temp variables 
int cnt_label = 0; //keeps the number of labels
registers reg;

string get_tmp() 
{ 
	char buf[10];
	sprintf(buf, "$T%d", ++cnt_tmp);
	return string(buf);
}

string get_label() 
{ 
	char buf[10];
	sprintf(buf, "label%d", ++cnt_label);
	return string(buf);
}
	
void IR_node::output()
{
	if(opcode != "")
		cout << opcode;
	if(op1 != "")
		cout << " " << op1;
	if(op2 != "")
		cout << " " << op2;
	if(op3 != "")
		cout << " " << op3;
}

bool IRnodeInList::isleader()
{
	if(node.opcode == "LABEL")
		return true;
	
	vector<IRnodeInList*>::iterator it = preVec.begin(); 
	for(; it != preVec.end(); ++it)
	{
		if( ((*it)->node.opcode == "NE") || ((*it)->node.opcode == "EQ") || ((*it)->node.opcode == "LE") ||
			((*it)->node.opcode == "GE") || ((*it)->node.opcode == "LT") || ((*it)->node.opcode == "GT") )
			return true;
	}
	return false;
}

void registers::markDirty(string rg)
{
	if("r0" == rg)
		isdirty0 = true;
	if("r1" == rg)
		isdirty1 = true;
	if("r2" == rg)
		isdirty2 = true;
	if("r3" == rg)
		isdirty3 = true;
}

void registers::reset(IRnodeInList* p, Symtable* pfunc, vector<IRnodeInList*> *pp)
{
	vector<IRnodeInList*>::iterator Iter = pp->begin();
	Iter++;
	for(; Iter != pp->end(); Iter++) 
	{
		if(p == *Iter)
		{
			Iter--;
			free("r0", *Iter, pfunc);
			free("r1", *Iter, pfunc);
			free("r2", *Iter, pfunc);
			free("r3", *Iter, pfunc);
			break;
		}
	}
}

string registers::allocate(string opr, IRnodeInList* p, Symtable* pfunc)
{
	if("" == r0)
	{
		r0 = opr;
		return "r0";
	}
	else if("" == r1)
	{
		r1 = opr;
		return "r1";
	}
	else if("" == r2)
	{
		r2 = opr;
		return "r2";
	}
	else if("" == r3)
	{
		r3 = opr;
		return "r3";
	}
	else
	{
		string ops[3];
		ops[0] = p->node.op1;
		ops[1] = p->node.op2;
		ops[2] = p->node.op3;
		
		int i;
		for(i = 0; i < 3; i++)
			if(r0 == ops[i]) break;
		
		if(i == 3)
		{
			free("r0", p, pfunc);
			r0 = opr;
			return "r0";
		}
		
		for(i = 0; i < 3; i++)
			if(r1 == ops[i]) break;
		
		if(i == 3)
		{
			free("r1", p, pfunc);
			r1 = opr;
			return "r1";
		}
		
		for(i = 0; i < 3; i++)
			if(r2 == ops[i]) break;
		
		if(i == 3)
		{
			free("r2", p, pfunc);
			r2 = opr;
			return "r2";
		}
		
		for(i = 0; i < 3; i++)
			if(r3 == ops[i]) break;
		
		if(i == 3)
		{
			free("r3", p, pfunc);
			r3 = opr;
			return "r3";
		}
	}	
}
	
string registers::ensure(string opr, IRnodeInList* p, Symtable* pfunc)
{
	if(opr == r0)
		return "r0";
	else if(opr == r1)
		return "r1";
	else if(opr == r2)
		return "r2";
	else if(opr == r3)
		return "r3";
	else
	{
		string r = allocate(opr, p, pfunc);
		cout << ";by ensure" << endl;
		cout << "move " << pfunc->IRtoTiny(opr) << " " << r << endl;
		return r;
	}
}
	
void registers::free(string rg/*"r0"*/, IRnodeInList* p, Symtable* pfunc)
{
	if(rg == "r0")
	{
		vector<string>::iterator it = p->node.live_out_vec.begin();
		vector<string>::iterator it_end = p->node.live_out_vec.end();
		for(; it != it_end; ++it) 
		{
			if(r0 == *it) break;
		}
		
		if(isdirty0 && (it != it_end))
		{
			cout << ";by free" << endl;
			cout << "move r0 " << pfunc->IRtoTiny(r0) << endl;
		}
		
		r0 = "";
		isdirty0 = false;
	}
	else if(rg == "r1")
	{
		vector<string>::iterator it = p->node.live_out_vec.begin();
		vector<string>::iterator it_end = p->node.live_out_vec.end();
		for(; it != it_end; ++it)
		{
			if(r1 == *it) break;
		}
		
		if(isdirty1 && (it != it_end))
		{
			cout << ";by free" << endl;
			cout << "move r1 " << pfunc->IRtoTiny(r1) << endl;
		}
		
		r1 = "";
		isdirty1 = false;
	}
	else if(rg == "r2")
	{
		vector<string>::iterator it = p->node.live_out_vec.begin();
		vector<string>::iterator it_end = p->node.live_out_vec.end();
		for(; it != it_end; ++it) 
		{
			if(r2 == *it) break;
		}
		
		if(isdirty2 && (it != it_end))
		{
			cout << ";by free" << endl;
			cout << "move r2 " << pfunc->IRtoTiny(r2) << endl;
		}
		
		r2 = "";
		isdirty2 = false;
	}
	else if(rg == "r3")
	{
		vector<string>::iterator it = p->node.live_out_vec.begin();
		vector<string>::iterator it_end = p->node.live_out_vec.end();
		for(; it != it_end; ++it)
		{
			if(r3 == *it) break;
		}
		
		if(isdirty3 && (it != it_end))
		{
			cout << ";by free" << endl;
			cout << "move r3 " << pfunc->IRtoTiny(r3) << endl;
		}
		
		r3 = "";
		isdirty3 = false;
	}
}	

exp_node::exp_node(exp_node *n)
{
	num = n->num;
	type = n->type;
	ir.opcode = n->ir.opcode;
	ir.op1 = n->ir.op1;
	ir.op2 = n->ir.op2;
	ir.op3 = n->ir.op3;
}
	
exp_node::exp_node(float value, string t) 
{ 
	num = value; 
	type = t; 
}
	
void exp_node::printIR() 
{
	ostringstream oss;
	oss << num;
	
	pbase = new IRnodeInList;	
	pbase->node.opcode = ir.opcode;
	pbase->node.op1 = oss.str();
	pbase->node.op3 = ir.op3;	
	pbase->node.kill_vec.push_back(ir.op3);
	IRnodeList->push_back(pbase);
}

string exp_node::genIR() 
{
	if(type == "INT")
		ir.opcode = "STOREI";
	else if(type == "FLOAT")
		ir.opcode = "STOREF";
	ir.op3 = get_tmp();
	return ir.op3;
}

void exp_node::printTiny() 
{
	if(pbase->isleader())
		reg.reset(pbase, pfunc, IRnodeList);
		
	string Ry = reg.allocate(pbase->node.op3, pbase, pfunc);
	cout << ";by exp_node" << endl;			
	cout << "move " << num << " " << Ry << endl;	
	reg.markDirty(Ry);	
}

exp_node::~exp_node() {}

id_node::id_node(string id, string t) 
{
	name = id;
	type = t;
}

void id_node::printIR() {}
void id_node::printTiny() {}

string id_node::genIR()
{
	if(type == "STRING") 
		return name;
		
	string str = pglobal->getIRname(name);
	if(str == name)
		str = pfunc->getIRname(name);
	return str;
}

exp_node_list::~exp_node_list()
{
	list<exp_node*>::iterator iter = exp_nodes->begin();
	for(; iter != exp_nodes->end(); ++iter)
		delete *iter;
	delete exp_nodes;
}

void fun_call_node::getSymtable()
{
	vector<Symtable*>::iterator iter = symtab_vec.begin(); 
	for(; iter != symtab_vec.end(); ++iter) 
	{
		Symtable* sym = *iter;
		if(sym && sym->scope == name)
		{
			funsym = sym;
			break;
		}
	}
	type = funsym->type;
}

void fun_call_node::printIR() 
{ 
	pPush = new IRnodeInList;	
	pPush->node.opcode = "PUSH";		
	IRnodeList->push_back(pPush);
	
	list<exp_node*>::iterator iter = explist->exp_nodes->begin();
	list<string>::iterator iter_tmp = tmplist.begin();
	for(; iter != explist->exp_nodes->end(); ++iter, ++iter_tmp) 
	{	
		(*iter)->printIR();
		
		IRnodeInList* p = new IRnodeInList;	
		p->node.opcode = "PUSH";
		p->node.op1 = *iter_tmp;
		p->node.gen_vec.push_back(*iter_tmp);
		IRnodeList->push_back(p);
		pushList.push_back(p);
	}
		
	pJsr = new IRnodeInList;	
	pJsr->node.opcode = "JSR";		
	pJsr->node.op1 = name;
	
	vector<string>::iterator iter1 = pglobal->syms_local.begin();  
	for(; iter1 != pglobal->syms_local.end(); ++iter1)
	{	
		Information* pi = pglobal->findInfo(*iter1);
		if(pi && pi->type != "STRING")
			pJsr->node.gen_vec.push_back(pglobal->getIRname(pi->name));
	}
	
	IRnodeList->push_back(pJsr);
	
	int size = explist->exp_nodes->size();
	for(int i = 0; i < size; i++)
	{
		IRnodeInList* p = new IRnodeInList;	
		p->node.opcode = "POP";		
		IRnodeList->push_back(p);
		popList.push_back(p);			
	}
	
	pPop = new IRnodeInList;	
	pPop->node.opcode = "POP";		
	pPop->node.op3 = ir.op3;
	pPop->node.kill_vec.push_back(ir.op3);
	IRnodeList->push_back(pPop);
}

string fun_call_node::genIR() 
{ 
	list<exp_node*>::iterator iter = explist->exp_nodes->begin(); 
	for(; iter != explist->exp_nodes->end(); ++iter) 
	{	
		(*iter)->pglobal = pglobal;
		(*iter)->pfunc = pfunc;
		(*iter)->IRnodeList = IRnodeList;

		string tmp = (*iter)->genIR();
		tmplist.push_back(tmp);
	}
	ir.op3 = get_tmp();
	return ir.op3;
}

void fun_call_node::printTiny() 
{
	if(pPush->isleader())
		reg.reset(pPush, pfunc, IRnodeList);
	cout << "push" << endl;
	
	list<exp_node*>::iterator iter = explist->exp_nodes->begin();
	list<exp_node*>::iterator iter_end = explist->exp_nodes->end(); 
	list<string>::iterator iter_tmp = tmplist.begin();
	list<IRnodeInList*>::iterator iter_push = pushList.begin();
	for(; iter != iter_end; ++iter, ++iter_tmp, ++iter_push) 
	{	
		(*iter)->printTiny();
		
		if((*iter_push)->isleader())
			reg.reset(*iter_push, pfunc, IRnodeList);
					
		string s1(*iter_tmp);
		bool b1 = false;
		std::size_t found = s1.find("$T");
		if(found != string::npos)
		{
			s1 = reg.ensure(*iter_tmp, *iter_push, pfunc); 
			b1 = true;
		}
		else		
			s1 = pfunc->IRtoTiny(s1);
		
		cout << "push " << s1 << endl;
		
		if(b1)
		{
			vector<string>::iterator it = (*iter_push)->node.live_out_vec.begin();
			vector<string>::iterator it_end = (*iter_push)->node.live_out_vec.end();
			for(; it != it_end; ++it) 
			{
				if(s1 == *it) break;
			}
			
			if(it == it_end)
				reg.free(s1, *iter_push, pfunc);
		}
	}
	
	if(pJsr->isleader())
		reg.reset(pJsr, pfunc, IRnodeList);
	cout << "push r0" << endl;
	cout << "push r1" << endl;
	cout << "push r2" << endl;
	cout << "push r3" << endl;
	cout << "jsr " << name << endl;
	cout << "pop r3" << endl;
	cout << "pop r2" << endl;
	cout << "pop r1" << endl;
	cout << "pop r0" << endl;
	
	list<IRnodeInList*>::iterator Iter = popList.begin();
	for(; Iter != popList.end(); Iter++)
	{
		if((*Iter)->isleader())
			reg.reset(*Iter, pfunc, IRnodeList);
		cout << "pop" << endl;
	}
	
	if(pPop->isleader())
		reg.reset(pPop, pfunc, IRnodeList);
	string s1 = reg.allocate(ir.op3, pPop, pfunc); 
	reg.markDirty(s1);
	cout << "pop " << s1 << endl;
}

fun_call_node::~fun_call_node()
{
	if(explist) delete explist;
}

cond_node::cond_node(exp_node *L, string op, exp_node *R) 
{   
	left = L;
	right = R;
	opcode = op;
}
	
string cond_node::genIR()
{
	left->pglobal = pglobal;
	left->pfunc = pfunc;
	left->IRnodeList = IRnodeList;
	
	right->pglobal = pglobal;
	right->pfunc = pfunc;
	right->IRnodeList = IRnodeList;
	
	if(opcode == "=" || opcode == "TRUE")
		ir.opcode = "NE";
	else if(opcode == "!=" || opcode == "FALSE")
		ir.opcode = "EQ";
	else if(opcode == "<")
		ir.opcode = "GE";
	else if(opcode == ">")
		ir.opcode = "LE";
	else if(opcode == "<=")
		ir.opcode = "GT";
	else if(opcode == ">=")
		ir.opcode = "LT";
	ir.op1 = left->genIR();
	ir.op2 = right->genIR();
	ir.op3 = get_label();
	
	std::size_t found = ir.op2.find("$T");
	if(found == string::npos)
		tmp = get_tmp();
		
	return ir.op3;
}

void cond_node::printIR() 
{ 
	left->printIR();
	right->printIR();
	
	pcond = new IRnodeInList;	
	pcond->node = ir;	
	pcond->node.gen_vec.push_back(ir.op1);
	pcond->node.gen_vec.push_back(ir.op2);
	IRnodeList->push_back(pcond);
}

void cond_node::printTiny() 
{
	left->printTiny();
	right->printTiny();
	
	if(pcond->isleader())
		reg.reset(pcond, pfunc, IRnodeList);
	
	string s1(ir.op1); 
	bool b1 = false;
	std::size_t found = s1.find("$T");
	if(found != string::npos)
	{
		s1 = reg.ensure(ir.op1, pcond, pfunc);
		b1 = true;
	}
	else		
		s1 = pfunc->IRtoTiny(s1);
	
	string s2(ir.op2);
	bool b2 = false;
	found = s2.find("$T");
	if(found != string::npos)
	{	
		s2 = reg.ensure(ir.op2, pcond, pfunc);
		b2 = true;
		if(left->type == "INT")
			cout << "cmpi " << s1 << " " << s2 << endl;
		else if(left->type == "FLOAT")	
			cout << "cmpr " << s1 << " " << s2 << endl;
	}
	else
	{	
		s2 = pfunc->IRtoTiny(s2);
		string s3 = reg.allocate(tmp, pcond, pfunc);
		cout << "move " << s2 << " " << s3 << endl;
		if(left->type == "INT")
			cout << "cmpi " << s1 << " " << s3 << endl;
		else if(left->type == "FLOAT")	
			cout << "cmpr " << s1 << " " << s3 << endl;
		reg.markDirty(s3);
	}
	
	if(b1)
	{
		vector<string>::iterator it = pcond->node.live_out_vec.begin();
		vector<string>::iterator it_end = pcond->node.live_out_vec.end();
		for(; it != it_end; ++it) 
		{
			if(s1 == *it) break;
		}
		
		if(it == it_end)
			reg.free(s1, pcond, pfunc);
	}
	
	if(b2)
	{
		vector<string>::iterator it = pcond->node.live_out_vec.begin();
		vector<string>::iterator it_end = pcond->node.live_out_vec.end();
		for(; it != it_end; ++it) 
		{
			if(s2 == *it) break;
		}
		
		if(it == it_end)
			reg.free(s2, pcond, pfunc);
	}
	
	string str_op;
	if(ir.opcode == "NE") str_op = "jne";
	else if(ir.opcode == "EQ") str_op = "jeq";
	else if(ir.opcode == "GE") str_op = "jge";
	else if(ir.opcode == "LE") str_op = "jle";
	else if(ir.opcode == "GT") str_op = "jgt";
	else if(ir.opcode == "LT") str_op = "jlt";
	
	cout << str_op << " " << ir.op3 << endl;	
}

cond_node::~cond_node() 
{ 
	delete left; 
	delete right; 
}

void operator_node::printIR() 
{ 
	left->printIR();
	right->printIR();
	
	pOperator = new IRnodeInList;	
	pOperator->node = ir;
	pOperator->node.gen_vec.push_back(ir.op1);
	pOperator->node.gen_vec.push_back(ir.op2);
	pOperator->node.kill_vec.push_back(ir.op3);
	IRnodeList->push_back(pOperator);
}

void operator_node::printTiny() 
{ 	
	left->printTiny();
	right->printTiny();
	
	if(pOperator->isleader())
		reg.reset(pOperator, pfunc, IRnodeList);
		
	string Rx = reg.ensure(ir.op1, pOperator, pfunc);
	string Ry = reg.ensure(ir.op2, pOperator, pfunc);		
	string Rz = reg.allocate(ir.op3, pOperator, pfunc);
		
	string str_op;
	
	if(ir.opcode == "ADDI") str_op = "addi";
	else if(ir.opcode == "ADDF") str_op = "addr";
	else if(ir.opcode == "SUBI") str_op = "subi";
	else if(ir.opcode == "SUBF") str_op = "subr";
	else if(ir.opcode == "MULTI") str_op = "muli";
	else if(ir.opcode == "MULTF") str_op = "mulr";
	else if(ir.opcode == "DIVI") str_op = "divi";
	else if(ir.opcode == "DIVF") str_op = "divr";
	
	cout << "move " << Rx << " " << Rz << endl;
	cout << str_op << " " << Ry << " " << Rz << endl;	
	
	vector<string>::iterator it = pOperator->node.live_out_vec.begin();
	vector<string>::iterator it_end = pOperator->node.live_out_vec.end();
	for(; it != it_end; ++it) 
	{
		if(ir.op1 == *it) break;
	}
	
	if(it == it_end)
		reg.free(Rx, pOperator, pfunc);
		
	it = pOperator->node.live_out_vec.begin();
	it_end = pOperator->node.live_out_vec.end();
	for(; it != it_end; ++it) 
	{
		if(ir.op2 == *it) break;
	}
	
	if(it == it_end)
		reg.free(Ry, pOperator, pfunc);

	reg.markDirty(Rz);
}

string operator_node::genIR()
{
	left->pglobal = right->pglobal = pglobal;
	left->pfunc = right->pfunc = pfunc;
	left->IRnodeList = right->IRnodeList = IRnodeList;

	if(type == "INT")
	{
		if(opCode == "+")
			ir.opcode = "ADDI";
		else if(opCode == "-")
			ir.opcode = "SUBI";
		else if(opCode == "*")
			ir.opcode = "MULTI";
		else if(opCode == "/")
			ir.opcode = "DIVI";
	}
	else if(type == "FLOAT")
	{
		if(opCode == "+")
			ir.opcode = "ADDF";
		else if(opCode == "-")
			ir.opcode = "SUBF";
		else if(opCode == "*")
			ir.opcode = "MULTF";
		else if(opCode == "/")
			ir.opcode = "DIVF";
	}
	ir.op1 = left->genIR();
	ir.op2 = right->genIR();
	ir.op3 = get_tmp();
	return ir.op3;
}

operator_node::~operator_node() 
{ 
	delete left; 
	delete right; 
}

statement::~statement() {}

void return_stmt::printIR() 
{ 
	ret_exp->printIR();	
	pStore = new IRnodeInList;
	if(ret_exp->type == "INT")
		pStore->node.opcode = "STOREI";
	else if(ret_exp->type == "FLOAT")
		pStore->node.opcode = "STOREF";
	
	pStore->node.op1 = ir.op1;
	pStore->node.op2 = "$R";
	pStore->node.kill_vec.push_back("$R");
	pStore->node.gen_vec.push_back(ir.op1);
	IRnodeList->push_back(pStore);
	
	pRet = new IRnodeInList;	
	pRet->node.opcode = "RET";
	pRet->node.gen_vec.push_back("$R");		
	IRnodeList->push_back(pRet);
}	

void return_stmt::genIR() 
{
	ret_exp->pglobal = pglobal;
	ret_exp->pfunc = pfunc;
	ret_exp->IRnodeList = IRnodeList;
	ir.op1 = ret_exp->genIR();
	
	std::size_t found = ir.op1.find("$T");
	if(found == string::npos)
		tmp = get_tmp();
}

void return_stmt::printTiny() 
{ 
	ret_exp->printTiny();
	
	if(pStore->isleader())
		reg.reset(pStore, pfunc, IRnodeList);
	
	char buf[10];
	if(pfunc->scope != "main")
		sprintf(buf, "$%d", 6+pfunc->syms_arg.size());
	else
		sprintf(buf, "$%d", 6);
	string ret(buf);	
	
	string s1(ir.op1); 
	std::size_t found = s1.find("$T");
	if(found != string::npos)
	{	
		string Rx = reg.ensure(pStore->node.op1, pStore, pfunc);
		
		vector<string>::iterator it = pStore->node.live_out_vec.begin();
		vector<string>::iterator it_end = pStore->node.live_out_vec.end();
		for(; it != it_end; ++it) 
		{
			if(pStore->node.op1 == *it) break;
		}
		
		if(it == it_end)
			reg.free(Rx, pStore, pfunc);
		
		cout << "move " << Rx << " " << ret << endl;
	}
	else
	{	
		string Rx = reg.allocate(tmp, pStore, pfunc);
		s1 = pfunc->IRtoTiny(s1);
		cout << "move " << s1 << " " << Rx << endl;
		cout << "move " << Rx << " " << ret << endl;
		reg.markDirty(Rx);
	}		

	if(pRet->isleader())
		reg.reset(pRet, pfunc, IRnodeList);
		
	cout << "unlnk" << endl;
	cout << "ret" << endl;		
}	

return_stmt::~return_stmt() { delete ret_exp; }

void assignment_stmt::printTiny() 
{
	exp->printTiny();
	
	if(pAssign->isleader())
		reg.reset(pAssign, pfunc, IRnodeList);
		
	string s3 = pfunc->IRtoTiny(ir.op3);
	
	string s1(ir.op1); 
	std::size_t found = s1.find("$T");
	if(found != string::npos)
	{	
		string Rx = reg.ensure(s1, pAssign, pfunc);
		
		vector<string>::iterator it = pAssign->node.live_out_vec.begin();
		vector<string>::iterator it_end = pAssign->node.live_out_vec.end();
		for(; it != it_end; ++it) 
		{
			if(s1 == *it) break;
		}
		
		if(it == it_end)
			reg.free(Rx, pAssign, pfunc);
			
		cout << "move " << Rx << " " << s3 << endl;
	}
	else
	{	
		string Rx = reg.allocate(tmp, pAssign, pfunc);						
		s1 = pfunc->IRtoTiny(s1);
		cout << "move " << s1 << " " << Rx << endl;
		cout << "move " << Rx << " " << s3 << endl;
		reg.markDirty(Rx);
	}		
}

void assignment_stmt::printIR() 
{ 
	exp->printIR();	
	pAssign = new IRnodeInList;
	pAssign->node = ir;
	pAssign->node.kill_vec.push_back(ir.op3);
	pAssign->node.gen_vec.push_back(ir.op1);
	IRnodeList->push_back(pAssign);
}

void assignment_stmt::genIR() 
{
	exp->pglobal = pglobal;
	exp->pfunc = pfunc;
	exp->IRnodeList = IRnodeList;
	
	if(exp->type == "INT")
		ir.opcode = "STOREI";
	else if(exp->type == "FLOAT")
		ir.opcode = "STOREF";
	ir.op1 = exp->genIR();
		
	string str = pglobal->getIRname(id);
	if(str == id)
		str = pfunc->getIRname(id);
	ir.op3 = str;
	
	std::size_t found = ir.op1.find("$T");
	if(found == string::npos)
		tmp = get_tmp();
}

assignment_stmt::~assignment_stmt() { delete exp; }

void read_stmt::printTiny() 
{
	string str;
	if(type == "INT")
		str = "sys readi ";
	else if(type == "FLOAT")
		str = "sys readr ";
	cout << str << pfunc->IRtoTiny(ir.op1) << endl;	
}

void read_stmt::printIR() 
{ 
	pRead = new IRnodeInList;
	pRead->node = ir;
	pRead->node.kill_vec.push_back(ir.op1);
	IRnodeList->push_back(pRead);
}

void read_stmt::genIR() 
{
	if(type == "INT")
		ir.opcode = "READI";
	else if(type == "FLOAT")
		ir.opcode = "READF";
		
	string str = pglobal->getIRname(id);
	if(str == id)
		str = pfunc->getIRname(id);
		
	ir.op1 = str;
}

read_stmt::~read_stmt() {}

void write_stmt::printTiny() 
{
	string str;
	if(type == "INT")
		str = "sys writei ";
	else if(type == "FLOAT")
		str = "sys writer ";
	else if(type == "STRING")
		str = "sys writes ";
	cout << str << pfunc->IRtoTiny(ir.op1) << endl;		
}

void write_stmt::printIR() 
{ 
	pWrite = new IRnodeInList;
	pWrite->node = ir;
	if(type != "STRING")
		pWrite->node.gen_vec.push_back(ir.op1);
	IRnodeList->push_back(pWrite);
}

void write_stmt::genIR() 
{
	if(type == "INT")
		ir.opcode = "WRITEI";
	else if(type == "FLOAT")
		ir.opcode = "WRITEF";
	else if(type == "STRING")
		ir.opcode = "WRITES";
	
	string str = pglobal->getIRname(id);
	if(str == id)
		str = pfunc->getIRname(id);		
	
	ir.op1 = str;
}

write_stmt::~write_stmt() {}

void brk_stmt::printTiny() 
{
	if(pJump->isleader())
		reg.reset(pJump, pfunc, IRnodeList);
	cout << "jmp " << loopoutlabel << endl;	
}

void brk_stmt::printIR() 
{
	pJump = new IRnodeInList;
	pJump->node.opcode = "JUMP";
	pJump->node.op1 = loopoutlabel;
	IRnodeList->push_back(pJump);
}

brk_stmt::~brk_stmt() {}

void con_stmt::printTiny() 
{
	if(pJump->isleader())
		reg.reset(pJump, pfunc, IRnodeList);
	cout << "jmp " << label_before_cond << endl;	
}

void con_stmt::printIR() 
{
	pJump = new IRnodeInList;
	pJump->node.opcode = "JUMP";
	pJump->node.op1 = label_before_cond;
	IRnodeList->push_back(pJump);
}

con_stmt::~con_stmt() {}

void else_stmt::genIR() 
{
	cond->pglobal = pglobal;
	cond->pfunc = pfunc;
	cond->IRnodeList = IRnodeList;
	
	nextlabel = cond->genIR();
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
	{
		(*stmtIter)->label_before_cond = label_before_cond;
		(*stmtIter)->loopoutlabel = loopoutlabel;
		(*stmtIter)->pglobal = pglobal;
		(*stmtIter)->pfunc = pfunc;
		(*stmtIter)->IRnodeList = IRnodeList;
		(*stmtIter)->genIR();
	}
}

void else_stmt::printIR() 
{
	cond->printIR();
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
		(*stmtIter)->printIR();

	pJump = new IRnodeInList;	
	pJump->node.opcode = "JUMP";	
	pJump->node.op1 = outlabel;		
	IRnodeList->push_back(pJump);
	
	pLabel = new IRnodeInList;	
	pLabel->node.opcode = "LABEL";	
	pLabel->node.op1 = nextlabel;		
	IRnodeList->push_back(pLabel);
}

void else_stmt::printTiny() 
{ 
	cond->printTiny();
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
		(*stmtIter)->printTiny();
		
	if(pJump->isleader())
		reg.reset(pJump, pfunc, IRnodeList);
	cout << "jmp " << outlabel << endl;
	
	if(pLabel->isleader())
		reg.reset(pLabel, pfunc, IRnodeList);
	cout << "label " << nextlabel << endl;
}

else_stmt::~else_stmt()
{
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++)
		delete *stmtIter;
	delete stmts;
	delete cond;
}

void if_stmt::printIR() 
{
	cond->printIR();
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
		(*stmtIter)->printIR();

	pJump = new IRnodeInList;	
	pJump->node.opcode = "JUMP";
	pJump->node.op1 = outlabel;		
	IRnodeList->push_back(pJump);

	pLabel1 = new IRnodeInList;	
	pLabel1->node.opcode = "LABEL";
	pLabel1->node.op1 = nextlabel;		
	IRnodeList->push_back(pLabel1);
	
	for(stmtIter = else_stmts->begin(); stmtIter != else_stmts->end(); stmtIter++)
		(*stmtIter)->printIR();
		
	pLabel2 = new IRnodeInList;	
	pLabel2->node.opcode = "LABEL";
	pLabel2->node.op1 = outlabel;		
	IRnodeList->push_back(pLabel2);
}

void if_stmt::printTiny() 
{ 
	cond->printTiny();
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
		(*stmtIter)->printTiny();
	
	if(pJump->isleader())
		reg.reset(pJump, pfunc, IRnodeList);
	cout << "jmp " << outlabel << endl;
	
	if(pLabel1->isleader())
		reg.reset(pLabel1, pfunc, IRnodeList);		
	cout << "label " << nextlabel << endl;
	
	for(stmtIter = else_stmts->begin(); stmtIter != else_stmts->end(); stmtIter++)
		(*stmtIter)->printTiny();
	
	if(pLabel2->isleader())
		reg.reset(pLabel2, pfunc, IRnodeList);	
	cout << "label " << outlabel << endl;
}

void if_stmt::genIR() 
{
	cond->pglobal = pglobal;
	cond->pfunc = pfunc;
	cond->IRnodeList = IRnodeList;
	
	nextlabel = cond->genIR();
	outlabel = get_label();
	list<statement *>::iterator stmtIter;
	for(stmtIter = else_stmts->begin(); stmtIter != else_stmts->end(); stmtIter++) 
	{
		else_stmt* p = dynamic_cast<else_stmt*>(*stmtIter);
		p->outlabel = outlabel;
	}
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
	{
		(*stmtIter)->label_before_cond = label_before_cond;
		(*stmtIter)->loopoutlabel = loopoutlabel;
		(*stmtIter)->pglobal = pglobal;
		(*stmtIter)->pfunc = pfunc;
		(*stmtIter)->IRnodeList = IRnodeList;
		(*stmtIter)->genIR();
	}
	for(stmtIter = else_stmts->begin(); stmtIter != else_stmts->end(); stmtIter++) 
	{
		(*stmtIter)->label_before_cond = label_before_cond;
		(*stmtIter)->loopoutlabel = loopoutlabel;
		(*stmtIter)->pglobal = pglobal;
		(*stmtIter)->pfunc = pfunc;
		(*stmtIter)->IRnodeList = IRnodeList;
		(*stmtIter)->genIR();
	}
}

if_stmt::~if_stmt()
{
	list<statement *>::iterator stmtIter;
	for(stmtIter = else_stmts->begin(); stmtIter != else_stmts->end(); stmtIter++) 
		delete *stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++)
		delete *stmtIter;
	delete else_stmts;
	delete stmts;	
	delete cond;
}

void dowhile_stmt::printIR() 
{
	pLabel1 = new IRnodeInList;	
	pLabel1->node.opcode = "LABEL";
	pLabel1->node.op1 = looplabel;		
	IRnodeList->push_back(pLabel1);
	
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
		(*stmtIter)->printIR();
		
	pLabel2 = new IRnodeInList;	
	pLabel2->node.opcode = "LABEL";
	pLabel2->node.op1 = label_before_cond;		
	IRnodeList->push_back(pLabel2);
	
	cond->printIR();
	
	pJump = new IRnodeInList;	
	pJump->node.opcode = "JUMP";
	pJump->node.op1 = looplabel;		
	IRnodeList->push_back(pJump);
	
	pLabel3 = new IRnodeInList;	
	pLabel3->node.opcode = "LABEL";
	pLabel3->node.op1 = outlabel;		
	IRnodeList->push_back(pLabel3);
}

void dowhile_stmt::printTiny() 
{ 
	if(pLabel1->isleader())
		reg.reset(pLabel1, pfunc, IRnodeList);
	cout << "label " << looplabel << endl;
	
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
		(*stmtIter)->printTiny();
	
	if(pLabel2->isleader())
		reg.reset(pLabel2, pfunc, IRnodeList);
	cout << "label " << label_before_cond << endl;
	
	cond->printTiny();
	
	if(pJump->isleader())
		reg.reset(pJump, pfunc, IRnodeList);
	cout << "jmp " << looplabel << endl;
	
	if(pLabel3->isleader())
		reg.reset(pLabel3, pfunc, IRnodeList);
	cout << "label " << outlabel << endl;
}

void dowhile_stmt::genIR() 
{
	cond->pglobal = pglobal;
	cond->pfunc = pfunc;
	cond->IRnodeList = IRnodeList;
	
	looplabel = get_label();
	outlabel = cond->genIR();
	
	label_before_cond = get_label();
	loopoutlabel = outlabel;
	
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
	{
		(*stmtIter)->label_before_cond = label_before_cond;
		(*stmtIter)->loopoutlabel = loopoutlabel;
		(*stmtIter)->pglobal = pglobal;
		(*stmtIter)->pfunc = pfunc;
		(*stmtIter)->IRnodeList = IRnodeList;
		(*stmtIter)->genIR();	
	}			
}

dowhile_stmt::~dowhile_stmt()
{
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
		delete *stmtIter;
	
	delete stmts;	
	delete cond;
}

void func_dec::printIR() 
{ 
	pLabel = new IRnodeInList;	
	pLabel->node.opcode = "LABEL";
	pLabel->node.op1 = psym->scope;		
	IRnodeList->push_back(pLabel);
	
	pLink = new IRnodeInList;	
	pLink->node.opcode = "LINK";		
	IRnodeList->push_back(pLink);
	
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
		(*stmtIter)->printIR();
	statement* last = stmts->back();
	if(last)
	{
		return_stmt* rsp = dynamic_cast<return_stmt*>(last);
		if(rsp == 0)
		{
			pRet = new IRnodeInList;	
			pRet->node.opcode = "RET";		
			IRnodeList->push_back(pRet);
		}
	}
}
	
void func_dec::printTiny() 
{ 
	if(pLabel->isleader())
		reg.reset(pLabel, psym, IRnodeList);
	cout << "label " << psym->scope << endl;
	
	if(pLink->isleader())
		reg.reset(pLink, psym, IRnodeList);
	int n = psym->syms_local.size();
	cout << "link " << n+tmpCount << endl;
	
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
		(*stmtIter)->printTiny();
		
	statement* last = stmts->back();
	if(last)
	{
		return_stmt* rsp = dynamic_cast<return_stmt*>(last);
		if(rsp == 0)
		{
			if(pRet->isleader())
				reg.reset(pRet, psym, IRnodeList);
			cout << "unlnk" << endl << "ret" << endl;
		}
	}
	IRnodeInList* pb = IRnodeList->back();
	reg.free("r0", pb, psym);
	reg.free("r1", pb, psym);
	reg.free("r2", pb, psym);
	reg.free("r3", pb, psym);
}

void func_dec::genIR() 
{
	cnt_tmp = 0; //each function has its own IR list
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
	{
		(*stmtIter)->pglobal = pglobal;
		(*stmtIter)->pfunc = psym;
		(*stmtIter)->IRnodeList = IRnodeList;
		(*stmtIter)->genIR();
	}
	tmpCount = cnt_tmp;
}

func_dec::~func_dec()
{
	list<statement*>::iterator stmtIter;
	for(stmtIter = stmts->begin(); stmtIter != stmts->end(); stmtIter++) 
		delete *stmtIter;
	delete stmts;

	vector<IRnodeInList*>::iterator it;
	for(it = IRnodeList->begin(); it != IRnodeList->end(); it++) 
		delete *it;
	delete IRnodeList;
}

void pgm::printTiny() 
{ 
	Symtable* tmp;
	psym->printSymbolTiny();
	cout << "push" << endl;
	
	list<func_dec*>::iterator func_decIter;
	for(func_decIter = func_decs->begin(); func_decIter != func_decs->end(); func_decIter++) 
	if((*func_decIter)->psym->scope == "main") 
	{	
		tmp = (*func_decIter)->psym;
		break;
	}	
	
	vector<string>::iterator iter = tmp->syms_arg.begin(); 
	vector<string>::iterator iter_end = tmp->syms_arg.end();
	for(; iter != iter_end; ++iter) 
		cout<< "push " << tmp->IRtoTiny(tmp->getIRname(*iter)) << endl;
		
	cout << "push r0" << endl;
	cout << "push r1" << endl;
	cout << "push r2" << endl;
	cout << "push r3" << endl;
	cout << "jsr main" << endl;
	cout << "pop r3" << endl;
	cout << "pop r2" << endl;
	cout << "pop r1" << endl;
	cout << "pop r0" << endl;
	
	iter = tmp->syms_arg.begin(); 
	iter_end = tmp->syms_arg.end(); 
	for(; iter != iter_end; ++iter) 
		cout << "pop" << endl;
	
	cout << "pop" << endl;		
	cout << "sys halt" << endl;
	
	for(func_decIter = func_decs->begin(); func_decIter != func_decs->end(); func_decIter++) 
		(*func_decIter)->printTiny();
}

void pgm::printIR() 
{ 
	list<func_dec*>::iterator func_decIter;
	for(func_decIter = func_decs->begin(); func_decIter != func_decs->end(); func_decIter++) 
		(*func_decIter)->printIR();
		
	for(func_decIter = func_decs->begin(); func_decIter != func_decs->end(); func_decIter++) 
	{
		vector<IRnodeInList*>::iterator Iter;
		vector<IRnodeInList*>::iterator IterEnd = (*func_decIter)->IRnodeList->end();
		IterEnd--;
		for(Iter = (*func_decIter)->IRnodeList->begin(); Iter != IterEnd; Iter++) 
		{
			IRnodeInList* p1 = *Iter;
			Iter++;
			IRnodeInList* p2 = *Iter;
			Iter--;
			
			if(p1->node.opcode != "RET" && p1->node.opcode != "JUMP")
			{
				p1->sucVec.push_back(p2);
				p2->preVec.push_back(p1);
			}
			
			if(p1->node.opcode == "JUMP")
			{///
				vector<IRnodeInList*>::iterator IterIn;
				vector<IRnodeInList*>::iterator IterEndIn = (*func_decIter)->IRnodeList->end();
				for(IterIn = (*func_decIter)->IRnodeList->begin(); IterIn != IterEndIn; IterIn++) 
				{
					if(p1->node.op1 == (*IterIn)->node.op1 && (*IterIn)->node.opcode == "LABEL")
					{	
						p1->sucVec.push_back(*IterIn);
						(*IterIn)->preVec.push_back(p1);
						break;
					}	
				}
			}///
			
			if(p1->node.opcode == "NE" || p1->node.opcode == "EQ"  || p1->node.opcode == "GE"
				 || p1->node.opcode == "LE"  || p1->node.opcode == "GT"  || p1->node.opcode == "LT")
			{//
				vector<IRnodeInList*>::iterator IterIn;
				vector<IRnodeInList*>::iterator IterEndIn = (*func_decIter)->IRnodeList->end();
				for(IterIn = (*func_decIter)->IRnodeList->begin(); IterIn != IterEndIn; IterIn++) 
				{
					if(p1->node.op3 == (*IterIn)->node.op1 && (*IterIn)->node.opcode == "LABEL")
					{	
						p1->sucVec.push_back(*IterIn);
						(*IterIn)->preVec.push_back(p1);
						break;
					}
				}
			}//
		}
	}
	
	for(func_decIter = func_decs->begin(); func_decIter != func_decs->end(); func_decIter++) 
	{	
		while(1)
		{
			bool unchanged = true;
			vector<IRnodeInList*>::reverse_iterator iter = (*func_decIter)->IRnodeList->rbegin(); 
			for(; iter != (*func_decIter)->IRnodeList->rend(); ++iter)
			{
				if((*iter)->node.opcode == "RET")
				{
					vector<string>::iterator iter1 = psym->syms_local.begin(); 
					vector<string>::iterator iter_end1 = psym->syms_local.end();
					for(; iter1 != iter_end1; ++iter1)
					{	
						Information* pi = psym->findInfo(*iter1);
						if(pi && pi->type != "STRING")
						{
							vector<string>::iterator it1 = (*iter)->node.live_out_vec.begin();
							vector<string>::iterator it1_end = (*iter)->node.live_out_vec.end();
							for(; it1 != it1_end; ++it1) 
							{
								if(*it1 == psym->getIRname(pi->name))
									break;
							}
							if(it1 == it1_end)
							{	
								unchanged = false;
								(*iter)->node.live_out_vec.push_back(psym->getIRname(pi->name));
								(*iter)->node.live_in_vec.push_back(psym->getIRname(pi->name));
							}
						}
					}
					
					vector<string>::iterator iteruse = (*iter)->node.gen_vec.begin();
					vector<string>::iterator iteruse_end = (*iter)->node.gen_vec.end();
					for(; iteruse != iteruse_end; ++iteruse)
					{
						vector<string>::iterator it1 = (*iter)->node.live_in_vec.begin();
						vector<string>::iterator it1_end = (*iter)->node.live_in_vec.end();
						for(; it1 != it1_end; ++it1) 
						{
							if(*it1 == *iteruse)
								break;
						}
						
						if(it1 == it1_end)
							(*iter)->node.live_in_vec.push_back(*iteruse);
					}	
				}
				else
				{
					vector<IRnodeInList*>::iterator iter2 = (*iter)->sucVec.begin();
					vector<IRnodeInList*>::iterator iter2_end = (*iter)->sucVec.end();
					for(; iter2 != iter2_end; ++iter2) 
					{//	
						vector<string>::iterator it = (*iter2)->node.live_in_vec.begin();
						vector<string>::iterator it_end = (*iter2)->node.live_in_vec.end();
						for(; it != it_end; ++it) 
						{
							vector<string>::iterator it1 = (*iter)->node.live_out_vec.begin();
							vector<string>::iterator it1_end = (*iter)->node.live_out_vec.end();
							for(; it1 != it1_end; ++it1) 
							{
								if(*it1 == *it)
									break;
							}
							
							if(it1 == it1_end)
							{	
								unchanged = false;
								(*iter)->node.live_out_vec.push_back(*it);
								(*iter)->node.live_in_vec.push_back(*it);
							}
						}
					}//
					
					vector<string>::iterator it = (*iter)->node.kill_vec.begin();
					vector<string>::iterator it_end = (*iter)->node.kill_vec.end();
					for(; it != it_end; ++it) 
					{
						vector<string>::iterator it1 = (*iter)->node.live_in_vec.begin();
						vector<string>::iterator it1_end = (*iter)->node.live_in_vec.end();
						for(; it1 != it1_end;) 
						{
							if(*it1 == *it)
							{
								it1 = (*iter)->node.live_in_vec.erase(it1);
								break;
							}
							else
								it1++;
						}
					}
					
					it = (*iter)->node.gen_vec.begin();
					it_end = (*iter)->node.gen_vec.end();
					for(; it != it_end; ++it) 
					{
						vector<string>::iterator it1 = (*iter)->node.live_in_vec.begin();
						vector<string>::iterator it1_end = (*iter)->node.live_in_vec.end();
						for(; it1 != it1_end; ++it1) 
						{
							if(*it1 == *it)
								break;
						}
						
						if(it1 == it1_end)
							(*iter)->node.live_in_vec.push_back(*it);
					}
					
				}
			}
			
			if(unchanged) break;
		}
	}
	
	for(func_decIter = func_decs->begin(); func_decIter != func_decs->end(); func_decIter++) 
	{
		vector<IRnodeInList*>::iterator Iter;
		vector<IRnodeInList*>::iterator IterEnd = (*func_decIter)->IRnodeList->end();
		for(Iter = (*func_decIter)->IRnodeList->begin(); Iter != IterEnd; Iter++) 
		{				
			cout << ";";
			((*Iter)->node).output();
			
			vector<IRnodeInList*>::iterator Iter1;
			cout << "		{PRED nodes:";
			for(Iter1 = (*Iter)->preVec.begin(); Iter1 != (*Iter)->preVec.end(); Iter1++) 
			{
				cout << "	";
				((*Iter1)->node).output();
			}
			cout << "}		{SUCC nodes:";
			for(Iter1 = (*Iter)->sucVec.begin(); Iter1 != (*Iter)->sucVec.end(); Iter1++) 
			{
				cout << "	";
				((*Iter1)->node).output();
			}
			cout << "}" << endl;
		}
	}
}

void pgm::genIR() 
{
	list<func_dec*>::iterator func_decIter;
	for(func_decIter = func_decs->begin(); func_decIter != func_decs->end(); func_decIter++) 
	{
		(*func_decIter)->pglobal = psym;
		(*func_decIter)->genIR();
	}
}

pgm::~pgm()
{
	list<func_dec*>::iterator func_decIter;
	for(func_decIter = func_decs->begin(); func_decIter != func_decs->end(); func_decIter++) 
		delete *func_decIter;
	delete func_decs;
} 
