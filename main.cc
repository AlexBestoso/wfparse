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
	int fd = open(command.c_str(), O_RDONLY);
	if(!fd){
		printf("Faild to find SO file.\n");
		return NULL;
	}
	struct stat st;
	if(fstat(fd, &st) == -1){
		printf("Failed to calcualte SO file stats.\n");
		close(fd);
		return NULL;
	}
	close(fd);
	if(st.st_uid != 0 || st.st_gid != 0){
		printf("SO files aren't fully owned by root. This is a security flaw. Refusing to load .so file.\n");
		return NULL;
	}
	void *ret = dlopen(command.c_str(), RTLD_LAZY);
	if(!ret){
		const char * dlsym_error = dlerror();
       		if (dlsym_error) {
        		printf(".SO error : %s\n", dlsym_error);
        	}
		return NULL;
	}
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
}
