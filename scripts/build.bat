call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
mkdir build
cd build
cmake ../ -G "NMake Makefiles"
nmake
nmake install
