#pragma once
#include "OverlaysTicker.h"

class UIEventWatcher {
public:
	UIEventWatcher(OverlaysTicker* ticker);
	void QueueTick();
	void ClearQueue();
private:
	OverlaysTicker* _ticker;
};