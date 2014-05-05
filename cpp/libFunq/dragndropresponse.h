#ifndef DRAGNDROPRESPONSE_H
#define DRAGNDROPRESPONSE_H

#include "delayedresponse.h"
#include <QWidget>

class DragNDropResponse : public DelayedResponse
{
public:
    enum State {
        Press,
        WaitForDragStart,
        Move,
        Release
    };

    explicit DragNDropResponse(JsonClient * client, const QtJson::JsonObject & command);

protected:
    virtual void execute(const QtJson::JsonObject & command);

signals:
    
public slots:
    
private:
    QWidget * m_src;
    QWidget * m_dest;
    QPoint m_srcPos, m_srcPosGlobal;
    QPoint m_destPos, m_destPosGlobal;
    State m_state;
};

#endif // DRAGNDROPRESPONSE_H
