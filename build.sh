if [ ! -d `pwd`/example/bin ]; then
    rm -rf `pwd`/example/bin
fi

if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

cd `pwd`/build && cmake .. && make

cd ..

rm -rf build
