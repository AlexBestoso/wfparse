#define WF_MAX_BUFFER_SIZE 1000000
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

#include "./obj.class.h"


class WavefrontImport{
	private:
		std::string objFileName="";
		std::string mtlFileName="";

		char *objFileBuffer=NULL;
		char *mtlFileBuffer=NULL;

		size_t objFileSize=0;
		size_t mtlFileSize=0;


		bool debugOutput = true; // enable verbose usage.

		void setError(const char *msg, int code){
			this->errorMessage = msg;
			this->error = code;	
		}
		void clearError(){
			this->errorMessage = "";
			this->error = 0;
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
		
		size_t computeVboStride(const char *fmt){
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
			this->vboStride=cellSize;
			return cellSize;
		}

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

		int objFileFd=-1;
	
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
			this->objFileBuffer = new char[WF_MAX_BUFFER_SIZE];
			this->objFileSize = this->openAndRead(this->objFileName.c_str(), &this->objFileFd, this->objFileBuffer, WF_MAX_BUFFER_SIZE);	
			
			
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
		
		/*
		 * Assumes that the object file buffer has been allocated.
		 * */
		bool processObj(std::string collectionName){
			if(!this->objectCollection.parse(collectionName, this->objFileBuffer, this->objFileSize)){
				std::string msg = "WavefrontImport::processObj(void) - Failed to parse object collection data.\n\t" + 
				this->objectCollection.errorMessage;
				this->setError(msg.c_str(), 1);
				return false;
			}
			return true;
		}

		/*
                 * Assumes that the material file buffer has been allocated.
                 * */
                bool processMtl(std::string collectionName){
			if(!this->objectCollection.parseMaterial(collectionName, this->mtlFileBuffer, this->mtlFileSize)){
                                std::string msg = "WavefrontImport::processMtl(void) - Failed to parse material collection data.\n\t" +
                                this->objectCollection.errorMessage;
                                this->setError(msg.c_str(), 1);
                                return false;
                        }
                        return true;
                }

		/*
		 * Assumes invoking function handled inputs.
		 * */
		bool _import(void){
			if(this->debugOutput) printf("[DEBUG] Importing %s and %s\n", this->objFileName.c_str(), this->mtlFileName.c_str());

			int fd = open(this->objFileName.c_str(), O_RDONLY);
			if(!fd){
				setError("WavefrontImport::_import(void) - Failed to open object file.", 1);
				return false;
			}

			struct stat st;
			if(fstat(fd, &st)){
				setError("WavefrontImport::_import(void) - Failed to get object file statistics.", 2);
				close(fd);
				return false;
			}
			this->objFileSize = st.st_size;
			if(this->objFileSize <= 0){
				setError("WavefrontImport::_import(void) - Object file is empty", 3);
				close(fd);
				return false;
			}
			if(this->objFileBuffer){
				delete[] this->objFileBuffer;
				this->objFileBuffer = NULL;
			}

			this->objFileBuffer = new char[this->objFileSize];
			if(read(fd, this->objFileBuffer, this->objFileSize) < this->objFileSize){
				setError("WavefrontImport::_import(void) - Failed to read object file's data.", 4);
				close(fd);
				return false;
			}
			close(fd);

			std::string collectionName = "";
			for(int i=0; i<this->objFileName.length(); i++){
				collectionName += this->objFileName[i];
				if(this->objFileName[i] == '/') collectionName = "";
			}
			

			if(!this->processObj(collectionName)){
				if(this->objFileBuffer){
                                	delete[] this->objFileBuffer;
                                	this->objFileBuffer = NULL;
                        	}
				return false;
			}

			if(this->objFileBuffer){
                                delete[] this->objFileBuffer;
                        	this->objFileBuffer = NULL;
                        }

			// mtl import
			fd = open(this->mtlFileName.c_str(), O_RDONLY);
                        if(!fd){
                                setError("WavefrontImport::_import(void) - Failed to open material file.", 1);
                                return true;
                        }

                        if(fstat(fd, &st)){
                                setError("WavefrontImport::_import(void) - Failed to get material file statistics.", 2);
				close(fd);
                                return false;
                        }
                        this->mtlFileSize = st.st_size;
                        if(this->mtlFileSize <= 0){
                                setError("WavefrontImport::_import(void) - Material file is empty", 3);
                                close(fd);
                                return false;
                        }
                        if(this->mtlFileBuffer){
                                delete[] this->mtlFileBuffer;
                                this->mtlFileBuffer = NULL;
                        }

                        this->mtlFileBuffer = new char[this->mtlFileSize];
                        if(read(fd, this->mtlFileBuffer, this->mtlFileSize) < this->mtlFileSize){
                                setError("WavefrontImport::_import(void) - Failed to read material file's data.", 4);
                                close(fd);
                                return false;
                        }
                        close(fd);

			collectionName = "";
                        for(int i=0; i<this->mtlFileName.length(); i++){
                                collectionName += this->mtlFileName[i];
                                if(this->mtlFileName[i] == '/') collectionName = "";
                        }
	
                        if(!this->processMtl(collectionName)){
                                if(this->mtlFileBuffer){
                                        delete[] this->mtlFileBuffer;
                                        this->mtlFileBuffer = NULL;
                                }
                                return false;
                        }

			if(this->mtlFileBuffer){
                               	delete[] this->mtlFileBuffer;
                        	this->mtlFileBuffer = NULL;
                        }
			

			return true;
		}

		void getVboSize(){
			this->vboSize = 0;
			if(this->vboData){
				delete[] this->vboData;
				this->vboData=NULL;
			}

			for(int i=0; i<this->objectCollection.data.objectCount; i++){
				wf_object_t object = this->objectCollection.data.objects[i];
				for(int j=0; j<object.faceCount; j++){
					wf_face_t face = object.faces[j];
					this->vboSize += this->vboStride*face.pointsCount;
				}
			}
		}
		void allocateVbo(){
			if(this->vboSize <= 0) return;
			if(this->vboData){
				delete[] this->vboData;
				this->vboData=NULL;
			}
			
			this->vboData = new float[this->vboSize];
		}
	public:
		
		WavefrontObjectCollection objectCollection;
		float *vboData;
		size_t vboStride=0;
		size_t vboSize=0;

		int error=0;
		std::string errorMessage="";

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

		bool failed(void){
			if(error > 0) return true;
			return false;
		}

		void formatVbo(std::string format){
			int vboIndex=0;
			for(int j=0; j<this->objectCollection.data.objectCount; j++){
				wf_object_t object = this->objectCollection.data.objects[j];
				for(int k=0; k<object.faceCount; k++){
					wf_face_t face = object.faces[k];
					for(int a=0; a<face.pointsCount; a++){
						wf_point_t point = face.points[a];
						for(int i=0; i<format.length(); i++){
							if(vboIndex >= vboSize){
								return;
							}
							if(this->vboData == NULL) return;

							switch(this->parseBufferFormat(format.c_str(), i)){
								case WF_FMT_V:{
									if(vboIndex+2 >= this->vboSize) continue;
									int index = point.index_vertex-1;
									if(index >= this->objectCollection.data.vertexCount) continue;
									this->vboData[vboIndex] = this->objectCollection.data.vertexData[index][0];
									vboIndex++;
									this->vboData[vboIndex] = this->objectCollection.data.vertexData[index][1];
									vboIndex++;
									this->vboData[vboIndex] = this->objectCollection.data.vertexData[index][2];
									vboIndex++;
        	                        	        		break;
								}
        	                        	        	case WF_FMT_N:{
									if(vboIndex+2 >= this->vboSize) continue;
									int index = point.index_normal-1;
									if(index >= this->objectCollection.data.normalCount) continue;
									this->vboData[vboIndex] = this->objectCollection.data.normalData[index][0];
									vboIndex++;
									this->vboData[vboIndex] = this->objectCollection.data.normalData[index][1];
									vboIndex++;
									this->vboData[vboIndex] = this->objectCollection.data.normalData[index][2];
									vboIndex++;

                                		        		break;
								}
                                		        	case WF_FMT_T:{
									if(vboIndex+1 >= this->vboSize) continue;
									int index = point.index_texture-1;
									if(index >= this->objectCollection.data.textureCount) continue;
									this->vboData[vboIndex] = this->objectCollection.data.textureData[index][0];
									vboIndex++;
									this->vboData[vboIndex] = this->objectCollection.data.textureData[index][1];
									vboIndex++;
                                		        		break;
								}
                                		        	case WF_FMT_A:
                                		        		break;
                                		        	case WF_FMT_E:
                                		        		break;
                                		        	case WF_FMT_3:
                                		        		break;
                                		        	case WF_FMT_KD:
                                		        		break;
                                		        	case WF_FMT_2:
                                		        		break;
                       	  			   		case WF_FMT_D:
                        			                	break;
                        			                case WF_FMT_S:
                        			                	break;
                        			                case WF_FMT_1:
                        			                	break;
                        			                case WF_FMT_4:
                        			                	break;
							}
						}
					}
				}
			}
		}
	
		void compileBuffer(std::string format){
			this->computeVboStride(format.c_str());
			if(debugOutput) printf("[DEBUG] VBO Stride Length %ld\n", (long)this->vboStride);
			this->getVboSize();
			if(debugOutput) printf("[DEBUG] VBO Size : %ld\n", (long)this->vboSize);
			this->allocateVbo();
			if(debugOutput) printf("[DEBUG] Allocated vbo buffer\n");
			this->formatVbo(format);
			if(this->debugOutput) printf("[DEBUG] Vbo formatted.\n");
			
		}

		void dumpObjFile(){
			printf("\n===Imported %ld Objects From Collection %s===\n", (long)this->objectCollection.data.objectCount, this->objectCollection.data.name.c_str());
			for(int i=0; i<this->objectCollection.data.objectCount; i++){
				wf_object_t object = this->objectCollection.data.objects[i];
				printf("Object Index %d, %s\n", i, object.name.c_str());
        			printf("Object Material %s\n", object.material_name.c_str());
        			printf("Object has %ld faces\n", (long)object.faceCount);
				for(int j=0; j<object.faceCount; j++){
					wf_face_t face = object.faces[j];
					printf("face %d has %ld points\n", j, (long)face.pointsCount);
					for(int k=0; k<face.pointsCount; k++){
						printf("(v:%ld, vt:%ld, vn:%ld)\n", (long)face.points[k].index_vertex, (long)face.points[k].index_texture, (long)face.points[k].index_normal);
					}
				}

				printf("\n");
			}
		}

		void dumpMtlFile(){
			printf("\n===Imported %ld Materials From Collection %s\n", this->objectCollection.material.data.materialCount, this->objectCollection.material.data.name.c_str());
			for(int i=0; i<this->objectCollection.material.data.materialCount; i++){
		        	wf_material_t material = this->objectCollection.material.data.materials[i];
				printf("Material Name : %s\n", material.material_name.c_str());
        			printf("Material Texture Image: %s\n", material.material_map_kd.c_str());
        			printf("Material Ns : %f\n", material.material_ns);
				printf("Material Ka : ");
				for(int j=0; j<3; j++)
	        			printf("%f ", material.material_ka[i]);
				printf("\n");
				printf("Material Kd : ");
				for(int j=0; j<3; j++)
	        			printf("%f ", material.material_kd[i]);
				printf("\n");
				printf("Material Ks : ");
				for(int j=0; j<3; j++)
        				printf("%f ", material.material_ks[i]);
				printf("\n");
				printf("Material Ke : ");
				for(int j=0; j<3; j++)
        				printf("%f ", material.material_ke[i]);
				printf("\n");
        			printf("Material Ni : %f\n", material.material_ni);
        			printf("Material d : %f\n", material.material_d);
        			printf("Material illum : %f\n", material.material_illum);
				printf("\n");
			}

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
		
		bool import(std::string objTarget, std::string mtlTarget){
			if(objTarget == ""){
                                this->setError("WavefrontImport::import(std::string objTartget, std::string mtlTarget) - objTarget is empty.", 1);
                                return false;
                        }

                        size_t targetLength = objTarget.length();
                        if(targetLength <= 4){
                                this->setError("WavefrontImport::import(std::string objTartget, std::string mtlTarget) - objTarget length is too short", 2);
                                return false;
                        }

                        this->objFileName = "";
                        if(objTarget[targetLength-1] == 'j' && objTarget[targetLength-2] == 'b' && objTarget[targetLength-3] == 'o' && objTarget[targetLength-4] == '.'){
                                this->objFileName = objTarget;
                        }else{
                                this->setError("WavefrontImport::import(std::string objTartget, std::string mtlTarget) - target is not a .obj file", 3);
                                return false;
                        }

			if(mtlTarget == ""){
                                this->setError("WavefrontImport::import(std::string objTartget, std::string mtlTarget) - mtlTarget is empty.", 1);
                                return false;
                        }

                        targetLength = mtlTarget.length();
                        if(targetLength <= 4){
                                this->setError("WavefrontImport::import(std::string objTartget, std::string mtlTarget) - mtlTarget length is too short", 2);
                                return false;
                        }

                        this->mtlFileName = "";
                        if(mtlTarget[targetLength-1] == 'l' && mtlTarget[targetLength-2] == 't' && mtlTarget[targetLength-3] == 'm' && mtlTarget[targetLength-4] == '.'){
                                this->mtlFileName = mtlTarget;
                        }else{
                                this->setError("WavefrontImport::import(std::string objTartget, std::string mtlTarget) - mtlTarget is not a .mtl file", 3);
                                return false;
                        }
			return this->_import();
		}

		bool import(std::string target){
			if(target == ""){
				this->setError("WavefrontImport::import(std::string target) - target is empty.", 1);
				return false;
			}

			size_t targetLength = target.length();
			if(targetLength <= 4){
				this->setError("WavefrontImport::import(std::string target) - target length is too short", 2);
				return false;
			}
			
			this->objFileName = "";
			if(target[targetLength-1] == 'j' && target[targetLength-2] == 'b' && target[targetLength-3] == 'o' && target[targetLength-4] == '.'){
				this->objFileName = target;
			}else{
				this->setError("WavefrontImport::import(std::string target) - target is not a .obj file", 3);
				return false;
			}

			target[targetLength-1] = 'l'; 
			target[targetLength-2] = 't'; 
			target[targetLength-3] = 'm';
			
			this->mtlFileName = target;
			if(this->debugOutput)
				printf("[DEBUG] Improting '%s' and '%s'\n", this->objFileName.c_str(), mtlFileName.c_str());
			return this->_import();
		}
};
