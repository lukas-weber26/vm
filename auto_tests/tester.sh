
for a in $(cat tests); do
	echo "Starting test: $a"

	nasm "$a" -o "./testfiles/temp.object" 
	./disassembler "./testfiles/temp.object" "./testfiles/temp.assmebly"
	nasm "./testfiles/temp.assmebly" -o "./testfiles/temp.object_out";

	if ! cmp -s "./testfiles/temp.object" "./testfiles/temp.object_out"; then 
		echo "Differences found. Origional vs disasembled:";
		echo "Origional:";
		cat "$a"
		echo "Disasembled:";
		cat "./testfiles/temp.assmebly"
		echo "Differences:";
		uniq "$a" "./testfiles/temp.assmebly"
		printf "\n\n\n"
		exit;
	else 
		echo "Test $a passed."
	fi
	printf "\n"
done 






