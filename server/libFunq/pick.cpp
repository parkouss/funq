#include "pick.h"
#include <QMouseEvent>
#include <QMetaProperty>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "objectpath.h"

Pick::Pick(PickHandler * handler, QObject * parent) : QObject(parent), m_handler(handler) {}

Pick::~Pick() {
    if (m_handler) {
        delete m_handler;
    }
}

bool Pick::handleEvent(QObject * receiver, QEvent * event) {
    if (!m_handler) { return false; }
    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *evt = static_cast<QMouseEvent *>(event);
        if (evt->modifiers() & Qt::ShiftModifier && evt->modifiers() & Qt::ControlModifier) {
            m_handler->handle(receiver, evt->pos());
            return true;
        }
    }
    return false;
}

PickFormatter::PickFormatter() : m_stream(stdout, QIODevice::WriteOnly), m_showProperties(true) {}

void print_object_props(QObject * object, QTextStream & stream) {
    for(int i = 0; i < object->metaObject()->propertyCount(); ++i) {
        QMetaProperty property = object->metaObject()->property(i);
        QString strValue = property.read(object).toString();
        if (! strValue.isEmpty()) {
            stream << "\t" << property.name() << ": " << strValue << endl;
        }
    }
}

void PickFormatter::handle(QObject * object, const QPoint & pos) {
    QString path = QString("WIDGET: `%1` (pos: %2, %3)")
                    .arg(ObjectPath::objectPath(object))
                    .arg(pos.x())
                    .arg(pos.y());
    m_stream << path << endl;
    if (m_showProperties) {
        print_object_props(object, m_stream);
    }
    
    QGraphicsView * view = dynamic_cast<QGraphicsView *>(object->parent());
    if (view) {
        QGraphicsItem * item = view->itemAt(pos);
        QObject * qitem = dynamic_cast<QObject *>(item);
        if (item) {
            m_stream << "GITEM: `" << ObjectPath::graphicsItemPath(item) << "` (QObject: " << (qitem !=0) << ")" << endl;
            if (qitem) {
                print_object_props(qitem, m_stream);
            }
        }
    }
}
