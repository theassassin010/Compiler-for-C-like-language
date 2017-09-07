%filenames="scanner"
%lex-source="scanner.cc"

%%

"void"	{store_token_name("VOID"); return Parser::VOID;}
"int" {return Parser::INTEGER;}
"float" {return Parser::FLOAT;}

"=" {return Parser::ASSIGN;}

"{" {return matched()[0];}
"}"	{return matched()[0];}
";"	{return matched()[0];}
"("	{return matched()[0];}
")"	{return matched()[0];}

[[:digit:]]+ {
````` ParserBase::STYPE__ *val = getSval();
      val->integer_value = atoi(matched().c_str());
	  return Parser::INTEGER_NUMBER;
	}

[[:alpha:]_][[:alpha:][:digit:]_]* {
	  ParserBase::STYPE__ *val = getSval();	
	  val->string_value = new std::string(matched());
	  return Parser::NAME; 
	}

(([:digit:]*\.[:digit:]+|[:digit:]+\.[:digit:]*)([eE][-+]?[0-9]+)?)|([:digit:]+[eE][-+]?[0-9]+){
	  ParserBase::STYPE__ *val = getSval();	
	  val->float_value = atof(matched().c_str());
	  return Parser::DOUBLE_NUMBER; 
	}

"+" {return matched()[0];}
"-" {return matched()[0];}
"*" {return matched()[0];}
"/" {return matched()[0];}

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