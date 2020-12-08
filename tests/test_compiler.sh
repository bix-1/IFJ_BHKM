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

echo "--Already fixed bug with /--"
./ifj20 < compiler_tests/01_le_comment_11.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
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
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Wrong func return 12--"
./ifj20 < compiler_tests/06_se_funcret_12.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
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
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--For expression 4--"
./ifj20 < compiler_tests/08_se_for_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--For expression 5--"
./ifj20 < compiler_tests/08_se_for_5.go > /dev/null 2>&1
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

echo "--If 2 FUNXP --"
./ifj20 < compiler_tests/09_se_if_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 7 ]; then
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

echo "--Adding float and int-- 11_sy_syntax_5.go"
./ifj20 < compiler_tests/11_sy_syntax_5.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
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

echo "--Adding int and float-- 11_sy_syntax_8.go "
./ifj20 < compiler_tests/11_sy_syntax_8.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
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
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Float / int-- 11_sy_syntax_11.go "
./ifj20 < compiler_tests/11_sy_syntax_11.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
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
if [ $OUT -eq 5 ]; then
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

echo "--Float / int-- 11_sy_syntax_16.go"
./ifj20 < compiler_tests/11_sy_syntax_16.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--int / float-- 11_sy_syntax_17.go"
./ifj20 < compiler_tests/11_sy_syntax_17.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------Bool operators------------------#"
echo "--EQ--"
./ifj20 < compiler_tests/12_sy_operations_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--EQ 2--"
./ifj20 < compiler_tests/12_sy_operations_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--EQ 3-- 12_sy_operations_3.go"
./ifj20 < compiler_tests/12_sy_operations_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--EQ 4--"
./ifj20 < compiler_tests/12_sy_operations_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--EQ 5--"
./ifj20 < compiler_tests/12_sy_operations_5.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--EQ 6-- 12_sy_operations_6.go"
./ifj20 < compiler_tests/12_sy_operations_6.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--NEQ-- 12_sy_operations_7.go"
./ifj20 < compiler_tests/12_sy_operations_7.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--NEQ 1--"
./ifj20 < compiler_tests/12_sy_operations_8.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--NEQ 2--"
./ifj20 < compiler_tests/12_sy_operations_9.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--NEQ 3--"
./ifj20 < compiler_tests/12_sy_operations_10.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--NEQ 4--"
./ifj20 < compiler_tests/12_sy_operations_11.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--NEQ 5-- 12_sy_operations_12.go"
./ifj20 < compiler_tests/12_sy_operations_12.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--NEQ 6--"
./ifj20 < compiler_tests/12_sy_operations_13.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--GREATER-- 12_sy_operations_14.go"
./ifj20 < compiler_tests/12_sy_operations_14.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--GREATER 2--"
./ifj20 < compiler_tests/12_sy_operations_15.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--GREATER 3-- 12_sy_operations_16.go"
./ifj20 < compiler_tests/12_sy_operations_16.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--GREATER 4-- 12_sy_operations_17.go"
./ifj20 < compiler_tests/12_sy_operations_17.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--GREATER 5-- 12_sy_operations_18.go "
./ifj20 < compiler_tests/12_sy_operations_18.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--GREATER OR EQ --"
./ifj20 < compiler_tests/12_sy_operations_19.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--GREATER OR EQ 2-- 12_sy_operations_20.go"
./ifj20 < compiler_tests/12_sy_operations_20.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--GREATER OR EQ 3--"
./ifj20 < compiler_tests/12_sy_operations_21.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--GREATER OR EQ 4--"
./ifj20 < compiler_tests/12_sy_operations_22.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 1 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--GREATER OR EQ 5-- 12_sy_operations_23.go"
./ifj20 < compiler_tests/12_sy_operations_23.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--GREATER OR EQ 6-- 12_sy_operations_24.go"
./ifj20 < compiler_tests/12_sy_operations_24.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--LESS-- 12_sy_operations_25.go"
./ifj20 < compiler_tests/12_sy_operations_25.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--LESS 2-- 12_sy_operations_26.go"
./ifj20 < compiler_tests/12_sy_operations_26.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--LESS 3--"
./ifj20 < compiler_tests/12_sy_operations_27.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--LESS 4-- 12_sy_operations_28.go"
./ifj20 < compiler_tests/12_sy_operations_28.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--LESS OR EQ-- 12_sy_operations_29.go"
./ifj20 < compiler_tests/12_sy_operations_29.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--LESS OR EQ-- 2 12_sy_operations_30"
./ifj20 < compiler_tests/12_sy_operations_30.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--LESS OR EQ 3-- 12_sy_operations_31.go "
./ifj20 < compiler_tests/12_sy_operations_31.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--LESS OR EQ 4-- 12_sy_operations_32.go"
./ifj20 < compiler_tests/12_sy_operations_32.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MUL-- 12_sy_operations_33.go"
./ifj20 < compiler_tests/12_sy_operations_33.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MUL 2-- 12_sy_operations_34.go"
./ifj20 < compiler_tests/12_sy_operations_34.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MUL 3--"
./ifj20 < compiler_tests/12_sy_operations_35.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MUL 4-- 12_sy_operations_36.go "
./ifj20 < compiler_tests/12_sy_operations_36.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MUL 5--"
./ifj20 < compiler_tests/12_sy_operations_37.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--SUB--"
./ifj20 < compiler_tests/12_sy_operations_38.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--SUB 2-- 12_sy_operations_39.go"
./ifj20 < compiler_tests/12_sy_operations_39.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--SUB 3--"
./ifj20 < compiler_tests/12_sy_operations_40.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--SUB 4-- 12_sy_operations_41.go"
./ifj20 < compiler_tests/12_sy_operations_41.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--SUB 5--"
./ifj20 < compiler_tests/12_sy_operations_42.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
       echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--SUB 6--"
./ifj20 < compiler_tests/12_sy_operations_43.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--12_sy_operations_44.go--"
./ifj20 < compiler_tests/12_sy_operations_44.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 5 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--12_sy_operations_45.go--"
./ifj20 < compiler_tests/12_sy_operations_45.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--12_sy_operations_46.go--"
./ifj20 < compiler_tests/12_sy_operations_46.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--12_sy_operations_47.go--"
./ifj20 < compiler_tests/12_sy_operations_47.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--12_sy_operations_48.go--"
./ifj20 < compiler_tests/12_sy_operations_48.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--12_sy_operations_49.go--"
./ifj20 < compiler_tests/12_sy_operations_49.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--12_sy_operations_50.go--"
./ifj20 < compiler_tests/12_sy_operations_50.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--12_sy_operations_51.go--"
./ifj20 < compiler_tests/12_sy_operations_51.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--12_sy_operations_52.go--"
./ifj20 < compiler_tests/12_sy_operations_52.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------Complex tests------------------#"
echo "--Complex tests--"
./ifj20 < compiler_tests/13_complex_01.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 2--"
./ifj20 < compiler_tests/13_complex_02.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 3--"
./ifj20 < compiler_tests/13_complex_03.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 4--"
./ifj20 < compiler_tests/13_complex_04.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 5--"
./ifj20 < compiler_tests/13_complex_05.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 6--"
./ifj20 < compiler_tests/13_complex_06.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 7--"
./ifj20 < compiler_tests/13_complex_07.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 8--"
./ifj20 < compiler_tests/13_complex_08.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 9--"
./ifj20 < compiler_tests/13_complex_09.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 10--"
./ifj20 < compiler_tests/13_complex_10.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 11-13_complex_11.go--"
./ifj20 < compiler_tests/13_complex_11.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 12-13_complex_12.go--"
./ifj20 < compiler_tests/13_complex_12.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 13-13_complex_13.go--"
./ifj20 < compiler_tests/13_complex_13.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 14-13_complex_14.go--"
./ifj20 < compiler_tests/13_complex_14.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 15-13_complex_15.go--"
./ifj20 < compiler_tests/13_complex_15.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 16-13_complex_16.go--"
./ifj20 < compiler_tests/13_complex_16.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--Complex tests 17-13_complex_17.go--"
./ifj20 < compiler_tests/13_complex_17.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--13_complex_18.go--"
./ifj20 < compiler_tests/13_complex_18.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--13_complex_19.go--"
./ifj20 < compiler_tests/13_complex_19.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--13_complex_20.go--"
./ifj20 < compiler_tests/13_complex_20.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--13_complex_21.go--"
./ifj20 < compiler_tests/13_complex_21.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--13_complex_22.go--"
./ifj20 < compiler_tests/13_complex_22.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--13_complex_23.go--"
./ifj20 < compiler_tests/13_complex_23.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--13_complex_24.go--"
./ifj20 < compiler_tests/13_complex_24.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi


echo "#------------------EOL tests------------------#"
echo "--compiler_tests/EOL_1.go--"
./ifj20 < compiler_tests/EOL_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/EOL_2.go--"
./ifj20 < compiler_tests/EOL_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/EOL_3.go--"
./ifj20 < compiler_tests/EOL_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/EOL_4.go--"
./ifj20 < compiler_tests/EOL_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/EOL_5.go--"
./ifj20 < compiler_tests/EOL_5.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/EOL_5.go--"
./ifj20 < compiler_tests/EOL_5.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/EOL_6.go--"
./ifj20 < compiler_tests/EOL_6.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/EOL_7.go--"
./ifj20 < compiler_tests/EOL_7.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi
echo "--compiler_tests/EOL_8.go--"
./ifj20 < compiler_tests/EOL_8.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/EOL_9.go--"
./ifj20 < compiler_tests/EOL_9.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/EOL_10.go--"
./ifj20 < compiler_tests/EOL_10.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 2 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------FUNEXP------------------#"
echo "--compiler_tests/FUNEXP_1.go--"
./ifj20 < compiler_tests/FUNEXP_1.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_2.go--"
./ifj20 < compiler_tests/FUNEXP_2.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_3.go--"
./ifj20 < compiler_tests/FUNEXP_3.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_4.go--"
./ifj20 < compiler_tests/FUNEXP_4.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_5.go--"
./ifj20 < compiler_tests/FUNEXP_5.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_6.go--"
./ifj20 < compiler_tests/FUNEXP_6.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_7.go--"
./ifj20 < compiler_tests/FUNEXP_7.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_8.go--"
./ifj20 < compiler_tests/FUNEXP_8.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_9.go--"
./ifj20 < compiler_tests/FUNEXP_9.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_10.go--"
./ifj20 < compiler_tests/FUNEXP_10.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_11.go--"
./ifj20 < compiler_tests/FUNEXP_11.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_12.go--"
./ifj20 < compiler_tests/FUNEXP_12.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi


echo "--compiler_tests/FUNEXP_13.go--"
./ifj20 < compiler_tests/FUNEXP_13.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_14.go--"
./ifj20 < compiler_tests/FUNEXP_14.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_15.go--"
./ifj20 < compiler_tests/FUNEXP_15.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi


echo "--compiler_tests/FUNEXP_16.go--"
./ifj20 < compiler_tests/FUNEXP_16.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_17.go--"
./ifj20 < compiler_tests/FUNEXP_17.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_18.go--"
./ifj20 < compiler_tests/FUNEXP_18.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_19.go--"
./ifj20 < compiler_tests/FUNEXP_19.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_20.go--"
./ifj20 < compiler_tests/FUNEXP_20.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_21.go--"
./ifj20 < compiler_tests/FUNEXP_21.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_22.go--"
./ifj20 < compiler_tests/FUNEXP_22.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_23.go--"
./ifj20 < compiler_tests/FUNEXP_23.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_24.go--"
./ifj20 < compiler_tests/FUNEXP_24.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--compiler_tests/FUNEXP_25.go--"
./ifj20 < compiler_tests/FUNEXP_25.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------ZADANIE------------------#"
echo "--ZADANIE_FACTORIAL.go--"
./ifj20 < compiler_tests/ZADANIE_FACTORIAL.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--ZADANIE_FACTORIAL_REC.go--"
./ifj20 < compiler_tests/ZADANIE_FACTORIAL_REC.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--ZADANIE_STRING.go--"
./ifj20 < compiler_tests/ZADANIE_STRING.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------BUILDIN------------------#"
echo "--BUILDIN_01.go - INPUTI--"
./ifj20 < compiler_tests/BUILDIN_01.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_02.go - INPUTF--"
./ifj20 < compiler_tests/BUILDIN_02.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_03.go - INPUTS--"
./ifj20 < compiler_tests/BUILDIN_03.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_04.go - INPUTS--"
./ifj20 < compiler_tests/BUILDIN_04.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_05.go - INPUTF--"
./ifj20 < compiler_tests/BUILDIN_05.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_06.go - INPUTI--"
./ifj20 < compiler_tests/BUILDIN_06.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_07.go - INPUTB--"
./ifj20 < compiler_tests/BUILDIN_07.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_08.go - INPUTB--"
./ifj20 < compiler_tests/BUILDIN_08.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_09.go - INPUTI--"
./ifj20 < compiler_tests/BUILDIN_09.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_10.go - int2fliat, float2int--"
./ifj20 < compiler_tests/BUILDIN_10.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_11.go - int2fliat, float2int--"
./ifj20 < compiler_tests/BUILDIN_11.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_12.go - int2fliat, float2int--"
./ifj20 < compiler_tests/BUILDIN_12.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_13.go - int2fliat, float2int--"
./ifj20 < compiler_tests/BUILDIN_13.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_14.go - String seg--"
./ifj20 < compiler_tests/BUILDIN_14.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_15.go - len--"
./ifj20 < compiler_tests/BUILDIN_15.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_16.go - substr--"
./ifj20 < compiler_tests/BUILDIN_16.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_17.go - substr--"
./ifj20 < compiler_tests/BUILDIN_17.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_18.go - ord--"
./ifj20 < compiler_tests/BUILDIN_18.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_19.go - substr--"
./ifj20 < compiler_tests/BUILDIN_19.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_20.go - substr--"
./ifj20 < compiler_tests/BUILDIN_20.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_21.go - chr--"
./ifj20 < compiler_tests/BUILDIN_21.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_22.go - chr--"
./ifj20 < compiler_tests/BUILDIN_22.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_23.go - chr--"
./ifj20 < compiler_tests/BUILDIN_23.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BUILDIN_24.go - chr--"
./ifj20 < compiler_tests/BUILDIN_24.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi


echo "#------------------BOOLTHEN------------------#"

echo "--BOOLTHEN_01.go--"
./ifj20 < compiler_tests/BOOLTHEN_01.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_02.go--"
./ifj20 < compiler_tests/BOOLTHEN_02.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_03.go--"
./ifj20 < compiler_tests/BOOLTHEN_03.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_04.go--"
./ifj20 < compiler_tests/BOOLTHEN_04.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_05.go--"
./ifj20 < compiler_tests/BOOLTHEN_05.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_06.go--"
./ifj20 < compiler_tests/BOOLTHEN_06.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_07.go--"
./ifj20 < compiler_tests/BOOLTHEN_07.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_08.go--"
./ifj20 < compiler_tests/BOOLTHEN_08.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_09.go--"
./ifj20 < compiler_tests/BOOLTHEN_09.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_10.go--"
./ifj20 < compiler_tests/BOOLTHEN_10.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_11.go--"
./ifj20 < compiler_tests/BOOLTHEN_11.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_12.go--"
./ifj20 < compiler_tests/BOOLTHEN_12.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_13.go--"
./ifj20 < compiler_tests/BOOLTHEN_13.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_14.go--"
./ifj20 < compiler_tests/BOOLTHEN_14.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_15.go--"
./ifj20 < compiler_tests/BOOLTHEN_15.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_16.go--"
./ifj20 < compiler_tests/BOOLTHEN_16.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_17.go--"
./ifj20 < compiler_tests/BOOLTHEN_17.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_18.go--"
./ifj20 < compiler_tests/BOOLTHEN_18.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--BOOLTHEN_19.go--"
./ifj20 < compiler_tests/BOOLTHEN_19.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "#------------------MULTIVAL------------------#"
echo "--MULTIVAL_01.go--"
./ifj20 < compiler_tests/MULTIVAL_01.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MULTIVAL_02.go--"
./ifj20 < compiler_tests/MULTIVAL_02.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MULTIVAL_03.go--"
./ifj20 < compiler_tests/MULTIVAL_03.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MULTIVAL_04.go--"
./ifj20 < compiler_tests/MULTIVAL_04.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MULTIVAL_05.go--"
./ifj20 < compiler_tests/MULTIVAL_05.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 3 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MULTIVAL_06.go--"
./ifj20 < compiler_tests/MULTIVAL_06.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MULTIVAL_07.go--"
./ifj20 < compiler_tests/MULTIVAL_07.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MULTIVAL_08.go--"
./ifj20 < compiler_tests/MULTIVAL_08.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 0 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MULTIVAL_09.go--"
./ifj20 < compiler_tests/MULTIVAL_09.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
        echo "TEST OK"
else
        errorcount=$((errorcount + 1))
        echo "TEST FAILED"
fi

echo "--MULTIVAL_10.go--"
./ifj20 < compiler_tests/MULTIVAL_10.go > /dev/null 2>&1
OUT=$?
if [ $OUT -eq 6 ]; then
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
