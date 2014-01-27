#!/bin/bash

set -e

rm -fr $HOME/mingw32 || true
rm -fr $HOME/packages || true

rm -fr x86-mingw32-build-1.0 || true
tar xfj third/x86-mingw32-build-1.0-sh.tar.bz2
cd x86-mingw32-build-1.0
patch -p0 < ../patch-local-function.patch
cp ../x86-mingw32-build.sh.conf .
sh x86-mingw32-build.sh --batch
cd ..

#iconv is needed
export PATH=$HOME/mingw32/bin:$PATH
tar xfz third/libiconv-1.14.tar.gz
cd libiconv-1.14/
./configure --prefix $HOME/mingw32/ --host=i386-mingw32
make 
make install
