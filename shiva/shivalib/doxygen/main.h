
// This is the main header for Doxygen documentation

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
You can then put these blocks together to from a single highly modular and
extendable application.
\n
Now, it takes alot more than just an application framework to achieve
this. Of course you need a highly modular framework to do this, but
you also need a good design philosophy.

\section sections Sections

Below is a list of sections in this manual:

- \subpage philosophy
- \subpage terminology
- \subpage tutorial


\page philosophy Design philosophy

\section problem The problem

The need for SHIVA came to be during the development of applications for
mobile computers. These computers are very different, thus making solutions
to simple problems alot harder if you want to design cross platform
applications.
\n
Since these computers are very different both in design and choice of
operating systems they require alot of code that are tailored to them. They
are often quite restricted when it comes to memory, so you can't have a too
large API dragging you down.

\section goal The goal

To create a lightweight and modular cross platform application framework
and API.

\section solution The solution

Alot of the terms in this section are explained in the section called
\ref terminology.
\n \n
In order to achieve this the code is split up into a list of modules. These
modules each solve a specific task. This can be simple things such as timers,
sound events, button panels, control factories etc.
\n \n
These modules are bound together by the module list. The application is started
through the module list, by registering them, and then running the main event
queue.
\n \n
On register each module will then resolve the other modules it depends on. If it
is dependent on events from other modules it will then subscribe to these events.
Each module that provides functionality for other modules can make this
dynamic by exposing them through methods, or by emitting events.\n
For instance, if you want something validated, you can emit a validation event
with an event object on it. Subscribers can then perform validation on the
subscriptions, and set the validation state on the object provided with the
event. That way the emitting module doesn't have to know about other modules.



\page terminology Terminology

\section term_event Event

An event is a reference counted object that is emitted to subscribers. There
are 2 types of events : normal and string based. The normal events are
identified by a number, and the string based ones are identified by a string.
Normal events are usually used when emitting from a known object (eg. a module),
whereas string based events are used when emitting global events (emitted on
the \ref term_modulelist).

\section term_eventqueue Event queue

A class that handles enqueueing and dispatching of events. Each event queue
normally has its own thread.

\section term_eventsubscriber Event subscriber

A subscriber is a reference counted object used when subscribing on events.
They can be used for callbacks as well.

\section term_module Module

A module is a building block for a program.

\section term_modulelist Module list

The module list is the glue for the modules. You run the application by
filling the modules into this list, and then registering them. Each module
will then resolve its dependencies and its event subscriptions, and the
program will start.



\page tutorial Small tutorial

\section intro Introduction

In this page we will create our first application in SHIVA.

\code
void main()
{
	printf("Hello world\n");
}
\endcode

Mmmkay, no SHIVA to see here - movealong ....
*/
