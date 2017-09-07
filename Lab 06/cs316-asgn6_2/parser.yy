%scanner scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
	pair<Data_Type, string> * decl;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Procedure * procedure;

	int integer_value;
	double double_value;
	std::string * string_value;
	list<string> * decl_list1;
	Sequence_Ast * sequence_ast;
	Ast * ast;
	list<Ast *> * ast_list;

	list<Symbol_Table_Entry*> * s_list;
}
;

%token <integer_value> INTEGER_NUMBER
%token <double_value> DOUBLE_NUMBER
%token <string_value> STRING_VALUE
%token VOID
%token INTEGER FLOAT
%token ASSIGN DO WHILE
%token IF RETURN PRINT
%token <string_value> NAME

%left ELSE
%left '?' ':'
%left OR
%left AND
%left NOT_EQ EQ
%left LT LT_EQ GRT GRT_EQ
%left '+' '-'
%left '*' '/'
%right NOT
%right UMINUS
%nonassoc '('

%type <symbol_table> optional_variable_declaration_list
%type <symbol_table> variable_declaration_list
%type <s_list> variable_declaration
%type <decl> declaration
%type <decl_list1> name_list
%type <sequence_ast> statement_list
%type <ast> statement
%type <ast> iterative_statement
%type <ast> controlflow_statement
%type <ast> assignment_statement
%type <ast> bool_expression
%type <ast> relational_expression
%type <ast> arith_expression
%type <ast> operand
%type <ast> expression_term
%type <ast> variable
%type <ast> constant


%type <> procedure_declaration_list
%type <> procedure_definition_list
%type <Data_Type> data_type
%type <Data_Type> typex
%type <symbol_entry> arg_list
%type <symbol_table> parameter_list
%type <ast> print_statement
%type <ast> return_stmt
%type <ast_list> function_call_stmt

%start program

%%

program:
	declaration_list procedure_definition_list
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");

		program_object.set_procedure(current_procedure, get_line_number());
		program_object.global_list_in_proc_check();
	}
	}
;

declaration_list:
	procedure_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table * global_table = new Symbol_Table();
		program_object.set_global_table(*global_table);
	}
	}
|
	variable_declaration_list
	procedure_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table * global_table = $1;

		CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");

		program_object.set_global_table(*global_table);
	}
	}
|
	procedure_declaration_list
	variable_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table * global_table = $2;

		CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");

		program_object.set_global_table(*global_table);
	}
	}
;

procedure_declaration_list:
	procedure_declaration
	{
	if(NOT_ONLY_PARSE)
	{

	}
	}
|
	procedure_declaration
	procedure_declaration_list
	{
	if(NOT_ONLY_PARSE)
	{
		
	}
	}
;

procedure_declaration:
	typex NAME '(' ')' ';'
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "Procedure name cannot be null");
	}
	}
|
	typex NAME '(' arg_list ')' ';'
	{
	if(NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "Procedure name cannot be null");
	}
	}
;

arg_list:
	data_type NAME ',' arg_list
	{
	if(NOT_ONLY_PARSE)
	{
		
	}
	}
|
	data_type NAME
	{
	if(NOT_ONLY_PARSE)
	{
		
	}
	}
;

typex:
	VOID
	{
	if(NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|
	data_type
	{
	if(NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
;

data_type:
	INTEGER
	{
	if(NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|
	FLOAT
	{
	if(NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
;


procedure_definition_list:
	procedure_definition
	{
	if(NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|
	procedure_definition procedure_definition_list
	{
	if(NOT_ONLY_PARSE)
	{
		
	}
	}
;

procedure_definition:
	NAME '(' ')'
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($1 != NULL), "Procedure name cannot be null");

		string proc_name = *$1;

		current_procedure = new Procedure(void_data_type, proc_name, get_line_number());

		CHECK_INPUT ((program_object.variable_in_symbol_list_check(proc_name) == false),
			"Procedure name cannot be same as global variable", get_line_number());
	}
	}

	'{' optional_variable_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{

		CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");

		Symbol_Table * local_table = $6;

		if (local_table == NULL)
			local_table = new Symbol_Table();

		current_procedure->set_local_list(*local_table);
	}
	}

	statement_list return_stmt '}' 
	{
	if (NOT_ONLY_PARSE)
	{
		Sequence_Ast* seq = $8;

		CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");

		current_procedure->set_sequence_ast(*seq);
	}
	}
	procedure_definition
;

return_stmt:
	RETURN ';'
	{
	if(NOT_ONLY_PARSE)
	{
		Ast *temp = new Return_Ast(get_line_number());
		$$ = temp;
	}
	}
|
	RETURN arith_expression ';'
	{
	if(NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "Argument cannot be null");

		Ast* temp_arith = new Arithmetic_Expr_Ast();
		temp_arith = $2;

		Ast *var = new Return_Ast(temp_arith, get_line_number());
		$$ = var;
	}
	}
;

optional_variable_declaration_list:
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = NULL;
	}
	}
|
	variable_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($1 != NULL), "Declaration statement list cannot be null here");

		$$ = $1;
	}
	}
;

variable_declaration_list:
	variable_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table * decl_list = new Symbol_Table();
		list<Symbol_Table_Entry *> * decl_stmt = $1;
		CHECK_INVARIANT((decl_stmt != NULL), "The declaration statement cannot be null");
		
		for (list<Symbol_Table_Entry *>::iterator it = decl_stmt->begin(); it != decl_stmt->end(); it++)
		{
			string decl_name = (*it)->get_variable_name();
			CHECK_INPUT((program_object.variable_proc_name_check(decl_name) == false),
				"Procedure name cannot be same as the variable name", get_line_number());
			if(current_procedure != NULL)
			{
				CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
					"Variable name cannot be same as procedure name", get_line_number());
			}
			CHECK_INPUT(((decl_list)->variable_in_symbol_list_check(decl_name) == false), 
				"Variable is declared twice", get_line_number());
			decl_list->push_symbol(*it);
		}
		$$ = decl_list;
	}
	}
|
	variable_declaration_list variable_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table * decl_list = $1;
		list<Symbol_Table_Entry *> * decl_stmt = $2;
		CHECK_INVARIANT((decl_stmt != NULL), "The declaration statement cannot be null");
		CHECK_INVARIANT((decl_list != NULL), "The declaration statement list cannot be null");
		for (list<Symbol_Table_Entry *>::iterator it = decl_stmt->begin(); it != decl_stmt->end(); it++)
		{
			string decl_name = (*it)->get_variable_name();
			CHECK_INPUT((program_object.variable_proc_name_check(decl_name) == false),
				"Procedure name cannot be same as the variable name", get_line_number());
			if(current_procedure != NULL)
			{
				CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
					"Variable name cannot be same as procedure name", get_line_number());
			}
			CHECK_INPUT(((decl_list)->variable_in_symbol_list_check(decl_name) == false), 
				"Variable is declared twice", get_line_number());
			decl_list->push_symbol(*it);
		}
		$$ = decl_list;
	}
	}
;

variable_declaration:
	declaration ';'
	{
	if (NOT_ONLY_PARSE)
	{
		pair<Data_Type, string> * decl = $1;

		CHECK_INVARIANT((decl != NULL), "Declaration cannot be null");

		Data_Type type = decl->first;
		string decl_name = decl->second;

		list<Symbol_Table_Entry *> * decl_list = new list<Symbol_Table_Entry *>;
		decl_list -> push_back(new Symbol_Table_Entry(decl_name, type, get_line_number()));

		$$ = decl_list;
	}
	}
|
	declaration ',' name_list ';'
	{
	if (NOT_ONLY_PARSE)
	{
		if(NOT_ONLY_PARSE)
	{
		pair<Data_Type, string> * decl = $1;

		CHECK_INVARIANT((decl != NULL), "Declaration cannot be null");

		Data_Type type = decl -> first;
		string decl_name = decl -> second;

		list<Symbol_Table_Entry *> * decl_list = new list<Symbol_Table_Entry *>;

		decl_list -> push_back(new Symbol_Table_Entry(decl_name, type, get_line_number()));

		list<string> * name_list = $3;

		for( list<string>::iterator it = name_list -> begin(); it != name_list ->end(); ++it )
		{
			decl_name = *it;
			decl_list -> push_back( new Symbol_Table_Entry(decl_name, type, get_line_number() ) );
		}

		$$ = decl_list;
	}
	}
	}
;
//change here
name_list:
	NAME
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($1 != NULL), "Name cannot be null");

		list<string> * decl_list = new list<string>;
		string name = *$1;
		
		decl_list -> push_back( name );
		$$ = decl_list;
	}
	}
|
	name_list ',' NAME
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($3 != NULL), "Name cannot be null");

		string name = *$3;

		$1 -> push_back( name );
		$$ = $1;
	}
	}
;

declaration:
	INTEGER NAME
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "NAME cannot be null");
		Data_Type i_type = int_data_type;
		string name = *$2;
		
		pair<Data_Type, string> * decl = new pair<Data_Type, string>(i_type, name);
		$$ = decl;
	}
	}
|
	FLOAT NAME
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "NAME cannot be null");
		Data_Type d_type = double_data_type;
		string name = *$2;
		
		pair<Data_Type, string> * decl = new pair<Data_Type, string>(d_type, name);
		$$ = decl;
	}
	}
;

statement:
	assignment_statement
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|
	iterative_statement
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|
	controlflow_statement
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|
	'{' statement_list '}'
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $2;
	}
	}
|
	function_call_statement ';'
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|
	print_statement
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
;

statement_list:
	{
	if (NOT_ONLY_PARSE)
	{	
		$$ = new Sequence_Ast(get_line_number());
	}
	}
|
	statement_list statement
	{
	if(NOT_ONLY_PARSE)
	{
		$1->ast_push_back($2);
		$$ = $1;
	}
	}
;

print_statement:
	PRINT '(' arith_expression ')' ';'
	{
	if(NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($3 != NULL), "Argument cannot be null");

		Ast *var = new Print_Variable_Ast($3, get_line_number());
		$$ = var;
	}
	}
|
	PRINT '(' STRING_VALUE ')' ';'
	{
	if(NOT_ONLY_PARSE)
	{
		Ast *str = new Print_String_Ast(*$3, get_line_number());
		$$ = str;
	}
	}
;
//Not completed yet
function_call_statement:
	NAME '(' parameter_list ')'
	{
	if(NOT_ONLY_PARSE)
	{

		Ast* temp = new Function_Ast($1, $3)
	}
	}
|
	NAME '(' ')'
	{
	if(NOT_ONLY_PARSE)
	{
		
	}
	}
;

parameter_list:
	arith_expression
	{
	if(NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($1 != NULL), "Parameter cannot be null");
		//Not done yet
		$$ = $1;
	}
	}
|
	arith_expression ',' parameter_list
	{
	if(NOT_ONLY_PARSE)
	{	//not done yet
		list<Ast *> temp = parameter_list;
		Ast* temp = new Arithmetic_Expr_Ast();
		temp = $1;

	}
	}
;

iterative_statement:
	WHILE '(' bool_expression ')' statement
	{
	 if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($3 != NULL), "Condition cannot be null");

		Ast * iteration_stmt = new Iteration_Statement_Ast($3, $5, get_line_number(), false);

		$$ = iteration_stmt;
	}
	}
|
	DO statement WHILE '(' bool_expression ')' ';'
	{
	 if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($5 != NULL), "Condition cannot be null");

		Ast * iteration_stmt = new Iteration_Statement_Ast($5, $2 , get_line_number(), true);

		$$ = iteration_stmt;
	}
	}
;

controlflow_statement:
	IF '(' bool_expression ')' statement 
	{
		CHECK_INVARIANT(($3 != NULL), "Condition cannot be null");

		Ast * else_part = new Sequence_Ast(get_line_number());
		Ast * control_stmt = new Selection_Statement_Ast($3, $5, else_part , get_line_number());

		$$ = control_stmt;
	}
|
	IF '(' bool_expression ')' statement ELSE statement
	{
		CHECK_INVARIANT(($3 != NULL), "Condition cannot be null");

		Ast * control_stmt = new Selection_Statement_Ast($3, $5, $7 , get_line_number());

		$$ = control_stmt;
	}
;

assignment_statement:
	variable ASSIGN arith_expression ';'
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");
		Ast *lhs = $1;
		Ast *rhs = $3;

		Ast * assignment_stmt = new Assignment_Ast(lhs, rhs, get_line_number());
		assignment_stmt->check_ast();

		$$ = assignment_stmt;
	}
	}
|
	variable ASSIGN function_call_statement ';'
	{
	if(NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");
		Ast *lhs = $1;
		Ast *rhs = $3;

		Ast * assignment_stmt = new Assignment_Ast(lhs, rhs, get_line_number());
		assignment_stmt->check_ast();

		$$ = assignment_stmt;
	}
	}
;

bool_expression:
	relational_expression
	{
		CHECK_INVARIANT(($1 != NULL), "relational expression cannot be null");
		$$ = $1;
	}
|
	bool_expression AND bool_expression
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * and_stmt = new Boolean_Expr_Ast($1, boolean_and, $3, get_line_number());
		and_stmt->check_ast();

		$$ = and_stmt;
	}
	}
|
	bool_expression OR bool_expression
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * or_stmt = new Boolean_Expr_Ast($1, boolean_or, $3, get_line_number());
		or_stmt->check_ast();

		$$ = or_stmt;
	}
	}
|
	NOT bool_expression
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "rhs cannot be null");

		Ast * not_stmt = new Boolean_Expr_Ast(NULL, boolean_not, $2, get_line_number());
		not_stmt->check_ast();

		$$ = not_stmt;
	}
	}
|
	'(' bool_expression ')'
	{
		CHECK_INVARIANT(($2 != NULL), "bool expression cannot be null");
		$$ = $2;
	}
;

relational_expression:
	operand LT operand
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * lessthan_stmt = new Relational_Expr_Ast($1, less_than, $3, get_line_number());
		lessthan_stmt->check_ast();

		$$ = lessthan_stmt;
	}
	}
|
	operand LT_EQ operand
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * lessthaneq_stmt = new Relational_Expr_Ast($1, less_equalto, $3, get_line_number());
		lessthaneq_stmt->check_ast();

		$$ = lessthaneq_stmt;
	}
	}
|
	operand GRT operand
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * greaterthan_stmt = new Relational_Expr_Ast($1, greater_than, $3, get_line_number());
		greaterthan_stmt->check_ast();

		$$ = greaterthan_stmt;
	}
	}
|
	operand GRT_EQ operand
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * greaterthaneq_stmt = new Relational_Expr_Ast($1, greater_equalto, $3, get_line_number());
		greaterthaneq_stmt->check_ast();

		$$ = greaterthaneq_stmt;
	}
	}
|
	operand EQ operand
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * eq_stmt = new Relational_Expr_Ast($1, equalto, $3, get_line_number());
		eq_stmt->check_ast();

		$$ = eq_stmt;
	}
	}
|
	operand NOT_EQ operand
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * noteq_stmt = new Relational_Expr_Ast($1, not_equalto, $3, get_line_number());
		noteq_stmt->check_ast();

		$$ = noteq_stmt;
	}
	}
;

arith_expression:
                // SUPPORT binary +, -, *, / operations, unary -, and allow parenthesization
                // i.e. E -> (E)
                // Connect the rules with the remaining rules given below
    operand '+' operand
    {
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * plus_stmt = new Plus_Ast($1, $3, get_line_number());
		plus_stmt->check_ast();

		$$ = plus_stmt;
	}
	}
|
	operand '-' operand
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * minus_stmt = new Minus_Ast($1, $3, get_line_number());
		minus_stmt->check_ast();

		$$ = minus_stmt;
	}
	}
|
	operand '*' operand
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * mult_stmt = new Mult_Ast($1, $3, get_line_number());
		mult_stmt->check_ast();

		$$ = mult_stmt;
	}
	}
|
	operand '/' operand
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * divide_stmt = new Divide_Ast($1, $3, get_line_number());
		divide_stmt->check_ast();

		$$ = divide_stmt;
	}
	}
|
	'-' operand %prec UMINUS
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "operand cannot be null");
		Ast * uminus_stmt = new UMinus_Ast($2, NULL, get_line_number());

		$$ = uminus_stmt;
	}
	}
|
	'(' operand ')'
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $2;
	}
	}
|
	bool_expression '?' operand ':' operand
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL) && ($5 != NULL)), "operands cannot be null");
		Ast * control_stmt = new Conditional_Operator_Ast($1, $3, $5 , get_line_number());
		control_stmt->check_ast();

		$$ = control_stmt;	
	}
	}
|
	expression_term
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
;

operand:
	arith_expression
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
;

expression_term:
	variable
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|
	constant
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
;

variable:
	NAME
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table_Entry * var_table_entry;

		CHECK_INVARIANT(($1 != NULL), "Variable name cannot be null");

		if (current_procedure->variable_in_symbol_list_check(*$1))
			 var_table_entry = &(current_procedure->get_symbol_table_entry(*$1));

		else if (program_object.variable_in_symbol_list_check(*$1))
			var_table_entry = &(program_object.get_symbol_table_entry(*$1));

		else
			CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Variable has not been declared", get_line_number());

		$$ = new Name_Ast(*$1, *var_table_entry, get_line_number());

		delete $1;
	}
	}
;

constant:
	INTEGER_NUMBER
	{
	if (NOT_ONLY_PARSE)
	{
		
		int in_name = $1;
		Ast* in_ast = new Number_Ast<int>(in_name, int_data_type , get_line_number());
		$$ = in_ast;
	
	}
	}
|
	DOUBLE_NUMBER
	{
	if (NOT_ONLY_PARSE)
	{
		double d_name = $1;
		Ast* d_ast = new Number_Ast<double>(d_name, double_data_type , get_line_number());
		$$ = d_ast;
	}
	}
;
