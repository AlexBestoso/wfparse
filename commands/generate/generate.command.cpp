#include "./generate.command.h"

extern "C"{
	GenerateCommand *create(void){
		return new GenerateCommand();
	}

	void destroy(GenerateCommand *ptr){
		delete ptr;
	}
}

GenerateCommand::GenerateCommand(){
	this->setName("generate");
	this->setDescription("Generate classes and code that enables the graphical object to be used in opengl and webgl.");
	std::string man = "Usage : generate [gentype] [format] [target] <mtl target>\n\n\t";
	man += "gentype: options are, --javascript\n\t";
	man += "format: formats can be any combination of the following chracters:\n\t\t";
	man += "v=vertex, only one\n\t\t";
        man += "t=texture, only one\n\t\t";
        man += "n=normal, only one\n\t\t";
        man += "a=color, sourced from material Na\n\t\t";
        man += "d=color, sourced from material Nd\n\t\t";
        man += "s=color, sourced from material Ns\n\t\t";
        man += "e=color, sourced from material Ne\n\t\t";
        man += "1-4=padding, add x digits of 0 padding. min:1, max:4. More than one supported.\n\t\t";
        man += "D=color, sourced from material Kd\n\t\t";
        man += "default=vtn. an empty format gives vertex, texture, normal\n\t";
	man += "target: path to the object file. If no .obj extension is provided, it will check for both .obj and .mtl in the same location.\n\t";
	man += "mtl target: Path to a .mtl file.\n";
	this->setManual(man);

	this->gentype = "";
	this->format = "";
	this->target = "";
	this->mtlTarget = "";
}

bool GenerateCommand::exec(void){
	int argc = this->getArgc();
	char **argv = this->getArgv();
	if(argc < 5){
		printf("Invalid argument count.\n");
		return false;
	}
	this->gentype = argv[2];
	this->format = argv[3];
	this->target = argv[4];
	std::string grabber = "";
	if(this->target.length() > 4){
		for(int i=this->target.length()-4; i<this->target.length(); i++)
			grabber += target[i];
	}
	if(grabber == ".mtl"){
		this->mtlTarget = this->target;
		this->target = "";
	}else if(grabber == ".obj" && argc >= 6){
		grabber = "";
		this->mtlTarget = argv[5];
		for(int i=this->mtlTarget.length()-4; i<this->mtlTarget.length(); i++)
                        grabber += mtlTarget[i];
		if(grabber != ".mtl"){
			mtlTarget += ".mtl";
		}
	}else if(grabber != ".obj" && argc >= 6){
		this->target += ".obj";
		this->mtlTarget = argv[5];
		grabber = "";
		for(int i=this->mtlTarget.length()-4; i<this->mtlTarget.length(); i++)
                        grabber += mtlTarget[i];
		if(grabber != ".mtl")
			mtlTarget += ".mtl";
	}else if(argc < 6){
		if(grabber != ".obj"){
			this->target += ".obj";
			this->mtlTarget = argv[4];
			if(grabber != ".mtl")
				this->mtlTarget += ".mtl";
		}else{
			this->mtlTarget = "";
		}
	}else{
		printf("input file processing error\n");
		return false;
	}


	return this->process();
}
