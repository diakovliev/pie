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

cp -vf $bdir/pie/pie $cdir/../../gn/bin/
strip --strip-unneeded $cdir/../../gn/bin/pie

$bdir/pie/pie gavc --help
