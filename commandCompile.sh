mkdir ./bin
for CMD in $(ls ./commands); do
	INPUT=$(find "./commands/$CMD/" -name *.cpp)
	OUTPUT="./bin/$CMD.so"
	g++ -fPIC -shared -o $OUTPUT $INPUT
done
