all:
#	g++ -fPIC -shared -o ./bin/helpCommand.so ./commands/help/help.command.cpp
	./commandCompile.sh
	g++ main.cc -g -ldl -o wfparse
	sudo chown root:root ./bin/*
clean:
	rm -rf ./bin
	rm ./wfparse
