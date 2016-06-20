#include "EventDispatcher.h"

#include "IEventHandler.h"
#include "Memtrack.h"
// pointer to singleton instance
EventDispatcher* EventDispatcher::_instanceOf = 0;

EventDispatcher* EventDispatcher::Get() {
	if (_instanceOf)
		return _instanceOf;
	return _instanceOf = new EventDispatcher();
}

void EventDispatcher::RegisterHandler(IEventHandler *device) {
  device->SetNextHandler(_deviceList);
  _deviceList = device;
}


void EventDispatcher::SendEvent(int eventType, int arg1, int arg2) {
  Event e;
  e.Type = eventType;
  e.arg1 = arg1;
  e.arg2 = arg2;
  IEventHandler * curDevice = _deviceList;
  for (; curDevice; curDevice = curDevice->GetNextHandler())
    curDevice->EventHandler(e);
}