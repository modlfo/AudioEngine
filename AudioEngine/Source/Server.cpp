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

#include "Server.h"

ServerConnections::ServerConnections() : Thread("AudioEngine Connections")
{
   connections = nullptr;
}

void ServerConnections::start(const int newPortNumber, const String &localHostName, OwnedArray<StreamingSocket> *connections_)
{
   connections = connections_;
   //stop();

   // creates a socket to start listening to connections
   socket = new StreamingSocket();

   if (socket->createListener(newPortNumber, localHostName))
   {
      startThread();
   }
}

void ServerConnections::stop()
{
   signalThreadShouldExit();

   if (socket != nullptr)
      socket->close();

   stopThread(4000);
   socket = nullptr;
}

void ServerConnections::run()
{
   while ((!threadShouldExit()) && socket != nullptr)
   {
      // wait for a new conenction ...
      StreamingSocket *clientSocket(socket->waitForNextConnection());

      // and adds it to the list
      if (clientSocket != nullptr && connections != nullptr)
      {
         connections->add(clientSocket);
      }
   }
}

ServerConnections::~ServerConnections()
{
   stop();
}

Server::Server() : Thread("AudioEngine Server"), evaluator(nullptr)
{
// avoids quiting the program when a connections closes
#ifndef _WIN32
   signal(SIGPIPE, SIG_IGN);
#endif
   buffer.ensureSize(8, true);
   nConnections = 0;
}

void Server::setEngine(ServerEvaluator *engine)
{
   evaluator = engine;
}

void Server::start(const int newPortNumber, const String &localHostName)
{
   // starts the connection handler
   serverConnections.start(newPortNumber, localHostName, &connections);
   startThread();
}

void Server::stop()
{
   signalThreadShouldExit();
   serverConnections.stop();
   stopThread(4000);
}

void Server::run()
{
   while (!threadShouldExit())
   {
      if (waitForMessage())
      {
         if (evaluateMessage())
         {
            broadCastResponse();
         }
      }
   }
}

bool Server::waitForMessage()
{
   while (!threadShouldExit())
   {
      if (nConnections != connections.size())
      {
         nConnections = connections.size();
         printf("Active connections = %i\n", nConnections);
      }
      // when there are no connections just wait
      if (nConnections <= 0)
      {
         wait(100);
      }
      else
      {
         // the timeout is calculated by dividing 100 ms over the number of connections.
         // A single connection will wait 100 ms. Calculating this way the timeout implies
         // that a message could take 100/size milliseconds to be processed when there's
         // more than one connection.
         // This could be done better
         int timeout = 100 / nConnections;
         StreamingSocket **iterator = connections.begin();
         while (iterator != connections.end())
         {
            ReceiveResult result = receiveMessage(*iterator, timeout);
            if (result == MessageReceived) // exit the function
               return true;
            if (result == RemoveConnection) // remove the connection and restart the iteration
            {
               connections.removeObject(*iterator, true);
               break;
            }
            iterator++;
         }
      }
   }
   return false;
}

bool Server::evaluateMessage()
{
   if (evaluator != nullptr)
   {
      String response = evaluator->evaluate(buffer.toString());
      // gets the size in hexadecimal
      String size_x = makeStringSize(response.length());
      response_buffer.replaceWith(size_x.toRawUTF8(), size_x.length());
      response_buffer.append(response.toRawUTF8(), response.length());
      response_buffer[response.length() + size_x.length()] = '\0';
      return true;
   }
   else
      return false;
}

void Server::broadCastResponse()
{
   if (connections.size() > 0)
   {
      StreamingSocket **iterator = connections.begin();
      while (iterator != connections.end())
      {
         StreamingSocket *connection = *iterator;
         connection->write((char *)response_buffer.getData(), strlen((char *)response_buffer.getData()));
         iterator++;
      }
      //printf("response: %s\n", (char *)response_buffer.getData());
      response_buffer.replaceWith(makeStringSize(0).toRawUTF8(), 8);
   }
}

Server::ReceiveResult Server::receiveMessage(StreamingSocket *connection, int timeout)
{
   if (connection->isConnected())
   {
      const int ready = connection->waitUntilReady(true, timeout);
      if (ready > 0)
      {
         // Reads the size of the message (first 8 characters in hexadecimal)
         int status = connection->read((char *)buffer.getData(), 8, true);
         if (status < 0)
            return RemoveConnection;
         // just in case, add a null character to mark the end of the string
         buffer[8] = '\0';
         // get the size of the message
         int size = buffer.toString().getHexValue32();
         if (size > 0)
         {
            // resize the buffer if necessary
            buffer.ensureSize(size, true);
            // read the command
            status = connection->read((char *)buffer.getData(), size, true);
            if (status < 0)
               return RemoveConnection;
            // marks the end of the message in the buffer
            buffer[size] = '\0';

            printf("message: %s\n", (char *)buffer.getData());

            return MessageReceived;
         }
         // returns true, keep the connection alive
         return MessageReceived;
      }
      else if (ready == 0)
      {
         // returns true, keep the connection alive
         return KeepAlive;
      }
      else
      {
         // returns false, remove the connection
         return RemoveConnection;
      }
   }
   else
   {
      // returns false, remove the connection
      return RemoveConnection;
   }
}

Server::~Server()
{
   stop();
}
