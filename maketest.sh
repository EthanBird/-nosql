g++ mytest.cpp -o mytest -L /usr/local/lib -lhiredis
if [ $? -eq 0 ];then
    ./mytest
fi
