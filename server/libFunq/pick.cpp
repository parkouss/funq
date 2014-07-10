/*
Copyright: SCLE SFE
Contributor: Julien Pagès <j.parkouss@gmail.com>

This software is a computer program whose purpose is to test graphical
applications written with the QT framework (http://qt.digia.com/).

This software is governed by the CeCILL v2.1 license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL v2.1 license and that you accept its terms.
*/

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
