/*
	Hooq: Qt4 UI recording, playback, and testing toolkit.
	Copyright (C) 2009  Mendeley Limited <copyright@mendeley.com>
	Copyright (C) 2009  Frederick Emmott <mail@fredemmott.co.uk>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "ObjectHookName.h"

#include <QObject>
#include <QStringList>
#include <QGraphicsItem>
#include <QGraphicsView>

namespace Hooq
{

QString ObjectHookName::objectName(QObject* object)
{
	QString name = rawObjectName(object);
	name.replace("::", ":_:"); // we use :: as path separators
	return name;
}

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
QString ObjectHookName::rawObjectName(QObject* object)
{
    QString rawName = _rawObjectName(object);

    if(!object->parent())
    {
        return rawName;
    }

	// It does - classname:Index
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

QString ObjectHookName::objectPath(QObject* object)
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

int ObjectHookName::graphicsItemPos(QGraphicsItem *item) {
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

QString ObjectHookName::graphicsItemPath(QGraphicsItem *item) {
    QStringList path;
    while (item) {
        path.prepend(QString::number(graphicsItemPos(item)));
        item = item->parentItem();
    }
    return path.join("/");
}

} // namespace
