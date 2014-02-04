#!/bin/bash

TARGETPATH=/tmp/usr/local/xpic
TARGETPATHCROSS=/tmp/xpic #resolves to C:/MinGW/msys/1.0/xpic
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

cd llvm-2.7/latest/
chmod +x configure autoconf/mkinstalldirs ./utils/mkpatch ./utils/GetSourceVersion ./utils/findmisopt ./utils/findoptdiff

rm -fr include/llvm/Config/config.h || true
#rm -fr localbuild || true
#mkdir localbuild
#cd localbuild

#../configure --prefix=$TARGETPATH  --enable-optimized --enable-targets=xpic,arm  --with-c-include-dirs=$TARGETPATH/include
./configure --prefix=$TARGETPATH  --enable-optimized --enable-targets=xpic  --with-c-include-dirs=$TARGETPATH/include
make clean
make -j4 LLVMC_BUILTIN_PLUGINS=Xpic LLVMC_BASED_DRIVER_NAME=xpic-llvmc
make LLVMC_BUILTIN_PLUGINS=Xpic LLVMC_BASED_DRIVER_NAME=xpic-llvmc install

#cd ..

# Cross LLVM

#export PATH=$HOME/mingw32/bin/:$PATH
rm -fr include/llvm/Config/config.h || true
rm -fr crossbuild || true
mkdir crossbuild
cd crossbuild
rm -fr include/llvm/Config/config.h || true
../configure --prefix=$TARGETPATHCROSS  --enable-optimized --enable-targets=xpic --host=i686-w64-mingw32 --with-c-include-dirs=$TARGETPATHCROSS/include
#rm -fr include/llvm/Config/config.h || true
# Dirty fixup for DOS Path
#sed -i 's|/c:/xpic|/c\\:/xpic|g' projects/sample/Makefile.common
make clean
make -j4 LLVMC_BUILTIN_PLUGINS=Xpic LLVMC_BASED_DRIVER_NAME=xpic-llvmc
make LLVMC_BUILTIN_PLUGINS=Xpic LLVMC_BASED_DRIVER_NAME=xpic-llvmc install
cd ..

cd ../..

# Binutils
cd binutils-2.20_xpic/latest
chmod +x configure mkinstalldirs missing
CFLAGS=-Wno-error ./configure --prefix=$TARGETPATH --target=xpic
make clean 
make -j 4 all 
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
make -j 4 all
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
cp $HOME/mingw32/bin/libiconv-2.dll $TARGETPATHCROSS/bin
