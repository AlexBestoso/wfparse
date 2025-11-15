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
	std::string man = "Usage : generate [gentype] [format] [target] <options>\n\n\t";
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
	man += "mtl target: Path to a .mtl file.\n\t";
	man += "-o : output directory, tells the program to output the generated file to a specific location\n\t";
	man += "-tt : Transfer Texture, defines a new location for all detected material textures, copys the images to the location, and updates the map_kd variable.\n\t";
	man += "-ml : Map Location, defines a custom location for map_kd materials. This overrides the location created by -tt and can be used without the transfer command\n\t.";
	man += "\n";
	
	this->setManual(man);

	this->gentype = "";
	this->format = "";
	this->target = "";
	this->mtlTarget = "";
	this->textureOutputDir  = "";
	this->outputDir = "";
	this->mapLocOverride = "";
}

bool GenerateCommand::exec(void){
	int argc = this->getArgc();
	char **argv = this->getArgv();
	int argIndex=0;
	if(argc < 5){
		printf("Invalid argument count.\n");
		return false;
	}
	this->gentype = argv[2];
	this->format = argv[3];
	this->target = argv[4];
	argIndex = 5;
	std::string grabber = "";
	this->mtlTarget = target + ".mtl";
	this->target += ".obj";

	int ctx=0;
	for(int i=argIndex; i<argc; i++){
		switch(ctx){
			case 1:
				this->outputDir = argv[i];
				ctx = 0;
				continue;
			case 2:
				this->textureOutputDir = argv[i];
				ctx = 0;
				continue;
			case 3:
				this->mapLocOverride = argv[i];
				ctx = 0;
				continue;
			default:
				grabber = argv[i];
				break;
		}
		if(grabber == "-o"){
			ctx=1;
		}else if(grabber == "-tt"){
			ctx=2;
		}else if(grabber == "-ml"){
			ctx=3;
		}else{
			ctx=0;
		}
	}

	return this->process();
}
