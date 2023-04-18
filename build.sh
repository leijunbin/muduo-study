if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

cd `pwd`/build && cmake .. && make

cd ..

rm -rf build