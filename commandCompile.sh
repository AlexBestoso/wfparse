echo "mkdir ./bin 2>./compile_error"
mkdir ./bin 2>./compile_error

echo "for CMD in \$(ls ./commands); do"
for CMD in $(ls ./commands); do
	INPUT=$(find "./commands/$CMD/" -name *.cpp)
	OUTPUT="./bin/$CMD.so"
	echo "\tg++ -fPIC -shared -lz -o $OUTPUT $INPUT"
	g++ $INPUT -g -lz -lcrypto -fPIC -shared -o $OUTPUT 
done
echo "done"
