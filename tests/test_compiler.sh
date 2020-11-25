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

echo "--Mixed comments--"
./ifj20 < compiler_tests/01_le_comment_9.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Mixed comments--"
./ifj20 < compiler_tests/01_le_comment_10.go > /dev/null 2>&1
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

echo "#------------------Sematic error tests------------------#"
echo "#------------------Assignments------------------#"
echo "--Not enoguh values to assign--"
./ifj20 < compiler_tests/05_se_assignment_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 7 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Too many values to assign--"
./ifj20 < compiler_tests/05_se_assignment_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 7 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong values to assign--"
./ifj20 < compiler_tests/05_se_assignment_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 7 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong values to assign--"
./ifj20 < compiler_tests/05_se_assignment_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 7 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong values to assign--"
./ifj20 < compiler_tests/05_se_assignment_5.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 7 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------Functions------------------#"
echo "--Wrong func return--"
./ifj20 < compiler_tests/06_se_funcret_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 2--"
./ifj20 < compiler_tests/06_se_funcret_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 3--"
./ifj20 < compiler_tests/06_se_funcret_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 4--"
./ifj20 < compiler_tests/06_se_funcret_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 5--"
./ifj20 < compiler_tests/06_se_funcret_5.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 6--"
./ifj20 < compiler_tests/06_se_funcret_6.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 7--"
./ifj20 < compiler_tests/06_se_funcret_7.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 8--"
./ifj20 < compiler_tests/06_se_funcret_8.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 9--"
./ifj20 < compiler_tests/06_se_funcret_9.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 10--"
./ifj20 < compiler_tests/06_se_funcret_10.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 11--"
./ifj20 < compiler_tests/06_se_funcret_11.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 7 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 12--"
./ifj20 < compiler_tests/06_se_funcret_12.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong function calls--"
./ifj20 < compiler_tests/07_se_funcall_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong function calls 2--"
./ifj20 < compiler_tests/07_se_funcall_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong function calls 3--"
./ifj20 < compiler_tests/07_se_funcall_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong function calls 4--"
./ifj20 < compiler_tests/07_se_funcall_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Undefined calls--"
./ifj20 < compiler_tests/08_se_undefined_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Undefined calls 2--"
./ifj20 < compiler_tests/08_se_undefined_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Undefined calls 3--"
./ifj20 < compiler_tests/08_se_undefined_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Undefined calls 4--"
./ifj20 < compiler_tests/08_se_undefined_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------For------------------#"
echo "--For expression--"
./ifj20 < compiler_tests/08_se_for_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--For expression 2--"
./ifj20 < compiler_tests/08_se_for_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--For expression 3--"
./ifj20 < compiler_tests/08_se_for_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--For expression 4--"
./ifj20 < compiler_tests/08_se_for_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--For expression 4--"
./ifj20 < compiler_tests/08_se_for_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--For expression 4--"
./ifj20 < compiler_tests/08_se_for_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--For expression 4--"
./ifj20 < compiler_tests/08_se_for_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--For expression 4--"
./ifj20 < compiler_tests/08_se_for_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------If------------------#"
echo "--If--"
./ifj20 < compiler_tests/09_se_if_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--If 2--"
./ifj20 < compiler_tests/09_se_if_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------Wrong keywords------------------#"
echo "--Main--"
./ifj20 < compiler_tests/10_se_keyword_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Main 2--"
./ifj20 < compiler_tests/10_se_keyword_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Func--"
./ifj20 < compiler_tests/10_se_keyword_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------Syntax tests------------------#"
echo "--Case sensitive--"
./ifj20 < compiler_tests/11_sy_syntax_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Main param--"
./ifj20 < compiler_tests/11_sy_syntax_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Main return--"
./ifj20 < compiler_tests/11_sy_syntax_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Missing main--"
./ifj20 < compiler_tests/11_sy_syntax_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Adding float and int--"
./ifj20 < compiler_tests/11_sy_syntax_5.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Adding string and int--"
./ifj20 < compiler_tests/11_sy_syntax_6.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Adding float and string--"
./ifj20 < compiler_tests/11_sy_syntax_7.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Adding wrong operands--"
./ifj20 < compiler_tests/11_sy_syntax_8.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Adding two strings--"
./ifj20 < compiler_tests/11_sy_syntax_9.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Dividing strings--"
./ifj20 < compiler_tests/11_sy_syntax_10.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Float / int--"
./ifj20 < compiler_tests/11_sy_syntax_11.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--String / float64--"
./ifj20 < compiler_tests/11_sy_syntax_12.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--String / string--"
./ifj20 < compiler_tests/11_sy_syntax_13.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Float / float--"
./ifj20 < compiler_tests/11_sy_syntax_14.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Dividing by zero--"
./ifj20 < compiler_tests/11_sy_syntax_15.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 9 ]; then
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
