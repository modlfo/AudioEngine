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

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "AudioEngine.h"
#include "LuaVM.h"
#include "Server.h"

class Main : public JUCEApplication
{
 public:
   //==============================================================================
   Main() {}

   const String getApplicationName() override { return ProjectInfo::projectName; }
   const String getApplicationVersion() override { return ProjectInfo::versionString; }
   bool moreThanOneInstanceAllowed() override { return true; }

   //==============================================================================
   void initialise(const String &commandLine) override
   {
      // This method is where you should put your application's initialisation code..
      server.start(8000, "127.0.0.1");
      server.setEngine((ServerEvaluator *)&engine);
   }

   void shutdown() override
   {
      // Add your application's shutdown code here..
   }

   //==============================================================================
   void systemRequestedQuit() override
   {
      // This is called when the app is being asked to quit: you can ignore this
      // request and let the app carry on running, or call quit() to allow the app to close.
      quit();
   }

   void anotherInstanceStarted(const String &commandLine) override
   {
      // When another instance of the app is launched while this one is running,
      // this method is invoked, and the commandLine parameter tells you what
      // the other instance's command-line arguments were.
   }

 private:
   AudioEngine engine;
   Server server;
};

#endif // MAIN_H_INCLUDED
