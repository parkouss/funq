#include "objectpath.h"

#include <QWidget>
#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsView>

/**
  * Renvoie le nom de l'objet, possiblement en doublon par rapport aux siblings
  */
inline QString _rawObjectName(QObject* object) {
    QString rawName = object->objectName();

    if (rawName.isEmpty()) {
        rawName = object->metaObject()->className();
    }
    return rawName;
}

/**
  * Renvoie le nom de l'objet, unique par rapport aux siblings
  */
QString rawObjectName(QObject* object)
{
    QString rawName = _rawObjectName(object);

    if(!object->parent())
    {
        return rawName;
    }

    const QList<QObject*> siblings = object->parent()->children();

    int index = 0;
    Q_FOREACH(QObject* sibling, siblings)
    {
        if(sibling == object)
        {
            break;
        }
        QString siblingName = _rawObjectName(sibling);
        if(siblingName == rawName)
        {
            ++index;
        }
    }
    if (index == 0) {
        return rawName;
    }
    return QString(
        "%1-%2"
    ).arg(
        rawName
    ).arg(
        index
    );
}

QString objectPath(QObject* object)
{
    QStringList components;
    QObject* current = object;
    while(current)
    {
        components.prepend(objectName(current));
        current = current->parent();
    }
    return components.join("::");
}

QString objectName(QObject* object)
{
    QString name = rawObjectName(object);
    name.replace("::", ":_:"); // we use :: as path separators
    return name;
}

QObject* findObject(const QString& path)
{
    const QString separator("::");
    QStringList parts = path.split(separator);
    if(parts.isEmpty())
    {
        return 0;
    }
    const QString name = parts.takeLast();
    QObject* parent = 0;
    if(parts.isEmpty())
    {
        // Top level widget
        Q_FOREACH(QWidget* widget, QApplication::topLevelWidgets())
        {
            if(objectName(widget) == name)
            {
                return widget;
            }
        }
        return 0;
    }
    else
    {
        parent = findObject(parts.join(separator));
        if(!parent)
        {
            return 0;
        }
    }

    Q_FOREACH(QObject* child, parent->children())
    {
        if(objectName(child) == name)
        {
            return child;
        }
    }

    return 0;
}

int graphicsItemPos(QGraphicsItem *item) {
    if (item->parentItem()) {
        return item->parentItem()->children().indexOf(item);
    }
    QGraphicsScene * scene = item->scene();
    int pos = 0;
    foreach (QGraphicsItem * item_, scene->items()) {
        if (!item_->parentItem()) {
            if (item == item_) {
                return pos;
            }
            pos++;
        }
    }
    return -1;
}

QString graphicsItemPath(QGraphicsItem *item) {
    QStringList path;
    while (item) {
        path.prepend(QString::number(graphicsItemPos(item)));
        item = item->parentItem();
    }
    return path.join("/");
}

QGraphicsItem * graphicsItemFromPath(QGraphicsView * view, const QString & stackPath) {
    QStringList path = stackPath.split('/');
    if (stackPath.isEmpty()) {
        return NULL;
    }
    bool conv_ok;
    int index;

    index = path.at(0).toInt(&conv_ok);
    if (!conv_ok || index < 0) {
        return NULL;
    }
    path.removeFirst();

    // recherche de la racine
    QGraphicsItem * root = NULL;
    int pos = 0;
    foreach (QGraphicsItem * item, view->items()) {
        if (!item->parentItem()) {
            if (pos == index) {
                root = item;
                break;
            }
            pos++;
        }
    }

    // recherche recursive du reste
    while (root && !path.isEmpty()) {
        index = path.at(0).toInt(&conv_ok);
        if (!conv_ok || index < 0) {
            return NULL;
        }
        path.removeFirst();
        root = root->children().at(index);
    }

    return root;
}
