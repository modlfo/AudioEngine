
if(UNIX AND NOT APPLE)
   add_custom_target(AudioEngineTarget ALL make -j 4 CONFIG=Release
                  DEPENDS LuaJIT
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Builds/LinuxMakefile
                  COMMENT "Building the AudioEngine")
   set(AUDIOENGINE_PATH Builds/LinuxMakefile/build)
endif(UNIX AND NOT APPLE)


if(APPLE)
   add_custom_target(AudioEngineTarget ALL
                  COMMAND sed -i.bu "s/DEAD_CODE_STRIPPING = YES/DEAD_CODE_STRIPPING = NO/g" AudioEngine.xcodeproj/project.pbxproj
                  COMMAND xcodebuild -target "AudioEngine - ConsoleApp"
                  VERBATIM
                  DEPENDS LuaJIT
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Builds/MacOSX
                  COMMENT "Building the AudioEngine")
   set(AUDIOENGINE_PATH Builds/MacOSX/build/Release)
endif(APPLE)


if(WIN32)
   add_custom_target(AudioEngineTarget ALL
                  COMMAND msbuild  /property:Configuration=Release AudioEngine_ConsoleApp.vcxproj
                  VERBATIM
                  DEPENDS LuaJIT
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Builds/VisualStudio2013
                  COMMENT "Building the AudioEngine")
   set(AUDIOENGINE_PATH Builds/VisualStudio2013/x64/Release/ConsoleApp)
endif(WIN32)

if(WIN32)
   set(AUDIOENGINE_EXE ${CMAKE_CURRENT_LIST_DIR}/${AUDIOENGINE_PATH}/AudioEngine.exe)
else(WIN32)
   set(AUDIOENGINE_EXE ${CMAKE_CURRENT_LIST_DIR}/${AUDIOENGINE_PATH}/AudioEngine)
endif(WIN32)


install(PROGRAMS ${AUDIOENGINE_EXE} DESTINATION ${DESTDIR})
