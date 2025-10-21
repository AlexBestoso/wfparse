struct WavefrontPoint{
	unsigned int index_vertex;
	unsigned int index_texture;
	unsigned int index_normal;
};
typedef WavefrontPoint wf_point_t;

struct WavefrontFaceStruct{
	size_t count;
	int *v_index;
	int *vt_index;
	int *vn_index;

	size_t pointsCount;
	wf_point_t *points;
};
typedef WavefrontFaceStruct wf_face_t;
class WavefrontFace{
	private:
		std::string rawBuffer="";
		
		bool validateFaceBuffer(std::string input){
			if(input.length() <= 1){
				return false;
			}
			if(input[0] != 'f' && input[1] != ' '){
				return false;
			}
			if(input[input.length()-1] != '\n'){
				return false;
			}
			return true;
		}

		size_t countPoints(void){
			this->face.pointsCount=0;
			for(int i=2; i<rawBuffer.length(); i++){
				if(rawBuffer[i] == ' ' || rawBuffer[i] == '\n')
					this->face.pointsCount++;
			}
			return this->face.pointsCount;
		}

		// return true on exit condition. This was designed to only be used in generatePoint.
		bool setPoint(int pointIndex, int switchCase, std::string data){
			switch(switchCase){
				default:
				case 0:
					this->face.points[pointIndex].index_vertex = std::stoi(data.c_str());
					return false;
				case 1:
					this->face.points[pointIndex].index_texture = std::stoi(data.c_str());
					return false;
				case 2:
					this->face.points[pointIndex].index_normal = std::stoi(data.c_str());
					return true;
			}
			return true;
		}
	
		// this was designed only to work in the compile function.
		void generatePoint(int pointIndex){
			std::string grabber="";
			int switchCase = 0;
			for(int i=2, target=0; i<this->rawBuffer.length(); i++){
				if(pointIndex == target){
					if(this->rawBuffer[i] == ' ' || this->rawBuffer[i] == '\n'){
						this->setPoint(pointIndex, switchCase, grabber);
						return;
					}else if(this->rawBuffer[i] == '/'){
						if(this->setPoint(pointIndex, switchCase, grabber))
							return;
						switchCase++;
						grabber = "";
					}else{
						grabber += this->rawBuffer[i];
					}
				}else if(this->rawBuffer[i] == ' ' || this->rawBuffer[i] == '\n'){
					target++;
				}
			}
		}
	public:
		wf_face_t face;
		bool compile(std::string faceBuffer){
			if(this->validateFaceBuffer(faceBuffer) == false)
				return false;

			this->rawBuffer = faceBuffer;
			if(this->countPoints() <= 0) return false;
			
			this->face.points = new wf_point_t[this->face.pointsCount];
			for(int i=0; i<this->face.pointsCount; i++){
				this->generatePoint(i);
			}
			return true;
		}
};


