#include "./dump.command.h"

extern "C"{
	DumpCommand *create(void){
		return new DumpCommand();
	}
	
	void destroy(DumpCommand *ptr){
		delete ptr;
	}
}

DumpCommand::DumpCommand(){
	this->filename = "";
	this->arg = "";
	this->setName("dump");
	this->setDescription("Enumerate .obj and .mtl file data and statistics.");
	std::string manualText = "Usage: dump [filename] <arguments>\n\t";
	manualText += "The filename argument doesn't need to have a file extension. This system will append it for you.\n\tThough the file on the actual file system shall have the extension.\n\n\t";
	manualText += "Valid arguments are:\n\t";
	manualText += "(NO ARGUMENT) : this will attempt to enumerate both .mtl and .obj file\n\t";
	manualText += "--obj : This will enumerate ONLY the .obj file.\n\t";
	manualText += "--mtl : this will enumerate ONLY the .mtl file.\n\t";
	manualText += "--stat : this will enumerate statistical data for both .mtl and .obj files.\n\t";
	manualText += "--stat-obj : this will enumerate statistical data for ONLY the .obj file.\n\t";
	manualText += "--stat-mtl : this will enumerate statistical data for ONLY the .mtl file.\n\t";
	this->setManual(manualText);
}

bool DumpCommand::exec(void){
	int argc = this->getArgc();
	if(argc <= 2){
		printf("No filename provided.\n");
		return false;
	}
	char **argv = this->getArgv();
	this->filename = argv[2];
	
	if(argc > 3){
		this->arg = argv[3];
	}else{
		this->arg = "";
	}
	
	if(this->arg == ""){
		this->fullDump();
	}else if(this->arg == "--obj"){
		this->objDump();
	}else if(this->arg == "--mtl"){
		this->mtlDump();
	}else if(this->arg == "--stat"){
		this->fullStatDump();
	}else if(this->arg == "--stat-obj"){
		this->objStatDump();
	}else if(this->arg == "--stat-mtl"){
		this->mtlStatDump();
	}else{
		printf("Invalid argument '%s'\n", this->arg.c_str());
		return false;
	}
	return true;
}
