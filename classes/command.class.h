#pragma once
class Command{
	private:
		std::string name;
		std::string description;
		std::string manual;
		int argc;
		char **argv;
	public:

	std::string getName(void){
		return this->name;
	}
	
	std::string getDescription(void){
		return this->description;
	}

	std::string getManual(void){
		return this->manual;
	}

	int getArgc(void){
		return this->argc;
	}
	char **getArgv(void){
		return this->argv;
	}

	void setArgc(int v){
		this->argc = v;
	}
	void setArgv(char **v){
		this->argv = v;
	}
	void setName(std::string v){
		this->name = v;
	}
	
	void setDescription(std::string v){
		this->description = v;
	}

	void setManual(std::string v){
		this->manual = v;
	}

	virtual ~Command() = default;

	virtual bool exec(void){
		throw "exec() not implemented.\n";
		return false;
	}
};

using command_t = Command *(*)();
