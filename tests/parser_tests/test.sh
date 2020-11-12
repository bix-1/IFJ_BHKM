DIR=tests/parser_tests

echo

echo "__________File in1__________"
cat ${DIR}/in1
echo "__________[TEST01]__________"
echo
./test_parser2 <${DIR}/in1

echo
echo
echo "****************************"

echo "__________File in2__________"
cat ${DIR}/in2
echo "__________[TEST02]__________"
echo
./test_parser2 <${DIR}/in2

echo
echo
echo "****************************"

echo "__________File in3__________"
cat ${DIR}/in3
echo "__________[TEST03]__________"
echo
./test_parser2 <${DIR}/in3

echo
echo
echo "****************************"

echo "__________File in4__________"
cat ${DIR}/in4
echo "__________[TEST04]__________"
echo
./test_parser2 <${DIR}/in3

echo
echo
echo "****************************"

echo "__________File in5__________"
cat ${DIR}/in5
echo "__________[TEST05]__________"
echo
./test_parser2 <${DIR}/in5

echo
