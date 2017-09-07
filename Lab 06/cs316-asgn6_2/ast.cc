#include<iostream>
#include<fstream>
#include<typeinfo>

using namespace std;

#include"common-classes.hh"
#include"error-display.hh"
#include"user-options.hh"
#include"symbol-table.hh"
#include"ast.hh"
#include"procedure.hh"
#include"program.hh"

int Ast::labelCounter = 0;

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast()
{
	stringstream msg;
	msg << "No check_ast() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Data_Type Ast::get_data_type()
{
	stringstream msg;
	msg << "No get_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Symbol_Table_Entry & Ast::get_symbol_entry()
{
	stringstream msg;
	msg << "No get_symbol_entry() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

bool Ast::is_value_zero()
{
	stringstream msg;
	msg << "No is_value_zero() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::set_data_type(Data_Type dt)
{
	stringstream msg;
	msg << "No set_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
	lineno = line;
	node_data_type = lhs -> get_data_type();
	ast_num_child = binary_arity;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

bool Assignment_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	// use typeid(), get_data_type()
	//ADD CODE HERE
	if(lhs->get_data_type() == rhs->get_data_type()){
		return 1;
	}

	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, 
		"Assignment statement data type not compatible", lineno);
}

void Assignment_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Asgn:"; 
	file_buffer << "\n" << AST_NODE_SPACE << "LHS (";
	lhs -> print(file_buffer);
	file_buffer << ")";
	file_buffer << "\n" << AST_NODE_SPACE << "RHS (";
	rhs -> print(file_buffer);
	file_buffer << ")";
}

////////////////////////////////////////////////////////////////

//NEW CODE
Relational_Expr_Ast::Relational_Expr_Ast(Ast * lhs, Relational_Op rop, Ast * rhs, int line)
{
	lhs_condition = lhs;
	rhs_condition = rhs;
	rel_op = rop;
	lineno = line;

	node_data_type = int_data_type;
	ast_num_child = binary_arity;
}

Relational_Expr_Ast::~Relational_Expr_Ast()
{
	delete lhs_condition;
	delete rhs_condition;
}

void Relational_Expr_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

Data_Type Relational_Expr_Ast::get_data_type()
{
	return node_data_type;
}

bool Relational_Expr_Ast::check_ast()
{

	CHECK_INVARIANT((rhs_condition != NULL), "Rhs of Relational_Expr_Ast cannot be null");
	CHECK_INVARIANT((lhs_condition != NULL), "Lhs of Relational_Expr_Ast cannot be null");


	if( lhs_condition -> get_data_type() == rhs_condition -> get_data_type() )
	{
		return 1;
	}

	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Relational statement data type not compatible", lineno);
}

void Relational_Expr_Ast::print(ostream & file_buffer)
{
	if(rel_op == 0)
	{
		file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: LE";
	}
	else if(rel_op == 1)
	{
		file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: LT";
	}
	else if(rel_op == 2)
	{
		file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: GT";
	}
	else if(rel_op == 3)
	{
		file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: GE";
	}
	else if(rel_op == 4)
	{
		file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: EQ";
	}
	else if(rel_op == 5)
	{
		file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: NE";
	} // Need to check this
	file_buffer<<"\n"<<AST_SUB_NODE_SPACE<<"LHS (";
	lhs_condition->print(file_buffer);
	file_buffer<<")";
	file_buffer<<"\n"<<AST_SUB_NODE_SPACE<<"RHS (";
	rhs_condition->print(file_buffer);
	file_buffer<<")";
}

////////////////////////////////////////////////////////////////

Boolean_Expr_Ast::Boolean_Expr_Ast(Ast * lhs, Boolean_Op bop, Ast * rhs, int line)
{
	lhs_op = lhs;
	rhs_op = rhs;
	bool_op = bop;
	lineno = line;
	node_data_type = int_data_type;

	if(bool_op == boolean_not)
	{
		ast_num_child = unary_arity;
	}
	else
	{
		ast_num_child = binary_arity;
	}
}

Boolean_Expr_Ast::~Boolean_Expr_Ast()
{
	delete lhs_op;
	delete rhs_op;
}


Data_Type Boolean_Expr_Ast::get_data_type()
{
	return node_data_type;
}


void Boolean_Expr_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

bool Boolean_Expr_Ast::check_ast()
{
	CHECK_INVARIANT((rhs_op != NULL), "Rhs of Boolean_Expr_Ast cannot be null");

	if( ( ast_num_child == unary_arity ) && ( rhs_op -> get_data_type() == int_data_type ) )
	{
		return 1;
	}
	if( ( lhs_op -> get_data_type() == rhs_op -> get_data_type() ) && ( lhs_op -> get_data_type() == int_data_type))
	{
		
		return 1;
	}

	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Boolean statement data type not compatible", lineno);
}

void Boolean_Expr_Ast::print(ostream & file_buffer)
{
	if(bool_op == 0)
	{
		file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: NOT";
	}
	else if(bool_op == 1)
	{
		file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: OR";
	}
	else if(bool_op == 2)
	{
		file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: AND";
	} // Need to check this
	if(bool_op != 0)
	{
		file_buffer<<"\n"<<AST_SUB_NODE_SPACE<<"LHS (";
		lhs_op->print(file_buffer);
		file_buffer<<")";
	}
	file_buffer<<"\n"<<AST_SUB_NODE_SPACE<<"RHS (";
	rhs_op->print(file_buffer);
	file_buffer<<")";
}

////////////////////////////////////////////////////////////////

Selection_Statement_Ast::Selection_Statement_Ast(Ast * s_cond,Ast* s_then_part, Ast* s_else_part, int line)
{
	cond = s_cond;
	then_part = s_then_part;
	else_part = s_else_part;
	lineno = line;

	node_data_type = void_data_type;
	ast_num_child = ternary_arity; // Need to check this
}

Selection_Statement_Ast::~Selection_Statement_Ast()
{
	delete cond;
	delete then_part;
	if(else_part != NULL)
		delete else_part;
}

void Selection_Statement_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

Data_Type Selection_Statement_Ast::get_data_type()
{
	return node_data_type;
}

bool Selection_Statement_Ast::check_ast()
{
	return 1;

	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Selection statement then, else and condition data type mismatch", lineno);
}

void Selection_Statement_Ast::print(ostream & file_buffer)
{
	if(else_part == NULL)
	{
		file_buffer<<endl;
		file_buffer<<AST_SPACE<<"IF : "<<endl;
		file_buffer<<AST_SPACE<<"CONDITION (";
		cond->print(file_buffer);
		file_buffer<<")"<<endl;
		file_buffer<<AST_SPACE<<"THEN (";
		then_part->print(file_buffer);
		file_buffer<<")"<<endl;
		file_buffer<<AST_SPACE<<"ELSE (";
		file_buffer<<endl;
		file_buffer<<SA_SPACE<<"Sequence Ast:";
		file_buffer<<endl;
		file_buffer<<")";
	}
	else
	{
		file_buffer<<endl;
		file_buffer<<AST_SPACE<<"IF : "<<endl;
		file_buffer<<AST_SPACE<<"CONDITION (";
		cond->print(file_buffer);
		file_buffer<<")"<<endl;
		file_buffer<<AST_SPACE<<"THEN (";
		then_part->print(file_buffer);
		file_buffer<<")"<<endl;
		file_buffer<<AST_SPACE<<"ELSE (";
		else_part->print(file_buffer);
		file_buffer<<")";
	}
}

////////////////////////////////////////////////////////////////

Iteration_Statement_Ast::Iteration_Statement_Ast(Ast * temp_cond, Ast * temp_body, int line, bool do_form)
{
	cond = temp_cond;
	body = temp_body;
	lineno = line;
	is_do_form = do_form;

	ast_num_child = binary_arity;
	node_data_type = void_data_type;
}

Iteration_Statement_Ast::~Iteration_Statement_Ast()
{
	delete cond;
	delete body;
}

void Iteration_Statement_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

Data_Type Iteration_Statement_Ast::get_data_type()
{
	return node_data_type;
}

bool Iteration_Statement_Ast::check_ast()
{
	return 1;
}

void Iteration_Statement_Ast::print(ostream & file_buffer)
{
	if(!is_do_form)
	{
		file_buffer<<endl;
		file_buffer<<AST_SPACE<<"WHILE : "<<endl;
		file_buffer<<AST_SPACE<<"CONDITION (";
		cond->print(file_buffer);
		file_buffer<<")"<<endl;
		file_buffer<<AST_SPACE<<"BODY (";
		body->print(file_buffer);
		file_buffer<<")";
	}
	else //do while
	{
		file_buffer<<endl;
		file_buffer<<AST_SPACE<<"DO (";
		body->print(file_buffer);
		file_buffer<<")"<<endl;
		file_buffer<<AST_SPACE<<"WHILE CONDITION (";
		cond->print(file_buffer);
		file_buffer<<")";
	}
}


/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line)
{

	variable_symbol_entry = &var_entry;
	CHECK_INVARIANT((variable_symbol_entry->get_variable_name() == name),
		"Variable's symbol entry is not matching its name");
	
	lineno = line;
	node_data_type = variable_symbol_entry->get_data_type();
	ast_num_child = zero_arity;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry->get_data_type();
}

Symbol_Table_Entry & Name_Ast::get_symbol_entry()
{
	return *variable_symbol_entry;
}

void Name_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

void Name_Ast::print(ostream & file_buffer)
{
	file_buffer<<"Name : "<<variable_symbol_entry->get_variable_name();
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type, int line)
{
	// use Ast_arity from ast.hh
	constant = number;
	node_data_type = constant_data_type;
	lineno = line;
	ast_num_child = zero_arity;
}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	return node_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

template <class DATA_TYPE>
bool Number_Ast<DATA_TYPE>::is_value_zero()
{
	if(constant == 0){ return 1;}
	else {return 0;}
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print(ostream & file_buffer)
{
	file_buffer<<"Num : "<<constant;
}

///////////////////////////////////////////////////////////////////////////////

Data_Type Arithmetic_Expr_Ast::get_data_type()
{
	return node_data_type;
}

Arithmetic_Expr_Ast::~Arithmetic_Expr_Ast(){
	delete lhs;
	delete rhs;
}

void Arithmetic_Expr_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

bool Arithmetic_Expr_Ast::check_ast()
{
	
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	if(lhs->get_data_type() == rhs->get_data_type())
	{
		return 1;
	}

	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Arithmetic statement data type not compatible", lineno);
	return true;
}

/////////////////////////////////////////////////////////////////////

Plus_Ast::Plus_Ast(Ast * l, Ast * r, int line)
{
	// set arity and data type
	lhs = l;
	rhs = r;
	lineno = line;
	ast_num_child = binary_arity;
	if(l -> get_data_type() == r -> get_data_type())
		node_data_type = l->get_data_type();
	else
		node_data_type = void_data_type;
}

Plus_Ast::~Plus_Ast(){
	delete lhs;
	delete rhs;
}

void Plus_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_NODE_SPACE << "Arith: PLUS" << endl;
	file_buffer << AST_SUB_NODE_SPACE << "LHS (";
    lhs->print (file_buffer);
    file_buffer << ")\n";
    file_buffer <<AST_SUB_NODE_SPACE << "RHS (";
    rhs->print(file_buffer);
    file_buffer << ")";
}

/////////////////////////////////////////////////////////////////

Minus_Ast::Minus_Ast(Ast * l, Ast * r, int line)
{
	lhs = l;
	rhs = r;
	lineno = line;
	ast_num_child = binary_arity;
	if(l -> get_data_type() == r -> get_data_type())
		node_data_type = l->get_data_type();
	else
		node_data_type = void_data_type;
}

Minus_Ast::~Minus_Ast(){
	delete lhs;
	delete rhs;
}

void Minus_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n"<<AST_NODE_SPACE << "Arith: MINUS"<<endl;
	file_buffer << AST_SUB_NODE_SPACE << "LHS (";
    lhs->print(file_buffer);
    file_buffer << ")\n";
    file_buffer << AST_SUB_NODE_SPACE <<"RHS (";
    rhs->print(file_buffer);
    file_buffer << ")";
}

//////////////////////////////////////////////////////////////////

Mult_Ast::Mult_Ast(Ast * l, Ast * r, int line)
{
	lhs = l;
	rhs = r;
	lineno = line;
	ast_num_child = binary_arity;
	if(l -> get_data_type() == r -> get_data_type())
		node_data_type = l->get_data_type();
	else
		node_data_type = void_data_type;
}

Mult_Ast::~Mult_Ast(){
	delete lhs;
	delete rhs;
}

void Mult_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_NODE_SPACE << "Arith: MULT" << endl;
	file_buffer << AST_SUB_NODE_SPACE << "LHS (";
    lhs->print(file_buffer);
    file_buffer << ")\n";
    file_buffer << AST_SUB_NODE_SPACE << "RHS (";
    rhs->print(file_buffer);
    file_buffer << ")";
}

////////////////////////////////////////////////////////////////////

Divide_Ast::Divide_Ast(Ast * l, Ast * r, int line)
{
	lhs = l;
	rhs = r;
	lineno = line;
	ast_num_child = binary_arity;

	if(l -> get_data_type() == r -> get_data_type())
		node_data_type = l->get_data_type();
	else
		node_data_type = void_data_type;
}

Divide_Ast::~Divide_Ast(){
	delete lhs;
	delete rhs;
}

void Divide_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n"<<AST_NODE_SPACE << "Arith: DIV" << endl;
	file_buffer << AST_SUB_NODE_SPACE << "LHS (";
    lhs->print(file_buffer);
    file_buffer << ")\n";
    file_buffer << AST_SUB_NODE_SPACE << "RHS (";
    rhs->print(file_buffer);
    file_buffer << ")";
}

//////////////////////////////////////////////////////////////////////

Conditional_Operator_Ast::Conditional_Operator_Ast(Ast* c_cond, Ast* l, Ast* r, int line)
{
	lhs = l;
	rhs = r;
	cond = c_cond;
	lineno = line;

	node_data_type = l->get_data_type();
	ast_num_child = ternary_arity;

}

Conditional_Operator_Ast::~Conditional_Operator_Ast()
{
	delete lhs;
	delete rhs;
	delete cond;
}
void Conditional_Operator_Ast::print(ostream & file_buffer)
{
	file_buffer<<"\n"<<AST_NODE_SPACE<<"Arith: Conditional"<<endl;
	file_buffer<<AST_SUB_NODE_SPACE<<"COND (";
	cond->print(file_buffer);
	file_buffer<<")\n";
	file_buffer<<AST_SUB_NODE_SPACE<<"LHS (";
    lhs->print(file_buffer);
    file_buffer<<")\n";
    file_buffer<<AST_SUB_NODE_SPACE<<"RHS (";
    rhs->print(file_buffer);
    file_buffer<<")";
}

//////////////////////////////////////////////////////////////////////

UMinus_Ast::UMinus_Ast(Ast * l, Ast * r, int line)
{
	lhs = l;
	rhs = r;
	lineno = line;
	node_data_type = l->get_data_type();
	ast_num_child = unary_arity;
}

UMinus_Ast::~UMinus_Ast(){
	delete lhs;
	delete rhs;
}

void UMinus_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_NODE_SPACE << "Arith: UMINUS" << endl;
	file_buffer << AST_SUB_NODE_SPACE << "LHS (";
    lhs->print(file_buffer);
    file_buffer << ")";
}

//////////////////////////////////////////////////////////////////////

Sequence_Ast::Sequence_Ast(int line)
{
	lineno = line;
	node_data_type = void_data_type;
	ast_num_child = unary_arity;
	statement_list.clear();
}

Sequence_Ast::~Sequence_Ast()
{
	if(!statement_list.empty())
	{
		list<Ast *>::iterator it;
		for (it = statement_list.begin(); it != statement_list.end(); it++)
		{
			delete (*it);
		}
	}
	statement_list.clear();
}

void Sequence_Ast::ast_push_back(Ast * ast)
{
	statement_list.push_back(ast);
}

void Sequence_Ast::print(ostream & file_buffer)
{
	file_buffer<<"\n"<<SA_SPACE<<"Sequence Ast:"<<endl;
	if(!statement_list.empty())
	{
		for(list<Ast *>::iterator it=statement_list.begin();it != statement_list.end(); it++)
		{
			(*it)->print(file_buffer);
		}
	}
}
/////////////////////////////////////////////////////////////////////

Print_String_Ast::Print_String_Ast(string s, int line)
{
	str = s;
	lineno = line;
	node_data_type = void_data_type;
	ast_num_child = unary_arity;
	val = 4;
}

Print_String_Ast::~Print_String_Ast()
{}

void Print_String_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "PRINT: \n";
	file_buffer << AST_SUB_NODE_SPACE << "expression (";
	file_buffer << "String : " << str << ")";
}

/////////////////////////////////////////////////////////////////////

Print_Variable_Ast::Print_Variable_Ast(Ast* temp, int v)
{
	var = temp;
	lineno = line;
	node_data_type = void_data_type;
	ast_num_child = unary_arity;
	if(var_temp -> get_data_type() == double_data_type)
	{
		val = 3;
	}
	else if(var_temp -> get_data_type() == int_data_type)
	{
		val = 1;
	}
	else
	{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Data Type not clear")
	}
}

Print_Variable_Ast::~Print_Variable_Ast()
{}

void Print_Variable_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "PRINT: \n";
	file_buffer << AST_SUB_NODE_SPACE << "expression (";
	var -> print(file_buffer) << ")";
}

//////////////////////////////////////////////////////////////////////

Function_Call_Ast::Function_Call_Ast()
{}

Function_Call_Ast::Function_Call_Ast(Procedure *temp)
{
	proc = temp;
	arg_list =  *(new list<Ast*>());  
	node_data_type = proc -> get_return_type();	// from procedure.hh
}

Function_Call_Ast::Function_Call_Ast(Procedure *temp, list<Ast*> temp_list)
{
	proc = temp;
	args = temp_list;
	node_data_type = proc -> get_return_type();
}

Function_Call_Ast::~Function_Call_Ast()
{}

Data_Type Function_Call_Ast::get_data_type()
{
	return node_data_type;
}

bool Function_Call_Ast::check_ast()
{
	node_data_type = proc -> get_return_type();
	if(proc -> check_arg_types(args))
	{
		return 1;
	}
	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Function call arguments are not type compatible", lineno);
}

void Function_Call_Ast::print(ostream & file_buffer)
{
	file_buffer<< "\n" << AST_SPACE << "FN CALL: " << proc -> get_proc_name() << "(";
	// auto iter = args.end(); 	// auto figures out what type iterator 'iter' is.
	for(auto iter = args.end()-1; iter != args.begin(); --iter) 
	{
		if((*iter) == NULL)
		{
			break;
		}
		file_buffer << "\n";
		(*iter) -> print(file_buffer);
	}
	// while(iter != args.begin())
	// {
	// 	--iter;
	// 	if((*iter) == NULL)
	// 	{
	// 		break;
	// 	}
	// 	file_buffer << "\n";
	// 	(*iter) -> print(file_buffer);
	// }

	file_buffer << ")";
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast(int line)
{
	lineno = line;
	retrn = NULL;
	node_data_type = void_data_type;
	ast_num_child = zero_arity;
}

Return_Ast::Return_Ast(Ast *temp, int line){
	lineno = line;
	retrn = temp;
	node_data_type = void_data_type;
	ast_num_child = zero_arity;
}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print(ostream & file_buffer)
{
	if(retrn != NULL)
	{
		file_buffer << "\n" << AST_SPACE << "RETURN ";
		retrn -> print(file_buffer);
		file_buffer << "\n";
	}
	else
	{
		file_buffer << "\n" << AST_SPACE << "RETURN <NOTHING>\n";
	}
}

///////////////////////////////////////////////////////////////////////////////


template class Number_Ast<double>;
template class Number_Ast<int>;