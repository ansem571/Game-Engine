#ifndef WATCH_INFO_H
#define WATCH_INFO_H
class QLabel;

class WatchInfo
{
	friend class DebugMenu;
	QLabel* theLabel;
	const float *theFloat;
};

#endif