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

#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

// Base class for clases that can be plugged to the server
class ServerEvaluator
{
 public:
   // Main evaluation function, takes the command as a string and returns the response as a string too.
   virtual String evaluate(const String &) = 0;
   virtual ~ServerEvaluator(){};
};

// Creates a thread that handles new connections from clients.abs
class ServerConnections : private Thread
{
 public:
   ServerConnections();
   ~ServerConnections();

   // Starts the server listening to the given port number and host, also receives the array of connections
   // Note: this function is called by Server.start(...)
   void start(const int newPortNumber, const String &localHostName, OwnedArray<StreamingSocket> *connections);
   void stop();

 private:
   void run() override;

   StreamingSocket *socket;
   // pointer to the array of connections
   OwnedArray<StreamingSocket> *connections;

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ServerConnections)
};

// Creates a thread that handles incomming messages
class Server : private Thread
{
 public:
   Server();
   ~Server();

   // Starts the server listening to the given port and host
   void start(const int newPortNumber, const String &localHostName);
   void stop();
   void setEngine(ServerEvaluator *evaluator);

 private:
   void run() override;

   enum ReceiveResult
   {
      MessageReceived,
      RemoveConnection,
      KeepAlive
   };

   bool receive(StreamingSocket *connection, int timeout);

   ReceiveResult receiveMessage(StreamingSocket *connection, int timeout);

   bool waitForMessage(void);
   bool evaluateMessage(void);
   void broadCastResponse();

   String makeStringSize(int size_i)
   {
      String size_x = String::toHexString(size_i);
      String fill = String::repeatedString("0", 8 - size_x.length());
      return fill + size_x;
   };

   int nConnections;

   ServerConnections serverConnections;
   OwnedArray<StreamingSocket> connections;

   MemoryBlock buffer;
   MemoryBlock response_buffer;

   ServerEvaluator *evaluator;

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Server)
};

#endif // SERVER_H_INCLUDED
