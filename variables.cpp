#include <vector>
#include <filesystem>
#include "classes.h"

RPGSH_CHAR c = RPGSH_CHAR();
RPGSH_VAR v = RPGSH_VAR();

bool is_operator(std::string s)
{
	if(s == "="  ||
	   s == "+"  || s == "-"  || s == "*"  || s == "/"  ||
	   s == "+=" || s == "-=" || s == "*=" || s == "/=" ||
	   s == "++" || s == "--")
	{
		return true;
	}
	else{return false;}
}
bool is_int(char* s)
{
	try
	{
		std::stoi(std::string(s));
		return true;
	}
	catch(...)
	{
		return false;
	}
}
void set_var(std::string var, RPGSH_VAR old_value, RPGSH_VAR new_value, bool is_char_var, std::vector<std::string> shell_vars)
{
	std::string var_type;

	if(!is_char_var)
	{
		var_type = "Shell variable";
	}
	else
	{
		var_type = "Character attribute";
	}

	bool old_value_is_digit, new_is_digit;

	try
	{
		int(old_value);
		old_value_is_digit = true;
	}
	catch(...)
	{
		old_value_is_digit = false;
	}

	try
	{
		int(new_value);
		new_is_digit = true;
	}
	catch(...)
	{
		new_is_digit = false;
	}

	if(old_value_is_digit && !new_is_digit)
	{
		RPGSH_OUTPUT(Warning,"%s \"%s\" is changing from an integer to a string.",var_type.c_str(),var.c_str());
	}
	else if(!old_value_is_digit && new_is_digit)
	{
		RPGSH_OUTPUT(Warning,"%s \"%s\" is changing from a string to an integer.",var_type.c_str(),var.c_str());
	}

	if(!is_char_var)
	{
		set_shell_var(var,std::string(new_value));
	}
	else
	{
		c.Attr[var] = new_value;
		c.set_current();
		c.save();
	}
	RPGSH_OUTPUT(Info,"%s \"%s\" has been changed from \"%s\" to \"%s\".",var_type.c_str(),var.c_str(),old_value.c_str(),new_value.c_str());
}

int main(int argc, char** argv)
{
	bool is_char_var = (argv[1][0] == CHAR_VAR);
	std::vector<std::string> shell_vars;
	std::string var = std::string(argv[1]).substr(1,std::string(argv[1]).length()-1);
	RPGSH_VAR old_value;

	if(!is_char_var)
	{
		old_value = get_shell_var(var);
	}
	else
	{
		c.load(false);
		old_value = std::string(c.Attr[var]);
	}

	if(argc == 2)
	{
		fprintf(stdout,"%s\n",old_value.c_str());
	}
	else if(argc == 3)
	{
		if(is_operator(std::string(argv[2])))
		{
			if(std::string(argv[2]) == "++")
			{
				set_var(var,old_value,old_value+1,is_char_var,shell_vars);
				return 0;
			}
			else if(std::string(argv[2]) == "--")
			{
				set_var(var,old_value,old_value-1,is_char_var,shell_vars);
				return 0;
			}
			else
			{
				RPGSH_OUTPUT(Error,"Expected new value after \'%s\'.",argv[argc-1]);
				exit(-1);
			}
		}
		else
		{
			RPGSH_OUTPUT(Error,"Expected operator before \'%s\'.",argv[argc-1]);
			exit(-1);
		}
	}
	else
	{
		RPGSH_VAR new_value = std::string(argv[3]);
		for(int i=4; i<argc; i++)
		{
			if(i%2 == 1 && is_operator(argv[i]))
			{
				RPGSH_OUTPUT(Error,"Expected non-operator new value at \'%s\'.",argv[i]);
				exit(-1);
			}
			else if(i%2 == 0 && !is_operator(argv[i]))
			{
				RPGSH_OUTPUT(Error,"Expected operator at \'%s\'.",argv[i]);
				exit(-1);
			}
			else if(i%2 == 0)
			{
				RPGSH_VAR current_operand = std::string(argv[i+1]);
				if(!strcmp(argv[i],"+"))
				{
					new_value += current_operand;
				}
				else if(!strcmp(argv[i],"-"))
				{
					new_value -= current_operand;
				}
				else if(!strcmp(argv[i],"*"))
				{
					new_value *= current_operand;
				}
				else if(!strcmp(argv[i],"/"))
				{
					new_value /= current_operand;
				}
				else
				{
					RPGSH_OUTPUT(Error,"Invalid operator \'%s\'.",argv[i]);
					exit(-1);
				}
			}
		}

		if(!strcmp(argv[2],"="))
		{
			set_var(var,old_value,new_value,is_char_var,shell_vars);
			return 0;
		}
		else if(!strcmp(argv[2],"+="))
		{
			set_var(var,old_value,std::string(old_value+new_value),is_char_var,shell_vars);
			return 0;
		}
		else if(!strcmp(argv[2],"-="))
		{
			set_var(var,old_value,std::string(old_value-new_value),is_char_var,shell_vars);
			return 0;
		}
		else if(!strcmp(argv[2],"*="))
		{
			set_var(var,old_value,std::string(old_value*new_value),is_char_var,shell_vars);
			return 0;
		}
		else if(!strcmp(argv[2],"/="))
		{
			set_var(var,old_value,std::string(old_value/new_value),is_char_var,shell_vars);
			return 0;
		}
		else
		{
			RPGSH_OUTPUT(Error,"Invalid operator \'%s\'.",argv[2]);
			exit(-1);
		}
	}
	return 0;
}
