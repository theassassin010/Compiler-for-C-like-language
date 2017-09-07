%filenames="scanner"
%lex-source="scanner.cc"

%%

[[:digit:]]+	{
		store_token_name("INT_NUMBER");
 	  ParserBase::STYPE__ *val = getSval();
      val->integer_value = atoi(matched().c_str());
	  return Parser::INTEGER_NUMBER; 
	}
(([0-9]+\.?[0-9]*)|(\.[0-9]+))([eE][+-]?[0-9]+)? {
								store_token_name("FLOAT_NUMBER");
								ParserBase::STYPE__ *val = getSval();
				          		val->double_value = atof(matched().c_str());
						   		return Parser::DOUBLE_NUMBER;
							}

"void"	{store_token_name("VOID");return Parser::VOID;}
"int" {store_token_name("INTEGER");return Parser::INTEGER;}
"float" {store_token_name("FLOAT"); return Parser:: FLOAT;}
"=" {store_token_name("ASSIGNMENT_OP");return Parser::ASSIGN;}
"return" {store_token_name("RETURN");return Parser::RETURN;}
"print" {store_token_name("RETURN");return Parser::RETURN;}

"while" {store_token_name("WHILE");return Parser::WHILE;}
"do" {store_token_name("DO"); return Parser::DO;}
"if" {store_token_name("IF"); return Parser::IF;}
"else" {store_token_name("ELSE"); return Parser::ELSE;}

"?" {store_token_name("CHAR"); return matched()[0];}
":" {store_token_name("CHAR"); return matched()[0];}




[[:alpha:]_][[:alpha:][:digit:]_]* {
		store_token_name("NAME");
	  ParserBase::STYPE__ *val = getSval();	
	  val->string_value = new std::string(matched());
	  return Parser::NAME; 
	}

"{" {store_token_name("CHAR");return matched()[0];}
"}"	{store_token_name("CHAR");return matched()[0];}
";"	{store_token_name("CHAR");return matched()[0];}
"("	{store_token_name("CHAR");return matched()[0];}
")"	{store_token_name("CHAR");return matched()[0];}
"+" {store_token_name("ARITHMETIC_OP"); return matched()[0];}
"-" {store_token_name("ARITHMETIC_OP"); return matched()[0];}
"*" {store_token_name("ARITHMETIC_OP"); return matched()[0];}
"/" {store_token_name("ARITHMETIC_OP"); return matched()[0];}

"<"  {store_token_name("LT"); return Parser::LT;}
"<=" {store_token_name("LT_EQ"); return Parser::LT_EQ;}
">"  {store_token_name("GRT"); return Parser::GRT;}
">=" {store_token_name("GRT_EQ"); return Parser::GRT_EQ;}
"==" {store_token_name("EQ"); return Parser::EQ;}
"!=" {store_token_name("NOT_EQ"); return Parser::NOT_EQ;}

"&&" {store_token_name("AND"); return Parser::AND;}
"||" {store_token_name("OR"); return Parser::OR;}
"!"  {store_token_name("NOT"); return Parser::NOT;}

"," {store_token_name("CHAR"); return matched()[0];}

\n    		|
";;".*  	|
[ \t]*";;".*	|
[ \t]*"//".*	|
[ \t]		{
			if (command_options.is_show_tokens_selected())
				ignore_token();
		}

.		{ 
			string error_message;
			error_message =  "Illegal character `" + matched();
			error_message += "' on line " + lineNr();
			
			CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, error_message, lineNr());
		}
		