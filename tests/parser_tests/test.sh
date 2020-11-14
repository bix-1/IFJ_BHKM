DIR=tests/parser_tests

echo

for i in 1 2 3 4 5 6
do
  echo "****************************"

  echo "__________File in${i}__________"
  cat ${DIR}/in${i}
  echo "__________[TEST0${i}]__________"
  echo
  ./test_parser2 <${DIR}/in${i}

  echo "---Passed"
  echo
done

echo
