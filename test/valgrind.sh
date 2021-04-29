echo "TESTING CLIENT"
cd bin

# Invalid input tests
valgrind -q --gen-suppressions=yes ./c
valgrind -q --gen-suppressions=yes ./c asd
valgrind -q --gen-suppressions=yes ./c asd -t 2
valgrind -q --gen-suppressions=yes ./c -t asd 2
valgrind -q --gen-suppressions=yes ./c 2 -t asd

# Valid input tests
./s -t 5 asd & valgrind --leak-check=full ./c -t 3 asd
echo "FINISHED TESTING CLIENT 1"
echo " "
./s -t 2 asd & valgrind --leak-check=full ./c -t 3 asd
echo "FINISHED TESTING CLIENT 2"
echo " "
valgrind --leak-check=full ./c -t 3 asd
echo "FINISHED TESTING CLIENT 3"
echo " "

cd ..