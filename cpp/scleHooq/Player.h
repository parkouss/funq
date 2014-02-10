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
#pragma once

#include <QObject>
#include <QPointer>
#include <QQueue>
#include <QString>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QEvent>

class QWidget;

namespace Hooq
{
class Event;
class ObjectPath;

class Player: public QObject, private QXmlStreamReader
{
    Q_OBJECT

	public:
        Player(QIODevice* device);
		~Player();
	private slots:
		void readNext();
	private:

        void dumpWidget(QXmlStreamWriter & streamWriter, QWidget* widget);
        void dumpWidgetsTree(QXmlStreamWriter & streamWriter, QWidget* root = 0);
        void dumpProperties(QWidget *widget);

        bool handleElement();
        void processEvents();

		QObject* findObject(const QString& path);

		void postDragAndDrop();
        void postClick();

		void postKeyEvent(int type);
        void postMouseEvent(QEvent::Type type);

        void ack();

        bool m_processingEvent;

        QQueue<Event*> m_eventQueue;

        bool m_error;
        QString m_return;
};

} // namespace
