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

#include "AudioEngine.h"

Audio::Audio()
{
   // prepares the application properties
   PropertiesFile::Options options;
   options.applicationName = String("AudioEngine");
   options.filenameSuffix = "settings";
   options.osxLibrarySubFolder = "Preferences";
   options.commonToAllUsers = true;
   properties.setStorageParameters(options);

   appProperties.setStorageParameters(options);

   ScopedPointer<XmlElement> savedAudioState(appProperties.getUserSettings()->getXmlValue("audioDeviceState"));

   String audioError = deviceManager.initialise(256, 256, savedAudioState, true);
   deviceManager.addAudioCallback(&audioSourcePlayer);
   audioSourcePlayer.setSource(this);
   deviceManager.addMidiInputCallback(String::empty, &midiMessageCollector);
   deviceManager.restartLastAudioDevice();
   AudioIODevice *device = deviceManager.getCurrentAudioDevice();
   if (device)
      device->stop();
}

Audio::~Audio()
{
   audioSourcePlayer.setSource(nullptr);
   deviceManager.removeAudioCallback(&audioSourcePlayer);
   deviceManager.closeAudioDevice();
}

void Audio::start()
{
   deviceManager.restartLastAudioDevice();
}

void Audio::stop()
{
   deviceManager.closeAudioDevice();
}

void Audio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
   midiMessageCollector.reset(sampleRate);
}

void Audio::releaseResources()
{
}

void Audio::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill)
{
   MidiBuffer midi_buffer;
   midiMessageCollector.removeNextBlockOfMessages(midi_buffer, bufferToFill.numSamples);
   if (!midi_buffer.isEmpty())
   {
      int n_events = midi_buffer.getNumEvents();
      printf("Got Events %i\n", n_events);
   }

   const float *in0 = bufferToFill.buffer->getReadPointer(0);
   const float *in1 = bufferToFill.buffer->getReadPointer(1);
   float *out0 = bufferToFill.buffer->getWritePointer(0);
   float *out1 = bufferToFill.buffer->getWritePointer(1);

   // sets the buffers in the lua side only if the pointers have changed
   if (in0 != in0_pre || in1 != in1_pre || out0 != out0_pre || out1 != out1_pre)
   {
      lua.setInputBuffers(in0, in1);
      lua.setOutputBuffers(out0, out1);
      in0_pre = in0;
      in1_pre = in1;
      out0_pre = out0;
      out1_pre = out1;
   }

   // calls the process function from the lua side
   lua.process(bufferToFill.numSamples);
}

const OwnedArray<AudioIODeviceType> &Audio::getAvailableDeviceTypes(void)
{
   return deviceManager.getAvailableDeviceTypes();
}

String Audio::getConfig(void)
{
   return deviceManager.createStateXml()->createDocument(String());
}

var Audio::getAudioDeviceSetup()
{
   DynamicObject *result = new DynamicObject();
   AudioDeviceManager::AudioDeviceSetup setup;
   deviceManager.getAudioDeviceSetup(setup);
   result->setProperty("outputDeviceName", var(setup.outputDeviceName));
   result->setProperty("inputDeviceName", var(setup.inputDeviceName));
   result->setProperty("sampleRate", var(setup.sampleRate));
   result->setProperty("bufferSize", var(setup.bufferSize));
   result->setProperty("inputChannels", var(setup.inputChannels.toInt64()));
   result->setProperty("useDefaultInputChannels", var(setup.useDefaultInputChannels));
   result->setProperty("outputChannels", var(setup.outputChannels.toInt64()));
   result->setProperty("useDefaultOutputChannels", var(setup.useDefaultOutputChannels));
   return result;
}

var Audio::setAudioDeviceSampleRate(var arg)
{
   AudioDeviceManager::AudioDeviceSetup setup;
   deviceManager.getAudioDeviceSetup(setup);
   setup.sampleRate = arg;
   deviceManager.setAudioDeviceSetup(setup, true);
   saveSettings();
   return getAudioDeviceSetup();
}

var Audio::setAudioDeviceBufferSize(var arg)
{
   AudioDeviceManager::AudioDeviceSetup setup;
   deviceManager.getAudioDeviceSetup(setup);
   setup.bufferSize = arg;
   deviceManager.setAudioDeviceSetup(setup, true);
   saveSettings();
   return getAudioDeviceSetup();
}

var Audio::setAudioDeviceInputName(var arg)
{
   AudioDeviceManager::AudioDeviceSetup setup;
   deviceManager.getAudioDeviceSetup(setup);
   setup.inputDeviceName = arg;
   deviceManager.setAudioDeviceSetup(setup, true);
   saveSettings();
   return getAudioDeviceSetup();
}

var Audio::setAudioDeviceOutputName(var arg)
{
   AudioDeviceManager::AudioDeviceSetup setup;
   deviceManager.getAudioDeviceSetup(setup);
   setup.outputDeviceName = arg;
   deviceManager.setAudioDeviceSetup(setup, true);
   return getAudioDeviceSetup();
}

void Audio::saveSettings(void)
{
   // Saves the settings
   ScopedPointer<XmlElement> audioState(deviceManager.createStateXml());

   appProperties.getUserSettings()->setValue("audioDeviceState", audioState);
   appProperties.getUserSettings()->saveIfNeeded();
}

var Audio::getCurrentAudioDeviceType()
{
   return var(deviceManager.getCurrentAudioDeviceType());
}

var Audio::setCurrentAudioDeviceType(var arg)
{
   deviceManager.setCurrentAudioDeviceType(arg.toString(), false);
   return var(deviceManager.getCurrentAudioDeviceType());
}

var stringArrayToVar(StringArray array)
{
   Array<var> empty;
   var result(empty);
   for (int i = 0; i < array.size(); i++)
   {
      result.append(var(array[i]));
   }
   return result;
}

var floatArrayToVar(Array<double> array)
{
   var result;
   for (int i = 0; i < array.size(); i++)
   {
      result.append(var(array[i]));
   }
   return result;
}

var intArrayToVar(Array<int> array)
{
   var result;
   for (int i = 0; i < array.size(); i++)
   {
      result.append(var(array[i]));
   }
   return result;
}

var Audio::getCurrentAudioDevice()
{
   DynamicObject *result = new DynamicObject();
   AudioIODevice *device = deviceManager.getCurrentAudioDevice();
   if (device)
   {
      result->setProperty("outputChannelNames", stringArrayToVar(device->getOutputChannelNames()));
      result->setProperty("inputChannelNames", stringArrayToVar(device->getInputChannelNames()));
      result->setProperty("availableSampleRates", floatArrayToVar(device->getAvailableSampleRates()));
      result->setProperty("availableBufferSizes", intArrayToVar(device->getAvailableBufferSizes()));
      result->setProperty("defaultBufferSize", var(device->getDefaultBufferSize()));
      result->setProperty("currentBufferSizeSamples", var(device->getCurrentBufferSizeSamples()));
      result->setProperty("currentSampleRate", var(device->getCurrentSampleRate()));
   }
   return result;
}

var Audio::loadFiles(var files)
{
   return lua.loadFiles(files);
}

// Evaluates the js code and returns the result
String AudioEngine::evaluate(const String &javascriptCode)
{
   return JSON::toString(js.evaluate(javascriptCode));
}

bool AudioEngine::repeat(void)
{
   // Returns 'audio.repeat'
   return audio_js->getProperty("repeat");
}

AudioEngine::AudioEngine()
{
   // Creates the 'audio' object that hold the properties
   audio_js = new DynamicObject();
   // Defines 'audio.quit()'
   audio_js->setMethod("quit", AudioEngine::quit);
   // Defines 'audio.stop()'
   audio_js->setMethod("stop", AudioEngine::stop);
   // Defines 'audio.start()'
   audio_js->setMethod("start", AudioEngine::start);
   // Defines 'audio.getAudioDevices()'
   audio_js->setMethod("getAudioDevices", AudioEngine::getAudioDevices);
   // Defines 'audio.getAudioDeviceSetup()'
   audio_js->setMethod("getAudioDeviceSetup", AudioEngine::getAudioDeviceSetup);
   // Defines 'audio.getCurrentAudioDeviceType()'
   audio_js->setMethod("getCurrentAudioDeviceType", AudioEngine::getCurrentAudioDeviceType);
   // Defines 'audio.setCurrentAudioDeviceType()'
   audio_js->setMethod("setCurrentAudioDeviceType", AudioEngine::setCurrentAudioDeviceType);
   // Defines 'audio.getCurrentAudioDevice()'
   audio_js->setMethod("getCurrentAudioDevice", AudioEngine::getCurrentAudioDevice);
   // Defines 'audio.getMidiDevices()'
   audio_js->setMethod("getMidiDevices", AudioEngine::getMidiDevices);
   // Defines 'audio.loadFiles([...])'
   audio_js->setMethod("loadFiles", AudioEngine::loadFiles);

   audio_js->setMethod("setAudioDeviceSampleRate", AudioEngine::setAudioDeviceSampleRate);
   audio_js->setMethod("setAudioDeviceBufferSize", AudioEngine::setAudioDeviceBufferSize);
   audio_js->setMethod("setAudioDeviceInputName", AudioEngine::setAudioDeviceInputName);
   audio_js->setMethod("setAudioDeviceOutputName", AudioEngine::setAudioDeviceOutputName);

   // Sets 'audio.repeat = true'
   audio_js->setProperty("repeat", var(true));
   audio_js->setProperty("status", var(new DynamicObject()));

   // Registers the object 'audio' in the JavaScript engine
   js.registerNativeObject("audio", audio_js);

   // Gets the 'Audio' instance for the first time so it will be initialized
   AudioInstance::get();
}

AudioEngine::~AudioEngine()
{
}

var AudioEngine::setStatusProperty(const var::NativeFunctionArgs &args, String property, var value)
{
   DynamicObject *status = args.thisObject.getDynamicObject()->getProperty("status").getDynamicObject();
   status->setProperty(property, value);
   var status_var = var(status);
   args.thisObject.getDynamicObject()->setProperty("status", var(status));
   return status_var;
}

var AudioEngine::getStatus(const var::NativeFunctionArgs &args)
{
   var status_var = args.thisObject.getDynamicObject()->getProperty("status");
   return status_var;
}

var AudioEngine::makeResponse(String tag, var status)
{
   DynamicObject *response = new DynamicObject();
   response->setProperty("tag", var(tag));
   response->setProperty("data", status);
   return var(response);
}

// Function 'audio.quit()'
var AudioEngine::quit(const var::NativeFunctionArgs &args)
{
   // Sets 'audio.repeat = false'
   args.thisObject.getDynamicObject()->setProperty("repeat", var(false));
   return makeResponse("status", getStatus(args));
}

// Function 'audio.stop()'
var AudioEngine::stop(const var::NativeFunctionArgs &args)
{
   AudioInstance::get().stop();
   var status = setStatusProperty(args, "dsp", var(false));
   return makeResponse("status", status);
}

// Function 'audio.start()'
var AudioEngine::start(const var::NativeFunctionArgs &args)
{
   AudioInstance::get().start();
   var status = setStatusProperty(args, "dsp", var(true));
   return makeResponse("status", status);
}

// Function 'audio.getAudioDevices()'
var AudioEngine::getAudioDevices(const var::NativeFunctionArgs &args)
{
   var result;

   // gets the types of the devices
   const OwnedArray<AudioIODeviceType> &types = AudioInstance::get().getAvailableDeviceTypes();

   for (int i = 0; i < types.size(); i++)
   {
      AudioIODeviceType *device_type = types[i];
      // each type can have more than one device
      device_type->scanForDevices();
      DynamicObject *elem = new DynamicObject();
      elem->setProperty("type", var(device_type->getTypeName()));
      var outputs, inputs;
      StringArray output_names = device_type->getDeviceNames();
      for (int j = 0; j < output_names.size(); j++)
      {
         outputs.append(var(output_names[j]));
      }
      StringArray input_names = device_type->getDeviceNames(true);
      for (int j = 0; j < input_names.size(); j++)
      {
         inputs.append(var(input_names[j]));
      }
      elem->setProperty("outputs", outputs);
      elem->setProperty("inputs", inputs);
      result.append(var(elem)); // using append makes it an array
   }

   return makeResponse("getAudioDevicesResponse", result);
}

// Function 'audio.getCurrentAudioDeviceType()'
var AudioEngine::getCurrentAudioDeviceType(const var::NativeFunctionArgs &args)
{
   var result = AudioInstance::get().getCurrentAudioDeviceType();
   return makeResponse("getCurrentAudioDeviceTypeResponse", result);
}

// Function 'audio.setCurrentAudioDeviceType()'
var AudioEngine::setCurrentAudioDeviceType(const var::NativeFunctionArgs &args)
{
   var result = AudioInstance::get().setCurrentAudioDeviceType(args.arguments[0]);
   return makeResponse("getCurrentAudioDeviceTypeResponse", result);
}

// Function 'audio.getCurrentAudioDevice()'
var AudioEngine::getCurrentAudioDevice(const var::NativeFunctionArgs &args)
{
   var result = AudioInstance::get().getCurrentAudioDevice();
   return makeResponse("getCurrentAudioDeviceResponse", result);
}

// Function 'audio.getAudioDeviceSetup()'
var AudioEngine::getAudioDeviceSetup(const var::NativeFunctionArgs &args)
{
   var result = AudioInstance::get().getAudioDeviceSetup();
   return makeResponse("getAudioDeviceSetupResponse", result);
}

// Function 'audio.getMidiDevices()'
var AudioEngine::getMidiDevices(const var::NativeFunctionArgs &args)
{
   DynamicObject *result = new DynamicObject();
   // two arrays for inputs and outputs
   var inputs, outputs;

   // fills the array of inputs
   StringArray inputs_names = MidiInput::getDevices();
   for (int i = 0; i < inputs_names.size(); i++)
   {
      inputs.append(var(inputs_names[i]));
   }

   // fills the array of outputs
   StringArray output_names = MidiOutput::getDevices();
   for (int i = 0; i < output_names.size(); i++)
   {
      outputs.append(var(output_names[i]));
   }

   // the result is an object with two properties
   result->setProperty("inputs", inputs);
   result->setProperty("outputs", outputs);

   return makeResponse("getMidiDevicesResponse", result);
}

var AudioEngine::loadFiles(const var::NativeFunctionArgs &args)
{
   if (args.numArguments == 1)
   {
      var files = args.arguments[0];
      if (files.isArray())
         AudioInstance::get().loadFiles(files);
      //else
      //   return (var(false));
   }
   //else
   //   return (var(false));

   return makeResponse("status", getStatus(args));
}

// Function 'audio.setAudioDeviceSampleRate()'
var AudioEngine::setAudioDeviceSampleRate(const var::NativeFunctionArgs &args)
{
   var result = AudioInstance::get().setAudioDeviceSampleRate(args.arguments[0]);
   return makeResponse("getAudioDeviceSetupResponse", result);
}

// Function 'audio.setAudioDeviceBufferSize()'
var AudioEngine::setAudioDeviceBufferSize(const var::NativeFunctionArgs &args)
{
   var result = AudioInstance::get().setAudioDeviceBufferSize(args.arguments[0]);
   return makeResponse("getAudioDeviceSetupResponse", result);
}

// Function 'audio.setAudioDeviceInputName()'
var AudioEngine::setAudioDeviceInputName(const var::NativeFunctionArgs &args)
{
   var result = AudioInstance::get().setAudioDeviceInputName(args.arguments[0]);
   return makeResponse("getAudioDeviceSetupResponse", result);
}

// Function 'audio.setAudioDeviceOutputName()'
var AudioEngine::setAudioDeviceOutputName(const var::NativeFunctionArgs &args)
{
   var result = AudioInstance::get().setAudioDeviceOutputName(args.arguments[0]);
   return makeResponse("getAudioDeviceSetupResponse", result);
}
