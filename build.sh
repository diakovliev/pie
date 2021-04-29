#! /bin/sh -ex

# From ./gnb --root-shell

apt update
apt install -y libboost1.67-all-dev libcurl4-gnutls-dev libssl-dev python2-dev python3-dev ragel

cdir=$(pwd)
bdir=/tmp/build_pie

mkdir -p $bdir

cd $bdir
  cmake $cdir
  make
cd -

pyver=2
echo "pyver: $pyver"
echo "python_pie: $bdir/pie/python_pie/python${pyver}/libpython${pyver}_pie.so"
echo "binary: $bdir/pie/pie"

cp -vf $bdir/pie/pie                                            $cdir/../../gn/bin/
cp -vf $bdir/python_pie/python${pyver}/libpython${pyver}_pie.so $cdir/../../gn/py_libs/
cp -vf $bdir/python_pie/python${pyver}/python_pie.py            $cdir/../../gn/py_libs/

strip --strip-unneeded $cdir/../../gn/bin/pie
strip --strip-unneeded $cdir/../../gn/py_libs/libpython${pyver}_pie.so
