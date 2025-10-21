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
	WavefrontImport importer;
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

			std::string materialName = o.material_name;
			wf_material_t material = collection.material.getMaterialByName(materialName);
			
			std::string map="";
			if(material.material_map_kd.length() > 0 && material.material_map_kd[material.material_map_kd.length()-1] == '\n'){
                                for(int i=0; i<material.material_map_kd.length()-1; i++)
                                        map+=material.material_map_kd[i];

                        }else{
                               map = material.material_map_kd;
                        }

			ret_material += "let "+material.material_name+" = new BtWebglMaterial('"+material.material_name+"', "+
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

			std::string ret_object = "let "+objName+" = new BtWebglObject('"+objName+"', "+collectionStride+", '"+material.material_name+"', [\n\t";
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
		printf("%s\n", ret.c_str());
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
