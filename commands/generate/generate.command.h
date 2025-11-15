#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "../../classes/command.class.h"
#include "../../classes/wavefrontImporter.class.h"

class GenerateCommand : public Command{
	private:
	std::string gentype;
	std::string format;
	std::string target;
	std::string mtlTarget;
	std::string outputDir;
	std::string textureOutputDir;
	std::string mapLocOverride;
	WavefrontImport importer;
	
	bool uniqueMaterial(std::string materialCache, std::string target){
		std::string grabber = "";
		for(int i=0; i<materialCache.length(); i++){
			if(materialCache[i] == '\n'){
				if(grabber == target) return false;
				grabber = "";
			}else{
				grabber += materialCache[i];
			}
		}
		return true;
	}

	bool copyFile(const char *src, const char *dst){
		int fd = open(src, O_RDONLY);
		if(!fd) return false;
		
		struct stat st;
		if(fstat(fd, &st) == -1){
			close(fd);
			return false;
		}

		size_t srcSize = st.st_size;
		char *srcBuff = new char[srcSize];
		if(read(fd, srcBuff, srcSize) != srcSize){
			perror("read");
			close(fd);
			return false;
		}
		close(fd);
		
		fd = open(dst, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
		if(!fd) return false;
		if(write(fd, srcBuff, srcSize) != srcSize){
			perror("write");
			close(fd);
			return false;
		}
		close(fd);
		return true;
	}

	bool outputFile(std::string data){
		if(this->outputDir == ""){
			printf("%s\n", data.c_str());
			return true;
		}
		
		int fd = open(this->outputDir.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IWUSR | S_IRUSR);
		if(!fd){
			printf("Error: failed to open %s\n", this->outputDir.c_str());
			return false;
		}
		
		if(size_t err = (write(fd, data.c_str(), data.length())) != data.length()){
			perror("write");
			close(fd);
			printf("Error: wrote %ld of %ld bytes. Missing %ld\n", err, data.length(), data.length() - err);
			return false;
		}
		close(fd);
		return true;
	}

	std::string processMapName(std::string mapPath){
		std::string ret = "";
		if(mapPath.length() > 0 && mapPath[mapPath.length()-1] == '\n'){ // theres a new line for some reason.
			for(int i=0; i<mapPath.length()-1; i++)
				ret+=mapPath[i];
		}else if(mapPath.length() <= 0){
			return "";
		}else{
			ret = mapPath;
		}
			
		if(this->textureOutputDir.length() <= 0 &&  this->mapLocOverride.length() <= 0)
			return ret;
		mapPath = ret;

		std::string name = "";
		int start=0;
		for(int i=0; i<mapPath.length(); i++){
			if(mapPath[i] == '/')
				start = i+1;
		}
		for(int i=start; i<mapPath.length(); i++){
			name += mapPath[i];
		}
		
		if(this->textureOutputDir != ""){
			ret = this->textureOutputDir;
			if(this->textureOutputDir[this->textureOutputDir.length()-1] != '/')
				ret += "/";
			ret += name;
			if(!this->copyFile(mapPath.c_str(), ret.c_str())){
				printf("Failed to copy texture files src('%s'), dst('%s')\n", mapPath.c_str(), ret.c_str());
			}
		}
		
		if(this->mapLocOverride != ""){
			ret = this->mapLocOverride;
			if(this->mapLocOverride[this->mapLocOverride.length()-1] != '/')
                                ret += "/";
			ret += name;
		}

		return ret;
	}
	
	public:
	GenerateCommand();
	~GenerateCommand() = default;

	bool generateJavascript(){
		std::string ret = "";
		std::string makersMark = "/* * * * * * * * * *\n";
		makersMark += " *  This file was generated using wfparse.\n";
		makersMark += " * https://github.com/AlexBestoso/wfparse\n";
		makersMark += " * plz respect my authoritar\n";
		makersMark += " * * * * * * * * * */\n";
		ret = makersMark;
		WavefrontObjectCollection collection = importer.objectCollection;
		std::string collectionName = collection.data.name;
		for(int i=0; i<collectionName.length(); i++){
			if(collectionName[i] == '.') collectionName[i] = '_';
		}

		ret += "let " + collectionName + " = new BtWebglCollection(Array(), '"+collectionName+"');\n";
		

		std::string objectNames = "";
		std::string materialCache = "";
		for(int j=0; j<collection.data.objectCount; j++){
			WavefrontObject obj = collection.data.obj[j];
			wf_object_t o = obj.getObject();
			int strideSize = obj.calculateStrideSize();
			std::string collectionStride = std::to_string(strideSize);

			std::string ret_material = "";

			std::string objName = "";
			for(int k=0; k<o.name.length(); k++){
				if(o.name[k] == '.'){
					objName += '_';
				}else{
					objName += o.name[k];
				}
			}
			objectNames += "wf_obj_"+objName+"\n";

			std::string materialName = "";
			for(int k=0; k<o.material_name.length(); k++){
                                if(o.material_name[k] == '.'){
                                        materialName += '_';
                                }else{
                                        materialName += o.material_name[k];
                                }
                        }
			wf_material_t material = collection.material.getMaterialByName(materialName);
			
			if(this->uniqueMaterial(materialCache, materialName)){
				materialCache += materialName + "\n";
				std::string map = this->processMapName(material.material_map_kd);

				ret_material += "let mtl_"+materialName+" = new BtWebglMaterial('"+materialName+"', "+
					std::to_string(material.material_ns)+
					", ["+
						std::to_string(material.material_ka[0])+
						", "+std::to_string(material.material_ka[1])+
						", "+std::to_string(material.material_ka[2])+
					"], ["+
						std::to_string(material.material_kd[0])+
						", "+std::to_string(material.material_kd[1])+
						", "+std::to_string(material.material_kd[2])+
					"], ["+
						std::to_string(material.material_ks[0])+
						", "+std::to_string(material.material_ks[1])+
						", "+std::to_string(material.material_ks[2])+
					"], ["+
						std::to_string(material.material_ke[0])+
						", "+std::to_string(material.material_ke[1])+
						", "+std::to_string(material.material_ke[1])+
					"], "+std::to_string(material.material_ni)+
					", "+std::to_string(material.material_d)+
					", "+std::to_string(material.material_illum)+
				", '"+map+"');\n";
			}
			std::string ret_object = "let "+objName+" = new BtWebglObject('"+objName+"', "+collectionStride+", mtl_"+materialName+", [\n\t";
			for(int i=0; i<o.dataSize; i++){
				if((i%strideSize) == 0){
					ret_object += "[";
				}
				ret_object += std::to_string(o.data[i]) + ", ";
				if((i%strideSize) == strideSize-1){
					ret_object += "],\n\t";
				}
                        }
			ret_object += "]);\n";
			
			ret += ret_material;
			ret += ret_object;
			ret += collectionName+".pushObjectCollection("+objName+");\n\n";
		}
		if(!this->outputFile(ret)){
			printf("Error, failed to output generated file.\n");
		}
		return true;
	}

	bool process(void){
		if(this->target == ""){
			printf("No object file provided.\n");
			return false;
		}
		importer.setExportFormat(this->format);
		if(this->mtlTarget == ""){
			importer.import(this->target);
		}else{
			importer.import(this->target, this->mtlTarget);
		}
		
		if(this->gentype == "--javascript"){
			return this->generateJavascript();
		}
		printf("invalid generation output '%s'\n", this->gentype.c_str());
		return false;
	}

	bool exec(void) override;
};
