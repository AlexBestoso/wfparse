echo "mkdir ./bin 2>./compile_error"
mkdir ./bin 2>./compile_error

echo "for CMD in \$(ls ./commands); do"
for CMD in $(ls ./commands); do
	INPUT=$(find "./commands/$CMD/" -name *.cpp)
	OUTPUT="./bin/$CMD.so"
	echo "\tg++ -fPIC -shared -o $OUTPUT $INPUT"
	g++ -g -fPIC -shared -o $OUTPUT $INPUT
done
echo "done"
