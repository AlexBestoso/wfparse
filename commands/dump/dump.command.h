#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "../../classes/command.class.h"
#include "../../classes/wavefrontImporter.class.h"

class DumpCommand : public Command{
	private:
		std::string filename;
		std::string arg;

	public:
		DumpCommand();
		~DumpCommand() = default;
	
		bool fullDump(void){
			WavefrontImport importer;
			std::string obj = this->filename + ".obj";
			std::string mtl = this->filename + ".mtl";
			if(!importer.import(obj, mtl)){
				printf("Importation failed\n");
				return false;
			}
			importer.dumpObjFile();
			importer.dumpMtlFile();
			return true;
		}
                bool objDump(void){
			WavefrontImport importer;
                        std::string obj = this->filename + ".obj";
                        std::string mtl = this->filename + ".mtl";
                        if(!importer.import(obj, mtl)){
                                printf("Importation failed\n");
                                return false;
                        }
                        importer.dumpObjFile();
			return true;
		}
                bool mtlDump(void){
			WavefrontImport importer;
                        std::string obj = this->filename + ".obj";
                        std::string mtl = this->filename + ".mtl";
                        if(!importer.import(obj, mtl)){
                                printf("Importation failed\n");
                                return false;
                        }
                        importer.dumpMtlFile();
			return true;
		}
                bool fullStatDump(void){
			printf("ERROR - Command still under development.\n");
			return false;
		}
                bool objStatDump(void){
			printf("ERROR - Command still under development.\n");
			return false;
		}
                bool mtlStatDump(void){
			printf("ERROR - Command still under development.\n");
			return false;
		}


		bool exec(void) override;
};
