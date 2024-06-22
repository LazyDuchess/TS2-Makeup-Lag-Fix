#pragma once
#include "OverlaysTicker.h"

/// <summary>
/// Listens for UI events and ticks the OverlaysTicker.
/// </summary>
class UIEventWatcher {
public:
	UIEventWatcher(OverlaysTicker* ticker);
	void QueueTick();
	void ClearQueue();
private:
	OverlaysTicker* _ticker;
};