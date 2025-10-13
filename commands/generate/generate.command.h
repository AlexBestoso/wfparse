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
		importer.compileBuffer(this->format);	
		std::string jsout="";
		std::string makersMark = "/*\n * Made lovingly using wfparse\n * */\n";
		WavefrontObjectCollection collection = importer.objectCollection;
		std::string collectionName = collection.data.name;
		for(int i=0; i<collectionName.length(); i++){
			if(collectionName[i] == '.') collectionName[i] = '_';
		}
		std::string collectionStride = std::to_string(importer.vboStride);

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
			if(material.material_map_kd.length() > 0 && material.material_map_kd[material.material_map_kd.length()-1] == '\n'){
				std::string grab = "";
				for(int i=0; i<material.material_map_kd.length()-1; i++)
					grab+=material.material_map_kd[i];
				jsout += "\t\t\t\t'"+grab+"'\n";
				
			}else{
				jsout += "\t\t\t\t'"+material.material_map_kd+"'\n";
			}

			jsout += "\t\t\t);\n";
			jsout += "\t\t}catch(e){\n";
			jsout += "\t\t\tconsole.log(e+' : Try adding \"defer\" to your script tag for this file if the material class has been defined.');\n";
			jsout += "\t\t}\n";
			jsout += "\t\tthis.stride = "+collectionStride+";\n";
			jsout += "\t\tthis.data = [\n";
			for(int i=0; i<importer.vboSize; i++){
				if((i%importer.vboStride) == 0){
					jsout += "\t\t\t[";
				}
				jsout += std::to_string(importer.vboData[i]) + ", ";
				if((i%importer.vboStride) == importer.vboStride-1){
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
		return true;
	}

	bool process(void){
		if(this->target == ""){
			printf("No object file provided.\n");
			return false;
		}
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
