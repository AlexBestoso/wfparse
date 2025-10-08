/*
 * This program is used to show case the results of parsing a simple .obj file. 
 * You can compile this by running `g++ main.cc`
 * Feel free to re-use this and the associated files for any purpose you desire…unless you're trying to stop me from using my own code.
 * If that's the case than you're forbidden from using this code.
 *
 * Enforced by the honor system.
 * */
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <dlfcn.h>

#include "./classes/command.class.h"

#include "./WavefrontImport.h"


/*
bool bufGen(int argc, char *argv[]){
	std::string cmd="";
	std::string cmd_value="";

	int switchCtrl = -1;
	for(int i=3; i<argc; i++){
		cmd = argv[i];
		int ctrl = fetchCommand(cmd, &cmd_value);
		if(cmd_value.length() >= 1){
			switch(ctrl){
        			case 10:
					globOutdir = cmd_value;
					break;
        			case 11: 
					globOutFile = cmd_value;
					break;
        			case 12: 
					globCollectionName = cmd_value;
					break;
				case 6:
					globFormat = cmd_value;
					break;
			}
		}else{
			switch(ctrl){
			default:	
				switchCtrl = 5;
				break;
			case 7:
			case 8:
			case 9:
				switchCtrl = ctrl;
				break;
			}
		}
		
	}

	printf("Running context %d.\noutdir = %s\nout file = %s\ncollection name = %s\ndata format = %s\n", switchCtrl, globOutdir.c_str(), globOutFile.c_str(), globCollectionName.c_str(), globFormat.c_str());
	
	std::string build = "";
	std::string classNames = "";
	std::string line = "";
	switch(switchCtrl){
		default:
			printBufGenUsage();
			return false;
		case 7: // --javascript
			truncate();
			for(int i=0; i<objImport.objCount; i++){
				std::string OG_name = objImport.objList[i].object_name;
				for(int j=0; j<objImport.objList[i].object_name.length(); j++){
					if(objImport.objList[i].object_name[j] == '.')
						objImport.objList[i].object_name[j]='_';
				}					
				objImport.genBuffer_format(globFormat.c_str(), i);
				build += "\n/*\n\tBuilt lovingly with wfparse\n *//*\n";
/*				build += "class WF_"+objImport.objList[i].object_name+"{\n";
				build += "\tstride = null;\n";
				build += "\tdata = null;\n";
				build += "\tmaterial = null;\n";
				build += "\tname=null;\n";
				build += "\ttexture=null\n";

				build += "\tconstructor(){\n";
					build += "\t\tthis.name = '"+OG_name+"';\n";
					build += "\t\ttry{\n";
					build += "\t\t\tthis.material=new BtWebglMaterial(\n\t\t\t\t'" +
						objImport.objList[i].material_name + "', " +
			
						std::to_string(objImport.objList[i].material_ns) + ", \n\t\t\t\t[" +

						std::to_string(objImport.objList[i].material_ka[0]) + ", "+
						std::to_string(objImport.objList[i].material_ka[1]) + ", "+
						std::to_string(objImport.objList[i].material_ka[2]) + "], \n\t\t\t\t["+

						std::to_string(objImport.objList[i].material_kd[0]) + ", "+
						std::to_string(objImport.objList[i].material_kd[1]) + ", "+
						std::to_string(objImport.objList[i].material_kd[2]) + "], \n\t\t\t\t["+

						std::to_string(objImport.objList[i].material_ks[0]) + ", " +
						std::to_string(objImport.objList[i].material_ks[1]) + ", " +
						std::to_string(objImport.objList[i].material_ks[2]) + "], \n\t\t\t\t[" +
							
						std::to_string(objImport.objList[i].material_ke[0]) + ", " +
						std::to_string(objImport.objList[i].material_ke[1]) + ", " +
						std::to_string(objImport.objList[i].material_ke[2]) + "], \n\t\t\t\t" +

						std::to_string(objImport.objList[i].material_ni) + ", \n\t\t\t\t"+
						std::to_string(objImport.objList[i].material_d) + ", \n\t\t\t\t" +
						std::to_string(objImport.objList[i].material_illum) + ", \n\t\t\t\t";
					if(!transferTexture(objImport.objList[i].material_map_kd)){
	                        		build += "'"+objImport.objList[i].material_map_kd+"');\n";
					}else{
						std::string _dir = globOutdir;
        					if(globOutdir == "") _dir = "./";
	
        					std::string path = _dir;
        					if(path[path.length()-1] != '/') path += '/';
        					path += objImport.objList[i].material_map_kd;
	                        		build += "'"+path+"');\n";
					}

					build += "\t\t}catch(e){\n\t\t\tconsole.log(e+' : Try adding \"defer\" to your script tag for this file if the material class has been defined.');\n\t\t}\n";

					build += "\t\tthis.stride = " + std::to_string(objImport.objList[i].gl_stride)+";\n";
					build += "\t\tthis.data = [\n";
					for(int j=0; j<objImport.objList[i].gl_buffer_size; j++){
						if((j%objImport.objList[i].gl_stride) == 0) build += "\t\t\t[";
				
						build += std::to_string(objImport.objList[i].gl_buffer[j]);
						build += ",";
	
						if((j%objImport.objList[i].gl_stride) == objImport.objList[i].gl_stride-1) build += "],\n";
					}
					build += "\t\t];\n";
				build += "\t}\n";

				build += "}\n";
				build += "let wf_"+objImport.objList[i].object_name + " = new "+"WF_"+objImport.objList[i].object_name+"();";
				classNames += "wf_"+objImport.objList[i].object_name+"\n";
				outputBuffer(build);
				build = "";
			}

			build = "\nlet "+globCollectionName+" = [";
			line = "";
			for(int i=0; i<classNames.length(); i++){
				if(classNames[i] == '\n'){
					build += line + ", ";
					line = "";
					continue;
				}
				line += classNames[i];
			}
			build += "];\n";

			/* check if there's an -o arg and output location, 
			 * move*//*
			outputBuffer(build);
			break;
		case 8: // --c++
			printf("C++ class generation not yet supported\n");
			return false;
			for(int i=0; i<objImport.objCount; i++){
				for(int j=0; j<objImport.objList[i].object_name.length(); j++){
                                        if(objImport.objList[i].object_name[j] == '.')
                                                objImport.objList[i].object_name[j]='_';
                                }
                                objImport.genBuffer_format(globFormat.c_str(), i);

				objImport.genBuffer_format(globFormat.c_str(), 0);
	                        printf("\nsize_t WF_OBJECT_SIZE=%ld;\nfloat WF_OBJECT[%ld] = {", objImport.gl_buffer_size, objImport.gl_buffer_size);
	                        for(int i=0; i<objImport.gl_buffer_size; i++){
	                                if((i%objImport.gl_stride) == 0) printf("\n\t");

	                                if(i == objImport.gl_buffer_size-1)
		                                printf("%f", objImport.gl_buffer[i]);
					else
		                                printf("%f, ", objImport.gl_buffer[i]);
	
                        	}
                        	printf("\n};\n");
			}
			break;
		case 9: //--raw-grep
			printf("grepable data generation not supported");
			objImport.genBuffer_format(globFormat.c_str(), 0);
			printf("Raw Buffer : \n");
			for(int i=0; i<objImport.gl_buffer_size; i++){
				if((i%objImport.gl_stride) == 0) printf("\n");
				printf("%f ", objImport.gl_buffer[i]);
			}
			printf("\n\n");

			break;

	}
	return true;
}*/

WavefrontImport objImport;

struct UserCommandStruct{
	int code;
	bool hasInput;
	std::string name;
	std::string value;
	std::string description;
};
typedef struct UserCommandStruct usr_cmd_t;

usr_cmd_t buildCommand(int code, bool hasInput, std::string name, std::string value, std::string description){
	usr_cmd_t ret;
	ret.code = code;
        ret.hasInput = hasInput;
        ret.name = name;
        ret.value = value;
        ret.description = description;
	return ret;
}

#define USER_CMD_CNT 9

#define CMD_CODE_HELP 0
#define CMD_CODE_DUMP 1
#define CMD_CODE_DUMPOBJ 2
#define CMD_CODE_DUMPMTL 3
#define CMD_CODE_MTLFILE 4
#define CMD_CODE_OBJFILE 5
#define CMD_CODE_GENBUF 6
#define CMD_CODE_JAVASCRIPT 7
#define CMD_CODE_RAW 8

class UserInput{
	private:

	bool testCommand(std::string cmdName){
		for(int i=0; i<USER_CMD_CNT; i++){
			if(cmdName == this->commandList[i].name)
				return true;
		}
		return false;
	}
	int getCommandIndex(std::string cmdName){
		for(int i=0; i<USER_CMD_CNT; i++){
			if(cmdName == this->commandList[i].name)
				return i;
		}
		return -1;
	}
	public:
	size_t cmdCount;
	usr_cmd_t *activeCommands;

	
	usr_cmd_t commandList[USER_CMD_CNT] = {
		buildCommand(CMD_CODE_HELP, false, "--help", "", "Display command help."),
		buildCommand(CMD_CODE_DUMP, true, "--dump", "", "Enumerate contents of a .obj file and it's associated .mtl file if it's available. Input: .obj file."),
		buildCommand(CMD_CODE_DUMPOBJ, true, "--dump-obj", "", "Enumerate only the .obj file. Input: .obj file."),
		buildCommand(CMD_CODE_DUMPMTL, true, "--dump-mtl", "", "Enumerate only the .mtl file. Input: .obj file."),
		buildCommand(CMD_CODE_MTLFILE, true, "--mtl", "", "Set an explicit .mtl file to load instead of guessing the name. Input: .mtl file"),
		buildCommand(CMD_CODE_OBJFILE, true, "--obj", "", "Set an explicit .obj file to load instead of using command input. Input: .obj file"),
		buildCommand(CMD_CODE_GENBUF, true, "--gen-buf", "", "Generate a useable opengl buffer. Input: gl buffer format [vnt], Requires --obj, optional --mtl."),
		buildCommand(CMD_CODE_JAVASCRIPT, false, "--javascript", "", "Output the generated data as a javascript class"),
		buildCommand(CMD_CODE_RAW, false, "--raw", "", "Output the generated data as a formatted newline seperated list.")
	};

	void parseCommands(int argc, char *argv[]){
		this->cmdCount = 0;
		for(int i=0; i<argc; i++){
			if(this->testCommand(argv[i])){
				cmdCount++;
			}
		}
		this->activeCommands = new usr_cmd_t[this->cmdCount];

		int activeIdx=0;
		for(int i=0; i<argc; i++){
			int idx = this->getCommandIndex(argv[i]);
			if(idx != -1){
				usr_cmd_t cmd = this->commandList[idx];
				if(cmd.hasInput && i+1 >= argc){
					cmd.value = "";
					i++;
                                        this->activeCommands[activeIdx] = cmd;
                                        activeIdx++;
				}else if(cmd.hasInput){
					cmd.value = argv[i+1];
					i++;
					this->activeCommands[activeIdx] = cmd;
					activeIdx++;
				}else{
					cmd.value = "";
					i++;
                                        this->activeCommands[activeIdx] = cmd;
                                        activeIdx++;
				}
			}
			
			
		}
	}
	
	int getMainCommand(int *outIdx){
		int ret=0; // defaults to help code
		for(int i=0; i<this->cmdCount; i++){
			usr_cmd_t cmd = this->activeCommands[i];
			if(cmd.name == "--help"){
				outIdx[0] = i;
				return CMD_CODE_HELP;
			}
			if(cmd.name == "--dump"){
				outIdx[0] = i;
				return CMD_CODE_DUMP;
			}
			if(cmd.name == "--dump-obj"){
				outIdx[0] = i;
				return CMD_CODE_DUMPOBJ;
			}
			if(cmd.name == "--dump-mtl"){
				outIdx[0] = i;
				return CMD_CODE_DUMPMTL;
			}
			if(cmd.name == "--gen-buf"){
				outIdx[0] = i;
				return CMD_CODE_GENBUF;
			}
		}
		return 0;
	}

	std::string getObjTarget(void){
		for(int i=0; i<this->cmdCount; i++){
			usr_cmd_t cmd = this->activeCommands[i];
			if(cmd.name == "--obj")
				return cmd.value;
		}
		return "";
	}

	std::string getMtlTarget(void){
                for(int i=0; i<this->cmdCount; i++){
                        usr_cmd_t cmd = this->activeCommands[i];
                        if(cmd.name == "--mtl")
                                return cmd.value;
                }
                return "";
        }
	int getBufGenMode(void){
		for(int i=0; i<this->cmdCount; i++){
			usr_cmd_t cmd = this->activeCommands[i];
			if(cmd.name == "--javascript"){
				return CMD_CODE_JAVASCRIPT;
			}
			if(cmd.name == "--raw"){
				return CMD_CODE_RAW;
			}
		}
		return CMD_CODE_RAW;
	}
	
}inputHandler;


void generateBuffer(std::string bufferFormat){
	objImport.compileBuffer(bufferFormat);	
	switch(inputHandler.getBufGenMode()){
		case CMD_CODE_JAVASCRIPT:{
		printf("Running gen buffers.\n");
			std::string jsout="";
                        std::string makersMark = "/*\n * Made lovingly using wfparse\n * */\n";
			WavefrontObjectCollection collection = objImport.objectCollection;
                        std::string collectionName = collection.data.name;
			for(int i=0; i<collectionName.length(); i++){
				if(collectionName[i] == '.') collectionName[i] = '_';
			}
                        std::string collectionStride = std::to_string(objImport.vboStride);

			std::string objectNames = "";
                        for(int j=0; j<collection.data.objectCount; j++){
				wf_object_t object = collection.data.objects[j];
				if(j == 0) jsout = makersMark;
				else jsout = "\n"+makersMark;
				
				std::string objName = "";
				for(int k=0; k<object.name.length(); k++){
					if(object.name[k] == '.'){
						objName += '_';
					}else{
						objName += object.name[k];
					}
				}				
				objectNames += "wf_obj_"+objName+"\n";

				std::string materialName = object.material_name;
				wf_material_t material = collection.material.getMaterialByName(materialName);

				jsout += "class WF_OBJ_"+objName+"{\n";
				jsout += "\tstride=null;\n";
				jsout += "\tdata=null;\n";
				jsout += "\tmaterial=null;\n";
				jsout += "\tname=null;\n";
				jsout += "\ttexture=null;\n";
				jsout += "\tconstructor(){\n";
				jsout += "\t\tthis.name = '"+objName+"';\n";
				jsout += "\t\ttry{\n";
				jsout += "\t\t\tthis.material = new BtWebglMaterial(\n";
				jsout += "\t\t\t\t'"+material.material_name+"',\n";
				
				jsout += "\t\t\t\t"+std::to_string(material.material_ns)+",\n";

				jsout += "\t\t\t\t["+std::to_string(material.material_ka[0])+", ";
				jsout += "\t"+std::to_string(material.material_ka[1])+", ";
				jsout += "\t"+std::to_string(material.material_ka[2])+"],\n";

				jsout += "\t\t\t\t["+std::to_string(material.material_kd[0])+", ";
				jsout += "\t"+std::to_string(material.material_kd[1])+", ";
				jsout += "\t"+std::to_string(material.material_kd[2])+"],\n";

				jsout += "\t\t\t\t["+std::to_string(material.material_ks[0])+", ";
				jsout += "\t"+std::to_string(material.material_ks[1])+", ";
				jsout += "\t"+std::to_string(material.material_ks[2])+"],\n";

				jsout += "\t\t\t\t["+std::to_string(material.material_ke[0])+", ";
				jsout += "\t"+std::to_string(material.material_ke[1])+", ";
				jsout += "\t"+std::to_string(material.material_ke[2])+"],\n";

				jsout += "\t\t\t\t"+std::to_string(material.material_ni)+",\n";
				jsout += "\t\t\t\t"+std::to_string(material.material_d)+",\n";
				jsout += "\t\t\t\t"+std::to_string(material.material_illum)+",\n";
				jsout += "\t\t\t\t"+material.material_map_kd+"\n";

				jsout += "\t\t\t);\n";
				jsout += "\t\t}catch(e){\n";
				jsout += "\t\t\tconsole.log(e+' : Try adding \"defer\" to your script tag for this file if the material class has been defined.');\n";
				jsout += "\t\t}\n";
				jsout += "\t\tthis.stride = "+collectionStride+";\n";
				jsout += "\t\tthis.data = [\n";
				for(int i=0; i<objImport.vboSize; i++){
					if((i%objImport.vboStride) == 0){
						jsout += "\t\t\t[";
					}
					jsout += std::to_string(objImport.vboData[i]) + ", ";
					if((i%objImport.vboStride) == objImport.vboStride-1){
						jsout += "],\n";
					}
				}
				jsout += "\t\t];\n";
				jsout += "\t}\n";
				jsout += "}\n";
				jsout += "let wf_obj_"+objName+" = new WF_OBJ_"+objName+"();\n";
				printf("%s\n", jsout.c_str());
				
                        }
			std::string line="";
			jsout = "\nlet "+collectionName+" = [";
                        for(int i=0; i<objectNames.length(); i++){
                        	if(objectNames[i] == '\n'){
                                	jsout += line + ", ";
                                        line = "";
                                        continue;
                                }
                                line += objectNames[i];
                       	}
                        jsout += "];\n";
			printf("%s\n", jsout.c_str());

	/*			objImport.genBuffer_format(globFormat.c_str(), i);
						build += "\n/*\n\tBuilt lovingly with wfparse\n *//*\n";
						build += "class WF_"+objImport.objList[i].object_name+"{\n";
						build += "\tstride = null;\n";
						build += "\tdata = null;\n";
						build += "\tmaterial = null;\n";
						build += "\tname=null;\n";
						build += "\ttexture=null\n";

						build += "\tconstructor(){\n";
						build += "\t\tthis.name = '"+OG_name+"';\n";
						build += "\t\ttry{\n";
						build += "\t\t\tthis.material=new BtWebglMaterial(\n\t\t\t\t'" +
							objImport.objList[i].material_name + "', " +
			
							std::to_string(objImport.objList[i].material_ns) + ", \n\t\t\t\t[" +

							std::to_string(objImport.objList[i].material_ka[0]) + ", "+
							std::to_string(objImport.objList[i].material_ka[1]) + ", "+
							std::to_string(objImport.objList[i].material_ka[2]) + "], \n\t\t\t\t["+

							std::to_string(objImport.objList[i].material_kd[0]) + ", "+
							std::to_string(objImport.objList[i].material_kd[1]) + ", "+
							std::to_string(objImport.objList[i].material_kd[2]) + "], \n\t\t\t\t["+

							std::to_string(objImport.objList[i].material_ks[0]) + ", " +
							std::to_string(objImport.objList[i].material_ks[1]) + ", " +
							std::to_string(objImport.objList[i].material_ks[2]) + "], \n\t\t\t\t[" +
								
							std::to_string(objImport.objList[i].material_ke[0]) + ", " +
							std::to_string(objImport.objList[i].material_ke[1]) + ", " +
							std::to_string(objImport.objList[i].material_ke[2]) + "], \n\t\t\t\t" +

							std::to_string(objImport.objList[i].material_ni) + ", \n\t\t\t\t"+
							std::to_string(objImport.objList[i].material_d) + ", \n\t\t\t\t" +
							std::to_string(objImport.objList[i].material_illum) + ", \n\t\t\t\t";
						if(!transferTexture(objImport.objList[i].material_map_kd)){
	                        			build += "'"+objImport.objList[i].material_map_kd+"');\n";
						}else{
							std::string _dir = globOutdir;
        						if(globOutdir == "") _dir = "./";
	
        						std::string path = _dir;
        						if(path[path.length()-1] != '/') path += '/';
        						path += objImport.objList[i].material_map_kd;
	                        			build += "'"+path+"');\n";
						}

						build += "\t\t}catch(e){\n\t\t\tconsole.log(e+' : Try adding \"defer\" to your script tag for this file if the material class has been defined.');\n\t\t}\n";

						build += "\t\tthis.stride = " + std::to_string(objImport.objList[i].gl_stride)+";\n";
						build += "\t\tthis.data = [\n";
						for(int j=0; j<objImport.objList[i].gl_buffer_size; j++){
							if((j%objImport.objList[i].gl_stride) == 0) build += "\t\t\t[";
					
							build += std::to_string(objImport.objList[i].gl_buffer[j]);
							build += ",";
	
							if((j%objImport.objList[i].gl_stride) == objImport.objList[i].gl_stride-1) build += "],\n";
						}
						build += "\t\t];\n";
						build += "\t}\n";
	
						build += "}\n";
						build += "let wf_"+objImport.objList[i].object_name + " = new "+"WF_"+objImport.objList[i].object_name+"();";
						classNames += "wf_"+objImport.objList[i].object_name+"\n";
						outputBuffer(build);
						build = "";
					}

					build = "\nlet "+globCollectionName+" = [";
					line = "";
					for(int i=0; i<classNames.length(); i++){
						if(classNames[i] == '\n'){
							build += line + ", ";
							line = "";
							continue;
						}
						line += classNames[i];
					}
					build += "];\n"; */

			break;
		}
		case CMD_CODE_RAW:{
			break;
		}
	}
}

/*
 * .so command functions
 * */
unsigned int soCount=0;
std::string sharedObjectNames="";

unsigned int countSharedObjects(std::string *outNames){
	unsigned int ret = 0;
	struct dirent *de;
	DIR *dr = opendir("./bin");
	if(dr == NULL){
		throw "No shared objects detected, program cannot function!\n";
	}
	while((de = readdir(dr)) != NULL){
		std::string name = de->d_name;
		if(name.length() > 3){
			ret++;
			outNames[0] += name;
			outNames[0] += "\n";
		}
	}
	closedir(dr);
	return ret;
}

bool validCommand(std::string name, std::string cache){
	int ret = -1;
	name += ".so";
	std::string grabber = "";
	for(int i=0; i<cache.length(); i++){
		if(cache[i] == '\n'){
			ret++;
			if(grabber == name){
				return true;
			}
			grabber = "";
		}else{
			grabber += cache[i];
		}
	}
	return false;
}

void *loadCommandHandle(std::string name){
	std::string command = "./bin/";
	command += name;
	command += ".so";
	void *ret = dlopen(command.c_str(), RTLD_LAZY);
	if(!ret) return NULL;
	return ret;
}

command_t createInvoker(void *handle){
	command_t ret = reinterpret_cast<command_t>(dlsym(handle, "create"));
	dlerror();
	return ret;
}

int main(int argc, char *argv[]){
	soCount = countSharedObjects(&sharedObjectNames);
	void *soHandle;
	if(argc <= 1){
 		soHandle = loadCommandHandle("help");
		if(soHandle == NULL){
			printf("Failed to find help command\n");
			exit(EXIT_FAILURE);
		}
		command_t invoker = createInvoker(soHandle);
		Command *help = invoker();
		help->exec();
		dlclose(soHandle);
		exit(EXIT_FAILURE);
	}

	soHandle = loadCommandHandle(argv[1]);
	if(soHandle == NULL){
		printf("invalid command.\n");
		soHandle = loadCommandHandle("help");
                if(soHandle == NULL){
                        printf("Failed to find help command\n");
                        exit(EXIT_FAILURE);
                }
                command_t invoker = createInvoker(soHandle);
                Command *help = invoker();
                help->exec();
                dlclose(soHandle);
                exit(EXIT_FAILURE);
	}
	command_t invoker = createInvoker(soHandle);
        Command *cmd = invoker();
	cmd->setArgc(argc);
	cmd->setArgv(argv);
        cmd->exec();
        dlclose(soHandle);
	
	exit(EXIT_SUCCESS);
	inputHandler.parseCommands(argc, argv);
	std::string objTarget = inputHandler.getObjTarget();
	std::string mtlTarget = inputHandler.getMtlTarget();
	int activeIdx=0;
	switch(inputHandler.getMainCommand(&activeIdx)){
		case CMD_CODE_HELP:{
			printf("====Help Menu====\n");
			for(int i=0; i<USER_CMD_CNT; i++){
				usr_cmd_t cmd = inputHandler.commandList[i];
				printf("%s | requires input '%s' | %s\n", cmd.name.c_str(), cmd.hasInput ? "true" : "false", cmd.description.c_str());

			}
			break;
		}
		case CMD_CODE_GENBUF:{
			usr_cmd_t cmd = inputHandler.activeCommands[activeIdx];
			if(cmd.value == "")
				cmd.value = "vnt";
			if(objTarget == ""){
				printf("--gen-buf : No --obj argument provided");
				exit(EXIT_FAILURE);
			}
			if(mtlTarget != ""){
				if(!objImport.import(objTarget, mtlTarget)){
                                        printf("Failed to import object\n");
                                        exit(EXIT_FAILURE);
                                }
			}else{
				if(!objImport.import(objTarget)){
                                        printf("Failed to import object\n");
                                        exit(EXIT_FAILURE);
                                }
			}
			
			generateBuffer(cmd.value);
			break;
		}
		case CMD_CODE_DUMP:{
			usr_cmd_t cmd = inputHandler.activeCommands[activeIdx];
			if(objTarget != "") cmd.value = objTarget;
			if(cmd.value==""){
				printf("--dump : No .obj file provided '%s'\n", cmd.value.c_str());
				exit(EXIT_FAILURE);
			}
			if(mtlTarget != ""){
				if(!objImport.import(cmd.value, mtlTarget)){
					printf("Failed to import object\n");
					exit(EXIT_FAILURE);
				}
			}else{
				if(!objImport.import(cmd.value)){
                                        printf("Failed to import object\n");
                                        exit(EXIT_FAILURE);
                                }
			}

			objImport.dumpObjFile();
			objImport.dumpMtlFile();

			
			break;
		}
		case CMD_CODE_DUMPOBJ:{
			usr_cmd_t cmd = inputHandler.activeCommands[activeIdx];
			if(objTarget != "") cmd.value = objTarget;
			if(cmd.value==""){
				printf("--dump-obj : No .obj file provided '%s'\n", cmd.value.c_str());
				exit(EXIT_FAILURE);
			}

			if(mtlTarget != ""){
                                if(!objImport.import(cmd.value, mtlTarget)){
                                        printf("Failed to import object\n");
                                        exit(EXIT_FAILURE);
                                }
                        }else{  
                                if(!objImport.import(cmd.value)){
                                        printf("Failed to import object\n");
                                        exit(EXIT_FAILURE);
                                }
                        }

                        objImport.dumpObjFile();

			break;
		}
		case CMD_CODE_DUMPMTL:{
			usr_cmd_t cmd = inputHandler.activeCommands[activeIdx];
			if(mtlTarget != "") cmd.value = mtlTarget;
			if(cmd.value==""){
				printf("--dump-mtl : No .obj file provided '%s'\n", cmd.value.c_str());
				exit(EXIT_FAILURE);
			}

			if(mtlTarget != ""){
                                if(!objImport.import(cmd.value, mtlTarget)){
                                        printf("Failed to import object\n");
                                        exit(EXIT_FAILURE);
                                }
                        }else{  
                                if(!objImport.import(cmd.value)){
                                        printf("Failed to import object\n");
                                        exit(EXIT_FAILURE);
                                }
                        }

                        objImport.dumpMtlFile();

			break;
		}	
	}
	exit(EXIT_SUCCESS);
}
