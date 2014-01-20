#pragma once

#include <QEvent>

namespace Hooq
{
	/** QEvent subclass to avoid logging of infered events.
	 *
	 * After an event is logged, this will be appended to the event queue.
	 * Until it is received, no more events will be logged. This should avoid
	 * infered events being logged.
	 */
	class CanaryEvent : public QEvent
	{
		public:
			/** Create a CanaryEvent.
			 *
			 * @p ignoreFlag is a variable that will be set to true while this object exists.
			 *    *ignoreFlag must be false when this object is created, and will be reset to
			 *    false when the application is destroyed (after the processing of the Qt event loop)
			 */
			CanaryEvent(bool* ignoreFlag);
			virtual ~CanaryEvent();
			static int staticType();
		private:
			static int m_type;
			bool* m_ignoreFlag;
	};
};
