#pragma once

#include "EventDispatcher.h"

// The event structure
struct Event {
  int Type;
  int arg1, arg2;
};


// Definiton of all the different type of events
enum EventType
{
   // Mouse button events. 
  E_MOUSELEFTBUTTONPRESS, 
  E_MOUSELEFTBUTTONRELEASE,

  E_WNDRESIZE,
  E_MOUSEMOVE,
  E_RELOADGUI,

  E_EDITBOX_WRITE_CHAR,
  E_EDITBOX_MOVE_CURSOR,

  //other events
  E_APPCLOSE
};


class IEventHandler {

public:
  virtual void EventHandler(const Event &e) = 0;

  // Mutator and selector
  IEventHandler * GetNextHandler(void) {return _nextHandler;}
  void SetNextHandler(IEventHandler *next) {_nextHandler = next;}

  IEventHandler() : _nextHandler(0) {
    EventDispatcher::Get()->RegisterHandler(this);
  }

protected:
  void SendEvent(int eventType, int arg1 = -1, int arg2 = -1) {
    EventDispatcher::Get()->SendEvent(eventType, arg1, arg2);
  }

private:
  IEventHandler *_nextHandler;
};