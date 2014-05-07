#ifndef OBJECTPATH_H
#define OBJECTPATH_H

#include <QString>
#include <QObject>

class QGraphicsItem;
class QGraphicsView;

namespace ObjectPath {
    QString objectPath(QObject* object);
    QString objectName(QObject* object);
    QObject* findObject(const QString& path);

    QString graphicsItemPath(QGraphicsItem * item);
    int graphicsItemPos(QGraphicsItem * item);
    QGraphicsItem * graphicsItemFromPath(QGraphicsView * view, const QString & stackPath);
} // namespace ObjectPath

#endif // OBJECTPATH_H
