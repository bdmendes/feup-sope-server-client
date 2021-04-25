echo "TESTING CLIENT"
cd bin

# Invalid input tests
valgrind -q --gen-suppressions=yes ./c
valgrind -q --gen-suppressions=yes ./c asd
valgrind -q --gen-suppressions=yes ./c asd -t 2
valgrind -q --gen-suppressions=yes ./c -t asd 2
valgrind -q --gen-suppressions=yes ./c 2 -t asd

# Valid input test
valgrind -q --gen-suppressions=yes ./c -t 2 asd

cd ..
echo "FINISHED TESTING CLIENT"