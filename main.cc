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

size_t main_command_count = 6;
std::string main_command_list[] = {
	"--help",
	"--full-dump",
	"--obj-dump",
	"--mtl-dump",
	"--buf-gen",
	"--buf-gen-help"
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
        "Output as a javascript matrix array",
        "Output as a float array",
        "Output the data as a raw string of floats."
};

std::string globFormat="vtn";
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
	for(int i=0; i<bufGen_command_count; i++){
		printf("%s : %s\n", bufGen_command_list[i].c_str(), bufGen_command_help[i].c_str());
	}
}

bool bufGen(int argc, char *argv[]){
	if(argc < 3) return false;
	std::string subarg = "--raw-dump";
	std::string secondArg = "";
	if(argc >= 4){
		subarg = argv[3];
	}
	if(argc >= 5){
		secondArg = argv[4];
	}
	int result = parseBufGenCommands(subarg.c_str());
	int result2=parseBufGenCommands(secondArg.c_str());
	int switchCtrl=result;
	if(switchCtrl == 1 && result2 != 1 && result2 != -1)
		switchCtrl = result2;
	
	std::string build = "";
	std::string classNames = "";
	std::string line = "";
	switch(switchCtrl){
		default:
			printBufGenUsage();
			return false;
		case 2: // --javascript
			for(int i=0; i<objImport.objCount; i++){
				objImport.genBuffer_format(globFormat.c_str(), i);
				build += "/*\n\tBuilt lovingly with wfparse\n */\n";
				build += "class WF_"+objImport.objList[i].object_name+"{\n";
				build += "\tgl_stride = null;\n";
				build += "\tgl_data = null;\n";
				build += "\tgl_material = null;\n";

				build += "\tconstructor(){\n";
					build += "\t\tthis.gl_material = [\n";
                        		build += "\t\t\t'"+objImport.objList[i].material_name+"',\n";
					build += "\t\t\t"+std::to_string(objImport.objList[i].material_ns)+",\n";
                        		build += "\t\t\t[" + std::to_string(objImport.objList[i].material_ka[0]) + ", " +
							     std::to_string(objImport.objList[i].material_ka[1]) + ", " +
							     std::to_string(objImport.objList[i].material_ka[2]) +
							 "],\n";
                        		build += "\t\t\t[" + std::to_string(objImport.objList[i].material_kd[0]) + ", " +
							     std::to_string(objImport.objList[i].material_kd[1]) + ", " +
							     std::to_string(objImport.objList[i].material_kd[2]) +
							 "],\n";
                        		build += "\t\t\t[" + std::to_string(objImport.objList[i].material_ks[0]) + ", " +
							     std::to_string(objImport.objList[i].material_ks[1]) + ", " +
							     std::to_string(objImport.objList[i].material_ks[2]) +
							"],\n";
                        		build += "\t\t\t[" + std::to_string(objImport.objList[i].material_ke[0]) + ", " + 
							     std::to_string(objImport.objList[i].material_ke[1]) + ", " +
							     std::to_string(objImport.objList[i].material_ke[2]) +
							"],\n";
                        		build += "\t\t\t"+std::to_string(objImport.objList[i].material_ni)+",\n";
                        		build += "\t\t\t"+std::to_string(objImport.objList[i].material_d)+",\n";
                        		build += "\t\t\t"+std::to_string(objImport.objList[i].material_illum)+",\n";
                        		build += "\t\t\t'"+objImport.objList[i].material_map_kd+"',\n";

					build += "\t\t];\n";
					build += "\t\tthis.gl_stride = " + std::to_string(objImport.gl_stride)+";\n";
					build += "\t\tthis.gl_data = [\n";
					for(int j=0; j<objImport.objList[i].gl_buffer_size; j++){
						if((j%objImport.objList[i].gl_stride) == 0) build += "\t\t\t[";
				
						build += std::to_string(objImport.objList[i].gl_buffer[j]);
						build += ",";
	
						if((j%objImport.objList[i].gl_stride) == objImport.objList[i].gl_stride-1) build += "],\n";
					}
					build += "\t\t];\n";
				build += "\t}\n";

                        	build += "\tgetMaterialName){return this.gl_material[0];}\n";
				build += "\tgetMaterialNs(){return this.gl_material[1];}\n";
                        	build += "\tgetMaterialKa(){return this.gl_material[2];}\n";
                        	build += "\tgetMaterialKd(){return this.gl_material[3];}\n";
                        	build += "\tgetMaterialKs(){return this.gl_material[4];}\n";
                        	build += "\tgetMaterialKe(){return this.gl_material[5];}\n";
                        	build += "\tgetMaterialNi(){return this.gl_material[6];}\n";
                        	build += "\tgetMaterialD(){return this.gl_material[7];}\n";
                        	build += "\tgetMaterialIllum(){return this.gl_material[8];}\n";
                        	build += "\tgetMaterialMapKd(){return this.gl_material[9];}\n";

				build += "}\n";
				build += "let wf_"+objImport.objList[i].object_name + " = new "+"WF_"+objImport.objList[i].object_name+"();";
				classNames += "wf_"+objImport.objList[i].object_name+"\n";
				printf("%s\n", build.c_str());
				build = "";
			}

			build = "\nlet WF_Collection_ = [";
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
			printf("%s\n", build.c_str());
			break;
		case 3: // --float-array
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
			break;
		case 4: //--raw-dump
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
	if(argc < 2){
		printUsage();
		return 1;
	}
	
	objImport.new_import(argv[1]);

	if(!objImport.import(argv[1])){
		printf("[E] Failed to import file \n");
		printUsage();
		return 1;
	}

		
	std::string cmd = "";
	if(argc > 2) cmd = argv[2];
	switch(parseMainCommands(cmd.c_str())){
		case 0: // --help
			printUsage();
			break;
        	case 2: //--obj-dump
			objImport.dumpObjFile();
			break;
		case 3: //--mtl-dump
			objImport.dumpMtlFile();
			break;
		case 4: //--buf-gen
			bufGen(argc, argv);
			break;
		case 5: //--buf-gen-help
			printBufGenUsage();
			break;
		default: // --full-dump
			objImport.dumpObjFile();
			objImport.dumpMtlFile();
			break;
	}
		return 0;
}
