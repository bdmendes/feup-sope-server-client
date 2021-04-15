echo "TESTING CLIENT"
cd bin

valgrind -q --gen-suppressions=yes ./c
valgrind -q --gen-suppressions=yes ./c asd
valgrind -q --gen-suppressions=yes ./c asd -t 2
valgrind -q --gen-suppressions=yes ./c -t asd 2
valgrind -q --gen-suppressions=yes ./c -t 2 asd
valgrind -q --gen-suppressions=yes ./c 2 -t asd

cd ..

echo "FINISHED TESTING CLIENT"
echo ""
echo ""