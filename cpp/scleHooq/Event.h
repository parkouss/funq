/*
    Hooq: Qt4 UI recording, playback, and testing toolkit.
    Copyright (C) 2010  Mendeley Limited <copyright@mendeley.com>
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

#include <QPoint>
#include <QString>
#include <QEvent>
#include <QKeySequence>
#include <QContextMenuEvent>
#include <QPoint>

namespace Hooq
{

class Event
{
public:
    enum Type
    {
        NoOp,    ///< No-operation event, just send the frontend an ack
        DumpProperties,   ///< Request for all the Q_PROPERTYs of a given QObject
        SetProperty,
        DumpWidgetsTree,    ///< Dump widgets tree
        DumpModel,
        GetWidget,
        Drop,   ///< Finish a drag-and-drop event
        Focus,  ///< A focus request for a given QObject
        Mouse, ///< A mouse event sent to a QObject
        MouseWheel, ///< A mouse wheel event sent to a QObject
        Pick,   ///< Request for the user to click on a QObject, then treat as the target of a Dump event
        Sleep,  ///< Wait a certain amount of time
        Flush,  ///< Call qApp->processEvents - be careful with this, may block Hooq
        Close,  ///< Close a widget
        Shortcut,
        KeyEvent,
        ContextMenu,
        ModelRowCount,
        ModelColumnCount,
        ItemGet,
        ItemProperty,
        ItemSelect,
        ItemClick,
        ItemDClick,
        ItemEdit,
        ScreenShot,
        KeyClick,
        DumpGraphicsItems,
        TabBarListTabs
    };

    enum TargetType {
        NoTarget,
        Widget,
        Model,
        Item
    };

    Event(Type e_type, TargetType e_targetType = NoTarget);
    virtual ~Event();

    Type type() const;
    QString tag() const;
    TargetType targetType() const;

    static Event* addTag(const QString& tag, Event* event);

private:
    Type m_type;
    QString m_tag;
    TargetType m_targetType;
};



/// Abstract base class for events that reference an object.
class ObjectPath
{
public:
    QString path() const;
    virtual ~ObjectPath();
protected:
    ObjectPath(const QString& path);
private:
    QString m_path;
};

class GenericObjectEvent: public Event, public ObjectPath
{
public:
    GenericObjectEvent(Type e_type, TargetType e_targetType, const QString& objectPath);
    virtual ~GenericObjectEvent();
};

class SetPropertyEvent: public Event, public ObjectPath {
    public:
    SetPropertyEvent(const QString & objectPath,
                       const QString & propName,
                       const QVariant & value) : Event(Event::SetProperty, Event::Widget),
                                                  ObjectPath(objectPath),
                                                  m_propName(propName),
                                                  m_propValue(value) {}
    inline QString propName() { return m_propName; }
    inline QVariant propValue() { return m_propValue; }
    
    private:
    QString m_propName;
    QVariant m_propValue;
};

class ModelItemEvent: public Event, public ObjectPath
{
public:
    ModelItemEvent(Type e_type, const QString& objectPath, const QString& item_path, int row, int column);
    virtual ~ModelItemEvent();
    inline QString itemPath() const { return m_item_path; }
    inline int row() const { return m_row; }
    inline int column() const { return m_column; }
private:
    QString m_item_path;
    int m_row;
    int m_column;
};


class ContextMenuEvent: public Event, public ObjectPath
{
public:
    ContextMenuEvent(const QString& objectPath,
                     QContextMenuEvent::Reason reason,
                     const QPoint &pos,
                     Qt::KeyboardModifiers modifiers);
    virtual ~ContextMenuEvent();
    QContextMenuEvent::Reason reason() const;
    const QPoint &pos() const;
    Qt::KeyboardModifiers modifiers() const;
private:
    QContextMenuEvent::Reason m_reason;
    const QPoint m_pos;
    Qt::KeyboardModifiers m_modifiers;
};


class KeyEvent: public Event, public ObjectPath
{
public:
    KeyEvent(const QString& objectPath,
             QEvent::Type type,
             int key,
             Qt::KeyboardModifiers modifiers,
             const QString& text,
             bool autorep,
             ushort count);
    virtual ~KeyEvent();
    QEvent::Type type() const;
    int key() const;
    Qt::KeyboardModifiers modifiers() const;
    const QString& text() const;
    bool autorep() const;
    ushort count() const;
private:
    QEvent::Type m_keyEventType;
    int m_key;
    Qt::KeyboardModifiers m_modifiers;
    const QString& m_text;
    bool m_autorep;
    ushort m_count;
};

class ShortcutEvent: public Event, public ObjectPath
{
public:
    ShortcutEvent(const QString& objectPath,
                  const QKeySequence &key);
    virtual ~ShortcutEvent();
    const QKeySequence &key() const;
private:
    const QKeySequence m_key;
};

class KeyClickEvent: public Event, public ObjectPath
{
public:
    KeyClickEvent(const QString& objectPath,
                  const QString &text);
    virtual ~KeyClickEvent();
    const QString &text() const { return m_text; }
private:
    const QString m_text;
};

class MouseEvent: public Event, public ObjectPath
{
public:
    MouseEvent(const QString& objectPath,
               QEvent::Type mouseEventType,
               Qt::MouseButton button,
               Qt::MouseButtons buttons,
               Qt::KeyboardModifiers modifiers,
               QPoint posOffset = QPoint());
    virtual ~MouseEvent();
    QEvent::Type mouseEventType() const;
    Qt::MouseButton button() const;
    Qt::MouseButtons buttons() const;
    Qt::KeyboardModifiers modifiers() const;
    QPoint posOffset() const;
private:
    QEvent::Type m_mouseEventType;
    Qt::MouseButton m_button;
    Qt::MouseButtons m_buttons;
    Qt::KeyboardModifiers m_modifiers;
    QPoint m_posOffset;
};

class MouseWheelEvent: public Event, public ObjectPath
{
public:
    MouseWheelEvent(const QString& objectPath,
                    int delta,
                    Qt::MouseButtons buttons,
                    Qt::KeyboardModifiers modifiers,
                    Qt::Orientation orient = Qt::Vertical);
    virtual ~MouseWheelEvent();
    int delta() const;
    Qt::MouseButtons buttons() const;
    Qt::KeyboardModifiers modifiers() const;
    Qt::Orientation orient() const;
private:
    int m_delta;
    Qt::MouseButtons m_buttons;
    Qt::KeyboardModifiers m_modifiers;
    Qt::Orientation m_orient;
};

class FocusEvent: public Event, public ObjectPath
{
public:
    FocusEvent(const QString& objectPath, Qt::FocusReason reason);
    virtual ~FocusEvent();
    Qt::FocusReason reason() const;
private:
    Qt::FocusReason m_reason;
};

class DropEvent : public Event, public ObjectPath
{
public:
    DropEvent(const QString& objectPath, const QPoint& pos);
    virtual ~DropEvent();

    QPoint pos() const;
private:
    QPoint m_pos;
};

class SleepEvent : public Event
{
public:
    SleepEvent(int msec);
    virtual ~SleepEvent();

    int msec() const;
private:
    int m_msec;
};

}
