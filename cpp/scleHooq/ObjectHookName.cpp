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

namespace Hooq
{

QString ObjectHookName::objectName(QObject* object)
{
	QString name = rawObjectName(object);
	name.replace("::", ":_:"); // we use :: as path separators
	return name;
}

QString ObjectHookName::rawObjectName(QObject* object)
{
	// Grab the object name
	if(!object->objectName().isEmpty())
	{
        return object->objectName()/* + "(" + object->metaObject()->className() + ")"*/;
	}

	// If it's got no parent, classname:0
	if(!object->parent())
	{
		return QString("%1-0").arg(object->metaObject()->className());
	}

	// It does - classname:Index
	const QList<QObject*> siblings = object->parent()->children();
	Q_ASSERT(siblings.contains(object));

	int index = 1;
	Q_FOREACH(QObject* sibling, siblings)
	{
		if(sibling == object)
		{
			break;
		}
		if(sibling->metaObject() == object->metaObject())
		{
			++index;
		}
	}

	return QString(
		"%1-%2"
	).arg(
		object->metaObject()->className()
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

} // namespace
