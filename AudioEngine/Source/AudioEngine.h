/*
The MIT License (MIT)

Copyright (c) 2017 Leonardo Laguna Ruiz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef AUDIOENGINE_H_INCLUDED
#define AUDIOENGINE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "LuaVM.h"
#include "Server.h"

//==============================================================================
class Audio : public AudioAppComponent
{
 public:
   Audio();

   ~Audio();

   void start(void);

   void stop(void);

   void prepareToPlay(int samplesPerBlockExpected, double sampleRate);

   void releaseResources();

   void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill);

   const OwnedArray<AudioIODeviceType> &getAvailableDeviceTypes(void);

   String showConfig(void);

   String getConfig(void);

   var getAudioDeviceSetup(void);

   var getCurrentAudioDeviceType(void);

   var getCurrentAudioDevice(void);

   var setAudioDeviceSetup(var arg);

   var loadFiles(var files);

 private:
   MidiMessageCollector midiMessageCollector;
   LuaVM lua;
   ApplicationProperties properties;

   const float *in0_pre;
   const float *in1_pre;
   float *out0_pre;
   float *out1_pre;

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Audio)
};

class AudioInstance
{
 public:
   static Audio &get()
   {
      static Audio inst;
      return inst;
   }
};

class AudioEngine : public ServerEvaluator
{
 public:
   AudioEngine();
   virtual ~AudioEngine();

   String evaluate(const String &javascriptCode);

   bool repeat(void);

 private:
   DynamicObject *audio_js;
   JavascriptEngine js;

   // Function 'audio.quit()'
   static var quit(const var::NativeFunctionArgs &args);

   // Function 'audio.stop()'
   static var stop(const var::NativeFunctionArgs &args);

   // Function 'audio.start()'
   static var start(const var::NativeFunctionArgs &args);

   // Function 'audio.getAudioDevices()'
   static var getAudioDevices(const var::NativeFunctionArgs &args);

   // Function 'audio.getCurrentAudioDeviceType()'
   static var getCurrentAudioDeviceType(const var::NativeFunctionArgs &args);

   // Function 'audio.getCurrentAudioDevice()'
   static var getCurrentAudioDevice(const var::NativeFunctionArgs &args);

   // Function 'audio.getAudioDeviceSetup()'
   static var getAudioDeviceSetup(const var::NativeFunctionArgs &args);

   // Function 'audio.setAudioDeviceSetup(data)'
   static var setAudioDeviceSetup(const var::NativeFunctionArgs &args);

   // Function 'audio.getMidiDevices()'
   static var getMidiDevices(const var::NativeFunctionArgs &args);

   // Function 'audio.showConfig()'
   static var showConfig(const var::NativeFunctionArgs &args);

   // Function 'audio.loadFiles()'
   static var loadFiles(const var::NativeFunctionArgs &args);

   // Sets the property var in 'audio.status'
   static var setStatusProperty(const var::NativeFunctionArgs &args, String, var);

   // Returns 'audio.status'
   static var getStatus(const var::NativeFunctionArgs &args);

   static var makeResponse(String tag, var status);

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};

#endif // AUDIOENGINE_H_INCLUDED
