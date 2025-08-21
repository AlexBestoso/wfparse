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
#include <unistd.h>
#include <string.h>
#include <string>

#include "./WavefrontImport.h"

WavefrontImport objImport;

std::string globFormat="vtn";
std::string globOutdir = "";
std::string globOutFile = "";
std::string globCollectionName="WF_COLLECTION_SED";

size_t commandCount = 13;
std::string commands[13] = {
	"--help",
	"--full-dump",
	"--obj-dump",
	"--mtl-dump",
	"--buf-gen",
	"--buf-genhelp",
	"--format=",
	"--javascript",
	"--c++",
	"--raw-grep",
	"--out-dir=",
	"--out-file=",
	"--collection-name="
};

int fetchCommand(std::string cmd, std::string *out){
	bool hasInput = false;
	int inputIndex=0;
	if(cmd.length() <= 2) return -1;
	if(cmd[0] != '-' && cmd[1] != '-') return -1;
	for(int i=2; i<cmd.length(); i++){
		if(cmd[i] == '='){
			hasInput = true;
			inputIndex=i+1;
			break;
		}
	}
	
	if(hasInput){
		if(out == NULL) return -1;
		if(inputIndex >= cmd.length()) return false;
		std::string _cmd = "";
		std::string _val = "";
		for(int i=0; i<inputIndex-1; i++){
			_cmd += cmd[i];
		}
		for(int i=inputIndex; i<cmd.length(); i++){
			_val += cmd[i];
		}
		for(int i=0; i<commandCount; i++){
			if(!strncmp(commands[i].c_str(), _cmd.c_str(), _cmd.length())){
				out[0] = _val;
				return i;
			}
		}
		return -1;
	}

	for(int i=0; i<commandCount; i++){
        	if(!strncmp(commands[i].c_str(), cmd.c_str(), cmd.length())){
			if(out != NULL) out[0] = "";
                	return i;
                }
        }
	return -1;
}

size_t main_command_count = 6;
std::string main_command_list[] = {
	"--help",
	"--full-dump",
	"--obj-dump",
	"--mtl-dump",
	"--buf-gen",
	"--buf-genhelp"
};
std::string main_command_help[] = {
	"Display this help menu.",
	"Dump all .obj and .mtl data.",
	"Dump ONLY .obj data.",
	"Dump ONLY .mtl data.",
	"Generate a data buffer.",
	"Displays the help menu for --buf-gen"
};

int parseMainCommands(const char *cmd){
	for(int i=0; i<main_command_count; i++){
		if(!strncmp(cmd, main_command_list[i].c_str(), main_command_list[i].length())) return i;
	}
	return -1;
}

void printUsage(){
	printf("Usage %s <file.obj> [options]\n", "wfparse");
        printf("Usage --help\n");
	printf("===== manual =====\n");
	printf("%s : %s\n", commands[0].c_str(), main_command_help[0].c_str());
	printf("%s : %s\n", commands[1].c_str(), main_command_help[1].c_str());
        printf("%s : %s\n", commands[2].c_str(), main_command_help[2].c_str());
        printf("%s : %s\n", commands[3].c_str(), main_command_help[3].c_str());
        printf("%s : %s\n", commands[4].c_str(), main_command_help[4].c_str());
        printf("%s : %s\n", commands[5].c_str(), main_command_help[5].c_str());

	for(int i=0; i<main_command_count; i++){
		printf("%s : %s\n", main_command_list[i].c_str(), main_command_help[i].c_str());
	}
}

size_t bufGen_command_count = 5;
std::string bufGen_command_list[] = {
	"--help",
	"--format=",
	"--javascript",
	"--float-array",
	"--raw-dump"
};
std::string bufGen_command_help[] = {
        "Display this help menu.",
        "Set the format of your output data. IE ; vnt to get just vertex normal and textures.\n\t0 v=vertex, only one\n\t1 t=texture, only one\n\t2 n=normal, only one\n\t3 a=color, sourced from material Na\n\t4 d=color, sourced from material Nd\n\t5 s=color, sourced from material Ns\n\t6 e=color, sourced from material Ne\n\t7 1-4=padding, add x digits of 0 padding. min:1, max:4. More than one supported.\n\tEMPTY=vtn. an empty format gives vertex, texture, normal",
        "Output as an array of  javascript class objects",
        "Output as an array of c++ class objects",
        "Output the data using a simple grepable format",
	"Set the output directory for your generated files. If no file name is selected, a default one is used.",
	"Set the output file for your generated files. If no directory is selected, it's made in the current working directory.",
	"Set the variable name of your collection of objects."
};

int parseBufGenCommands(const char *cmd){
        for(int i=0; i<bufGen_command_count; i++){
                if(!strncmp(cmd, bufGen_command_list[i].c_str(), bufGen_command_list[i].length())) return i;
		if(strlen(cmd) >= 9){
			std::string test = "";
			for(int j=0; j<9;j++) test += cmd[j];
				if(!strncmp(test.c_str(), "--format=", 9)){
					globFormat = "";
					for(int k=9; k<strlen(cmd); k++){
						globFormat+=cmd[k];
					}
					return 1;
				}
		}
	}
        return -1;
}
void printBufGenUsage(){
	printf("Usage %s [options]\n", "--buf-gen");
        printf("Usage : --help\n");
	printf("===== manual =====\n");
        printf("%s : %s\n", commands[5].c_str(), bufGen_command_help[0].c_str());
        printf("%s : %s\n", commands[6].c_str(), bufGen_command_help[1].c_str());
        printf("%s : %s\n", commands[7].c_str(), bufGen_command_help[2].c_str());
        //8"--c++",
        //9"--raw-grep",
        printf("%s : %s\n", commands[10].c_str(), bufGen_command_help[5].c_str());
        printf("%s : %s\n", commands[11].c_str(), bufGen_command_help[6].c_str());
        printf("%s : %s\n", commands[12].c_str(), bufGen_command_help[7].c_str());
}

bool transferTexture(std::string target){
	if(target == "")return false;
	int fd = open(target.c_str(), O_RDONLY);
	if(!fd) return false;
	struct stat st;
	if(fstat(fd, &st)){
		close(fd);
	}
	if(st.st_size <= 0){
		close(fd);
		return false;
	}
	char buffer[st.st_size] = {0};
	if(read(fd, buffer, st.st_size) != st.st_size){
		close(fd);
		return false;
	}
	close(fd);
	
	std::string _file = "";
	for(int i=0; i<target.length(); i++){
		if(target[i] == '/') _file="";
		_file += target[i];
	}
        std::string _dir = globOutdir;
        if(globOutdir == "") _dir = "./";

        std::string path = _dir;
        if(path[path.length()-1] != '/') path += '/';
        path += _file;
	if(target == path) return false;
	
	fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755);
	if(!fd) return false;

	if(write(fd, buffer, st.st_size) != st.st_size){
		close(fd);
		return false;
	}
	close(fd);

	return true;
}

bool writeToFile(std::string data){
	std::string _file = globOutFile;
	if(globOutFile == "") _file = "WF_OUTPUT_FILE";
	std::string _dir = globOutdir;
	if(globOutdir == "") _dir = "./";

	std::string path = _dir;
	if(path[path.length()-1] != '/') path += '/';
	path += _file;

	int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
	if(!fd) return false;
	
	write(fd, data.c_str(), data.length());
	close(fd);
	return true;
}

void truncate(){
	if(globOutFile == "" && globOutdir == "") return;
	std::string _file = globOutFile;
	if(globOutFile == "")
	std::string _file = globOutFile;
        if(globOutFile == "") _file = "WF_OUTPUT_FILE";
        std::string _dir = globOutdir;
        if(globOutdir == "") _dir = "./";

        std::string path = _dir;
	if(path[path.length()-1] != '/') path += '/';
        path += _file;

        int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
	close(fd);
}

bool outputBuffer(std::string buffer){
	if(globOutdir != ""){
		mkdir(globOutdir.c_str(), 0755);
		return writeToFile(buffer);
	}else if(globOutFile != ""){
		return writeToFile(buffer);
	}

	printf("%s\n", buffer.c_str());
	return true;
}

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
				build += "\n/*\n\tBuilt lovingly with wfparse\n */\n";
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
			build += "];\n";

			/* check if there's an -o arg and output location, 
			 * move*/
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
}

int main(int argc, char *argv[]){
	if(argc < 3){
		printUsage();
		printf("\n");
		printBufGenUsage();
		exit(EXIT_FAILURE);
	}
	
	objImport.new_import(argv[1]);
	
	std::string cmd = "";
	if(argc >= 3) cmd = argv[2];
	int switchCtrl = fetchCommand(cmd, NULL);
	switch(switchCtrl){
		default:
			printUsage();
			break;
        	case 1: // "--full-dump",
			objImport.dumpObjFile();
			objImport.dumpMtlFile();
			break;
        	case 2: // "--obj-dump",
			objImport.dumpObjFile();
			break;
        	case 3: // "--mtl-dump",
			objImport.dumpMtlFile();
			break;
        	case 4: // "--buf-gen",
			bufGen(argc, argv);
			break;
        	case 5: // "--buf-genhelp",
			printBufGenUsage();
			break;
	}
	exit(EXIT_SUCCESS);
}
