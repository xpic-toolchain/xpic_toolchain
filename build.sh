#!/bin/bash

# Allow overriding the number parallel make jobs with the
# environment variable MAKE_PARALLEL_JOBS. The default is 8.
[ -z "$MAKE_PARALLEL_JOBS" ] && MAKE_PARALLEL_JOBS="8"

TARGETPATH=/usr/local/xpic
TARGETPATHCROSS=/xpic #resolves to C:/MinGW/msys/1.0/xpic

set -e

#VERSION=`svnversion . `
VERSION=`date +%Y%m%d%H%M`
export INSTALLPATH_LOCAL=`pwd`/install/local
export INSTALLPATH_CROSS=`pwd`/install/xpic-llvm
export PATH_GNU_XPIC=$INSTALLPATH_LOCAL

#rm -fr $TARGETPATH/*
#rm -fr $TARGETPATHCROSS/*

#wget http://llvm.org/releases/2.7/llvm-gcc-4.2-2.7-i686-linux.tgz
#tar xfz llvm-gcc-4.2-2.7-i686-linux.tgz -C $TARGETPATH
#pushd $TARGETPATH
#mv */* .
#popd

# LLVM
echo
echo ================================================================================
echo "  building llvm-3.4.2 ..."
echo --------------------------------------------------------------------------------

cd llvm-3.4.2
#rm -fr localbuild || true
mkdir -p localbuild
cd localbuild

#../configure --prefix=$INSTALLPATH_LOCAL  --enable-optimized --enable-targets=host,xpic,cpp  --with-c-include-dirs=$INSTALLPATH_LOCAL/include
../configure --prefix=$TARGETPATH  --enable-debug-symbols --enable-debug-runtime --enable-targets=host,xpic  --with-c-include-dirs=/usr/include:$TARGETPATH/include
#make clean
make -j$MAKE_PARALLEL_JOBS
make install

cd ..

# Cross LLVM
echo
echo ================================================================================
echo "  crossbuilding llvm-2.8 ..."
echo --------------------------------------------------------------------------------

#export PATH=$HOME/mingw32/bin/:$PATH
#rm -fr crossbuild || true
mkdir -p crossbuild
cd crossbuild
#../configure --prefix=$INSTALLPATH_CROSS --enable-optimized --enable-targets=xpic --host=i686-w64-mingw32 --with-c-include-dirs=$INSTALLPATH_CROSS/include
../configure --prefix=$TARGETPATHCROSS  --enable-optimized --enable-targets=xpic --host=i686-w64-mingw32 --with-c-include-dirs=$TARGETPATHCROSS/include
sed -i 's/#define HAVE_STRERROR_S 1/\/* #undef HAVE_STRERROR_S *\//g' include/llvm/Config/config.h
# Dirty fixup for DOS Path
#sed -i 's|/c:/xpic|/c\\:/xpic|g' projects/sample/Makefile.common
export "LDFLAGS=--static -static-libgcc -static-libstdc++"
#make clean
make -j$MAKE_PARALLEL_JOBS
#make install DESTDIR=$INSTALLPATH_CROSS
make install

cd ../..

# Binutils
echo
echo ================================================================================
echo "  building binutils ..."
echo --------------------------------------------------------------------------------

cd binutils-2.20_xpic/latest
chmod +x configure mkinstalldirs missing

# Local build
#rm -fr localbuild || true
mkdir -p localbuild
cd localbuild
CFLAGS=-Wno-error ../configure --prefix=$INSTALLPATH_LOCAL --target=xpic
#make clean
make all
make install
cd ..


# Cross Binutils
echo
echo ================================================================================
echo "  crossbuilding binutils ..."
echo --------------------------------------------------------------------------------


#make distclean || true
#rm -fr ` find . -name config.cache ` || true
#rm -fr ` find . -name \*.o` ` find . -name config.status ` || true
#rm -fr crossbuild || true
mkdir -p crossbuild
cd crossbuild
CFLAGS=-Wno-error ../configure --prefix=$INSTALLPATH_CROSS --target=xpic --host=i686-w64-mingw32
#CFLAGS=-Wno-error ../configure --prefix=$TARGETPATHCROSS --target=xpic --host=i686-w64-mingw32
#make clean 
make all
make install
#make install DESTDIR=$INSTALLPATH_CROSS
cd ..

cd ../..

export PATH=$INSTALLPATH_LOCAL/bin:$PATH
echo PATH: $PATH

# Obviously, scons doesn't copy includes
cd newlib
cp -a ./newlib/libc/include/* $INSTALLPATH_LOCAL/include
rm -fr ` find $INSTALLPATH_LOCAL/include  -name .svn ` 
cp -a ./newlib/libc/include/* $INSTALLPATH_CROSS/include
rm -fr ` find $INSTALLPATH_CROSS/include  -name .svn ` 
cd ..

# libgcc
echo
echo ================================================================================
echo "  libgcc ..."
echo --------------------------------------------------------------------------------
cd llvm-libgcc
scons 
scons prefix=$INSTALLPATH_LOCAL install
scons prefix=$INSTALLPATH_CROSS install
cd ..

# Newlib
echo
echo ================================================================================
echo "  newlib ..."
echo --------------------------------------------------------------------------------
cd newlib
chmod +x configure mkinstalldirs mkdep move-if-change 
scons
scons prefix=$INSTALLPATH_LOCAL install
scons prefix=$INSTALLPATH_CROSS install
cd ..


#iconv is needed and compiled by ../BuildTools/Linux/build.sh
#cp $HOME/mingw32/bin/libiconv-2.dll $TARGETPATHCROSS/bin

#zip -9r xpic-$VERSION.zip $INSTALLPATH_CROSS
rm -f xpic-$VERSION.zip
7z a -tzip -mx=9 xpic-llvm_$VERSION.zip $INSTALLPATH_CROSS

