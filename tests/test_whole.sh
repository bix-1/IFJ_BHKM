#!/bin/bash

errorcount=0
total=$(ls -1q compiler_tests/ | wc -l)
path="compiler_tests/"
failed="failed_tests"
ret_string=`grep -n "\-eq" test_compiler.sh | grep -oP "\-eq\s+\K\w+" | tr '\n' ' '`
file_string=`grep -n "< compiler_tests/" test_compiler.sh | awk -F " < " '{print $2}' | awk -F " > " '{print $1}'`
IFS=' ' read -r -a array_ret <<< "$ret_string"
array_file=($file_string)

truncate -s 0 $failed

declare -A rets
i=0
for val in ${array_file[@]}
do
	rets[$val]=${array_ret[i]}
	((i++))
done

for file in $path*.go
do
	./ifj20 <$file >/dev/null 2>&1
	OUT=$?
	if [ $OUT -eq ${rets[$file]} ]; then
		printf '[OK]\t\t%s\n' `echo $file | cut -d "/" -f 2`
	else
		errorcount=$((errorcount + 1))
		echo "atom tests/$file &&" >>$failed
		printf '[FAILED]\t%s\n' `echo $file | cut -d "/" -f 2`
	fi
done

if [ $errorcount -eq 0 ]; then
	echo "#------------------ALL [${total}] TESTS PASSED------------------#"
else
	echo
	echo
	echo "#------------------TESTS FAILED------------------#"
	echo "#------------------NUMBER OF FAILED TESTS: [$errorcount] / [${total}]------------------#"
	echo
	echo
fi


echo "echo" >>$failed
