struct Wavefront_Material_Struct{
	std::string material_name;
	std::string material_map_kd; 	// location of texture map
	float material_ns;      	// specular highlights
	float material_ka[3]; 		// ambient color
	float material_kd[3]; 		// diffuse color
	float material_ks[3]; 		// specular color
	float material_ke[3];		// dont know :p
	float material_ni;      	// Optical Density
	float material_d;       	// Dissolve
	float material_illum;   	// illumination model
};
typedef Wavefront_Material_Struct wf_material_t;

struct Wavefront_Material_Collection_Struct{
	std::string name; // collection name
	size_t materialCount;
	wf_material_t *materials;
};
typedef struct Wavefront_Material_Collection_Struct wf_material_collection_t;

class WavefrontMaterialCollection{
	private:
		size_t fileBufferSize;
                char *fileBuffer;

		bool validObjElement(std::string line, std::string target){
                        std::string word = "";
                        for(int i=0; i<line.length(); i++){
                                if(line[i] == ' ')break;
                                word += line[i];
                        }
                        if(word == target) return true;
                        return false;
                }

		void setError(const char *msg, int code){
                        this->error = code;
                        this->errorMessage = msg;
                }

		void initData(void){
			this->data.name = ""; // collection name
        		this->data.materialCount = 0;
        		this->data.materials = NULL;
		}

		void countMaterials(void){
			std::string line="";
			this->data.materialCount = 0;
			if(this->data.materials){
				delete[] this->data.materials;
			}
			for(int i=0; i<fileBufferSize; i++){
				line += fileBuffer[i];
				if(fileBuffer[i] == '\n'){
					if(validObjElement(line, "newmtl")){
						this->data.materialCount++;
					}
					line="";
				}
			}
			if(this->data.materialCount > 0)
				this->data.materials = new wf_material_t[this->data.materialCount];
		}

		void fetchMaterials(void){
			int index=-1;
			std::string line = "";
			for(int i=0; i<this->fileBufferSize; i++){
				line += fileBuffer[i];
				if(fileBuffer[i] == '\n'){
					if(this->validObjElement(line, "newmtl")){
						index++;
						std::string value = "";
						for(int j=7; j<line.length()-1; j++){
							value += line[j];
						}
						this->data.materials[index].material_name = value;
					}

					if(this->validObjElement(line, "map_Kd")){
                                                std::string value = "";
                                                for(int j=7; j<line.length(); j++){
                                                        value += line[j];
                                                }
                                                this->data.materials[index].material_map_kd = value;
                                        }

					if(this->validObjElement(line, "Ns")){
                                                std::string value = "";
                                                for(int j=3; j<line.length(); j++){
                                                        value += line[j];
                                                }
                                                this->data.materials[index].material_ns = std::stof(value.c_str());
                                        }

					if(this->validObjElement(line, "Ni")){
                                                std::string value = "";
                                                for(int j=3; j<line.length(); j++){
                                                        value += line[j];
                                                }
                                                this->data.materials[index].material_ni = std::stof(value.c_str());
                                        }
				
					if(this->validObjElement(line, "d")){
                                                std::string value = "";
                                                for(int j=2; j<line.length(); j++){
                                                        value += line[j];
                                                }
                                                this->data.materials[index].material_d = std::stof(value.c_str());
                                        }

					if(this->validObjElement(line, "illum")){
                                                std::string value = "";
                                                for(int j=6; j<line.length(); j++){
                                                        value += line[j];
                                                }
                                                this->data.materials[index].material_illum = std::stof(value.c_str());
                                        }

					if(this->validObjElement(line, "Ka")){
						int subIndex=0;
                                                std::string value = "";
                                                for(int j=3; j<line.length(); j++){
							if(line[j] == ' ' || line[j] == '\n'){
								if(subIndex >= 3) continue;
								this->data.materials[index].material_ka[subIndex] = std::stof(value.c_str());
								subIndex++;
								value="";
							}else{
                                                        	value += line[j];
							}
                                                }
                                        }
			
					if(this->validObjElement(line, "Kd")){
						int subIndex=0;
                                                std::string value = "";
                                                for(int j=3; j<line.length(); j++){
							if(line[j] == ' ' || line[j] == '\n'){
								if(subIndex >= 3) continue;
								this->data.materials[index].material_kd[subIndex] = std::stof(value.c_str());
								subIndex++;
								value="";
							}else{
                                                        	value += line[j];
							}
                                                }
                                        }

					if(this->validObjElement(line, "Ks")){
						int subIndex=0;
                                                std::string value = "";
                                                for(int j=3; j<line.length(); j++){
							if(line[j] == ' ' || line[j] == '\n'){
								if(subIndex >= 3) continue;
								this->data.materials[index].material_ks[subIndex] = std::stof(value.c_str());
								subIndex++;
								value="";
							}else{
                                                        	value += line[j];
							}
                                                }
                                        }

					if(this->validObjElement(line, "Ke")){
						int subIndex=0;
                                                std::string value = "";
                                                for(int j=3; j<line.length(); j++){
							if(line[j] == ' ' || line[j] == '\n'){
								if(subIndex >= 3) continue;
								this->data.materials[index].material_ke[subIndex] = std::stof(value.c_str());
								subIndex++;
								value="";
							}else{
                                                        	value += line[j];
							}
                                                }
                                        }

					line = "";
				}
			}
		}
	public:
		wf_material_collection_t data;
		int error=0;
		std::string errorMessage="";

		WavefrontMaterialCollection(){
                        this->setFileBuffer(NULL, 0);
                        this->initData();
                }

                WavefrontMaterialCollection(char *fileBuffer, size_t fileBufferSize){
                        this->setFileBuffer(fileBuffer, fileBufferSize);
                        this->initData();
                }
		
		void setFileBuffer(char *fileBuffer, size_t fileBufferSize){
                        this->fileBuffer = fileBuffer;
                        this->fileBufferSize = fileBufferSize;
                }

		bool parse(std::string collectionName, char *fileBuffer, size_t fileBufferSize){
                        this->setFileBuffer(fileBuffer, fileBufferSize);
                        return this->parse(collectionName);
                }
                bool parse(std::string collectionName){
			this->data.name=collectionName;	
			this->countMaterials();
			this->fetchMaterials();
			return true;
		}

		wf_material_t getMaterialByName(std::string name){
			wf_material_t ret;
			for(int i=0; i<this->data.materialCount; i++){
				wf_material_t test = this->data.materials[i];
				if(test.material_name == name) return test;
			}
			return ret;
		}
};

