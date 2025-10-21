#include "./mtl.class.h"
#include "./obj.class.h"
struct Wavefront_Collection_Struct{
	float **masterVertexStore;
	size_t masterVertexStoreSize;
	float **masterTextureStore;
	size_t masterTextureStoreSize;
	float **masterNormalStore;
	size_t masterNormalStoreSize; 
	size_t objectCount;
	WavefrontObject *obj;
	std::string name; // collection name
	std::string mtllib; // target material library.
	
};
typedef struct Wavefront_Collection_Struct wf_collection_t;

class WavefrontObjectCollection{
	private:
		bool debugOutput = true;

		size_t fileBufferSize = 0;
		char *fileBuffer = NULL;

		bool validateBuffer(void){
			if(this->fileBuffer == NULL){
                                this->setError("WavefrontObjectCollection::parse(void) - file buffer is null.", 1);
                                return false;
                        }
                        if(this->fileBufferSize <= 0){
                                this->setError("WavefrontObjectCollection::parse(void) - file buffer size is too small.", 2);
                                return false;
                        }
			return true;
		}

		int countElement(std::string element){
			std::string line="";
			int ret=0;
                        for(int i=0; i<this->fileBufferSize; i++){
                                if(this->fileBuffer[i] == '\n'){
                                        if(this->validObjElement(line, element)){
						ret++;
                                        }
                                        line = "";
                                }else{
                                        line += this->fileBuffer[i];
                                }
                        }
			return ret;
		}

		void compileCoreValue(int start, int depth, int Index, std::string line, float **out){
			std::string grabber = "";
			for(int i=start, charIndex=0; i<line.length() && charIndex<depth; i++){
				if(line[i] == ' ' || line[i] == '\n'){
					out[Index][charIndex] = std::stof(grabber.c_str());
					charIndex++;
					grabber = "";
				}else{
					grabber += line[i];
				}
			}

		}

	
		bool buildMasterVertexStore(void){
			if(!this->validateBuffer()) return false;
			this->data.masterVertexStoreSize = this->countElement("v")+1;
			if(this->data.masterVertexStoreSize <= 0){
				return false;
			}
			this->data.masterVertexStore = new float *[this->data.masterVertexStoreSize];
			for(int i=0; i<this->data.masterVertexStoreSize; i++)
				this->data.masterVertexStore[i] = new float[3];

			std::string line="";
			for(int i=0, storeIndex=0; i<this->fileBufferSize && storeIndex<this->data.masterVertexStoreSize; i++){
				if(this->fileBuffer[i] == '\n'){
					if(this->validObjElement(line, "v")){
						line+= '\n';
						this->compileCoreValue(2, 3, storeIndex, line, this->data.masterVertexStore);
						storeIndex++;
					}
					line = "";
				}else{
					line += this->fileBuffer[i];
				}
			}
			return true;
		}

		bool buildMasterTextureStore(void){
			if(!this->validateBuffer()) return false;
			this->data.masterTextureStoreSize = this->countElement("vt") + 1;
			if(this->data.masterTextureStoreSize <= 0){
				return false;
			}
			this->data.masterTextureStore = new float *[this->data.masterTextureStoreSize];
			for(int i=0; i<this->data.masterTextureStoreSize; i++)
				this->data.masterTextureStore[i] = new float[2];
			
			std::string line = "";
			for(int i=0, storeIndex=0; i<this->fileBufferSize && storeIndex<this->data.masterTextureStoreSize; i++){
				if(this->fileBuffer[i] == '\n'){
					if(this->validObjElement(line, "vt")){
						line+= '\n';
						this->compileCoreValue(3, 2, storeIndex, line, this->data.masterTextureStore);
						storeIndex++;
					}
					line = "";
				}else{
					line += this->fileBuffer[i];
				}
			}
			
			return true;
		}

		bool buildMasterNormalStore(void){
			if(!this->validateBuffer()) return false;
			this->data.masterNormalStoreSize = this->countElement("vn")+1;
			if(this->data.masterNormalStoreSize <= 0){
				return false;
			}
			this->data.masterNormalStore = new float *[this->data.masterNormalStoreSize];
			for(int i=0; i<this->data.masterNormalStoreSize; i++)
				this->data.masterNormalStore[i] = new float[3];

			std::string line = "";
			for(int i=0, storeIndex=0; i<this->fileBufferSize && storeIndex<this->data.masterNormalStoreSize; i++){
				if(this->fileBuffer[i] == '\n'){
					if(this->validObjElement(line, "vn")){
						line+= '\n';
						this->compileCoreValue(3, 3, storeIndex, line, this->data.masterNormalStore);
						storeIndex++;
					}
					line = "";
				}else{
					line += this->fileBuffer[i];
				}
			}
			return true;
		}
		
		/*
		 * This function deallocates the face structs upon success
		 * */
		bool realizeObjects(void){
			for(int i=0; i<this->data.objectCount; i++){
				if(this->format != "") this->data.obj[i].setExportFormat(this->format);
				this->data.obj[i].realizeSelf(this->data.masterVertexStore, this->data.masterVertexStoreSize, this->data.masterTextureStore, this->data.masterTextureStoreSize, this->data.masterNormalStore, this->data.masterNormalStoreSize);
			}
			return true;
		}
		
		void initData(){
			this->data.name="";
			this->data.masterVertexStore = NULL;
        		this->data.masterVertexStoreSize = 0;
        		this->data.masterTextureStore = NULL;
        		this->data.masterTextureStoreSize = 0;
        		this->data.masterNormalStore = NULL;
        		this->data.masterNormalStoreSize=0;
        		this->data.objectCount=0;
        		this->data.obj=NULL;

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

		void countEntry(size_t *sizeOut, std::string target){
			sizeOut[0] = 0;
			std::string line = "";
			for(int i=0; i<this->fileBufferSize; i++){
				line += fileBuffer[i];
				if(fileBuffer[i] == '\n'){
					if(this->validObjElement(line, target)){
						sizeOut[0]++;
					}
					line = "";
				}
			}
		}
		
		size_t countFacePoints(std::string line){
			size_t ret = 0;
			for(int i=2; i<line.length(); i++){
				if(line[i] == ' ' || line[i] == '\n')
					ret++;
			}
			return ret;
		}
		
		void setError(const char *msg, int code){
			this->error = code;
			this->errorMessage = msg;
		}

		void fetchMtllib(void){
			std::string line= "";
			for(int i=0; i<this->fileBufferSize; i++){
				line += this->fileBuffer[i];
				if(this->fileBuffer[i] == '\n'){
					if(this->validObjElement(line, "mtllib")){
						std::string value="";
						for(int j=7; j<line.length(); j++){
							if(line[j] == '\n') break;
							value += line[j];
						}
						this->data.mtllib = value;
						return;
					}
					line ="";
				}
			}
		}
	public:
		wf_collection_t data;
		WavefrontMaterialCollection material;
		int error=0;
		std::string errorMessage="";
		std::string format="";

		bool failed(void){
			if(this->error > 0) return true;
			return false;
		}
		std::string getErrorMessage(void){
			return this->errorMessage;
		}

		/*
		 * Empty init
		 * */
		WavefrontObjectCollection(){
			this->setFileBuffer(NULL, 0);
			this->initData();	
		}
		
		/*
		 * Create object with with a buffer.
		 * */
		WavefrontObjectCollection(char *fileBuffer, size_t fileBufferSize){
			this->setFileBuffer(fileBuffer, fileBufferSize);
			this->initData();
		}

		/*
		 * Set the file buffer data pointer and buffer size.
		 */
		void setFileBuffer(char *fileBuffer, size_t fileBufferSize){
			this->fileBuffer = fileBuffer;
                        this->fileBufferSize = fileBufferSize;
		}

		/*
		 * Parse the provide filedata for a .obj collection.
		 * */
		bool parse(std::string collectionName, char *fileBuffer, size_t fileBufferSize){
			this->setFileBuffer(fileBuffer, fileBufferSize);
			return this->parse(collectionName);
		}

		void getObjectBuffer(int objectIndex, char **dataStorage, size_t *storageSize_out){
			int targetIndex = -1;
			std::string line="";
			int offset = 0;
			storageSize_out[0] = 0;
			for(int i=0; i<this->fileBufferSize; i++){
				if(this->fileBuffer[i] == '\n'){
					if(this->validObjElement(line, "o")){
						targetIndex++;
						if(targetIndex == objectIndex)
							offset = i - 1 - line.length();
					}
					line = "";
				}else{
					line += this->fileBuffer[i];
				}
			}

			line = "";
			bool first = true;
			for(int i=offset; i<this->fileBufferSize; i++){
				if(this->fileBuffer[i] == '\n'){
					if(this->validObjElement(line, "o")){
						if(first)
							first=false;
						else
							break;
					}
					storageSize_out[0]++;
					for(int j=0; j<line.length(); j++)
						storageSize_out[0]++;
					line = "";
				}else{
					line+= this->fileBuffer[i];
				}
			}

			dataStorage[0] = new char[storageSize_out[0]];
			int exitInt = 0;
			for(int i=offset; i<this->fileBufferSize && exitInt<storageSize_out[0]; i++, exitInt++){
				dataStorage[0][exitInt] = this->fileBuffer[i];
			}
		}
		
		/*
		 * Parse .obj collection data out of the existing file data.
		 * */
		bool parse(std::string collectionName){
			this->data.name = collectionName;
			if(false == this->validateBuffer()) return false;

			this->countEntry(&this->data.objectCount, "o");
			this->data.obj = new WavefrontObject[this->data.objectCount];
			
			for(int i=0; i<this->data.objectCount; i++){
				this->getObjectBuffer(i, &this->data.obj[i].rawBuffer, &this->data.obj[i].rawSize);
				this->data.obj[i].compile();
			}

			// generate master stores
			if(!this->buildMasterVertexStore())
				return false;
			if(!this->buildMasterTextureStore())
				return false;
			if(!this->buildMasterNormalStore())
				return false;

			if(!this->realizeObjects())
				return false;
			
			return true;
		}
		
		bool parseMaterial(std::string collectionName, char *fileBuffer, size_t fileBufferSize){
                        this->setFileBuffer(fileBuffer, fileBufferSize);
                        return this->parseMaterial(collectionName);
                }
		bool parseMaterial(std::string collectionName){
			if(!this->material.parse(collectionName, this->fileBuffer, this->fileBufferSize)){
				std::string msg = "WavefrontObjectCollection::parseMaterials(std::string collectionName) - Failed to parse material.\n\t";
				msg += this->material.errorMessage;
				this->setError(msg.c_str(), 1);
				return false;
			}
			return true;
		}
};
