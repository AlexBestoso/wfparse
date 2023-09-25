typedef struct obj_data{
	float vertex[3];
	float texture[2];
	float normal[3];
}obj_data_t;

typedef struct obj_face_data{
	int vector;
	int texture;
	int normal;
}obj_face_data_t;

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
		int *faceArrayLens = NULL;
		int faceArraySize = 0;


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
			faceArrayLens = new int[faceCount];
			faceArraySize = 0;
			for(int j=0; j<faceCount; j++){
				faceMultiplyer = 0;
				for(int i=0; i<faces[j].length(); i++){
					if(faces[j][i] == ' '){
						faceMultiplyer++;
					}
				}
				faceArrayLens[j] = faceMultiplyer;
				faceArraySize += 3*faceMultiplyer;
			}

			;
			faceArray = new int[faceArraySize];
			int k = 0;
                        for(int i=0; i<faceCount; i++){
                                std::string grab = "";
                                for(int j=0; j<faces[i].length(); j++){
					if(k >= faceArraySize)
                                        	break;
                                        if((faces[i][j] == ' ' || faces[i][j] == '/') && grab.length() != 0){
                                                faceArray[k] = std::stoi(grab);
                                                grab = "";
                                                k++;
                                        }else{
                                                grab += faces[i][j];
                                        }
                                }
				if(k >= faceArraySize)
                                        break;
                        }
			delete[] faces;

		}

		void createGlBuffer(void){
			glObjBufferSize = 0;
			for(int i=0; i<faceCount; i++){
				int val = (faceArrayLens[i]%3) == 1 ? 2 : (faceArrayLens[i]%3) == 2 ? 1 : 0;
				glObjBufferSize += 8*(faceArrayLens[i]+val);
			}
                	glObjBuffer = new float[glObjBufferSize];
			int tracker = 0;
			int faceTrack = 0;
			for(int i=0; i<faceCount; i++){
				int vec_a=0, vec_b=0, vec_c=0;
				int tex_a=0, tex_b=0, tex_c=0;
				int norm_a=0, norm_b=0, norm_c=0;
				int vec=0, tex=0, norm=0;
				int correct = faceArrayLens[i] % 3;
				for(int j=0; j<3*faceArrayLens[i]; j++){
					vec_c = vec_b;
					tex_c = tex_b;
					norm_c = norm_b;

					vec_b = vec_a;
					tex_b = tex_a;
					norm_b = norm_a;

					vec_a = vec;
					tex_a = tex;
					norm_a = norm;

					vec = (faceArray[faceTrack] - 1)*vertexMultiplyer;
                                        tex = (faceArray[faceTrack+1] - 1)*textureMultiplyer;
                                        norm = (faceArray[faceTrack+2] - 1)*normalMultiplyer;

					faceTrack+=3;

					if(correct == 1 && j-3 >= faceArrayLens[i] || correct == 2 && j-6 >=faceArrayLens[i]){
						break;
					
					}
					if(correct == 0){
						printf("In thirds %d %d\n", i, j);
						printf("\t%f, %f, %f\n", vertexArray[vec], vertexArray[vec+1], vertexArray[vec+1]);
					}
					
					glObjBuffer[tracker] = vertexArray[vec];
                                        glObjBuffer[tracker+1] = vertexArray[vec+1];
                                        glObjBuffer[tracker+2] = vertexArray[vec+2];
                                        glObjBuffer[tracker+3] = textureArray[tex];
                                        glObjBuffer[tracker+4] = textureArray[tex+1];
                                        glObjBuffer[tracker+5] = normalArray[norm];
                                        glObjBuffer[tracker+6] = normalArray[norm+1];
                                        glObjBuffer[tracker+7] = normalArray[norm+2];
                                        j+=2;
                                        tracker +=8;
				}

				if(correct == 1){ // We have one vertex placed and need to place 2 more.
					glObjBuffer[tracker] = vertexArray[vec_a];
                                        glObjBuffer[tracker+1] = vertexArray[vec_a+1];
                                        glObjBuffer[tracker+2] = vertexArray[vec_a+2];
                                        glObjBuffer[tracker+3] = textureArray[tex_a];
                                        glObjBuffer[tracker+4] = textureArray[tex_a+1];
                                        glObjBuffer[tracker+5] = normalArray[norm_a];
                                        glObjBuffer[tracker+6] = normalArray[norm_a+1];
                                        glObjBuffer[tracker+7] = normalArray[norm_a+2];
					tracker +=8;

					glObjBuffer[tracker] = vertexArray[vec];
                                        glObjBuffer[tracker+1] = vertexArray[vec+1];
                                        glObjBuffer[tracker+2] = vertexArray[vec+2];
                                        glObjBuffer[tracker+3] = textureArray[tex];
                                        glObjBuffer[tracker+4] = textureArray[tex+1];
                                        glObjBuffer[tracker+5] = normalArray[norm];
                                        glObjBuffer[tracker+6] = normalArray[norm+1];
                                        glObjBuffer[tracker+7] = normalArray[norm+2];
					tracker += 8;

					glObjBuffer[tracker] = vertexArray[vec_c];
                                        glObjBuffer[tracker+1] = vertexArray[vec_c+1];
                                        glObjBuffer[tracker+2] = vertexArray[vec_c+2];
                                        glObjBuffer[tracker+3] = textureArray[tex_c];
                                        glObjBuffer[tracker+4] = textureArray[tex_c+1];
                                        glObjBuffer[tracker+5] = normalArray[norm_c];
                                        glObjBuffer[tracker+6] = normalArray[norm_c+1];
                                        glObjBuffer[tracker+7] = normalArray[norm_c+2];
					tracker += 8;
				}

				if(correct == 2){
					printf("Correct 2 by adding 1\n");
					glObjBuffer[tracker] = vertexArray[vec_b];
                                        glObjBuffer[tracker+1] = vertexArray[vec_b+1];
                                        glObjBuffer[tracker+2] = vertexArray[vec_b+2];
                                        glObjBuffer[tracker+3] = textureArray[tex_b];
                                        glObjBuffer[tracker+4] = textureArray[tex_b+1];
                                        glObjBuffer[tracker+5] = normalArray[norm_b];
                                        glObjBuffer[tracker+6] = normalArray[norm_b+1];
                                        glObjBuffer[tracker+7] = normalArray[norm_b+2];
                                        tracker += 8;
				}

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

			printf("Counting Fields.\n");
			countFields();

			printf("Allocating field strings.\n");
			allocateFieldStrings();

			printf("Parsing Vertex Floats.\n");
			parseVertexFloats();
			
			printf("Parsing Texture Floats.\n");
			parseTextureFloats();
			
			printf("Parsing noramls floats.\n");
			parseNormalFloats();

			printf("Parsing face indecies.\n");
			parseFaceIndecies();

			printf("Creating the buffer.\n");
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
