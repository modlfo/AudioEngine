
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
                  COMMAND xcodebuild -target "AudioEngine (ConsoleApp)"
                  VERBATIM
                  DEPENDS LuaJIT
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Builds/MacOSX
                  COMMENT "Building the AudioEngine")
   set(AUDIOENGINE_PATH Builds/MacOSX/build/Release)
endif(APPLE)


#if(WIN32)
#   add_custom_target(AudioEngineTarget ALL
#                  COMMAND msbuild AudioEngine.vcxproj
#                  VERBATIM
#                  DEPENDS LuaJIT
#                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Builds/VisualStudio2013
#                  COMMENT "Building the AudioEngine")
#   set(AUDIOENGINE_PATH Builds/MacOSX/build/Release)
#endif(WIN32)
if(WIN32)

add_definitions(-D_CONSOLE -DJUCE_WINDOWS)

set(AUDIOENGINE_SRC
   ${CMAKE_CURRENT_LIST_DIR}/Source/AudioEngine.cpp
   ${CMAKE_CURRENT_LIST_DIR}/Source/AudioEngine.h
   ${CMAKE_CURRENT_LIST_DIR}/Source/LuaVM.cpp
   ${CMAKE_CURRENT_LIST_DIR}/Source/LuaVM.h
   ${CMAKE_CURRENT_LIST_DIR}/Source/Server.cpp
   ${CMAKE_CURRENT_LIST_DIR}/Source/Server.h
   ${CMAKE_CURRENT_LIST_DIR}/Source/Main.cpp
   ${CMAKE_CURRENT_LIST_DIR}/Source/Main.h
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/BinaryData.cpp
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/juce_audio_basics.cpp
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/juce_audio_devices.cpp
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/juce_audio_formats.cpp
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/juce_audio_processors.cpp
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/juce_audio_utils.cpp
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/juce_core.cpp
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/juce_data_structures.cpp
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/juce_events.cpp
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/juce_graphics.cpp
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/juce_gui_basics.cpp
   ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/juce_gui_extra.cpp)

include_directories(${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/modules)
include_directories(${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode)

add_executable(AudioEngine ${AUDIOENGINE_SRC})

target_link_libraries(AudioEngine lua51)

endif(WIN32)


if(WIN32)
   set(AUDIOENGINE_EXE ${CMAKE_BINARY_DIR}/${AUDIOENGINE_PATH}/AudioEngine/AudioEngine.exe)
else(WIN32)
   set(AUDIOENGINE_EXE ${CMAKE_CURRENT_LIST_DIR}/${AUDIOENGINE_PATH}/AudioEngine)
endif(WIN32)


install(PROGRAMS ${AUDIOENGINE_EXE} DESTINATION ${DESTDIR})
