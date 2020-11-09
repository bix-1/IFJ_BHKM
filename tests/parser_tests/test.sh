DIR=tests/parser_tests

echo

echo "__________File in1__________"
cat ${DIR}/in1
echo "__________[TEST01]__________"
echo
./parser <${DIR}/in1

echo
echo
echo "****************************"

echo "__________File in2__________"
cat ${DIR}/in2
echo "__________[TEST02]__________"
echo
./parser <${DIR}/in2

echo
echo
echo "****************************"

echo "__________File in3__________"
cat ${DIR}/in3
echo "__________[TEST03]__________"
echo
./parser <${DIR}/in3

echo
