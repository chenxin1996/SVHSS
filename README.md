# SVHSS
The code of SVHSS scheme
## IDE
In the coding, we chose CLion as IDE. So, you need to run the code in the CLion.
## Dependencies
### Install m4

'''
wget http://mirrors.kernel.org/gnu/m4/m4-1.4.13.tar.gz
tar -xzvf m4-1.4.13.tar.gz
cd m4-1.4.13
./configure --prefix=/usr/local
sudo make
sudo make install
'''

安装autoconf

wget http://mirrors.kernel.org/gnu/autoconf/autoconf-2.65.tar.gz
tar -xzvf autoconf-2.65.tar.gz
cd autoconf-2.65
./configure --prefix=/usr/local
sudo make
sudo make install

安装automake

wget http://mirrors.kernel.org/gnu/automake/automake-1.11.tar.gz
tar xzvf automake-1.11.tar.gz
cd automake-1.11
./configure --prefix=/usr/local
sudo make
sudo make install

安装libtool

wget http://mirrors.kernel.org/gnu/libtool/libtool-2.2.6b.tar.gz
tar xzvf libtool-2.2.6b.tar.gz
cd libtool-2.2.6b
./configure --prefix=/usr/local
sudo make
sudo make install

安装GMP

tar -jxvf  gmp-6.1.2.tar.bz2
cd gmp-6.1.2
./configure --prefix=/usr/local
sudo make
sudo make install
make check
安装MPFR

tar -xzvf mpfr-4.0.1.tar.gz
cd mpfr-4.0.1
./configure --prefix=/usr/local
sudo make
sudo make install

安装FLINT

tar -xzvf flint-2.5.2.tar.gz
cd flint-2.5.2
./configure --with-gmp  --with-mpfr --prefix=/usr/local
sudo make
sudo make install

安装CMake

tar -xzvf cmake-3.10.2.tar.gz
cd cmake-3.10.2
./configure --prefix=/usr/local
sudo make
sudo make install

安装RELIC
https://github.com/relic-toolkit/relic/wiki/Building

tar -xzvf relic-relic-toolkit-0.4.0.tar.gz
mkdir -p relic-target
sudo cmake ./relic-relic-toolkit-0.4.0
sudo make
sudo make install
