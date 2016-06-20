#pragma once


class IEventHandler;

class EventDispatcher {

// definitions necessary to handle a singleton class
public:
  // returns a pointer to the EventDispatcher singleton instance
  static EventDispatcher*	Get();
  static void ResetInstance()
  {
	  delete _instanceOf; // REM : it works even if the pointer is NULL (does nothing then)
	  _instanceOf = nullptr; // so GetInstance will still work.
  }

private:
  // these are private so that only instance will ever be created
  // the _deviceList pointer is initialized to null as the linked list
  // is null-terminated
  EventDispatcher(void) : _deviceList(0) {;}
  ~EventDispatcher(void) {;}

  // pointer to singleton class
  static EventDispatcher*	_instanceOf;

// definitions of methods which handle events
public:

  void RegisterHandler(IEventHandler *device);

  // sends the event to all the devices registered to listen
  void SendEvent(int eventType, int arg1 = -1, int arg2 = -1);

private:
  IEventHandler *_deviceList;
};