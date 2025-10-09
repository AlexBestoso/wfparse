all:
#	g++ -fPIC -shared -o ./bin/helpCommand.so ./commands/help/help.command.cpp
	./commandCompile.sh
	g++ main.cc -ldl -o wfparse
clean:
	rm -rf ./bin
