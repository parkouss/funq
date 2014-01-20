#include "CanaryEvent.h"

namespace Hooq
{
	int CanaryEvent::m_type = QEvent::registerEventType();

	CanaryEvent::CanaryEvent(bool* ignoreFlag)
	: QEvent(static_cast<QEvent::Type>(m_type))
	, m_ignoreFlag(ignoreFlag)
	{
		Q_ASSERT(ignoreFlag);
		Q_ASSERT(*ignoreFlag == false);
		*ignoreFlag = true;
	}

	CanaryEvent::~CanaryEvent()
	{
		Q_ASSERT(*m_ignoreFlag == true);
		*m_ignoreFlag = false;
	}

	int CanaryEvent::staticType()
	{
		return m_type;
	}
};
