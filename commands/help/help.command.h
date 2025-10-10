#pragma once

#include <exception>
#include <string>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include "../../classes/command.class.h"

class HelpCommand : public Command{
	private:

	public:
		HelpCommand();
		~HelpCommand() = default;

		unsigned int soCount=0;
		std::string outNames="";

		void countSharedObjects(void){
			this->soCount = 0;
			struct dirent *de;
			DIR *dr = opendir("./bin");
			if(dr == NULL){
				throw "No shared objects detected, program cannot function!\n";
			}
	
			this->outNames = "";
			while((de = readdir(dr)) != NULL){
				std::string name = de->d_name;
				if(name.length() > 3){
					this->soCount++;
					this->outNames += name;
					this->outNames += "\n";
				}
			}
			closedir(dr);
		}

		std::string getCommandName(int idx){
			std::string grabber = "";
			int track = -1;
			for(int i=0; i<outNames.length(); i++){
				if(outNames[i] == '\n'){
					track++;
					if(track == idx)
						return grabber;
					grabber = "";
				}else{
					grabber += outNames[i];
				}
			}
			return "";
		}

		void *loadCommandHandle(std::string name){
			std::string command = "./bin/";
			command += name;
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
				printf("Failed to open %s\n", command.c_str());
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

		void printManuals(void){
			this->countSharedObjects();
			for(int i=0; i<this->soCount; i++){
				std::string target = this->getCommandName(i);
				void *handle = this->loadCommandHandle(target);
				if(handle == NULL) continue;
				command_t invoker = createInvoker(handle);
				Command *cmd = invoker();
				std::string name = cmd->getName();
				std::string desc = cmd->getDescription();
				std::string man = cmd->getManual();
			
				printf("\n%s - %s\n\t%s\n", name.c_str(), desc.c_str(), man.c_str());
				dlclose(handle);
			}
		
		}
		
		bool exec(void) override;
};
