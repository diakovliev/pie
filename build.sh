#! /bin/sh -ex

# From ./gnb --root-shell

apt install -y libboost-all-dev libcurl4-gnutls-dev libssl-dev

cdir=$(pwd)
bdir=/tmp/build_pie

mkdir -p $bdir

cd $bdir

cmake $cdir
make

echo "binary: $bdir/pie/pie"
echo "python_pie: $bdir/pie/python_extension/libpython_pie.so"

cp -vf $bdir/pie/pie                                            $cdir/../../gn/bin/
cp -vf $bdir/python_extension/libpython_pie.so                  $cdir/../../gn/py_libs/

strip --strip-unneeded $cdir/../../gn/bin/pie
strip --strip-unneeded $cdir/../../gn/py_libs/libpython_pie.so

$bdir/pie/pie gavc --help

cd $bdir/pie/python_extension
python ./test.py
