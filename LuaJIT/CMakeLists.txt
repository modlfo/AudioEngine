
include(ExternalProject)


if(WIN32)
	ExternalProject_Add(LuaJIT
		SOURCE_DIR LuaJIT
		GIT_REPOSITORY https://github.com/LuaJIT/LuaJIT.git
		GIT_TAG v2.1
		CONFIGURE_COMMAND echo "Configuring LuaJIT"
		BUILD_IN_SOURCE 1
		BUILD_COMMAND cd src COMMAND msvcbuild.bat static
		INSTALL_COMMAND echo "LuaJIT is ready")
else(WIN32)
	ExternalProject_Add(LuaJIT
		SOURCE_DIR LuaJIT
		GIT_REPOSITORY https://github.com/LuaJIT/LuaJIT.git
		GIT_TAG v2.1
		CONFIGURE_COMMAND echo "Configuring LuaJIT"
		BUILD_IN_SOURCE 1
		BUILD_COMMAND make BUILDMODE=static
		INSTALL_COMMAND echo "LuaJIT is ready")
endif(WIN32)


