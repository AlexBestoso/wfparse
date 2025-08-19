#define WF_FMT_V 0
#define WF_FMT_T 1
#define WF_FMT_N 2
#define WF_FMT_A 3
#define WF_FMT_D 4
#define WF_FMT_S 5
#define WF_FMT_E 6
#define WF_FMT_1 7
#define WF_FMT_2 8
#define WF_FMT_3 9
#define WF_FMT_4 10
#define WF_FMT_KD 11


struct WavefrontFace{
	size_t count;
	int *v_index;
	int *vt_index;
	int *vn_index;
};
class WavefrontObject{
	private:
		
		
		bool import_obj_texture(void){
			if(this->object_texture){
				for(int i=0; i<this->object_texture_size; i++)
					delete[] this->object_texture[i];
				delete[] this->object_texture;
			}
			this->object_texture_size = 0;
			std::string line="";
			std::string v_data="";
			for(int i=0;i<this->obj_data_size;i ++){
				line += this->obj_data[i];
				if(this->obj_data[i] == '\n'){
					if(line[0] == 'v' && line[1] == 't'){
						v_data += line;
						this->object_texture_size++;
					}
					line="";
				}
			}
			if(line[0] == 'v' && line[1] == 't'){
				v_data += line;
				this->object_texture_size++;
			}

			this->object_texture = new float*[this->object_texture_size];
			for(int i=0;i<this->object_texture_size;i++)
				this->object_texture[i] = new float[2];
			
			int I=0;
			line="";
			for(int i=0; i<v_data.length(); i++){
				line += v_data[i];
				if(v_data[i] == '\n'){
					if(line.length() <= 2) return false;
					std::string val="";
					int J=0;
					for(int j=3; j<line.length(); j++){	
						if((line[j] == ' ' || line[j] == '\n') && J < 2 && val.length() >= 3){
							this->object_texture[I][J] = std::stof(val.c_str());
							J++;
							val="";
							continue;
						}
						val+=line[j];
					}
					I++;
					line = "";
				}
			}
			return true;
		}	

		bool import_obj_normal(void){
			if(this->object_normal){
				for(int i=0; i<this->object_normal_size; i++)
					delete[] this->object_normal[i];
				delete[] this->object_normal;
			}
			this->object_normal_size = 0;
			std::string line="";
			std::string v_data="";
			for(int i=0;i<this->obj_data_size;i ++){
				line += this->obj_data[i];
				if(this->obj_data[i] == '\n'){
					if(line[0] == 'v' && line[1] == 'n'){
						v_data += line;
						this->object_normal_size++;
					}
					line="";
				}
			}
			if(line[0] == 'v' && line[1] == 'n'){
				v_data += line;
				this->object_normal_size++;
			}

			this->object_normal = new float*[this->object_normal_size];
			for(int i=0;i<this->object_normal_size;i++)
				this->object_normal[i] = new float[3];
			
			int I=0;
			line="";
			for(int i=0; i<v_data.length(); i++){
				line += v_data[i];
				if(v_data[i] == '\n'){
					if(line.length() <= 2) return false;
					std::string val="";
					int J=0;
					for(int j=3; j<line.length(); j++){	
						if((line[j] == ' ' || line[j] == '\n') && J < 3 && val.length() >= 3){
							this->object_normal[I][J] = std::stof(val.c_str());
							J++;
							val="";
							continue;
						}
						val+=line[j];
					}
					I++;
					line = "";
				}
			}
			return true;
		}	
	
		bool import_obj_face(void){
			if(this->object_face){
				for(int i=0; i<this->object_face_size; i++){
					delete[] this->object_face[i].v_index;
					delete[] this->object_face[i].vt_index;
					delete[] this->object_face[i].vn_index;
				}
				delete[] this->object_face;
			}
			this->object_face_size = 0;
			std::string line="";
			std::string v_data="";
			for(int i=0;i<this->obj_data_size;i ++){
				line += this->obj_data[i];
				if(this->obj_data[i] == '\n'){
					if(line[0] == 'f' && line[1] == ' '){
						v_data += line;
						this->object_face_size++;
					}
					line="";
				}
			}

			if(line[0] == 'f' && line[1] == ' '){
				v_data += line;
				this->object_face_size++;
			}

			this->object_face = new WavefrontFace [this->object_face_size];
			
			line="";
			int I=0;
			for(int i=0; i<v_data.length(); i++){
				line += v_data[i];
				if(v_data[i] == '\n'){
					this->object_face[I].count=0;
					for(int j=2;j<line.length(); j++){
						if(line[j] == ' ' || line[j] == '\n') 
							this->object_face[I].count++;
					}
					this->object_face[I].v_index = new int[this->object_face[I].count];
					this->object_face[I].vt_index = new int[this->object_face[I].count];
					this->object_face[I].vn_index = new int[this->object_face[I].count];
					I++;
					line="";
				}
			}

			line="";
			I=0;
			for(int i=0; i<v_data.length(); i++){
				line += v_data[i];
				if(v_data[i] == '\n'){
					std::string element="";
					int J=0;
					for(int j=2;j<line.length(); j++){
						element += line[j];
						if(line[j] == ' ' || line[j] == '\n'){
							element[element.length()-1]='/';
							std::string data="";
							int ctx=0;
							for(int k=0; k<element.length(); k++){
								if(element[k] == '/'){
									switch(ctx){
										case 0:
											this->object_face[I].v_index[J] = std::stoi(data.c_str());
											break;
										case 1:
											this->object_face[I].vt_index[J] = std::stoi(data.c_str());
											break;
										case 2:
											this->object_face[I].vn_index[J] = std::stoi(data.c_str());
											break;
									}
									ctx++;
									data = "";
								}else{
									data += element[k];
								}
								
							}
							J++;
							element = "";
						}
					}
					I++;
					line="";
				}
			}

			
			return true;
		}	
		
		bool import_obj_name(void){
			std::string line="";
			std::string v_data="";
			for(int i=0;i<this->obj_data_size;i ++){
				line += this->obj_data[i];
				if(this->obj_data[i] == '\n'){
					if(line[0] == 'o'){
						for(int j=2; j<line.length(); j++){
							if(line[j] == '\n') break;
							v_data += line[j];
						}
						break;
					}
					line="";
				}
			}
			
			this->object_name = v_data;
			
			return true;
		}	

		bool import_obj_mtllib(void){
			this->object_mtllib = "";
			std::string line="";
			std::string v_data="";
			for(int i=0;i<this->obj_data_size;i ++){
				line += this->obj_data[i];
				if(this->obj_data[i] == '\n'){
					if(line.length() > 6){
						std::string data="";
						for(int j=0; j<6; j++)
							data+=line[j];
						if(!strncmp(data.c_str(), "mtllib", 6)){
							data = "";
							for(int j=7; j<line.length()-1; j++)
								data+=line[j];
							this->object_mtllib = data;
							return true;
						}
					}
					line="";
				}
			}
			return false;
		}	

		bool import_obj_material(void){
			this->object_material = "";
			std::string line="";
			std::string v_data="";
			for(int i=0;i<this->obj_data_size;i ++){
				line += this->obj_data[i];
				if(this->obj_data[i] == '\n'){
					if(line.length() > 6){
						std::string data="";
						for(int j=0; j<6; j++)
							data+=line[j];
						if(!strncmp(data.c_str(), "usemtl", 6)){
							data = "";
							for(int j=7; j<line.length()-1; j++)
								data+=line[j];
							this->object_material = data;
							return true;
						}
					}
					line="";
				}
			}
			return false;
		}	
		bool import_obj_vertex(void){
			if(this->object_vertex){
				for(int i=0; i<this->object_vertex_size; i++)
					delete[] this->object_vertex[i];
				delete[] this->object_vertex;
			}
			this->object_vertex_size = 0;
			std::string line="";
			std::string v_data="";
			for(int i=0;i<this->obj_data_size;i ++){
				line += this->obj_data[i];
				if(this->obj_data[i] == '\n'){
					if(line[0] == 'v' && line[1] == ' '){
						v_data += line;
						this->object_vertex_size++;
					}
					line="";
				}
			}
			if(line[0] == 'v' && line[1] == ' '){
				v_data += line;
				this->object_vertex_size++;
			}

			this->object_vertex = new float*[this->object_vertex_size];
			for(int i=0;i<this->object_vertex_size;i++)
				this->object_vertex[i] = new float[3];
			
			int I=0;
			line="";
			for(int i=0; i<v_data.length(); i++){
				line += v_data[i];
				if(v_data[i] == '\n'){
					if(line.length() <= 2) return false;
					std::string val="";
					int J=0;
					for(int j=2; j<line.length(); j++){	
						if((line[j] == ' ' || line[j] == '\n') && J < 3 && val.length() >= 3){
							this->object_vertex[I][J] = std::stof(val.c_str());
							J++;
							val="";
							continue;
						}
						val+=line[j];
					}
					I++;
					line = "";
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
			this->gl_stride=cellSize;
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
		std::string object_name="";
		float **object_vertex=NULL;
		size_t object_vertex_size=0;
		float **object_texture=NULL;
		size_t object_texture_size=0;
		float **object_normal=NULL;
		size_t object_normal_size=0;
		WavefrontFace *object_face=NULL;
		size_t object_face_size=0;
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


		char *obj_data=NULL;
		size_t obj_data_size=0;		

		char *mtl_data=NULL;
		size_t mtl_data_size=0;

		float *gl_buffer=NULL;
		size_t gl_buffer_size=0;
		size_t gl_stride=0;

		WavefrontObject(void){

		}

		bool import_obj(const char *mtlTarget, float **v, size_t vSize, float **vn, size_t vnSize, float **vt, size_t vtSize, char *data, size_t dataSize){
			this->obj_data = data;
			this->obj_data_size = dataSize;
			this->object_vertex = v;
			this->object_vertex_size = vSize;
			this->object_normal = vn;
			this->object_normal_size = vnSize;
			this->object_texture = vt;
			this->object_texture_size = vtSize;
			
                        this->object_mtllib = mtlTarget;
			this->import_obj_name();
			this->import_obj_face();
                        this->import_obj_material();
                        if(this->import_mtl() == false){
                                printf("Failed to  import material.\n");
                                return false;
                        }
			

			return true;
		}
		bool import_obj(const char *mtlTarget, char *data, size_t data_size){
			this->obj_data = data;
                        this->obj_data_size = data_size;
			this->import_obj_name();
                        this->import_obj_vertex();
                        this->import_obj_normal();
                        this->import_obj_texture();
                        this->import_obj_face();
                        this->import_obj_material();
                        this->import_obj_mtllib();
			this->object_mtllib = mtlTarget;
			this->material_name = this->object_material;
			if(this->import_mtl() == false){
				printf("Failed to  import material.\n");
				return false;
			}

			return true;
		}
		bool import_obj(char *data, size_t data_size){
			this->obj_data = data;
			this->obj_data_size = data_size;
			
			this->import_obj_name();
			this->import_obj_vertex();
			this->import_obj_normal();
			this->import_obj_texture();
			this->import_obj_face();
			this->import_obj_material();
			this->import_obj_mtllib();

			this->material_name = this->object_material;
			if(this->import_mtl() == false){
				printf("Failed to  import material.\n");
				return false;
			}
			return true;
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

		bool buildGlBuffer(const char *fmt){
			// Determine size of formatted string
			std::string fmt_str="";
			if(strlen(fmt) <= 0){ 
				fmt_str = "vtn";
			}else{
				fmt_str = fmt;
			}
	
			// determine total size relative to face count and format cell size.
			size_t formatCellSize = this->getCellLength(fmt_str.c_str());
			this->gl_buffer_size = 0;
			if(this->gl_buffer){
				delete[] this->gl_buffer;
				this->gl_buffer = NULL;
			}
			size_t pointCount = 0;
			for(int i=0; i<this->object_face_size; i++){
				for(int j=0; j<this->object_face[i].count; j++){
					this->gl_buffer_size += formatCellSize;
					pointCount++;
				}
			}
			this->gl_buffer = new float[this->gl_buffer_size];

			int gl=0;
			int activeFace=0;
			int activePoint=0;
			for(int i=0; i<pointCount; i++){
				if(activeFace >= this->object_face_size) break;
				if(activePoint >= this->object_face[activeFace].count) break;
				int vertexIdx = this->object_face[activeFace].v_index[activePoint]-1;
				int textureIdx = this->object_face[activeFace].vt_index[activePoint]-1;
				int normalIdx = this->object_face[activeFace].vn_index[activePoint]-1;
				
				for(int F=0; F<strlen(fmt); F++){
					switch(this->parseBufferFormat(fmt, F)){
						case WF_FMT_V:
							if(gl >=this->gl_buffer_size ||  gl+2 >= this->gl_buffer_size){
								break;
							}
							if(vertexIdx >= this->object_vertex_size) {
								continue;
							}
							this->gl_buffer[gl] = this->object_vertex[vertexIdx][0];
							gl++;
							this->gl_buffer[gl] = this->object_vertex[vertexIdx][1];
							gl++;
							this->gl_buffer[gl] = this->object_vertex[vertexIdx][2];
							gl++;
						break;
						case WF_FMT_T:
							if(textureIdx >= this->object_texture_size) {continue;}
							if(gl >=this->gl_buffer_size || gl+1 >= this->gl_buffer_size) continue;
							this->gl_buffer[gl] = this->object_texture[textureIdx][0];
							gl++;
							this->gl_buffer[gl] = this->object_texture[textureIdx][1];
							gl++;
						break;
						case WF_FMT_N:
							if(normalIdx >= this->object_normal_size) {continue;}
							if(gl >=this->gl_buffer_size || gl+2 >= this->gl_buffer_size) break;
                                                               this->gl_buffer[gl] = this->object_normal[normalIdx][0];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->object_normal[normalIdx][1];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->object_normal[normalIdx][2];
                                                               gl++;
						break;
						case WF_FMT_A:
							if(gl >=this->gl_buffer_size || gl+2 >= this->gl_buffer_size) break;
                                                               this->gl_buffer[gl] = this->material_ka[0];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->material_ka[1];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->material_ka[2];
                                                               gl++;
						break;
						case WF_FMT_D:
							if(gl >=this->gl_buffer_size ) break;
                                                               this->gl_buffer[gl] = this->material_d;
                                                               gl++;
						break;
						case WF_FMT_S:
							if(gl >=this->gl_buffer_size ) break;
                                                               this->gl_buffer[gl] = this->material_ns;
                                                               gl++;
						break;
						case WF_FMT_E:
							if(gl >=this->gl_buffer_size || gl+2 >= this->gl_buffer_size) break;
                                                               this->gl_buffer[gl] = this->material_ke[0];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->material_ke[1];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->material_ke[2];
                                                               gl++;
						break;
						case WF_FMT_1:
							if(gl >=this->gl_buffer_size) break;
                                                               this->gl_buffer[gl] = 0.0;
                                                               gl++;
						break;
						case WF_FMT_2:
							if(gl >=this->gl_buffer_size || gl+1 >= this->gl_buffer_size) break;
                                                               this->gl_buffer[gl] = 0.0;
                                                               gl++;
                                                               this->gl_buffer[gl] = 0.0;
                                                               gl++;
						break;
						case WF_FMT_3:
							if(gl >=this->gl_buffer_size || gl+2 >= this->gl_buffer_size) break;
                                                               this->gl_buffer[gl] = 0.0;
                                                               gl++;
                                                               this->gl_buffer[gl] = 0.0;
                                                               gl++;
                                                               this->gl_buffer[gl] = 0.0;
                                                               gl++;
						break;
						case WF_FMT_4:
							if(gl >=this->gl_buffer_size || gl+3 >= this->gl_buffer_size) break;
                                                               this->gl_buffer[gl] = 0.0;
                                                               gl++;
                                                               this->gl_buffer[gl] = 0.0;
                                                               gl++;
                                                               this->gl_buffer[gl] = 0.0;
                                                               gl++;
                                                               this->gl_buffer[gl] = 0.0;
                                                               gl++;
						break;
					}
				}
                        	activePoint++;
				if(activePoint >= this->object_face[activeFace].count){
                        		activePoint=0;
					activeFace++;
					if(activeFace >= this->object_face_size) activeFace = 0;
				}
			}
			return true;
		}

};
