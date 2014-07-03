#ifndef DRAGNDROPRESPONSE_H
#define DRAGNDROPRESPONSE_H

#include "delayedresponse.h"
#include <QWidget>

class DragNDropResponse : public DelayedResponse
{
public:
    explicit DragNDropResponse(JsonClient * client, const QtJson::JsonObject & command);

protected:
    virtual void execute(int call);

signals:
    
public slots:
    
private:
    QWidget * m_src;
    QWidget * m_dest;
    QPoint m_srcPos, m_srcPosGlobal;
    QPoint m_destPos, m_destPosGlobal;
};

#endif // DRAGNDROPRESPONSE_H
