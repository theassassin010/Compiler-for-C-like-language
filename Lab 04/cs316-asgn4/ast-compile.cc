#include <iostream>
#include <fstream>
#include <typeinfo>

using namespace std;

#include "common-classes.hh"
#include "error-display.hh"
#include "user-options.hh"
#include "icode.hh"
#include "reg-alloc.hh"
#include "symbol-table.hh"
#include "ast.hh"
#include "procedure.hh"
#include "program.hh"

Code_For_Ast & Ast::create_store_stmt(Register_Descriptor * store_register)
{
	stringstream msg;
	msg << "No create_store_stmt() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_assembly()
{
	stringstream msg;
	msg << "No print_assembly() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_icode()
{
	stringstream msg;
	msg << "No print_icode() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Code_For_Ast & Assignment_Ast::compile()
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Assignment_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Assignment_Ast");

	Code_For_Ast & load_stmt = rhs->compile();

	Register_Descriptor * load_register = load_stmt.get_reg();
	CHECK_INVARIANT(load_register, "Load register cannot be null in Assignment_Ast");
	load_register->set_use_for_expr_result();

	Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);

	CHECK_INVARIANT((load_register != NULL), "Load register cannot be null in Assignment_Ast");
	load_register->reset_use_for_expr_result();

	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, load_register);

	return *assign_stmt;
}


/////////////////////////////////////////////////////////////////

Code_For_Ast & Name_Ast::compile()
{
	CHECK_INVARIANT(variable_symbol_entry, "Variable symbol entry cannot be null in Name_Ast");

	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

	Register_Descriptor * result_register = NULL;
	
    // Initialise result register appropriately
    // Take care that the result register is of proper type
    
    // TODO
	if(variable_symbol_entry->get_data_type() == int_data_type)
		result_register = machine_desc_object.get_new_register<gp_data>();
	else if(variable_symbol_entry->get_data_type() == double_data_type)
		result_register = machine_desc_object.get_new_register<float_reg>();
	else
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid type of variable");

    CHECK_INVARIANT((result_register != NULL), "Result register cannot be null in Name_Ast");

	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * load_stmt = NULL;
	
    // Initialise load_stmt appropriately
    // TODO

    if(get_data_type() == int_data_type)
    	load_stmt = new Move_IC_Stmt(load, opd, register_opd);
    else if(get_data_type() == double_data_type)
    	load_stmt = new Move_IC_Stmt(load_d, opd, register_opd);
    else
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid type of variable");

	CHECK_INVARIANT((load_stmt != NULL), "Load statement cannot be null in Name_Ast");

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

Code_For_Ast & Name_Ast::create_store_stmt(Register_Descriptor * store_register)
{
	CHECK_INVARIANT((store_register != NULL), "Store register cannot be null in Name_Ast");
	CHECK_INVARIANT(variable_symbol_entry, "Variable symbol entry cannot be null in Name_Ast");

	Ics_Opd * register_opd = new Register_Addr_Opd(store_register);
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

	Icode_Stmt * store_stmt = NULL;
	
    // Initialise store_stmt appropriately
    // TODO
	if(variable_symbol_entry->get_data_type() == int_data_type)
		store_stmt = new Move_IC_Stmt(store, register_opd, opd);
	else if(variable_symbol_entry->get_data_type() == double_data_type)
		store_stmt = new Move_IC_Stmt(store_d, register_opd, opd);
	else
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value in store variable");


	CHECK_INVARIANT((store_stmt != NULL), "Store statement cannot be null in Name_Ast");

	if (command_options.is_do_lra_selected() == false)
		variable_symbol_entry->free_register(store_register);

	else
	{
		variable_symbol_entry->update_register(store_register);
		store_register->reset_use_for_expr_result();
	}

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(store_stmt);

	Code_For_Ast & name_code = *new Code_For_Ast(ic_list, store_register);

	return name_code;
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile(){

	Register_Descriptor * result_register;

	if(node_data_type == int_data_type)	{
		result_register = machine_desc_object.get_new_register<gp_data>();
		result_register->set_use_for_expr_result();
	}
	else if(node_data_type == double_data_type){
		result_register = machine_desc_object.get_new_register<float_reg>();
		result_register->set_use_for_expr_result();
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value type of Number Ast");
	}

	CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
	
	Ics_Opd * load_register = new Register_Addr_Opd(result_register);
	Ics_Opd * opd;
	Icode_Stmt * load_stmt;

	if(node_data_type == int_data_type){
		opd = new Const_Opd<int>(constant);
		load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);
	}
	else if(node_data_type == double_data_type){
		opd = new Const_Opd<double>(constant);
		load_stmt = new Move_IC_Stmt(imm_load_d, opd, load_register);
	}

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(load_stmt);

	Code_For_Ast & return_stmt = *new Code_For_Ast(ic_list, result_register);
	return return_stmt;
}

///////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Relational_Expr_Ast::compile(){
	CHECK_INVARIANT((lhs_condition != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs_condition != NULL), "Rhs cannot be null");
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	Code_For_Ast & lhs_stmt = lhs_condition->compile();

	if(lhs_stmt.get_icode_list().empty() == false){
		ic_list = lhs_stmt.get_icode_list();
	}
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs_condition->compile();

	if(rhs_stmt.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
	}

	Register_Descriptor * rhs_register = rhs_stmt.get_reg();
	rhs_register->set_use_for_expr_result();

	Register_Descriptor * result_register;
	result_register = machine_desc_object.get_new_register<gp_data>();
	result_register->set_use_for_expr_result();

	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_register);
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_register);
	Ics_Opd * res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * relational_stmt;

	switch(rel_op){
		case(0):
			relational_stmt = new Compute_IC_Stmt(sle, res_opd, lhs_opd, rhs_opd);
			break;

		case(1):
			relational_stmt = new Compute_IC_Stmt(slt, res_opd, lhs_opd, rhs_opd);
			break;

		case(2):
			relational_stmt = new Compute_IC_Stmt(sgt, res_opd, lhs_opd, rhs_opd);
			break;

		case(3):
			relational_stmt = new Compute_IC_Stmt(sge, res_opd, lhs_opd, rhs_opd);
			break;

		case(4):
			relational_stmt = new Compute_IC_Stmt(seq, res_opd, lhs_opd, rhs_opd);
			break;

		case(5):
			relational_stmt = new Compute_IC_Stmt(sne, res_opd, lhs_opd, rhs_opd);
			break;

		default: CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,"Relational Ast shouldn't reach here");
			break;
	}

	ic_list.push_back(relational_stmt);
	lhs_register->reset_use_for_expr_result();
	rhs_register->reset_use_for_expr_result();

	Code_For_Ast * return_stmt;

	if (ic_list.empty() == false){
		return_stmt = new Code_For_Ast(ic_list, result_register);
	}
	
	return *return_stmt;
}

//////////////////////////////////////////////////////////////////////

Code_For_Ast & Boolean_Expr_Ast::compile(){
	
	CHECK_INVARIANT((rhs_op != NULL), "Rhs cannot be null");
	
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	Ics_Opd * lhs_opd;
	Register_Descriptor * lhs_register;
	if(bool_op != 0){
		CHECK_INVARIANT((lhs_op != NULL), "Lhs cannot be null");
		Code_For_Ast & lhs_stmt = lhs_op->compile();

		if(lhs_stmt.get_icode_list().empty() == false){
			ic_list = lhs_stmt.get_icode_list();
		}
		lhs_register = lhs_stmt.get_reg();
		lhs_register->set_use_for_expr_result();
		lhs_opd = new Register_Addr_Opd(lhs_register);
	}
	else{
		lhs_register = machine_desc_object.get_new_register<gp_data>();
		lhs_register->set_use_for_expr_result();
		lhs_opd = new Register_Addr_Opd(lhs_register);

		Ics_Opd * one_opd;
		one_opd = new Const_Opd<int>(1);
		Icode_Stmt * lhs_stmt = new Move_IC_Stmt(imm_load, one_opd, lhs_opd);
		ic_list.push_back(lhs_stmt);
	}
	// printf("Hi\n");
	Code_For_Ast & rhs_stmt = rhs_op->compile();

	if(rhs_stmt.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
	}

	Register_Descriptor * rhs_register = rhs_stmt.get_reg();
	rhs_register->set_use_for_expr_result();
	Register_Descriptor * result_register;
	result_register = machine_desc_object.get_new_register<gp_data>();
	result_register->set_use_for_expr_result();

	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_register);
	Ics_Opd * res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * boolean_stmt;

	switch(bool_op){
		case(0):
			boolean_stmt = new Compute_IC_Stmt(not_t, res_opd, rhs_opd, lhs_opd);
			break;

		case(1):
			boolean_stmt = new Compute_IC_Stmt(or_t, res_opd, lhs_opd, rhs_opd);
			break;

		case(2):
			boolean_stmt = new Compute_IC_Stmt(and_t, res_opd, lhs_opd, rhs_opd);
			break;

		default: CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,"Boolean Ast shouldn't reach here");
			break;
	}

	ic_list.push_back(boolean_stmt);

	lhs_register->reset_use_for_expr_result();
	rhs_register->reset_use_for_expr_result();

	Code_For_Ast * return_stmt;

	if (ic_list.empty() == false){
		return_stmt = new Code_For_Ast(ic_list, result_register);
	}
	
	return *return_stmt;	
}
///////////////////////////////////////////////////////////////////////

Code_For_Ast & Selection_Statement_Ast::compile(){
	Code_For_Ast & cond_stmt = cond->compile();
	Register_Descriptor * cond_register = cond_stmt.get_reg();
	cond_register->set_use_for_expr_result();

	Ics_Opd * cond_opd = new Register_Addr_Opd(cond_register);
	Ics_Opd * zero_opd  = new Register_Addr_Opd(machine_desc_object.spim_register_table[zero]);

	Code_For_Ast & then_part_code = then_part->compile();
	Code_For_Ast & else_part_code = else_part->compile();
	
	int condition_beq_stmt_label_number = labelCounter;
	string off1 = get_new_label();
	int goto_stmt_label_number = labelCounter;
	string off2 = get_new_label();
	
	Icode_Stmt * condition_beq_stmt = new Control_Flow_IC_Stmt(beq, cond_opd, zero_opd, off1);
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(j, off2);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	if(cond_stmt.get_icode_list().empty() == false){
		ic_list = cond_stmt.get_icode_list();
	}
	ic_list.push_back(condition_beq_stmt);

	if(then_part_code.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), then_part_code.get_icode_list());
	}

	ic_list.push_back(goto_stmt);
	Icode_Stmt * label_1_stmt = new Control_Flow_IC_Stmt(label, off1);
	ic_list.push_back(label_1_stmt);

	if(else_part_code.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), else_part_code.get_icode_list());
	}
	Icode_Stmt * label_2_stmt = new Control_Flow_IC_Stmt(label, off2);
	ic_list.push_back(label_2_stmt);

	Register_Descriptor * empty_register = machine_desc_object.get_new_register<gp_data>();
	empty_register->reset_use_for_expr_result();
	cond_register->reset_use_for_expr_result();

	Code_For_Ast * if_stmt_code;
	if (ic_list.empty() == false){
		if_stmt_code = new Code_For_Ast(ic_list, empty_register);
	}

	return *if_stmt_code;
}

///////////////////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Iteration_Statement_Ast::compile(){
	Code_For_Ast & cond_stmt = cond->compile();
	Register_Descriptor * cond_register = cond_stmt.get_reg();
	cond_register->set_use_for_expr_result();

	Ics_Opd * cond_opd = new Register_Addr_Opd(cond_register);
	Ics_Opd * zero_opd  = new Register_Addr_Opd(machine_desc_object.spim_register_table[zero]);

	int body_label_number = labelCounter;
	string off1 = get_new_label();
	int goto_stmt_label_number = labelCounter;
	string off2 = get_new_label();

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if(!is_do_form){
		Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(j, off2);
		ic_list.push_back(goto_stmt);
	}

	Icode_Stmt * label_1_stmt = new Control_Flow_IC_Stmt(label, off1);
	ic_list.push_back(label_1_stmt);
	Code_For_Ast & body_code = body->compile();
	
	if(body_code.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), body_code.get_icode_list());
	}
	
	Icode_Stmt * label_2_stmt = new Control_Flow_IC_Stmt(label, off2);
	ic_list.push_back(label_2_stmt);
	
	if(cond_stmt.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), cond_stmt.get_icode_list());
	}
	Icode_Stmt * condition_bne_stmt = new Control_Flow_IC_Stmt(bne, cond_opd, zero_opd, off1);
	ic_list.push_back(condition_bne_stmt);

	Register_Descriptor * empty_register = machine_desc_object.get_new_register<gp_data>();
	empty_register->reset_use_for_expr_result();
	cond_register->reset_use_for_expr_result();

	Code_For_Ast * return_stmt;

	if (ic_list.empty() == false){
		return_stmt = new Code_For_Ast(ic_list, empty_register);
	}

	return *return_stmt;
}

////////////////////////////////////////////////////////////////////

Code_For_Ast & Conditional_Operator_Ast::compile(){
	
	Code_For_Ast & cond_stmt = cond->compile();
	Register_Descriptor * cond_register = cond_stmt.get_reg();
	cond_register->set_use_for_expr_result();

	Ics_Opd * cond_opd = new Register_Addr_Opd(cond_register);
	Ics_Opd * zero_opd  = new Register_Addr_Opd(machine_desc_object.spim_register_table[zero]);

	Code_For_Ast & if_code = lhs->compile();
	Code_For_Ast & else_code = rhs->compile();

	string off1 = get_new_label();
	int goto_stmt_label_number = labelCounter;
	string off2 = get_new_label();

	Icode_Stmt * beq_stmt = new Control_Flow_IC_Stmt(beq, cond_opd, zero_opd, off1);
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(j, off2);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	if(cond_stmt.get_icode_list().empty() == false){
		ic_list = cond_stmt.get_icode_list();
	}
	ic_list.push_back(beq_stmt);

	if(if_code.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), if_code.get_icode_list());
	}

	Register_Descriptor * result_register;
	if(node_data_type == int_data_type){
		result_register = machine_desc_object.get_new_register<gp_data>();
		result_register->set_use_for_expr_result();
	}
	else if(node_data_type == double_data_type){
		result_register = machine_desc_object.get_new_register<float_reg>();
		result_register->set_use_for_expr_result();
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value of Mult Ast");
	}

	Register_Descriptor * if_part_register = if_code.get_reg();
	if_part_register->set_use_for_expr_result();

	Ics_Opd * lhs_opd = new Register_Addr_Opd(if_part_register);
	Ics_Opd * res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * cond_oper_stmt;
	cond_oper_stmt =  new Compute_IC_Stmt(or_t, res_opd, lhs_opd, zero_opd);
	ic_list.push_back(cond_oper_stmt);

	ic_list.push_back(goto_stmt);
	result_register->reset_use_for_expr_result();

	Icode_Stmt * label_1_stmt = new Control_Flow_IC_Stmt(label, off1);
	ic_list.push_back(label_1_stmt);

	if(else_code.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), else_code.get_icode_list());
	}

	if(node_data_type == int_data_type){
		result_register = machine_desc_object.get_new_register<gp_data>();
	}
	else if(node_data_type == double_data_type){
		result_register = machine_desc_object.get_new_register<float_reg>();
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value of Mult Ast");
	}

	Register_Descriptor * else_part_register = else_code.get_reg();
	else_part_register->set_use_for_expr_result();

	Ics_Opd * rhs_opd = new Register_Addr_Opd(else_part_register);
	res_opd = new Register_Addr_Opd(result_register);

	cond_oper_stmt =  new Compute_IC_Stmt(or_t, res_opd, rhs_opd, zero_opd);
	ic_list.push_back(cond_oper_stmt);

	Icode_Stmt * label_2_stmt = new Control_Flow_IC_Stmt(label, off2);
	ic_list.push_back(label_2_stmt);

	if_part_register->reset_use_for_expr_result();
	else_part_register->reset_use_for_expr_result();
	cond_register->reset_use_for_expr_result();

	Code_For_Ast * return_stmt;

	if (ic_list.empty() == false){
		return_stmt = new Code_For_Ast(ic_list, result_register);
	}

	return *return_stmt;	
}

///////////////////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Plus_Ast::compile(){
	
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	
	Code_For_Ast & opd1_stmt = lhs->compile();
	Code_For_Ast & opd2_stmt = rhs->compile();

	if(opd1_stmt.get_icode_list().empty() == false){
		ic_list = opd1_stmt.get_icode_list();
	}
	Register_Descriptor * opd1_register = opd1_stmt.get_reg();
	opd1_register->set_use_for_expr_result();

	if(opd2_stmt.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), opd2_stmt.get_icode_list());
	}
	Register_Descriptor * opd2_register = opd2_stmt.get_reg();
	opd2_register->set_use_for_expr_result();

	Register_Descriptor * result_register;
	if(node_data_type == int_data_type){
		result_register = machine_desc_object.get_new_register<gp_data>();
		result_register->set_use_for_expr_result();
	}
	else if(node_data_type == double_data_type){
		result_register = machine_desc_object.get_new_register<float_reg>();
		result_register->set_use_for_expr_result();
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value of Plus Ast");
	}

	Ics_Opd * lhs_opd = new Register_Addr_Opd(opd1_register);
	Ics_Opd * rhs_opd = new Register_Addr_Opd(opd2_register);
	Ics_Opd * res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * plus_stmt;
	if(node_data_type == int_data_type){
		plus_stmt =  new Compute_IC_Stmt(add, res_opd, lhs_opd, rhs_opd);
	}
	else if(node_data_type == double_data_type){
		plus_stmt =  new Compute_IC_Stmt(add_d, res_opd, lhs_opd, rhs_opd);
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value of Plus Ast");
	}

	ic_list.push_back(plus_stmt);
	opd1_register->reset_use_for_expr_result();
	opd2_register->reset_use_for_expr_result();

	Code_For_Ast * return_stmt;

	if (ic_list.empty() == false){
		return_stmt = new Code_For_Ast(ic_list, result_register);
	}

	return *return_stmt;
}

/////////////////////////////////////////////////////////////////

Code_For_Ast & Minus_Ast::compile(){
	
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	
	Code_For_Ast & opd1_stmt = lhs->compile();
	Code_For_Ast & opd2_stmt = rhs->compile();

	if(opd1_stmt.get_icode_list().empty() == false){
		ic_list = opd1_stmt.get_icode_list();
	}
	Register_Descriptor * opd1_register = opd1_stmt.get_reg();
	opd1_register->set_use_for_expr_result();

	if(opd2_stmt.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), opd2_stmt.get_icode_list());
	}
	Register_Descriptor * opd2_register = opd2_stmt.get_reg();
	opd2_register->set_use_for_expr_result();

	Register_Descriptor * result_register;
	if(node_data_type == int_data_type){
		result_register = machine_desc_object.get_new_register<gp_data>();
		result_register->set_use_for_expr_result();
	}
	else if(node_data_type == double_data_type){
		result_register = machine_desc_object.get_new_register<float_reg>();
		result_register->set_use_for_expr_result();
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value type of Minus Ast");
	}

	Ics_Opd * lhs_opd = new Register_Addr_Opd(opd1_register);
	Ics_Opd * rhs_opd = new Register_Addr_Opd(opd2_register);
	Ics_Opd * res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * minus_stmt;
	if(node_data_type == int_data_type){
		minus_stmt =  new Compute_IC_Stmt(sub, res_opd, lhs_opd, rhs_opd);
	}
	else if(node_data_type == double_data_type){
		minus_stmt =  new Compute_IC_Stmt(sub_d, res_opd, lhs_opd, rhs_opd);
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value type of Minus Ast");
	}

	ic_list.push_back(minus_stmt);
	opd1_register->reset_use_for_expr_result();
	opd2_register->reset_use_for_expr_result();

	Code_For_Ast * return_stmt;

	if (ic_list.empty() == false){
		return_stmt = new Code_For_Ast(ic_list, result_register);
	}

	return *return_stmt;
}

//////////////////////////////////////////////////////////////////

Code_For_Ast & Mult_Ast::compile(){
	
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	Code_For_Ast & opd1_stmt = lhs->compile();

	if(opd1_stmt.get_icode_list().empty() == false){
		ic_list = opd1_stmt.get_icode_list();
	}
	Register_Descriptor * opd1_register = opd1_stmt.get_reg();
	opd1_register->set_use_for_expr_result();

	Code_For_Ast & opd2_stmt = rhs->compile();

	if(opd2_stmt.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), opd2_stmt.get_icode_list());
	}
	Register_Descriptor * opd2_register = opd2_stmt.get_reg();
	opd2_register->set_use_for_expr_result();

	Register_Descriptor * result_register;
	if(node_data_type == int_data_type){
		result_register = machine_desc_object.get_new_register<gp_data>();
		result_register->set_use_for_expr_result();
	}
	else if(node_data_type == double_data_type){
		result_register = machine_desc_object.get_new_register<float_reg>();
		result_register->set_use_for_expr_result();
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value type of Mult Ast");
	}

	Ics_Opd * lhs_opd = new Register_Addr_Opd(opd1_register);
	Ics_Opd * rhs_opd = new Register_Addr_Opd(opd2_register);
	Ics_Opd * res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * mult_stmt;
	if(node_data_type == int_data_type){
		mult_stmt =  new Compute_IC_Stmt(mult, res_opd, lhs_opd, rhs_opd);
	}
	else if(node_data_type == double_data_type){
		mult_stmt =  new Compute_IC_Stmt(mult_d, res_opd, lhs_opd, rhs_opd);
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value type of Mult Ast");
	}

	ic_list.push_back(mult_stmt);
	opd1_register->reset_use_for_expr_result();
	opd2_register->reset_use_for_expr_result();

	Code_For_Ast * return_stmt;

	if (ic_list.empty() == false){
		return_stmt = new Code_For_Ast(ic_list, result_register);
	}

	return *return_stmt;
}

////////////////////////////////////////////////////////////////////

Code_For_Ast & Divide_Ast::compile(){

	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	Code_For_Ast & opd1_stmt = lhs->compile();

	if(opd1_stmt.get_icode_list().empty() == false){
		ic_list = opd1_stmt.get_icode_list();
	}
	Register_Descriptor * opd1_register = opd1_stmt.get_reg();
	opd1_register->set_use_for_expr_result();

	Code_For_Ast & opd2_stmt = rhs->compile();

	if(opd2_stmt.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), opd2_stmt.get_icode_list());
	}
	Register_Descriptor * opd2_register = opd2_stmt.get_reg();
	opd2_register->set_use_for_expr_result();

	Register_Descriptor * result_register;
	if(node_data_type == int_data_type){
		result_register = machine_desc_object.get_new_register<gp_data>();
		result_register->set_use_for_expr_result();
	}
	else if(node_data_type == double_data_type){
		result_register = machine_desc_object.get_new_register<float_reg>();
		result_register->set_use_for_expr_result();
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value type of Divide Ast");
	}

	Ics_Opd * lhs_opd = new Register_Addr_Opd(opd1_register);
	Ics_Opd * rhs_opd = new Register_Addr_Opd(opd2_register);
	Ics_Opd * res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * divd_stmt;
	if(node_data_type == int_data_type){
		divd_stmt =  new Compute_IC_Stmt(divd, res_opd, lhs_opd, rhs_opd);
	}
	else if(node_data_type == double_data_type){
		divd_stmt =  new Compute_IC_Stmt(div_d, res_opd, lhs_opd, rhs_opd);
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value type of Divide Ast");
	}

	ic_list.push_back(divd_stmt);
	opd1_register->reset_use_for_expr_result();
	opd2_register->reset_use_for_expr_result();

	Code_For_Ast * return_stmt;

	if (ic_list.empty() == false){
		return_stmt = new Code_For_Ast(ic_list, result_register);
	}

	return *return_stmt;
}


//////////////////////////////////////////////////////////////////////

Code_For_Ast & UMinus_Ast::compile(){

	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	Code_For_Ast & load_stmt = lhs->compile();

	if(load_stmt.get_icode_list().empty() == false){
		ic_list = load_stmt.get_icode_list();
	}
	Register_Descriptor * load_register = load_stmt.get_reg();
	load_register->set_use_for_expr_result();

	Register_Descriptor * result_register;
	if(node_data_type == int_data_type){
		result_register = machine_desc_object.get_new_register<gp_data>();
		result_register->set_use_for_expr_result();
	}
	else if(node_data_type == double_data_type){
		result_register = machine_desc_object.get_new_register<float_reg>();
		result_register->set_use_for_expr_result();
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value of UMinus Ast");
	}

	Ics_Opd * opd1 = new Register_Addr_Opd(load_register);
	Ics_Opd * result = new Register_Addr_Opd(result_register);

	Icode_Stmt * uminus_stmt;
	if(node_data_type == int_data_type){
		uminus_stmt =  new Move_IC_Stmt(uminus, opd1, result);
	}
	else if(node_data_type == double_data_type){
		uminus_stmt =  new Move_IC_Stmt(uminus_d, opd1, result);
	}
	else{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Invalid value of UMinus Ast");
	}

	ic_list.push_back(uminus_stmt);
	load_register->reset_use_for_expr_result();

	Code_For_Ast * return_stmt;

	if (ic_list.empty() == false){
		return_stmt = new Code_For_Ast(ic_list, result_register);
	}
	
	return *return_stmt;
}

//////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Sequence_Ast::compile(){
	Code_For_Ast sequence_ast;

	list<Ast *>::iterator i;
	for (i = statement_list.begin(); i != statement_list.end(); i++){
		Ast *ast = *i;
		sequence_ast = ast->compile();

		list<Icode_Stmt *> & icode_list = sequence_ast.get_icode_list();
		if(icode_list.empty() == false){
			if(sa_icode_list.empty())
				sa_icode_list = icode_list;
			else
				sa_icode_list.splice(sa_icode_list.end(), icode_list);
		}
	}

	machine_desc_object.clear_local_register_mappings();

	Register_Descriptor * seq_register = machine_desc_object.get_new_register<gp_data>();
	seq_register->reset_use_for_expr_result();

	Code_For_Ast * return_seq;
	//doubt
	return_seq = new Code_For_Ast(sa_icode_list, seq_register);
	
	return *return_seq;
}

void Sequence_Ast::print_assembly(ostream & file_buffer){
	list<Icode_Stmt *>::iterator i;
	for (i = sa_icode_list.begin(); i != sa_icode_list.end(); i++){
		(*i)->print_assembly(file_buffer);
	}
}

void Sequence_Ast::print_icode(ostream & file_buffer){
	list<Icode_Stmt *>::iterator i;
	for (i = sa_icode_list.begin(); i != sa_icode_list.end(); i++){
		(*i)->print_icode(file_buffer);
	}
}

//////////////////////////////////////////////////////////////////////////////

template class Number_Ast<double>;
template class Number_Ast<int>;
