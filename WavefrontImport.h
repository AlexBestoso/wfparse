typedef struct obj_data{
	float vertex[3];
	float texture[2];
	float normal[3];
}obj_data_t;

class WavefrontImport{
	private:
		size_t vertexCount = 0;
		int vertexMultiplyer = 0;
		
		size_t textureCount = 0;
		int textureMultiplyer = 0;
		
		size_t normalCount = 0;
		int normalMultiplyer = 0;

		size_t faceCount = 0;
		int faceMultiplyer = 0;
		
		size_t objFileSize = 0;
		char *objFileData = NULL;
		
		int fd;

		float *vertexArray = NULL; 
		float *textureArray = NULL;
		float *normalArray = NULL;
		int *faceArray = NULL;


		std::string *textures = NULL;
                std::string *normals = NULL;
                std::string *vertexs = NULL;
                std::string *faces = NULL;

		void countFields(void){
			for(int i=0; i<objFileSize; i++){
				if((i!=0) && objFileData[i] == 'v' && objFileData[i+1] =='t' && objFileData[i-1] == '\n'){
					textureCount++;
				}else if((i!=0) && objFileData[i] == 'v' && objFileData[i+1] =='n' && objFileData[i-1] == '\n'){
                                        normalCount++;
                                }else if((i!=0) && objFileData[i] == 'v' && objFileData[i-1] == '\n'){
					vertexCount++;
				}else if((i!=0) && objFileData[i] == 'f' && objFileData[i-1] == '\n'){
                                        faceCount++;
                                }
			}
		
		}

		void allocateFieldStrings(void){
			textures = new std::string[textureCount];
                        normals = new std::string[normalCount];
                        vertexs = new std::string[vertexCount];
                        faces = new std::string[faceCount];

			int t=0, n=0, v=0, f=0;
                        for(int i=0; i<objFileSize; i++){
                                if((i!=0) && objFileData[i] == 'v' && objFileData[i+1] =='t' && objFileData[i-1] == '\n'){
                                        textures[t] = "";
                                        i+=3;
                                        int j = 0;
                                        while((i+j) < objFileSize && objFileData[i+j] != '\n'){
                                                textures[t] += objFileData[i+j];
                                                j++;
                                        }
                                        i+=j;
                                        textures[t]+=' ';
                                        t++;
                                }else if((i!=0) && objFileData[i] == 'v' && objFileData[i+1] =='n' && objFileData[i-1] == '\n'){
                                        normals[n] = "";
                                        i+=3;
                                        int j = 0;
                                        while((i+j) < objFileSize && objFileData[i+j] != '\n'){
                                                normals[n] += objFileData[i+j];
                                                j++;
                                        }
                                        i+=j;
                                        normals[n] += ' ';
                                        n++;
                                }else if((i!=0) && objFileData[i] == 'v' && objFileData[i-1] == '\n'){
                                        vertexs[v] = "";
                                        i+=2;
                                        int j = 0;
                                        while((i+j) < objFileSize && objFileData[i+j] != '\n'){
                                                vertexs[v] += objFileData[i+j];
                                                j++;
                                        }
                                        i+=j;
                                        vertexs[v] += ' ';
                                        v++;
                                }else if((i!=0) && objFileData[i] == 'f' && objFileData[i-1] == '\n'){
                                        faces[f] = "";
                                        i+=2;
                                        int j = 0;
                                        while((i+j) < objFileSize && objFileData[i+j] != '\n'){
                                                faces[f] += objFileData[i+j];
                                                j++;
                                        }
                                        i+=j;
                                        faces[f] += ' ';
                                        f++;
                                }
                        }
		}

		void parseVertexFloats(void){
			vertexMultiplyer = 0;
			for(int i=0; i<vertexs[0].length(); i++){
				if(vertexs[0][i] == ' '){
					vertexMultiplyer++;
				}
			}

			vertexArray = new float[vertexCount*vertexMultiplyer];
			int k = 0;
                        for(int i=0; i<vertexCount; i++){
                                std::string grab = "";
                                for(int j=0; j<vertexs[i].length(); j++){
                                        if(vertexs[i][j] == ' '){
						if(k >= vertexCount*vertexMultiplyer)
							break;
                                                vertexArray[k] = std::stof(grab);
                                                grab = "";
                                                k++;
                                        }else{
                                                grab += vertexs[i][j];
                                        }
                                }
				if(k >= vertexCount*vertexMultiplyer)
                                	break;
                        }
			delete[] vertexs;
		}

		void parseTextureFloats(void){
			textureMultiplyer = 0;
			for(int i=0; i<textures[0].length(); i++){
                                if(textures[0][i] == ' '){
                                        textureMultiplyer++;
                                }
                        }

			textureArray = new float[textureCount*textureMultiplyer];

			int k = 0;
                        for(int i=0; i<textureCount; i++){
                                std::string grab = "";
                                for(int j=0; j<textures[i].length(); j++){
                                        if(textures[i][j] == ' ' && grab.length() != 0){
						if(k >= textureCount*textureMultiplyer)
		                                        break;
                                                textureArray[k] = std::stof(grab);
                                                grab = "";
                                                k++;
                                        }else{
                                                grab += textures[i][j];
                                        }
                                }
				if(k >= textureCount*textureMultiplyer)
                                        break;
                        }

			delete[] textures;
		}

		void parseNormalFloats(void){
			normalMultiplyer = 0;
			for(int i=0; i<normals[0].length(); i++){
                                if(normals[0][i] == ' '){
                                        normalMultiplyer++;
                                }
                        }

                	normalArray = new float[normalCount*normalMultiplyer];
			
			int k = 0;
                        for(int i=0; i<normalCount; i++){
                                std::string grab = "";
                                for(int j=0; j<normals[i].length(); j++){
					if(k >= normalCount*normalMultiplyer)
                                        	break;
                                        if(normals[i][j] == ' ' && grab.length() != 0){
                                                normalArray[k] = std::stof(grab);
                                                grab = "";
                                                k++;
                                        }else{
                                                grab += normals[i][j];
                                        }
                                }
				if(k >= normalCount*normalMultiplyer)
                                        break;
                        }
			delete[] normals;
		}

		void parseFaceIndecies(void){
			int k = 0;
			for(int i=0; i<faces[0].length(); i++){
				if(faces[0][i] == ' '){
					faceMultiplyer++;
				}
			}

			faceArray = new int[faceCount*3*faceMultiplyer];
                        for(int i=0; i<faceCount; i++){
                                std::string grab = "";
                                for(int j=0; j<faces[i].length(); j++){
					if(k >= faceCount*faceMultiplyer*3)
                                        	break;
                                        if((faces[i][j] == ' ' || faces[i][j] == '/') && grab.length() != 0){
                                                faceArray[k] = std::stoi(grab);
                                                grab = "";
                                                k++;
                                        }else{
                                                grab += faces[i][j];
                                        }
                                }
				if(k >= faceCount*faceMultiplyer*3)
                                        break;
                        }
			delete[] faces;
		}

		void createGlBuffer(void){
			glObjBufferSize = 8 * faceCount * (faceMultiplyer+2);
                	glObjBuffer = new float[glObjBufferSize];

			int tracker = 0;
			int infrenceTrack = 0;
			for(int i=0; i<faceCount*faceMultiplyer*3; i++){

				if((i%4) == 1){
					// Get Previous
					i -= 3;
					int vec = (faceArray[i] - 1)*vertexMultiplyer;
                                        int tex = (faceArray[i+1] - 1)*textureMultiplyer;
                                        int norm = (faceArray[i+2] - 1)*normalMultiplyer;

                                        glObjBuffer[tracker] = vertexArray[vec];
                                        glObjBuffer[tracker+1] = vertexArray[vec+1];
                                        glObjBuffer[tracker+2] = vertexArray[vec+2];
                                        glObjBuffer[tracker+3] = textureArray[tex];
                                        glObjBuffer[tracker+4] = textureArray[tex+1];
                                        glObjBuffer[tracker+5] = normalArray[norm];
                                        glObjBuffer[tracker+6] = normalArray[norm+1];
                                        glObjBuffer[tracker+7] = normalArray[norm+2];
					tracker += 8;
					i+=3;

					// Apply current
					vec = (faceArray[i] - 1)*vertexMultiplyer;
                                        tex = (faceArray[i+1] - 1)*textureMultiplyer;
                                        norm = (faceArray[i+2] - 1)*normalMultiplyer;

                                        glObjBuffer[tracker] = vertexArray[vec];
                                        glObjBuffer[tracker+1] = vertexArray[vec+1];
                                        glObjBuffer[tracker+2] = vertexArray[vec+2];
                                        glObjBuffer[tracker+3] = textureArray[tex];
                                        glObjBuffer[tracker+4] = textureArray[tex+1];
                                        glObjBuffer[tracker+5] = normalArray[norm];
                                        glObjBuffer[tracker+6] = normalArray[norm+1];
                                        glObjBuffer[tracker+7] = normalArray[norm+2];
                                        tracker += 8;

					// Apply first vertex of previous triangle.
					i -= 9;
					vec = (faceArray[i] - 1)*vertexMultiplyer;
                                        tex = (faceArray[i+1] - 1)*textureMultiplyer;
                                        norm = (faceArray[i+2] - 1)*normalMultiplyer;

                                        glObjBuffer[tracker] = vertexArray[vec];
                                        glObjBuffer[tracker+1] = vertexArray[vec+1];
                                        glObjBuffer[tracker+2] = vertexArray[vec+2];
                                        glObjBuffer[tracker+3] = textureArray[tex];
                                        glObjBuffer[tracker+4] = textureArray[tex+1];
                                        glObjBuffer[tracker+5] = normalArray[norm];
                                        glObjBuffer[tracker+6] = normalArray[norm+1];
                                        glObjBuffer[tracker+7] = normalArray[norm+2];
                                        tracker += 8;
					i += 9;

					// Restore Flow
					i+=2;

				}else{
					int vec = (faceArray[i] - 1)*vertexMultiplyer;
					int tex = (faceArray[i+1] - 1)*textureMultiplyer;
					int norm = (faceArray[i+2] - 1)*normalMultiplyer;

					glObjBuffer[tracker] = vertexArray[vec];
					glObjBuffer[tracker+1] = vertexArray[vec+1];
					glObjBuffer[tracker+2] = vertexArray[vec+2];
					glObjBuffer[tracker+3] = textureArray[tex];
					glObjBuffer[tracker+4] = textureArray[tex+1];
					glObjBuffer[tracker+5] = normalArray[norm];
					glObjBuffer[tracker+6] = normalArray[norm+1];
					glObjBuffer[tracker+7] = normalArray[norm+2];
					i+=2;
					tracker +=8;
				}
				infrenceTrack++;
			}


			obj = (obj_data_t *)glObjBuffer;
			objCount = glObjBufferSize/8;///sizeof(obj_data_t);
			delete[] faceArray;
			delete[] vertexArray;
			delete[] textureArray;
			delete[] normalArray;
		}
	public:
		size_t objCount = 0;
		obj_data_t *obj = NULL;
		size_t glObjBufferSize = 0;
		float *glObjBuffer = NULL;

		bool import(const char *objFile){
			fd = open(objFile, O_RDONLY);
			if(!fd){
				printf("Failed to open '%s'\n", objFile);
				return false;
			}

			struct stat st;
			if(fstat(fd, &st)){
				printf("fstat failed.\n");
				close(fd);
				return false;
			}
			this->objFileSize = st.st_size;
			this->objFileData = new char[objFileSize];

			if(read(fd, objFileData, objFileSize) < objFileSize){
				printf("Failed to read.\n");
				close(fd);
				delete[] this->objFileData;
				return false;
			}
			close(fd);

			countFields();

			allocateFieldStrings();

			parseVertexFloats();
			parseTextureFloats();
			parseNormalFloats();
			parseFaceIndecies();

			createGlBuffer();
			delete[] this->objFileData;


			return true;
		}

		void destroy(void){
			objCount = 0;
                	glObjBufferSize = 0;
                	delete[] glObjBuffer;
			vertexCount = 0;
                	vertexMultiplyer = 0;
                	textureCount = 0;
                	textureMultiplyer = 0;

                	normalCount = 0;
                	normalMultiplyer = 0;

                	faceCount = 0;
                	faceMultiplyer = 0;

                	objFileSize = 0;
                	objFileData = NULL;

                	vertexArray = NULL;
                	textureArray = NULL;
                	normalArray = NULL;
                	faceArray = NULL;
		
			textures = NULL;
               		normals = NULL;
               		vertexs = NULL;
             		faces = NULL;

		}
};
