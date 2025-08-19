#include "./obj.class.h"


class WavefrontImport{
	private:
		int openAndRead(const char *target, int *fd, char *out, size_t out_size){
			if(!(fd[0] = open(target, O_RDONLY))){
				printf("Failed to read '%s'", target);
				return 0;
			}
			struct stat st;
			if(fstat(fd[0], &st)){
				printf("Failed to get file stats\n");
				close(fd[0]);
			}
			int ret  = st.st_size;
			if(ret >= out_size){
				ret = out_size;
			}
			
			memset(out, 0x00, out_size);
			read(fd[0], out, ret);
			close(fd[0]);
			return ret;
		}

		std::string objFileName="";
		int objFileFd=-1;
		char *objFileBuffer=NULL;
		size_t objFileSize=0;
	
		std::string mtlFileName="";
		int mtlFileFd=-1;
		bool getRawFileData(const char *targetName){
			this->objFileName = targetName;
			size_t nameLength = this->objFileName.length();
			if(nameLength <= 4) return false;
			this->mtlFileName = this->objFileName;
			this->mtlFileName[nameLength-3] = 'm';
			this->mtlFileName[nameLength-2] = 't';
			this->mtlFileName[nameLength-1] = 'l';
			
			if(this->objFileBuffer){ 
				free(this->objFileBuffer);
				this->objFileBuffer = NULL;
			}
			this->objFileBuffer = new char[1000000];
			this->objFileSize = this->openAndRead(this->objFileName.c_str(), &this->objFileFd, this->objFileBuffer, 1000000);	
			
			
			return true;
		}

		void countObjects(){
			std::string line="";
			for(int i=0; i<this->objFileSize; i++){
				line += this->objFileBuffer[i];
				if(this->objFileBuffer[i] == '\n'){
					if(line[0] == 'o') this->objCount++;
					line = "";
				}
			}
		}
		void countV(){
			std::string line="";
			std::string v_data="";
			if(this->vCache){
				for(int i=0; i<this->vCount; i++){
					delete[] this->vCache[i];
				}
				delete[] this->vCache;
			}

			for(int i=0; i<this->objFileSize; i++){
				line += this->objFileBuffer[i];
				if(this->objFileBuffer[i] == '\n'){
					if(line[0] == 'v' && line[1] == ' '){
						v_data += line;
						this->vCount++;
					}
					line = "";
				}
			}
				
			this->vCache = new float*[this->vCount];
			for(int i=0; i<this->vCount; i++)
				this->vCache[i] = new float[3];


			
			int I=0;
			line="";
			for(int i=0; i<v_data.length(); i++){
				line += v_data[i];
				if(v_data[i] == '\n'){
					if(line.length() <= 2) return;
					std::string val="";
					int J=0;
					for(int j=2; j<line.length(); j++){	
						if((line[j] == ' ' || line[j] == '\n') && J < 3 && val.length() >= 3){
							this->vCache[I][J] = std::stof(val.c_str());
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
		}

		void countVn(){
			std::string line="";
			std::string v_data="";
			if(this->vnCache){
				for(int i=0; i<this->vnCount; i++){
					delete[] this->vnCache[i];
				}
				delete[] this->vnCache;
			}

			for(int i=0; i<this->objFileSize; i++){
				line += this->objFileBuffer[i];
				if(this->objFileBuffer[i] == '\n'){
					if(line[0] == 'v' && line[1] == 'n'){
						v_data+=line;
						this->vnCount++;
					}
					line = "";
				}
			}
			this->vnCache = new float*[this->vnCount];
			for(int i=0; i<this->vnCount; i++)
				this->vnCache[i] = new float[3];

			int I=0;
                        line="";
                        for(int i=0; i<v_data.length(); i++){
                                line += v_data[i];
                                if(v_data[i] == '\n'){
                                        if(line.length() <= 2) return;
                                        std::string val="";
                                        int J=0;
                                        for(int j=3; j<line.length(); j++){
                                                if((line[j] == ' ' || line[j] == '\n') && J < 3 && val.length() >= 3){
                                                        this->vnCache[I][J] = std::stof(val.c_str());
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
		}
		
		void countVt(){
			std::string line="";
			std::string v_data="";
			if(this->vtCache){
				for(int i=0; i<this->vtCount; i++){
					delete[] this->vtCache[i];
				}
				delete[] this->vtCache;
				this->vtCache = NULL;
			}
			this->vtCount=0;
			for(int i=0; i<this->objFileSize; i++){
				line += this->objFileBuffer[i];
				if(this->objFileBuffer[i] == '\n'){
					if(line[0] == 'v' && line[1] == 't') {
						v_data += line;
						this->vtCount++;
					}
					line = "";
				}
			}
			this->vtCache = new float*[this->vtCount];
			for(int i=0; i<this->vtCount; i++)
				this->vtCache[i] = new float[2];

			int I=0;
                        line="";
                        for(int i=0; i<v_data.length(); i++){
                                line += v_data[i];
                                if(v_data[i] == '\n'){
                                        if(line.length() <= 2) return;
                                        std::string val="";
                                        int J=0;
                                        for(int j=3; j<line.length(); j++){
                                                if((line[j] == ' ' || line[j] == '\n') && J < 2 && val.length() >= 3){
                                                        this->vtCache[I][J] = std::stof(val.c_str());
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
			
		}
	public:
		WavefrontObject waveobj;
		size_t objCount=0;
		size_t vCount=0;
		size_t vnCount=0;
		size_t vtCount=0;
		WavefrontObject *objList=NULL;
		float **vCache=NULL;
		float **vnCache=NULL;
		float **vtCache=NULL;
		float *gl_buffer;
		size_t gl_buffer_size;
		size_t gl_stride=0;

		void dumpObjFile(){
			printf("Loaded file (%ld bytes) '%s'\n", this->objFileSize, this->objFileName.c_str());
			
			for(int j=0; j<this->objCount; j++){
				printf("Imported Object Name '%s'\n", this->objList[j].object_name.c_str());
                		printf("Imported %ld vertecies\n", this->objList[j].object_vertex_size);
				for(int i=0; i<this->objList[j].object_vertex_size; i++){
					printf("(%f, %f, %f)\n", this->objList[j].object_vertex[i][0], this->objList[j].object_vertex[i][1], this->objList[j].object_vertex[i][2]);
				}
                		printf("Imported %ld texteture points\n", this->objList[j].object_texture_size);
				for(int i=0; i<this->objList[j].object_texture_size; i++){
					printf("(%f, %f)\n", this->objList[j].object_texture[i][0], this->objList[j].object_texture[i][1]);
				}
                		printf("Imported %ld normals\n", this->objList[j].object_normal_size);
				for(int i=0; i<this->objList[j].object_normal_size; i++){
					printf("(%f, %f, %f)\n", this->objList[j].object_normal[i][0], this->objList[j].object_normal[i][1], this->objList[j].object_normal[i][2]);
				}
                		printf("Imported %ld faces\n", this->objList[j].object_face_size);
				for(int i=0; i<this->objList[j].object_face_size; i++){
					printf("\tface %d: ", i);
					printf("(");
					for(int k=0; k<this->objList[j].object_face[i].count; k++){
						printf("%d ", this->objList[j].object_face[i].v_index[k]);
					}
					printf(") ");
					printf("(");
					for(int k=0; k<this->objList[j].object_face[i].count; k++){
						printf("%d ", this->objList[j].object_face[i].vt_index[k]);
					}
					printf(") ");
					printf("(");
					for(int k=0; k<this->objList[j].object_face[i].count; k++){
						printf("%d ", this->objList[j].object_face[i].vn_index[k]);
					}
					printf(")\n");
				}			
	
				printf("Target Material File : '%s'\n", this->objList[j].object_mtllib.c_str());
				printf("Target Material Name : '%s'\n", this->objList[j].object_material.c_str());

			}
			printf("\n\n");
		}

		void dumpMtlFile(){
			for(int i=0; i<this->objCount; i++){
				printf("\n===%s's Material '%s'===\n", this->objList[i].object_name.c_str(), this->objList[i].material_name.c_str());
				printf("Material Ns : %f\n", this->objList[i].material_ns);
				printf("Material Ka : (%f, %f, %f)\n", this->objList[i].material_ka[0], this->objList[i].material_ka[1], this->objList[i].material_ka[2]);
				printf("Material Kd : (%f, %f, %f)\n", this->objList[i].material_kd[0], this->objList[i].material_kd[1], this->objList[i].material_kd[2]);
				printf("Material Ks : (%f, %f, %f)\n", this->objList[i].material_ks[0], this->objList[i].material_ks[1], this->objList[i].material_ks[2]);
				printf("Material Ke : (%f, %f, %f)\n", this->objList[i].material_ke[0], this->objList[i].material_ke[1], this->objList[i].material_ke[2]);
				printf("Material Ni : %f\n", this->objList[i].material_ni);
				printf("Material d : %f\n", this->objList[i].material_d);
				printf("Material illum : %f\n", this->objList[i].material_illum);
				printf("Material map_Kd : '%s'\n", this->objList[i].material_map_kd.c_str());
			}

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
	
		/*
		 * Supported formats
		 * Key: 
			0 v=vertex, only one 
			1 t=texture, only one
			2 n=normal, only one
			3 a=color, sourced from material Na
			4 d=color, sourced from material Nd
			5 s=color, sourced from material Ns
			6 e=color, sourced from material Ne
			7 1-4=padding, add x digits of 0 padding. min:1, max:4. More than one supported. 
			8 D=color, sourced from material Kd
			EMPTY=vtn. an empty format gives vertex, texture, normal
			
		* *
		 * There should be no spaces between items.
		 * An empty string will build the format 'vtn'
		 * */
		bool genBuffer_format(const char *fmt, int objIndex){
			if(objIndex == -1 || objIndex >= this->objCount) return false;
			this->objList[objIndex].buildGlBuffer(fmt);
			return true;
		}

		
		std::string getObjectDataByIndex(int idx){
			std::string ret = "";
			bool found=false;
			std::string line ="";
			std::string mtlLine="";
			int I=0;
			for(int i=0; i<this->objFileSize; i++){
				line += this->objFileBuffer[i];
                                if(this->objFileBuffer[i] == '\n'){
					if(line.length() >= 6){
						std::string data="";
						for(int j=0;j<6; j++) data += line[j];
						if(!strncmp(data.c_str(), "mtllib", 6)){
							mtlLine = line;
						}
					}
					if(found){
                                                if(line[0] == 'o'){
							return mtlLine+ret;
                                                }
						ret += line;
						line="";
						continue;
					}
					
					if(line[0] == 'o'){
						if(I==idx){
							found = true;
							ret = line;
						}
						I++;
					}
					line = "";
				}
			}
			return mtlLine+ret;
		}
		
		bool new_import(const char *target){
			this->getRawFileData(target);
			this->countObjects();
			this->countV();
			this->countVn();
			this->countVt();
			this->objList = new WavefrontObject[this->objCount];
			
			std::string data = this->getObjectDataByIndex(0);
			std::string mtlTarget = "";
			std::string line = "";
			for(int i=0; i<data.length(); i++){
				if(mtlTarget != "" && data[i] == '\n'){
					mtlTarget = line;
					break;
				}
				line += data[i];
				if(line.length() == 6 && !strncmp(line.c_str(), "mtllib", 6)){
					mtlTarget = "ready";
					line="";
					i++;
				}else if(data[i] == '\n'){
					line = "";
				}
			}
			for(int i=0; i<this->objCount; i++){
				data = this->getObjectDataByIndex(i);
				this->objList[i].import_obj(mtlTarget.c_str(), 
								this->vCache, this->vCount, 
								this->vnCache, this->vnCount,
								this->vtCache, this->vtCount, 
								(char *)data.c_str(), data.length());
			}	
			return true;
		}
		bool import(const char *target){
			this->getRawFileData(target);
			this->waveobj.import_obj(this->objFileBuffer, this->objFileSize);
			return true;
		}
};
