#include "./obj.class.h"

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
			this->objFileBuffer = new char[50000];
			this->objFileSize = this->openAndRead(this->objFileName.c_str(), &this->objFileFd, this->objFileBuffer, 50000);	
			
			this->waveobj.import_obj(this->objFileBuffer, this->objFileSize);
			
			return true;
		}
	public:
		WavefrontObject waveobj;
		float *gl_buffer;
		size_t gl_buffer_size;
		size_t gl_stride=0;

		void dumpObjFile(){
			printf("Loaded file (%ld bytes) '%s'\n", this->objFileSize, this->objFileName.c_str());
			
			printf("Imported Object Name '%s'\n", this->waveobj.object_name.c_str());
                	printf("Imported %ld vertecies\n", this->waveobj.object_vertex_size);
			for(int i=0; i<this->waveobj.object_vertex_size; i++){
				printf("(%f, %f, %f)\n", this->waveobj.object_vertex[i][0], this->waveobj.object_vertex[i][1], this->waveobj.object_vertex[i][2]);
			}
                	printf("Imported %ld texteture points\n", this->waveobj.object_texture_size);
			for(int i=0; i<this->waveobj.object_texture_size; i++){
				printf("(%f, %f)\n", this->waveobj.object_texture[i][0], this->waveobj.object_texture[i][1]);
			}
                	printf("Imported %ld normals\n", this->waveobj.object_normal_size);
			for(int i=0; i<this->waveobj.object_normal_size; i++){
				printf("(%f, %f, %f)\n", this->waveobj.object_normal[i][0], this->waveobj.object_normal[i][1], this->waveobj.object_normal[i][2]);
			}
                	printf("Imported %ld faces\n", this->waveobj.object_face_size);
			for(int i=0; i<this->waveobj.object_face_size; i++){
				printf("\tface %d: ", i);
				printf("(");
				for(int j=0; j<this->waveobj.object_face[i].count; j++){
					printf("%d ", this->waveobj.object_face[i].v_index[j]);
				}
				printf(") ");
				printf("(");
				for(int j=0; j<this->waveobj.object_face[i].count; j++){
					printf("%d ", this->waveobj.object_face[i].vt_index[j]);
				}
				printf(") ");
				printf("(");
				for(int j=0; j<this->waveobj.object_face[i].count; j++){
					printf("%d ", this->waveobj.object_face[i].vn_index[j]);
				}
				printf(")\n");


			}			
	
			printf("Target Material File : '%s'\n", this->waveobj.object_mtllib.c_str());
			printf("Target Material Name : '%s'\n", this->waveobj.object_material.c_str());

			/*for(int i=0;i<this->objFileSize;i++){
				printf("%c", this->objFileBuffer[i]);
			}printf("\n\n");*/
		}

		void dumpMtlFile(){
			printf("\n===Objects Material '%s'===\n", this->waveobj.material_name.c_str());
			printf("Material Ns : %f\n", this->waveobj.material_ns);
			printf("Material Ka : (%f, %f, %f)\n", this->waveobj.material_ka[0], this->waveobj.material_ka[1], this->waveobj.material_ka[2]);
			printf("Material Kd : (%f, %f, %f)\n", this->waveobj.material_kd[0], this->waveobj.material_kd[1], this->waveobj.material_kd[2]);
			printf("Material Ks : (%f, %f, %f)\n", this->waveobj.material_ks[0], this->waveobj.material_ks[1], this->waveobj.material_ks[2]);
			printf("Material Ke : (%f, %f, %f)\n", this->waveobj.material_ke[0], this->waveobj.material_ke[1], this->waveobj.material_ke[2]);
			printf("Material Ni : %f\n", this->waveobj.material_ni);
			printf("Material d : %f\n", this->waveobj.material_d);
			printf("Material illum : %f\n", this->waveobj.material_illum);
			printf("Material map_Kd : '%s'\n", this->waveobj.material_map_kd.c_str());

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
			EMPTY=vtn. an empty format gives vertex, texture, normal
			
		* *
		 * There should be no spaces between items.
		 * An empty string will build the format 'vtn'
		 * */
		bool genBuffer_format(const char *fmt){
			// Determine size of formatted string
			std::string fmt_str="";
			if(strlen(fmt) <= 0){ 
				fmt_str = "vtn";
			}else{
				fmt_str = fmt;
			}
			size_t formatCellSize = this->getCellLength(fmt_str.c_str());
	
			// determine total size relative to face count and format cell size.
			this->gl_buffer_size = 0;
			if(this->gl_buffer){
				delete[] this->gl_buffer;
				this->gl_buffer = NULL;
			}
			size_t pointCount = 0;
			for(int i=0; i<this->waveobj.object_face_size; i++){
				for(int j=0; j<this->waveobj.object_face[i].count; j++){
					this->gl_buffer_size += formatCellSize;
					pointCount++;
				}
			}
			this->gl_buffer = new float[this->gl_buffer_size];
			int gl=0;

			// Populate the buffer.
			for(int i=0; i<this->waveobj.object_face_size; i++){

			}
			int activeFace=0;
			int activePoint=0;
			for(int i=0; i<pointCount; i++){
				if(activeFace >= this->waveobj.object_face_size) break;
				if(activePoint >= this->waveobj.object_face[activeFace].count) break;
				int vertexIdx = this->waveobj.object_face[activeFace].v_index[activePoint]-1;
				int textureIdx = this->waveobj.object_face[activeFace].vt_index[activePoint]-1;
				int normalIdx = this->waveobj.object_face[activeFace].vn_index[activePoint]-1;
				
				for(int F=0; F<strlen(fmt); F++){
					switch(this->parseBufferFormat(fmt, F)){
						case WF_FMT_V:
							if(gl >=this->gl_buffer_size ||  gl+2 >= this->gl_buffer_size){
								break;
							}
							if(vertexIdx >= this->waveobj.object_vertex_size) {
								continue;
							}
							this->gl_buffer[gl] = this->waveobj.object_vertex[vertexIdx][0];
							gl++;
							this->gl_buffer[gl] = this->waveobj.object_vertex[vertexIdx][1];
							gl++;
							this->gl_buffer[gl] = this->waveobj.object_vertex[vertexIdx][2];
							gl++;
						break;
						case WF_FMT_T:
							if(textureIdx >= this->waveobj.object_texture_size) {continue;}
							if(gl >=this->gl_buffer_size || gl+1 >= this->gl_buffer_size) continue;
							this->gl_buffer[gl] = this->waveobj.object_texture[textureIdx][0];
							gl++;
							this->gl_buffer[gl] = this->waveobj.object_texture[textureIdx][1];
							gl++;
						break;
						case WF_FMT_N:
							if(normalIdx >= this->waveobj.object_normal_size) {continue;}
							if(gl >=this->gl_buffer_size || gl+2 >= this->gl_buffer_size) break;
                                                               this->gl_buffer[gl] = this->waveobj.object_normal[normalIdx][0];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->waveobj.object_normal[normalIdx][1];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->waveobj.object_normal[normalIdx][2];
                                                               gl++;
						break;
						case WF_FMT_A:
							if(gl >=this->gl_buffer_size || gl+2 >= this->gl_buffer_size) break;
                                                               this->gl_buffer[gl] = this->waveobj.material_ka[0];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->waveobj.material_ka[1];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->waveobj.material_ka[2];
                                                               gl++;
						break;
						case WF_FMT_D:
							if(gl >=this->gl_buffer_size ) break;
                                                               this->gl_buffer[gl] = this->waveobj.material_d;
                                                               gl++;
						break;
						case WF_FMT_S:
							if(gl >=this->gl_buffer_size ) break;
                                                               this->gl_buffer[gl] = this->waveobj.material_ns;
                                                               gl++;
						break;
						case WF_FMT_E:
							if(gl >=this->gl_buffer_size || gl+2 >= this->gl_buffer_size) break;
                                                               this->gl_buffer[gl] = this->waveobj.material_ke[0];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->waveobj.material_ke[1];
                                                               gl++;
                                                               this->gl_buffer[gl] = this->waveobj.material_ke[2];
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
				if(activePoint >= this->waveobj.object_face[activeFace].count){
                        		activePoint=0;
					activeFace++;
					if(activeFace >= this->waveobj.object_face_size) activeFace = 0;
				}
			}
			return true;
		}

		bool import(const char *target){
			this->getRawFileData(target);
			
			return true;
		}
};
