#include "./help.command.h"
extern "C"{
	HelpCommand *create(void){
		return new HelpCommand();
	}
	void destroy(HelpCommand *ptr){
		delete ptr;
	}
}

HelpCommand::HelpCommand(){
	this->setName("help");
	this->setDescription("Displays the help screen for wfparse");
	this->setManual("Run wfparse with no arguments, invalid arguments, or with the command 'help' to invoke this command.");
}

bool HelpCommand::exec(void){
	printf("---- Help Menu ----\n");
	printf("Usage : ./wfparse [commands] <command options>\n");
	this->printManuals();
	return true;
}
