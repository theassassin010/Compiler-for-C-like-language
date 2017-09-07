#include <iostream>
#include <fstream>
#include <typeinfo>
#include <set>

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

Procedure *curr_proc;

Procedure::Procedure(Data_Type proc_return_type, string proc_name, int line)
{
	return_type = proc_return_type;
	name = proc_name;
	lineno = line;
}

Procedure::~Procedure()
{
	
}

string Procedure::get_proc_name()
{
	return name;
}

void Procedure::set_sequence_ast(Sequence_Ast &sa)
{
	sequence_ast = &sa;
}

void Procedure::set_local_list(Symbol_Table &new_list)
{
	local_symbol_table = new_list;
	local_symbol_table.set_table_scope(local);
}

void Procedure::set_arg_list(Symbol_Table &arg_list)
{
	arg_symbol_table = arg_list;
	arg_symbol_table.set_table_scope(local);
}

Data_Type Procedure::get_return_type()
{
	return return_type;
}

Symbol_Table *Procedure::get_arg_symbol_table()
{
	return &arg_symbol_table;
}

Symbol_Table_Entry&Procedure::get_symbol_table_entry(string variable_name)
{
	if(local_symbol_table.variable_in_symbol_list_check(variable_name))
	{
		return local_symbol_table.get_symbol_table_entry(variable_name);
	}
	else
	{
		return arg_symbol_table.get_symbol_table_entry(variable_name);
	}
}

void Procedure::print(ostream &file_buffer)
{
	file_buffer<<PROC_SPACE<<"Procedure: "<<name<<", Return Type: ";
	if(return_type == void_data_type)
	{
		file_buffer<<"VOID\n";
	}
	else if(return_type == int_data_type)
	{
		file_buffer<<"INT\n";
	}
	else if(return_type == double_data_type)
	{
		file_buffer<<"FLOAT\n";
	}
	else
	{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Return type declared is not Valid");
	}

	sequence_ast->print(file_buffer);
}

void Procedure::print_sym(ostream &file_buffer)
{
	file_buffer<<PROC_SPACE<<"Procedure: "<<name<<", Return Type: ";
	if(return_type == void_data_type)
	{
		file_buffer<<"VOID\n";
	}
	else if(return_type == int_data_type)
	{
		file_buffer<<"INT\n";
	}
	else if(return_type == double_data_type)
	{
		file_buffer<<"FLOAT\n";
	}
	else
	{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Return type declared is not Valid");
	}

	file_buffer<<"   Fromal Parameter List\n";
	// arg_symbol_table.print_reverse(file_buffer);
	arg_symbol_table.get_table().reverse();
	arg_symbol_table.print(file_buffer);
	arg_symbol_table.get_table().reverse();
	file_buffer<<"   Local Declarartions\n";
	local_symbol_table.print(file_buffer);
}

bool Procedure::check_arg_types(list<Ast *> &arg_t)
{
	if(arg_symbol_table.get_table().size() != arg_t.size())
	{
		return false;
	}

	auto it1 = arg_symbol_table.get_table().begin(); // from declaration
	auto it2 = arg_t.begin(); // from definition

	for(it1; it1 != arg_symbol_table.get_table().end(); it1++, it2++)
	{
		if((*it1)->get_data_type() != (*it2)->get_data_type())
		{
			return false;
		}
		// This code should be removed Checkkkk
		// if((*it1)->get_variable_name() != (*it2)->get_variable_name())
		// {
		// 	cout<<"Names don't match"<<endl;
		// 	return false;
		// }
	}
	return true;
}

bool Procedure::check_arg_types(Symbol_Table &arg_t)
{
	if(arg_symbol_table.get_table().size() != arg_t.get_table().size())
	{
		return false;
	}

	auto it1 = arg_symbol_table.get_table().begin();
	auto it2 = arg_t.get_table().begin();

	for(it1; it1 != arg_symbol_table.get_table().end(); it1++, it2++)
	{
		if((*it1)->get_data_type() != (*it2)->get_data_type())
		{
			return false;
		}
		if((*it1)->get_variable_name() != (*it2)->get_variable_name()) // Checking if names in declaration match and changing the entries accordingly
		{
			// cout<<"Names don't match"<<endl;
			// return false;
			(*it1)->set_variable_name((*it2)->get_variable_name());
		}
	}
	return true;
}

bool Procedure::variable_in_symbol_list_check(string variable)
{
	if((local_symbol_table.variable_in_symbol_list_check(variable) || arg_symbol_table.variable_in_symbol_list_check(variable)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Procedure::print_prologue(ostream & file_buffer)
{
	stringstream prologue;

	prologue<<"\n";
	prologue<<"\t.text \t\t\t# The .text assembler directive indicates\n";
	prologue<<"\t.globl "<<name<<"\t\t# The following is the code (as oppose to data)\n";
	prologue<<name<<":\t\t\t\t# .globl makes main know to the \n\t\t\t\t# outside of the program.\n";
	prologue<<"# Prologue begins \n";
	prologue<<"\tsw $ra, 0($sp)\t\t# Save the return address\n";
	prologue<<"\tsw $fp, -4($sp)\t\t# Save the frame pointer\n";
	prologue<<"\tsub $fp, $sp, 8\t\t# Update the frame pointer\n";
	// cout<<local_symbol_table.get_size()<<" seen in proc"<<endl;
	prologue<<"\n\tsub $sp, $sp, "<<local_symbol_table.get_size() + 8<<"\t\t# Make space for the locals\n";
	prologue<<"# Prologue ends\n\n";

	file_buffer<<prologue.str();
}

void Procedure::print_epilogue(ostream & file_buffer)
{
	stringstream epilogue;

	epilogue<<"\n# Epilogue Begins\nepilogue_"<<name<<":\n\tadd $sp, $sp, "<<local_symbol_table.get_size() + 8<<"\n";
	epilogue<<"\tlw $fp, -4($sp)  \n\tlw $ra, 0($sp)  \n\tjr        $31\t\t# Jump back to the called procedure\n# Epilogue Ends\n\n";

	file_buffer<<epilogue.str();
}