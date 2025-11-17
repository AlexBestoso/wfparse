all:
	./commandCompile.sh
	g++ main.cc -g -ldl -lz -o wfparse
install:
	sudo chown root:root ./bin/*
clean:
	rm -rf ./bin
	rm ./wfparse
	rm ./compile_error
