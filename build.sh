#!/bin/bash

# Allow overriding the number parallel make jobs with the
# environment variable MAKE_PARALLEL_JOBS. The default is 8.
[ -z "$MAKE_PARALLEL_JOBS" ] && MAKE_PARALLEL_JOBS="8"

TARGETPATH=/usr/local/xpic
TARGETPATHCROSS=/xpic #resolves to C:/MinGW/msys/1.0/xpic
export PATH_GNU_XPIC=$TARGETPATH

set -e

# LLVM

rm -fr $TARGETPATH/*
rm -fr $TARGETPATHCROSS/*

#wget http://llvm.org/releases/2.7/llvm-gcc-4.2-2.7-i686-linux.tgz
#tar xfz llvm-gcc-4.2-2.7-i686-linux.tgz -C $TARGETPATH
#pushd $TARGETPATH
#mv */* .
#popd

cd llvm-2.8
rm -fr localbuild || true
mkdir localbuild
cd localbuild

../configure --prefix=$TARGETPATH  --enable-optimized --enable-targets=xpic  --with-c-include-dirs=$TARGETPATH/include
#./configure --prefix=$TARGETPATH  --enable-debug-symbols --enable-debug-runtime --enable-targets=xpic  --with-c-include-dirs=$TARGETPATH/include
make clean
make -j$MAKE_PARALLEL_JOBS
make -j$MAKE_PARALLEL_JOBS install

cd ..

# Cross LLVM

#export PATH=$HOME/mingw32/bin/:$PATH
rm -fr crossbuild || true
mkdir crossbuild
cd crossbuild
../configure --prefix=$TARGETPATHCROSS  --enable-optimized --enable-targets=xpic --host=i686-w64-mingw32 --with-c-include-dirs=$TARGETPATHCROSS/include
sed -i 's/#define HAVE_STRERROR_S 1/\/* #undef HAVE_STRERROR_S *\//g' include/llvm/Config/config.h
# Dirty fixup for DOS Path
#sed -i 's|/c:/xpic|/c\\:/xpic|g' projects/sample/Makefile.common
export "LDFLAGS=--static -static-libgcc -static-libstdc++"
make clean
make -j$MAKE_PARALLEL_JOBS
make -j$MAKE_PARALLEL_JOBS install

cd ../..

# Binutils
cd binutils-2.20_xpic/latest
chmod +x configure mkinstalldirs missing
CFLAGS=-Wno-error ./configure --prefix=$TARGETPATH --target=xpic
make clean
make all
make install

# Cross Binutils

make distclean || true
rm -fr ` find . -name config.cache ` || true
rm -fr ` find . -name \*.o` ` find . -name config.status ` || true
rm -fr crossbuild || true
mkdir crossbuild
cd crossbuild
CFLAGS=-Wno-error ../configure --prefix=$TARGETPATHCROSS --target=xpic --host=i686-w64-mingw32
make clean 
make all
make install
cd ..

cd ../..

export PATH=$TARGETPATH/bin:$PATH

# Obviously, scons doesn't copy includes
cd newlib
cp -a ./newlib/libc/include/* $TARGETPATH/include
rm -fr ` find $TARGETPATH/include  -name .svn ` 
cp -a ./newlib/libc/include/* $TARGETPATHCROSS/include
rm -fr ` find $TARGETPATHCROSS/include  -name .svn ` 
cd ..

# libgcc
cd llvm-libgcc
scons
scons prefix=$TARGETPATH install
scons prefix=$TARGETPATHCROSS install
cd ..

# Newlib

cd newlib
chmod +x configure mkinstalldirs mkdep move-if-change 
scons
scons prefix=$TARGETPATH install
scons prefix=$TARGETPATHCROSS install
cd ..


#iconv is needed and compiled by ../BuildTools/Linux/build.sh
#cp $HOME/mingw32/bin/libiconv-2.dll $TARGETPATHCROSS/bin

VERSION=`svnversion . `
zip -9r xpic-$VERSION.zip $TARGETPATHCROSS
