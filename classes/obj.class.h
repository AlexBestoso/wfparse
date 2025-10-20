#include "./face.class.h"
struct Wavefront_Object_Struct{
	std::string name;
	std::string material_name;
	size_t faceCount;
	WavefrontFace *Faces;
	size_t dataSize;
	float *data;
};
typedef Wavefront_Object_Struct wf_object_t;

class WavefrontObject{
	private:
		wf_object_t object;
		std::string exportFormat="vtn";
			
		bool calculateDataSize(void){
			if(this->object.data != NULL){
                                delete[] this->object.data;
				this->object.data = NULL;
			}
                        this->object.dataSize = 0;
		
			if(this->object.faceCount <= 0 || this->object.Faces == NULL){
				return false;
			}

                        for(int i=0; i<this->object.faceCount; i++){
                                WavefrontFace face = this->object.Faces[i];
                                for(int j=0; j<face.face.pointsCount; j++){
                                        for(int f=0; f<this->exportFormat.length(); f++){
                                                switch(this->parseBufferFormat(this->exportFormat.c_str(), f)){
                                                        case WF_FMT_V:
                                                                this->object.dataSize+=3;
                                                                break;
                                                        case WF_FMT_N:
                                                                this->object.dataSize+=3;
                                                                break;
                                                        case WF_FMT_T:
                                                                this->object.dataSize += 2;
                                                                break;
                                                }
                                        }

                                }
                        }
			if(this->object.dataSize <= 0)
				return false;
			this->object.data = new float[this->object.dataSize];
			return true;
		}

		bool generateData(float **vertex, size_t vertex_size, float **texture, size_t texture_size, float **normal, size_t normal_size){
			if(this->object.faceCount <= 0 || this->object.Faces == NULL){
                                return false;
                        }
			if(this->object.data == NULL || this->object.dataSize == 0){
				return false;
			}

			int dataIndex=0;
			for(int i=0; i<this->object.faceCount && dataIndex < this->object.dataSize; i++){
                                WavefrontFace face = this->object.Faces[i];
                                for(int j=0; j<face.face.pointsCount; j++){
                                        for(int f=0; f<this->exportFormat.length() && dataIndex < this->object.dataSize; f++){
                                                switch(this->parseBufferFormat(this->exportFormat.c_str(), f)){
                                                        case WF_FMT_V:{
								int target = face.face.points[j].index_vertex;
								if(target >= vertex_size) {
									printf("vertex index to value overflow.\n");
									return false;
								}
								for(int a=0; a<3 && dataIndex<this->object.dataSize; a++){
									this->object.data[dataIndex] = vertex[target][a];
									dataIndex++;
								}
								break;
							}
                                                        case WF_FMT_N:{
                                                        	int target = face.face.points[j].index_normal;
								if(target >= normal_size){
									printf("normal index to value overflow.\n");
									return false;
								}
								for(int a=0; a<3 && dataIndex<this->object.dataSize; a++){
									this->object.data[dataIndex] = normal[target][a];
									dataIndex++;
								}
                                                                break;
							}
                                                        case WF_FMT_T:{
                                                                int target = face.face.points[j].index_texture;
								if(target >= texture_size){
									printf("vertex index to value overflow.\n");
									return false;
								}
								for(int a=0; a<2 && dataIndex < this->object.dataSize; a++){
									this->object.data[dataIndex] = texture[target][a];
									dataIndex++;
								}
                                                                break;
							}
                                                }
                                        }
                                }
                        }
			return true;
		}
		
		bool validObjElement(std::string line, std::string target){
			std::string word = "";
			for(int i=0; i<line.length(); i++){
				if(line[i] == ' ')break;
				word += line[i];
			}
			if(word == target) return true;
			return false;
		}

		bool compileName(void){
			this->object.name = "";
			std::string line="";
			for(int i=0; i<this->rawSize; i++){
				if(this->rawBuffer[i] == '\n'){
					if(this->validObjElement(line, "o")){
						for(int j=2; j<line.length(); j++){
							this->object.name+= line[j];
						}
						return true;
					}
					line = "";
				}else{
					line+=this->rawBuffer[i];
				}
			}
			return false;
		}

		bool compileMaterialName(void){
			this->object.material_name = "";
			std::string line="";
			for(int i=0; i<this->rawSize; i++){
                                if(this->rawBuffer[i] == '\n'){
                                        if(this->validObjElement(line, "usemtl")){
                                                for(int j=7; j<line.length(); j++){
                                                        this->object.material_name= line[i];
                                                }
                                                return true;
                                        }
                                        line = "";
                                }else{
                                        line+=this->rawBuffer[i];
                                }
                        }
			return false;
		}

		bool compileFaces(void){
			if(this->object.Faces != NULL){
				delete[] this->object.Faces;
			}
			this->object.faceCount=0;
			std::string line="";
			for(int i=0; i<this->rawSize; i++){
				if(this->rawBuffer[i] == '\n'){
					if(this->validObjElement(line, "f")){
						this->object.faceCount++;
					}
					line = "";
				}else{
					line += this->rawBuffer[i];
				}
			}
			if(this->object.faceCount <= 0){
				printf("no faces.\n");
				return false;
			}
			this->object.Faces = new WavefrontFace[this->object.faceCount];

			for(int i=0, faceIdx=0; i<this->rawSize; i++){
                                if(this->rawBuffer[i] == '\n'){
                                        if(this->validObjElement(line, "f")){
						line+='\n';
						this->object.Faces[faceIdx].compile(line);
                                                faceIdx++;
                                        }
                                        line = "";
                                }else{
                                        line += this->rawBuffer[i];
                                }
                        }
			
			return true;
		}

		bool import_mtl_material(void){
			this->material_name = "";
			std::string line="";
			std::string test="newmtl "+this->object_material+"\n";
			for(int i=0;i<this->mtl_data_size;i++){
				line += this->mtl_data[i];
				if(this->mtl_data[i] == '\n'){
					if(!strncmp(line.c_str(), test.c_str(), test.length())){
						for(int j=7; j<line.length()-1; j++)
							this->material_name += line[j];
						return true;
					}
					line="";
				}
			}
			return false;
		}	

		bool import_mtl_ns(void){
			int startIndex=0;
			std::string test="newmtl "+this->object_material+"\n";
			std::string line="";
                        for(int i=0;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(!strncmp(line.c_str(), test.c_str(), test.length())){
						startIndex=i;
						break;
                                        }
                                        line="";
                                }
                        }
		
                        line="";
			for(int i=startIndex; i<this->mtl_data_size;i++){
				line += this->mtl_data[i];
				if(this->mtl_data[i] == '\n'){
					if(line[0] == 'N' && line[1] == 's' && line[2] == ' '){
						std::string data="";
						for(int j=3; j<line.length()-1; j++){
							data += line[j];
						}
						this->material_ns = std::stof(data.c_str());
						return true;
					}
					line="";
				}
			}
			return false;
		}	

		bool import_mtl_ni(void){
                        int startIndex=0;
                        std::string test="newmtl "+this->object_material+"\n";
                        std::string line="";
                        for(int i=0;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(!strncmp(line.c_str(), test.c_str(), test.length())){
                                                startIndex=i;
                                                break;
                                        }
                                        line="";
                                }
                        }

                        line="";
                        for(int i=startIndex;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(line[0] == 'N' && line[1] == 'i' && line[2] == ' '){
                                                std::string data="";
                                                for(int j=3; j<line.length()-1; j++){
                                                        data += line[j];
                                                }
                                                this->material_ni = std::stof(data.c_str());
                                                return true;
                                        }
                                        line="";
                                }
                        }
                        return false;
                }

		bool import_mtl_d(void){
			int startIndex=0;
                        std::string test="newmtl "+this->object_material+"\n";
                        std::string line="";
                        for(int i=0;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(!strncmp(line.c_str(), test.c_str(), test.length())){
                                                startIndex=i;
                                                break;
                                        }
                                        line="";
                                }
                        }

                        line="";
                        for(int i=startIndex; i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(line[0] == 'd' && line[1] == ' '){
                                                std::string data="";
                                                for(int j=2; j<line.length()-1; j++){
                                                        data += line[j];
                                                }
                                                this->material_d = std::stof(data.c_str());
                                                return true;
                                        }
                                        line="";
                                }
                        }
                        return false;
                }

		bool import_mtl_ka(void){
                        int startIndex=0;
                        std::string test="newmtl "+this->object_material+"\n";
                        std::string line="";
                        for(int i=0;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(!strncmp(line.c_str(), test.c_str(), test.length())){
                                                startIndex=i;
                                                break;
                                        }
                                        line="";
                                }
                        }

                        line="";
                        for(int i=startIndex;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(line[0] == 'K' && line[1] == 'a' && line[2] == ' '){
                                                std::string data="";
						int J=0;
                                                for(int j=3; j<line.length(); j++){
							if(J>= 3){
								return true;
							}
							if(line[j] == ' ' || line[j] == '\n'){
								
								this->material_ka[J] = std::stof(data.c_str());
								data = "";
								J++;
								continue;
							}
                                                        data += line[j];
                                                }
						return true;
                                        }
                                        line="";
                                }
                        }	
                        return false;
                }

		bool import_mtl_kd(void){
                        int startIndex=0;
                        std::string test="newmtl "+this->object_material+"\n";
                        std::string line="";
                        for(int i=0;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(!strncmp(line.c_str(), test.c_str(), test.length())){
                                                startIndex=i;
                                                break;
                                        }
                                        line="";
                                }
                        }

                        line="";
                        for(int i=startIndex;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(line[0] == 'K' && line[1] == 'd' && line[2] == ' '){
                                                std::string data="";
						int J=0;
                                                for(int j=3; j<line.length(); j++){
							if(J>= 3) return true;
							if(line[j] == ' ' || line[j] == '\n'){
								this->material_kd[J] = std::stof(data.c_str());
								data = "";
								J++;
								continue;
							}
                                                        data += line[j];
                                                }
						return true;
                                        }
                                        line="";
                                }
                        }
                        return false;
                }

		bool import_mtl_ks(void){
                        int startIndex=0;
                        std::string test="newmtl "+this->object_material+"\n";
                        std::string line="";
                        for(int i=0;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(!strncmp(line.c_str(), test.c_str(), test.length())){
                                                startIndex=i;
                                                break;
                                        }
                                        line="";
                                }
                        }

                        line="";
                        for(int i=startIndex;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(line[0] == 'K' && line[1] == 's' && line[2] == ' '){
                                                std::string data="";
						int J=0;
                                                for(int j=3; j<line.length(); j++){
							if(J>= 3) return true;
							if(line[j] == ' ' || line[j] == '\n'){
								this->material_ks[J] = std::stof(data.c_str());
								data = "";
								J++;
								continue;
							}
                                                        data += line[j];
                                                }
						return true;
                                        }
                                        line="";
                                }
                        }
                        return false;
                }

		bool import_mtl_ke(void){
                        int startIndex=0;
                        std::string test="newmtl "+this->object_material+"\n";
                        std::string line="";
                        for(int i=0;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(!strncmp(line.c_str(), test.c_str(), test.length())){
                                                startIndex=i;
                                                break;
                                        }
                                        line="";
                                }
                        }

                        line="";
                        for(int i=startIndex;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(line[0] == 'K' && line[1] == 'e' && line[2] == ' '){
                                                std::string data="";
						int J=0;
                                                for(int j=3; j<line.length(); j++){
							if(J>= 3) return true;
							if(line[j] == ' ' || line[j] == '\n'){
								this->material_ke[J] = std::stof(data.c_str());
								data = "";
								J++;
								continue;
							}
                                                        data += line[j];
                                                }
						return true;
                                        }
                                        line="";
                                }
                        }
                        return false;
                }

		bool import_mtl_illum(void){
                        int startIndex=0;
                        std::string test="newmtl "+this->object_material+"\n";
                        std::string line="";
                        for(int i=0;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(!strncmp(line.c_str(), test.c_str(), test.length())){
                                                startIndex=i;
                                                break;
                                        }
                                        line="";
                                }
                        }

                        line="";
                        for(int i=startIndex;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
					if(line.length() > 5){
						std::string cmd = "";
						for(int j=0; j<5; j++) cmd+=line[j];
						if(!strncmp(cmd.c_str(), "illum", 5)){
							std::string data = "";
							for(int j=6; j<line.length()-1; j++){
								data += line[j];
							}
							this->material_illum = std::stof(data);
							return true;
						}
						return true;
					}
                                        line="";
                                }
                        }
                        return false;
                }

		bool import_mtl_map_kd(void){
                        int startIndex=0;
                        std::string test="newmtl "+this->object_material+"\n";
                        std::string line="";
                        for(int i=0;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
                                        if(!strncmp(line.c_str(), test.c_str(), test.length())){
                                                startIndex=i;
                                                break;
                                        }
                                        line="";
                                }
                        }

                        line="";
                        for(int i=startIndex;i<this->mtl_data_size;i++){
                                line += this->mtl_data[i];
                                if(this->mtl_data[i] == '\n'){
					if(line.length() > 6){
						std::string cmd = "";
						for(int j=0; j<6; j++) cmd+=line[j];
						if(!strncmp(cmd.c_str(), "map_Kd", 6)){
							std::string data = "";
							for(int j=7; j<line.length()-1; j++){
								data += line[j];
							}
							this->material_map_kd = data;
							return true;
						}
					}
                                        line="";
                                }
                        }
                        return false;
                }

		size_t getCellLength(const char *fmt){
			size_t cellSize=0;
			for(int i=0; i<strlen(fmt); i++){
				switch(this->parseBufferFormat(fmt, i)){
					case WF_FMT_V:
					case WF_FMT_N:
					case WF_FMT_A:
					case WF_FMT_E:
					case WF_FMT_3:
					case WF_FMT_KD:
						cellSize+=3;
					break;
					case WF_FMT_T:
					case WF_FMT_2:
						cellSize+=2;
					break;
					case WF_FMT_D:
					case WF_FMT_S:
					case WF_FMT_1:
						cellSize+=1;
					break;
					case WF_FMT_4:
						cellSize+=4;
					break;
				}
			}
			return cellSize;
		}

		int parseBufferFormat(const char *fmt, unsigned int index){
			if(index >= strlen(fmt)) return -1;
			
			char target = fmt[index];
			if(target == 'v') return WF_FMT_V;
			if(target == 't') return WF_FMT_T;
			if(target == 'n') return WF_FMT_N;
			if(target == 'a') return WF_FMT_A;
			if(target == 'd') return WF_FMT_D;
			if(target == 'D') return WF_FMT_KD;
			if(target == 's') return WF_FMT_S;
			if(target == 'e') return WF_FMT_E;
			if(target == '1') return WF_FMT_1;
			if(target == '2') return WF_FMT_2;
			if(target == '3') return WF_FMT_3;
			if(target == '4') return WF_FMT_4;
			return -1;
		}	

	public:
		char *rawBuffer=NULL;
		size_t rawSize=0;

		std::string object_mtllib="";
		std::string object_material="";
	
		std::string material_name="";
		float material_ns = 0.0;      // specular highlights
		float material_ka[3] = {0.0}; // ambient color
		float material_kd[3] = {0.0}; // diffuse color
		float material_ks[3] = {0.0}; // specular color
		float material_ke[3] = {0.0}; 
		float material_ni = 0.0;      // Optical Density
		float material_d = 0.0;       // Dissolve
		float material_illum = 0.0;   // illumination model
		std::string material_map_kd = ""; // location of texture map


		char *mtl_data=NULL;
		size_t mtl_data_size=0;

		WavefrontObject(void){

		}


		bool import_mtl(){
			if(this->mtl_data == NULL || this->mtl_data_size == 0){
				if(this->mtl_data){
					free(this->mtl_data);
					this->mtl_data=NULL;
				}
				this->mtl_data_size = 0;
				if(this->object_mtllib == ""){
					printf("no obj mtllib\n");
					return false;
				}
				
				int fd = open(this->object_mtllib.c_str(), O_RDONLY);
				if(!fd) return false;
				struct stat st;
				if(fstat(fd, &st)){
					close(fd);
					return false;
				}
				this->mtl_data_size = st.st_size;
				this->mtl_data = new char[this->mtl_data_size];
				
				read(fd, this->mtl_data, this->mtl_data_size);
				close(fd);
			}

			this->import_mtl_material();
			this->import_mtl_ns();
			this->import_mtl_ka();
			this->import_mtl_kd();
			this->import_mtl_ks();
			this->import_mtl_ke();
			this->import_mtl_ni();
			this->import_mtl_d();
			this->import_mtl_illum();
			this->import_mtl_map_kd();
			return true;
		}
		bool import_mtl(char *data, size_t data_size){
			this->mtl_data=data;
			this->mtl_data_size = data_size;
			return this->import_mtl();
		}


		bool compile(void){
			if(this->rawBuffer == NULL){
				printf("Buffer is null.\n");
				return false;
			}
			if(this->compileName() == false){
				printf("failed to compile name.\n");
				return false;
			}

			if(this->compileMaterialName() == false){
				printf("failed to compile material name.\n");
			}	

			if(this->compileFaces() == false){
				printf("Failed to compile faces.\n");
				return false;
			}
			return true;
		}

		void setExportFormat(std::string fmt){
			this->exportFormat = fmt;
		}
		
		/*
			Master Vertex Store, Master Texture Store, Master Normal Store
		*/
		bool realizeSelf(float **vertex, size_t vertex_size, float **texture, size_t texture_size, float **normal, size_t normal_size){
			int vertex_depth = 3;
			int texture_depth = 2;
			int normal_depth = vertex_depth;

			if(this->calculateDataSize() == false)
				return false;
			this->generateData(vertex, vertex_size, texture, texture_size, normal, normal_size);
			return true;
		}

		std::string getObjectName(void){
			return this->object.name;
		}

		wf_object_t getObject(void){
			return this->object;
		}
		
		size_t getFaceCount(void){
			return this->object.faceCount;
		}

		WavefrontFace getFace(int idx){
			if(idx >= this->object.faceCount) throw "Object faces not generated.";
			return this->object.Faces[idx];
		}

		size_t calculateStrideSize(void){
			return getCellLength(this->exportFormat.c_str());
		}
};
