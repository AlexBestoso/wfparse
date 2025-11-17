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

#include "./objCollection.class.h"


class WavefrontImport{
	private:
		std::string objFileName="";
		std::string mtlFileName="";

		char *objFileBuffer=NULL;
		char *mtlFileBuffer=NULL;

		size_t objFileSize=0;
		size_t mtlFileSize=0;


		bool debugOutput = true; // enable verbose usage.

		bool importSuccessful=false;

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

		std::string collectionName = "";
		/*
		 * Assumes invoking function handled inputs.
		 * */
		bool _import(void){
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
			this->collectionName = collectionName;
			

			if(!this->processObj(collectionName)){
				if(this->objFileBuffer){
                                	delete[] this->objFileBuffer;
                                	this->objFileBuffer = NULL;
                        	}
				return false;
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

	public:
		WavefrontObjectCollection objectCollection;

		int error=0;
		std::string errorMessage="";

		bool failed(void){
			if(error > 0) return true;
			return false;
		}
		
		void dumpObjFile(){
			printf("\n===Imported %ld Objects From Collection %s===\n", (long)this->objectCollection.data.objectCount, this->objectCollection.data.name.c_str());
			for(int i=0; i<this->objectCollection.data.objectCount; i++){
				WavefrontObject object = this->objectCollection.data.obj[i];
				printf("Object Index %d, %s\n", i, object.getObjectName().c_str());
        			printf("Object Material %s\n", object.getMaterialName().c_str());
        			printf("Object has %ld faces\n", (long)object.getFaceCount());

			}
		}

		void dumpMtlFile(){
			printf("\n===Imported %ld Materials From Collection %s\n", this->objectCollection.material.data.materialCount, this->objectCollection.material.data.name.c_str());
			for(int i=0; i<this->objectCollection.material.data.materialCount; i++){
		        	wf_material_t material = this->objectCollection.material.data.materials[i];
				printf("Material Name : %s\n", material.material_name.c_str());
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
        			printf("Material Texture Image: %s\n", (material.material_map_kd == "" ) ? "\n" : material.material_map_kd.c_str());
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
	/*	bool genBuffer_format(const char *fmt, int objIndex){
			if(objIndex == -1 || objIndex >= this->objCount) return false;
			this->objList[objIndex].buildGlBuffer(fmt);
			return true;
		}*/

		
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

		WavefrontObject getObject(int id){
			WavefrontObject ret;

			return ret;	
		}

		int getObjectCount(void){
			return this->objectCollection.data.objectCount;
		}

		int getMtlCount(void){
			return this->objectCollection.material.data.materialCount;
		}

		bool sucessful(void){
			return this->importSuccessful;
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
			this->importSuccessful = this->_import();
			return this->importSuccessful;
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
			this->importSuccessful = this->_import();
			return this->importSuccessful;
		}

		void setExportFormat(std::string v){
			this->objectCollection.format = v;
		}
};
