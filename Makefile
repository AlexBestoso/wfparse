all:
	./commandCompile.sh
	g++ main.cc -g -ldl -o wfparse
install:
	sudo chown root:root ./bin/*
clean:
	rm -rf ./bin
	rm ./wfparse
