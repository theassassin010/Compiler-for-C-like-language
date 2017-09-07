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
	Sequence_Ast * sequence_ast;
	Ast * ast;

	list<string> namelist;
}
;

%token <integer_value> INTEGER_NUMBER
%token <double_value> DOUBLE_NUMBER
%token VOID
%token INTEGER FLOAT
%token ASSIGN DO WHILE
%token IF ELSE ? :
%token <string_value> NAME


%left OR
%left AND
%left NOT_EQ EQ
%left LT LT_EQ GRT GRT_EQ
%left '+' '-'
%left '*' '/'
%right UMINUS
%right NOT
%nonassoc '('

%type <symbol_table> optional_variable_declaration_list
%type <symbol_table> variable_declaration_list
%type <symbol_entry> variable_declaration
%type <decl> declaration
%type <> name_list
%type <sequence_ast> statement_list
%type <> iterative_statement:
%type <> controlflow_statement:
%typr <> else_statement:
%type <ast> assignment_statement
%type <ast> bool_expression
%type <ast> relational_expression
%type <ast> arith_expression
%type <ast> operand
%type <ast> expression_term
%type <ast> variable
%type <ast> constant


%start program

%%

program:
	declaration_list procedure_definition
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
	procedure_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table * global_table = new Symbol_Table();
		program_object.set_global_table(*global_table);
	}
	}
|
	variable_declaration_list
	procedure_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table * global_table = $1;

		CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");

		program_object.set_global_table(*global_table);
	}
	}
|
	procedure_declaration
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

procedure_declaration:
	VOID NAME '(' ')' ';'
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "Procedure name cannot be null");
		CHECK_INVARIANT((*$2 == "main"), "Procedure name must be main in declaration");
	}
	}
;

procedure_definition:
	NAME '(' ')'
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($1 != NULL), "Procedure name cannot be null");
		CHECK_INVARIANT((*$1 == "main"), "Procedure name must be main");

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

	statement_list '}'
	{
	if (NOT_ONLY_PARSE)
	{
		Sequence_Ast* seq = $8;

		CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");
		CHECK_INVARIANT((seq != NULL), "statement list cannot be null");

		current_procedure->set_sequence_ast(*seq);
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
		Symbol_Table_Entry * decl_stmt = $1;

		CHECK_INVARIANT((decl_stmt != NULL), "Non-terminal declaration statement cannot be null");

		string decl_name = decl_stmt->get_variable_name();
		CHECK_INPUT ((program_object.variable_proc_name_check(decl_name) == false),
				"Variable name cannot be same as the procedure name", get_line_number());

		if(current_procedure != NULL)
		{
			CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
				"Variable name cannot be same as procedure name", get_line_number());
		}

		Symbol_Table * decl_list = new Symbol_Table();
		decl_list->push_symbol(decl_stmt);

		$$ = decl_list;
	}
	}
|
	variable_declaration_list variable_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list

		Symbol_Table_Entry * decl_stmt = $2;
		Symbol_Table * decl_list = $1;

		CHECK_INVARIANT((decl_stmt != NULL), "The declaration statement cannot be null");
		CHECK_INVARIANT((decl_list != NULL), "The declaration statement list cannot be null");

		string decl_name = decl_stmt->get_variable_name();
		CHECK_INPUT((program_object.variable_proc_name_check(decl_name) == false),
			"Procedure name cannot be same as the variable name", get_line_number());
		if(current_procedure != NULL)
		{
			CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
				"Variable name cannot be same as procedure name", get_line_number());
		}

		CHECK_INPUT((decl_list->variable_in_symbol_list_check(decl_name) == false), 
				"Variable is declared twice", get_line_number());

		decl_list->push_symbol(decl_stmt);
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

		Symbol_Table_Entry * decl_entry = new Symbol_Table_Entry(decl_name, type, get_line_number());

		$$ = decl_entry;

	}
	}
;
//change here
declaration:
	INTEGER name_list
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
	FLOAT name_list
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

name_list:
	NAME ',' name_list
	{

	}
:
// change above
statement_list:
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = NULL;
	}
	}
|
	statement_list assignment_statement
	{
	if (NOT_ONLY_PARSE)
	{

		if($1 == NULL)
		{
			$1 = new Sequence_Ast(get_line_number());
		}
		$1->ast_push_back($2);
		$$ = $1;
	}
|
	statement_list iterative_statement
	{
	if (NOT_ONLY_PARSE)
	{
		if($1 == NULL)
		{
			$1 = new Sequence_Ast(get_line_number());
		}
		$1->ast_push_back($2);
		$$ = $1;
	}
	}
|
	statement_list controlflow_statement
	{
	if (NOT_ONLY_PARSE)
	{
		if($1 == NULL)
		{
			$1 = new Sequence_Ast(get_line_number());
		}
		$1->ast_push_back($2);
		$$ = $1;
	}
	}
	}
|
	'{' statement_list '}'
	{
	if (NOT_ONLY_PARSE)
	{
		if($2 == NULL)
		{
			$2 = new Sequence_Ast(get_line_number());
		}
		$$ = $2;
	}
	}
;

iterative_statement:
	WHILE '(' bool_expression ')' statement_list
	{
	 if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($3 != NULL), "Condition cannot be null");

		Ast * iteration_stmt = new Iteration_Statement_AST($3, $5, get_line_number());
		iteration_stmt->check_ast();

		$$ = controlflow_statement;
	}
	}
|
	DO statement_list WHILE '(' bool_expression ')' ';'
	{
	 if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($3 != NULL), "Condition cannot be null");

		Ast * iteration_stmt = new Iteration_Statement_AST($5, $2 , get_line_number());
		iteration_stmt->check_ast();

		$$ = controlflow_statement;
	}
	}
;

controlflow_statement:
	IF '(' bool_expression ')' statement_list
	{
		CHECK_INVARIANT(($3 != NULL), "Condition cannot be null");

		Ast * control_stmt = new Selection_Statement_Ast($3, $5, NULL , get_line_number());
		control_stmt->check_ast();

		$$ = controlflow_statement;
	}
|
	IF '(' bool_expression ')' statement_list else_statement
	{
		CHECK_INVARIANT(($3 != NULL), "Condition cannot be null");

		Ast * control_stmt = new Selection_Statement_Ast($3, $5, $6 , get_line_number());
		control_stmt->check_ast();

		$$ = controlflow_statement;
	}
;
// doubts here
else_statement:
	ELSE statement_list
	{
		else_part = $2
		$$ = else_statement
	}
;
// till here
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
;

bool_expression:
	relational_expression
	{
		$$ = $1;
	}
|
	bool_expression AND bool_expression
	{

	}
|
	bool_expression OR bool_expression
	{

	}
|
	NOT bool_expression
	{

	}
;

relational_expression:
	operand LT operand
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * plus_stmt = new Plus_Ast($1, $3, get_line_number());
		plus_stmt->check_ast();

		$$ = ;
	}
	}
|
	operand LT_EQ operand
	{

	}
|
	operand GRT operand
	{

	}
|
	operand GRT_EQ operand
	{

	}
	operand EQ operand
	{

	}
|
	operand NOT_EQ operand
	{

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
		Ast * divide_stmt = new Divide_Ast($1, $3, get_line_number());
		divide_stmt->check_ast();

		$$ = divide_stmt;
	}
	}
|
	bool_expression '?' arith_expression ':' arith_expression
	{

	}
|
	'-' operand %prec UMINUS
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * uminus_stmt = new UMinus_Ast($2, NULL, get_line_number());
		uminus_stmt->check_ast();

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
