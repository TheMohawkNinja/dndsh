#include <stdio.h>
#include <string.h>
#include <spawn.h>
#include <sys/wait.h>
#include <string>
#include "text.h"
#include "classes.h"

#define MAX_BUFFER_SIZE 256

void run_dndsh_prog(DNDSH_CHAR c, std::string args)
{
	extern char** environ;
	pid_t pid;

	//Get number of args, and largest arg for efficient memory allocation.
	int args_ctr = 1;
	int arg_size = 0;
	int largest_arg_size = 0;
	std::string args_temp;
	for(int i=0; i<args.length(); i++)
	{
		if(args.substr(i,1) == " ")
		{
			args_ctr++;
		}
		if(args.substr(i,1) == " " || args.substr(i,1) == "\0")
		{
			if(largest_arg_size < arg_size)
			{
				largest_arg_size = arg_size;
			}
			arg_size = 0;
		}
		else
		{
			arg_size++;
		}
	}

	//Set parse parameters and set char[][] for args.
	int argv_length = (args_ctr+2);
	char* argv[argv_length - 1];
	std::string program;
	std::string prefix = "dndsh-";
	std::string path = "./";
	std::string full_path;
	std::string full_prog_args[args_ctr];
	int args_index = 0;
	int arg_index = 0;
	for(int i=0; i<=args.length(); i++)
	{
		if(args.substr(i,1) == " " || args.substr(i,1) == "\0")
		{
			full_prog_args[args_index] = (args.substr(arg_index,(i-arg_index)).data());

			if(args_index == 0)
			{
				//Defines naming convention for all sub-programs of dndsh to be prefixed with "dndsh-"
				program = full_prog_args[args_index];
				full_path = path + prefix + program;
				argv[0] = (char*)full_path.c_str();
				argv[1] = (char*)std::string(c.Attr["Name"]).c_str();
			}

			args_index++;
			arg_index = i+1;
		}
	}

	argv[1] = (char*)std::string(c.Attr["Name"]).c_str();
	for(int i=0; i<(args_ctr-1); i++)
	{
		argv[i+2] = const_cast<char*>(full_prog_args[i+1].c_str());
	}
	argv[argv_length - 1] = NULL;//NULL needs to be here, otherwise a 14: Bad address is thrown

	fprintf(stdout,"\n");

	int status = posix_spawn(&pid, argv[0], NULL, NULL, (char* const*)argv, environ);

	if(status == 0)
	{
		do
		{
			if(waitpid(pid, &status, 0) == -1)
			{
				exit(1);
			}
		}while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	else
	{
		if(status == 2)//File not found
		{
			DNDSH_OUTPUT(Error,"(%d): \"%s\" is not a valid dndsh command.",status,program.c_str());
		}
		else
		{
			DNDSH_OUTPUT(Error,"(%d): %s",status,strerror(status));
		}
	}

	fprintf(stdout,"\n");
}

int prompt(DNDSH_CHAR c)
{
	fprintf(stdout,"%s┌─%s[%s%s%s%s%s%s%s]%s─%s(%s%hhu/%hhu%s %s(%hhu)%s%s%s)%s%s\n",TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_ITALIC,TEXT_RED,std::string(c.Attr["Name"]).c_str(),TEXT_NOITALIC,TEXT_WHITE,TEXT_BOLD,TEXT_NOBOLD,TEXT_BOLD,TEXT_GREEN,int(c.Attr["HP"]),int(c.Attr["MaxHP"]),TEXT_NOBOLD,TEXT_ITALIC,int(c.Attr["TempHP"]),TEXT_NOITALIC,TEXT_BOLD,TEXT_WHITE,TEXT_NOBOLD,TEXT_NORMAL);
	fprintf(stdout,"%s└─%sĐ₦Đ%s─%s$%s ",TEXT_WHITE,TEXT_CYAN,TEXT_WHITE,TEXT_CYAN,TEXT_NORMAL);

	char buffer[MAX_BUFFER_SIZE];
	fgets(buffer,sizeof(buffer),stdin);
	buffer[strcspn(buffer,"\n")] = 0; //Omits newline character from input buffer (https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input)

	if(!strcmp(buffer,"exit"))
	{
		fprintf(stdout,"Exiting...\n");
		return -1;
	}
	else
	{
		if(buffer[0] == '%')//Check if user is operating on a variable
		{
			std::string prefix = "variables ";
			prefix += buffer;
			strcpy(buffer,prefix.c_str());
			fprintf(stdout,"buffer = \"%s\"\n",buffer);
		}
		run_dndsh_prog(c,buffer);
		return 0;
	}

	return 0;
}
int main()
{
	DNDSH_CHAR character = DNDSH_CHAR();

	run_dndsh_prog(character,(char*)"banner");
	run_dndsh_prog(character,(char*)"version");

	DNDSH_OUTPUT(Info,"This is info text");
	DNDSH_OUTPUT(Warning,"This is warning text");
	DNDSH_OUTPUT(Error,"This is error text");

	while(prompt(character) >= 0){}

	return 0;
}
