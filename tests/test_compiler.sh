#!/bin/bash

errorcount=0

echo "#------------------Lexikal error tests------------------#"
echo "#------------------Comments------------------#"
echo "--Block comment without end--"
./ifj20 < compiler_tests/01_le_comment_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
	echo "TEST OK"
else
	errorcount=$((errorcount + 1))
	echo "TEST FAILED"
fi

echo "--Wrong end of block comment--"
./ifj20 < compiler_tests/01_le_comment_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Nested block comment--"
./ifj20 < compiler_tests/01_le_comment_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Nested block comment 2--"
./ifj20 < compiler_tests/01_le_comment_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Line comment--"
./ifj20 < compiler_tests/01_le_comment_5.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Block comment started after expression--"
./ifj20 < compiler_tests/01_le_comment_6.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Block comment started after expression 2--"
./ifj20 < compiler_tests/01_le_comment_7.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Mixed comments--"
./ifj20 < compiler_tests/01_le_comment_8.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------Wrong expressions------------------#"
echo "--Wrong operand--"
./ifj20 < compiler_tests/02_le_wrongtoken_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong operand 2--"
./ifj20 < compiler_tests/02_le_wrongtoken_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong number--"
./ifj20 < compiler_tests/02_le_wrongtoken_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong number 2--"
./ifj20 < compiler_tests/02_le_wrongtoken_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong number 3--"
./ifj20 < compiler_tests/02_le_wrongtoken_5.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong number 4--"
./ifj20 < compiler_tests/02_le_wrongtoken_6.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong keyword--"
./ifj20 < compiler_tests/02_le_wrongtoken_7.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong keyword 2--"
./ifj20 < compiler_tests/02_le_wrongtoken_8.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong tokene befor main function--"
./ifj20 < compiler_tests/02_le_wrongtoken_9.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong token--"
./ifj20 < compiler_tests/02_le_wrongtoken_10.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong token in package--"
./ifj20 < compiler_tests/02_le_wrongtoken_11.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------Numbers------------------#"
echo "--Wrong zeros--"
./ifj20 < compiler_tests/03_le_numbers_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong decimal--"
./ifj20 < compiler_tests/03_le_numbers_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Decimal number--"
./ifj20 < compiler_tests/03_le_numbers_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------Strings------------------#"
echo "--String without end--"
./ifj20 < compiler_tests/04_le_strings_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong excape sequence--"
./ifj20 < compiler_tests/04_le_strings_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong excape sequence 2--"
./ifj20 < compiler_tests/04_le_strings_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong excape sequence 3--"
./ifj20 < compiler_tests/04_le_strings_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong excape sequence 4--"
./ifj20 < compiler_tests/04_le_strings_5.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong excape sequence 5--"
./ifj20 < compiler_tests/04_le_strings_6.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong excape sequence 6--"
./ifj20 < compiler_tests/04_le_strings_7.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong excape sequence 7--"
./ifj20 < compiler_tests/04_le_strings_8.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong string--"
./ifj20 < compiler_tests/04_le_strings_9.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi



if [ $errorcount -eq 0 ]; then
	echo "#------------------TESTS PASSED------------------#"
else
	echo "#------------------TESTS FAILED------------------#"
	echo "#------------------NUMBER OF FAILED TESTS: $errorcount------------------#"
fi
