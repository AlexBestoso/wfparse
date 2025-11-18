#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "../../classes/command.class.h"
#include "../../classes/wavefrontImporter.class.h"
#include "../../classes/compression.class.h"
#include "../../classes/encryptionSnake.class.h"

class GenerateCommand : public Command{
	private:
	Compression compress;
	EncryptionSnake encSnek;

	std::string gentype;
	std::string format;
	std::string target;
	std::string mtlTarget;
	std::string outputDir;
	std::string textureOutputDir;
	std::string mapLocOverride;
	WavefrontImport importer;

	float *rawBuffer;
	char *convertedBuffer;
	char *compressionBuffer;
	
	void clearCompressionBuffer(void){
		if(this->compressionBuffer != NULL){
			delete[] this->compressionBuffer;
		}
		this->compressionBuffer = NULL;
	}
	
	void clearConvertedBuffer(void){
		if(this->convertedBuffer != NULL){
			delete[] this->convertedBuffer;
		}
		this->convertedBuffer = NULL;
	}
	
	void clearRawBuffer(void){
		if(this->rawBuffer != NULL){
			delete[] this->rawBuffer;
		}
		this->rawBuffer = NULL;
	}
	
	bool allocateRawBuffer(size_t size){
		if(size < 0) return false;
		this->rawBuffer = new float[size];
		return true;
	}

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

	bool writeFile(std::string data, std::string target){
                if(this->outputDir == ""){
                        printf("%s\n", data.c_str());
                        return true;
                }

                int fd = open(target.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IWUSR | S_IRUSR);
                if(!fd){
                        printf("Error: failed to open %s\n", target.c_str());
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

	std::string sanitizeSwapDots(std::string in){
		for(int i=0; i<in.length(); i++){
                        if(in[i] == '.') in[i] = '_';
                }
		return in;
	}

	bool createDir(std::string target){
		int err = mkdir(target.c_str(), 0777);
		if(!err){
			return true;
		}
		perror("mkdir");
		return false;
	}
	
	public:
	GenerateCommand();
	~GenerateCommand() = default;

	bool generatePhpREST(void){
		WavefrontObject object;
		wf_object_t o;
		std::string subObj = "";
		std::string objMap="";
		std::string objDir="";
		std::string collectionMap = "";
		std::string data = "";
		std::string materialList = "";
		
		/* 
		 * Generate the Collection container
		 */
		WavefrontObjectCollection objCollection = importer.objectCollection;
		std::string collectionName = this->sanitizeSwapDots(objCollection.getCollectionName());
		if(this->outputDir == ""){
			this->outputDir = "./wfparse_out";
		}else{
			if(this->outputDir[this->outputDir.length()-1] == '/')
				this->outputDir.erase(this->outputDir.length()-1);
		}
		
		// Create the Collection directory
		if((access(this->outputDir.c_str(), F_OK) != 0) && !this->createDir(this->outputDir)){
			printf("Failed to create output directory.\n");
			return false;
		}

		
		collectionMap = this->outputDir + "/collection.json";
		data = "{\n";
		data += "\t\"name\":\""+collectionName+"\",\n";
		data += "\t\"objCount\":"+std::to_string(objCollection.getObjectCount())+",\n";
		data += "\t\"mtlCount\":"+std::to_string(objCollection.getMtlCount())+",\n";
		data += "\t\"subObjCount\":"+std::to_string(objCollection.getSubObjectCount())+"\n";
		data += "}";
		if(!writeFile(data, collectionMap)){
			printf("Failed to create collection.json.\n");
			return false;
		}

		/*
		 * Generate object Containers
		 * */
		for(int i=0; i<objCollection.getObjectCount(); i++){
			objDir = this->outputDir + "/"+std::to_string(i);
			if(access(objDir.c_str(), F_OK) != 0 && !this->createDir(objDir)){
				printf("Failed to create object directory '%s'\n", objDir.c_str());
				return false;
			}
			try{
				object = objCollection.getObjectByIndex(i);
			}catch(const char *e){
				printf("Error : %s", e);
				return false;
			}
			objMap = objDir + "/obj.json";
			data = "{\n";
			data += "\t\"name\":\""+object.getObjectName()+"\",\n";
			data += "\t\"subObjectCount\":"+std::to_string(object.getSubObjectCount())+",\n";
			data += "}";

			if(!this->writeFile(data, objMap)){
				printf("Failed to create %s\n", objMap.c_str());
				return false;
			}
				
			
			/*
			 * Generate subObject data and create material list.
			 * */
			for(int j=0; j<object.getSubObjectCount(); j++){
				subObj = objDir + "/"+std::to_string(j)+".json";
				data = "{\n";
				std::string mtlName = object.getSubObjMtlByIndex(j);
				if(this->uniqueMaterial(materialList, mtlName))
					materialList += mtlName + "\n";
				data += "\t\"mtlName\":\""+mtlName+"\",\n";
				data += "\t\"format\":\""+object.getExportFormat()+"\",\n";
				data += "\t\"stride\":"+std::to_string(object.calculateStrideSize())+",\n";
				size_t bSize = object.getSubObjectSizeByIndex(j);
					
				this->clearRawBuffer();
				this->allocateRawBuffer(bSize);
				if(!object.exportSubObjectByIndex(j, this->rawBuffer, bSize)){
					printf("Failed to export sub object %s:%d\n", object.getObjectName().c_str(), j);
					return false;
				}
				size_t convertedSize = 0;
				for(int k=0; k<bSize; k++) {	
					convertedSize += std::to_string(this->rawBuffer[k]).length()+1;
				}
				this->clearConvertedBuffer();
				convertedBuffer = new char[convertedSize];
				for(int k=0, a=0; k<bSize; k++) {
					std::string val = std::to_string(this->rawBuffer[k]);
					for(int l=0; l<val.length(); l++, a++){
						convertedBuffer[a] = val[l];
					}
					convertedBuffer[a] = ',';
					a++;
				}

				data += "\t\"unzippedSize\":"+std::to_string(bSize)+",\n";
				size_t compSize = (convertedSize/COMPRESSION_CHUNK) < 1 ? COMPRESSION_CHUNK : (convertedSize/COMPRESSION_CHUNK) * COMPRESSION_CHUNK;
				this->clearCompressionBuffer();
				this->compressionBuffer = new char[compSize];
				if(!this->compress.zlibDeflate(this->convertedBuffer, convertedSize, this->compressionBuffer, &compSize, 9)){
					printf("Failed to compress sub object data.\n");
					return false;
				}

				std::string base64Data = "";
				for(int k=0; k<compSize; k++){
					base64Data+= this->compressionBuffer[k];
				}

				base64Data = this->encSnek.base64(true, base64Data, compSize);
				data += "\t\"data\":\"";
				for(int k=0; k<base64Data.length(); k++){
					if(base64Data[k] != '\n')
						data += base64Data[k];
				} data+="\"\n";
				data += "}";


				if(!this->writeFile(data, subObj)){
					printf("Failed to write subobject '%s'\n", subObj.c_str());
					return false;
				}
			}
		}
		/*
		 * generate material list */
		size_t mtlCount = 0;
		std::string dataPath = this->outputDir + "/mtl.json";
		std::string grabber="";
		data = "{\n";
		data += "\t\"mtlNameCache\":[\n";
		for(int i=0; i<materialList.length(); i++){
			if(materialList[i] == '\n'){
				if(i+1 >= materialList.length())
					data += "\t\t\""+grabber+"\"\n";
				else
					data += "\t\t\""+grabber+"\",\n";
				
				wf_material_t material = objCollection.material.getMaterialByName(grabber);
				std::string data2Path = this->outputDir + "/m_" + std::to_string(mtlCount)+".json";
				std::string data2="{\n";
				data2 += "\t\"name\":\""+material.material_name+"\",\n";
				data2 += "\t\"ns\":\""+std::to_string(material.material_ns)+"\",\n";
                                data2 += "\t\"ka\":["+std::to_string(material.material_ka[0])+", "+std::to_string(material.material_ka[1])+", "+std::to_string(material.material_ka[2])+"],\n"; 
                                data2 += "\t\"kd\":["+std::to_string(material.material_kd[0])+", "+std::to_string(material.material_kd[1])+", "+std::to_string(material.material_kd[2])+"],\n"; 
                                data2 += "\t\"ks\":["+std::to_string(material.material_ks[0])+", "+std::to_string(material.material_ks[1])+", "+std::to_string(material.material_ks[2])+"],\n"; 
                                data2 += "\t\"ke\":["+std::to_string(material.material_ke[0])+", "+std::to_string(material.material_ke[1])+", "+std::to_string(material.material_ke[2])+"],\n"; 
				data2 += "\t\"ni\":\""+std::to_string(material.material_ni)+"\",\n";
				data2 += "\t\"d\":\""+std::to_string(material.material_d)+"\",\n";
				data2 += "\t\"illum\":\""+std::to_string(material.material_illum)+"\",\n";
				data2 += "\t\"map_kd\":\""+this->processMapName(material.material_map_kd)+"\"\n";
				data2 += "}";
				if(!this->writeFile(data2, data2Path)){
					printf("Failed to store material %ld\n", mtlCount);
					return false;
				}
				
				grabber = "";
				mtlCount++;
			}else{
				grabber += materialList[i];
			}
		}
		data += "\t]\n}";
		if(!this->writeFile(data, dataPath)){
			printf("Failed to write out material name cache.\n");
			return false;
		}
		return true;
	}

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
		}else if(this->gentype == "--php-rest"){
			return this->generatePhpREST();
		}
		printf("invalid generation output '%s'\n", this->gentype.c_str());
		return false;
	}

	bool exec(void) override;
};
