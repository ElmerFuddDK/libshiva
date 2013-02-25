// This is the main header for SHIVA documentation

#error Do not include doxysource headers - they are for documentation only

/**

\mainpage SHIVA - Cross platform application framework

\section abstract Introduction

SHIVA stands for Small High performance Intelligent Versatile Application
framework.\n

The general idea about SHIVA is fine grained modularity. A SHIVA program
concists of a huge amount of building blocks called modules. These are
put together to form the actual application. It is the UNIX idea of having
alot of small entities that each solve a simple task, and solves it well.
You can then put these blocks together to form a single highly modular and
extendable application.
\n
Furthermore the goal of the framework is to achieve the best possible
cross-platform compatibility, without sacrificing too much performance to
overhead.

\section sections Sections

Below is a list of sections in this manual:

- \subpage shivabasics
- \subpage gettingstarted
- \subpage codingconventions
- \subpage tutorials
- \subpage examples
- \subpage referencemanual
- \subpage licenses


\page shivabasics Shiva basics

A SHIVA application is in its essence a group of application building
blocks called modules. These are loaded into the module list, and then
booted up. This module list is the glue that keeps the modules together,
and it is from here they can resolve each other.\n
They can talk to each other through events. An event can be emitted from
a module, but also from the module list.\n
What kind of application a SHIVA application is, is defined by something
called a main event dispatcher. This class will be responsible for
distributing events during the applications lifetime, and the way the events
are distributed depends on what the application is doing. For instance,
when we are talking about a GUI, the events will be dispatched when the
GUI is idle.

\section term_event Event

An event is an object that is emitted to subscribers. There are 2 types of
events : normal and string based. The normal events are identified by a number,
and the string based ones are identified by a string.
Normal events are usually used when emitting from a known object (eg. a module),
whereas string based events are used when emitting global events (emitted on
the \ref term_modulelist).

\section term_eventqueue Event queue

A class that handles enqueueing and dispatching of events. Each event queue
normally has its own thread.

\section term_eventsubscriber Event subscriber

A subscriber is an object used when subscribing on events. They can be used
for callbacks as well. The subscriber determines how the target receives the
event. It can queue it up on an event queue, or process it directly.\n
The standard event subscriber will process the event on the target through a
method called OnEvent.

\section term_module Module

A module is a building block for a program. In order to use a module you will
need to create it and add it to the module list before the dispatcher is
booted up.

\section term_modulelist Module list

The module list is the glue for the modules. You run the application by
filling the modules into this list, and then registering them. Each module
will then resolve its dependencies and its event subscriptions, and the
program will start.


\page gettingstarted Getting started

For the impatient I suggest checking out the \ref tutorials page.\n
First off a little explanation as to what different parts SHIVA concists of.

\section getstart_sourcestructure Source code structure

- examples\n
  Contains example projects
- include\n
  Contains the public include files for libshiva and the included
  module libraries:
  - framework\n
    The SHIVA application framework (libshiva)
  - frameworkimpl\n
    The framework includes needed to implement an actual SHIVA app
  - gui\n
    SHIVA gui includes
  - modules\n
    Contains includes for each module library
  - mysql\n
    Headers for the SHIVA mysql library
  - platforms\n
    Platform specific headers included form platformspc.h
  - sqlite\n
    Headers for the SHIVA sqlite library
  - threadutils\n
    Headers for thread oriented utility classes
  - utils\n
    Headers for utility classes
- shivagui\n
  Standard implementations of SHIVA gui
  - gtk\n
    GTK based SHIVA gui
  - gui\n
    Common classes to include in shivagui implementations
  - utilsimpl\n
    Common classes to include in shivagui implementations
  - win32\n
    Win32 based SHIVA gui
- shivalib\n
  Implementation of shivalib. This library is what every SHIVA
  module lirary or application links against.
- shivamodulelibs\n
  Contains implementation of various standard module libraries
  - dataengine\n
    Implements a simple data engine that uses shiva sqlite
  - socketserver\n
    Implements a TCP/UDP socket system
  - subprocess\n
    Implements a method to launch sub processes and stream
    data to and from them
  - xmlstream\n
    Implements an xml reader (using expat) and writer
- shivamysql\n
  Interface for a MySQL database
- shivasqlite\n
  Includes implementation of sqlite for SHIVA

\section getstart_libshiva libshiva

This is the library that contains the cross platform utility classes, as well
as the SHIVA framework. It is the main building block of all SHIVA libraries.

\section getstart_shivagui shivagui

The general idea behind SHIVA GUI is that we should utilize the GUI subsystem
if the different platforms, in order to produce an application that is as true
as possible to the different platforms.\n
This is done through wrapping the native controls into control implementers
that are in turn used by the SHIVA controls. SHIVA then takex over layout
management, and implements a uniform way of communicating with the controls.


\page codingconventions Coding conventions

This page will contain the various coding conventions used in SHIVA.


\page tutorials Small tutorial

\section intro Introduction

In this page we will create a simple console application

\code
#include "shiva/include/platformspc.h"
#include "shiva/include/shvversion.h"
#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/framework/shveventstdin.h"
#include "shiva/include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"

class SHVMain : public SHVModule
{
public:

	// constructor
	SHVMain(SHVModuleList& modules);
	
	// Register method - will be called before the application is "booted"
	SHVBool Register();
	void PostRegister();

	// The event target method
	void OnEvent(SHVEvent* event);
	
};

// Constructor
SHVMain::SHVMain(SHVModuleList& modules) : SHVModule(modules,"Main")
{}

// Register
SHVBool SHVMain::Register()
{
	SHVConsole::Printf8("In register\n");
	Modules.EventSubscribe(__EVENT_GLOBAL_STDIN, new SHVEventSubscriber(this));
	return SHVModule::Register();
}

// PostRegister
void SHVMain::PostRegister()
{
	SHVModule::PostRegister();
	SHVConsole::Printf8("Application running\n");
	SHVConsole::Printf8("You can now write /quit to quit the application\n");
}

// OnEvent
void SHVMain::OnEvent(SHVEvent* event)
{
	if (SHVEventString::Equals(event,__EVENT_GLOBAL_STDIN))
	{
	SHVString8 str(SHVEventStdin::StdinFromEvent(event));
	
		if (str == SHVString8C("/quit"))
		{
			Modules.CloseApp();
		}
		else
		{
			SHVConsole::Printf8("Invalid input : %s\n", str.GetSafeBuffer());
		}
	}
}


// The main function - boots up the application
CONSOLEMAIN()
{
	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		SHVConsole::ErrPrintf8("WRONG SHIVA VERSION\n");
	}
	else
	{
	// Initialize the main thread event queue - we are a console application
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());

		// Add our application modules
		mainqueue.GetModuleList().AddModule(new SHVMain(mainqueue.GetModuleList()));
		
		// run the application and return
		return mainqueue.Run().GetError();
	}
	
	return -1;
}
\endcode

In this example we create a simple module, and boot up a console application with
this module present.\n
A console application emits a global event with console input. This can be used
to perform actions, and the main module uses it to quit the application.

\page examples Examples

This page will contain code samples.

\page referencemanual Reference manual

Here is a link to reference manuals for the different libraries in SHIVA.\n
<a class="el" href="libshiva/index.html">lib shiva</a>\n
<a class="el" href="shivagui/index.html">shiva gui</a>\n

\page licenses SHIVA license, and included software

All parts but the ones listed here are covered by the license at the bottom:\n
 - SQLite in shivasqlite/src/sqlite
 - expat in shivamodulelibs/xmlstream/src/expat
 - UUID generation in shivalib/src/uuid
 - md5 generation in shivalib/src/md5
 - libunicode in shivalib/src/libunicode
 - lua in shivamodulelibs/luaengine/src/lua

\n
Copyright (C) 2008 by Lars Eriksen and others\n
\n
\n
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version with the following
exeptions:\n
\n
1) Static linking to the library does not constitute derivative work\n
   and does not require the author to provide source code for the\n
   application.\n
   Compiling applications with the source code directly linked in is\n
   Considered static linking as well.\n
\n
2) You do not have to provide a copy of the license with programs\n
   that are linked against this code.\n
\n
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.\n
\n
You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc.,\n
59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n


*/
