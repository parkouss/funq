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
#include "Event.h"

namespace Hooq
{


Event::Event(Type e_type, TargetType e_targetType) :
    m_type(e_type),
    m_targetType(e_targetType)
{
}

Event::~Event()
{
}

Event::Type Event::type() const
{
    return m_type;
}

QString Event::tag() const
{
    return m_tag;
}

Event* Event::addTag(const QString& tag, Event* event)
{
    event->m_tag = tag;
    return event;
}

Event::TargetType Event::targetType() const {
    return m_targetType;
}


ObjectPath::ObjectPath(const QString& path) :
    m_path(path)
{
}

QString ObjectPath::path() const
{
    return m_path;
}

ObjectPath::~ObjectPath()
{
}

GenericObjectEvent::GenericObjectEvent(Event::Type e_type, TargetType e_targetType, const QString& objectPath) :
    Event(e_type, e_targetType),
    ObjectPath(objectPath)
{
}

GenericObjectEvent::~GenericObjectEvent()
{
}

ModelItemEvent::ModelItemEvent(Type e_type, const QString& objectPath, const QString& item_path, int row, int column) :
    Event(e_type, Event::Model),
    ObjectPath(objectPath),
    m_item_path(item_path),
    m_row(row),
    m_column(column)
{
}

ModelItemEvent::~ModelItemEvent()
{
}

ContextMenuEvent::ContextMenuEvent(const QString& objectPath,
                                   QContextMenuEvent::Reason reason,
                                   const QPoint &pos,
                                   Qt::KeyboardModifiers modifiers):
    Event(Event::ContextMenu, Event::Widget),
    ObjectPath(objectPath),
    m_reason(reason),
    m_pos(pos),
    m_modifiers(modifiers)
{
}

ContextMenuEvent::~ContextMenuEvent()
{
}

QContextMenuEvent::Reason ContextMenuEvent::reason() const {
    return m_reason;
}

const QPoint &ContextMenuEvent::pos() const {
    return m_pos;
}

Qt::KeyboardModifiers ContextMenuEvent::modifiers() const {
    return m_modifiers;
}


KeyEvent::KeyEvent(const QString& objectPath,
                   QEvent::Type type,
                   int key,
                   Qt::KeyboardModifiers modifiers,
                   const QString& text,
                   bool autorep,
                   ushort count):
    Event(Event::KeyEvent, Event::Widget),
    ObjectPath(objectPath),
    m_keyEventType(type),
    m_key(key),
    m_modifiers(modifiers),
    m_text(text),
    m_autorep(autorep),
    m_count(count)
{
}

KeyEvent::~KeyEvent()
{
}

QEvent::Type KeyEvent::type() const {
    return m_keyEventType;
}

int KeyEvent::key() const {
    return m_key;
}

Qt::KeyboardModifiers KeyEvent::modifiers() const {
return m_modifiers;
}

const QString& KeyEvent::text() const {
    return m_text;
}

bool KeyEvent::autorep() const {
    return m_autorep;
}

ushort KeyEvent::count() const {
    return m_count;
}


ShortcutEvent::ShortcutEvent(const QString& objectPath,
                             const QKeySequence &key):
    Event(Event::Shortcut, Event::NoTarget), // Event::NoTarget as the widget path is not required.
    ObjectPath(objectPath),
    m_key(key)
{
}

ShortcutEvent::~ShortcutEvent()
{
}

const QKeySequence &ShortcutEvent::key() const {
    return m_key;
}

MouseEvent::MouseEvent(const QString& objectPath,
                       QEvent::Type mouseEventType,
                       Qt::MouseButton button,
                       Qt::MouseButtons buttons,
                       Qt::KeyboardModifiers modifiers,
                       QPoint posOffset):
    Event(Event::Mouse, Event::Widget),
    ObjectPath(objectPath),
    m_mouseEventType(mouseEventType),
    m_button(button),
    m_buttons(buttons),
    m_modifiers(modifiers),
    m_posOffset(posOffset)
{
}

MouseEvent::~MouseEvent() {
}

QEvent::Type MouseEvent::mouseEventType() const {
    return m_mouseEventType;
}

Qt::MouseButton MouseEvent::button() const {
    return m_button;
}

Qt::MouseButtons MouseEvent::buttons() const {
    return m_buttons;
}

Qt::KeyboardModifiers MouseEvent::modifiers() const {
    return m_modifiers;
}

QPoint MouseEvent::posOffset() const {
    return m_posOffset;
}

MouseWheelEvent::MouseWheelEvent(const QString& objectPath,
                                 int delta,
                                 Qt::MouseButtons buttons,
                                 Qt::KeyboardModifiers modifiers,
                                 Qt::Orientation orient):
    Event(Event::MouseWheel, Event::Widget),
    ObjectPath(objectPath),
    m_delta(delta),
    m_buttons(buttons),
    m_modifiers(modifiers),
    m_orient(orient)
{
}

MouseWheelEvent::~MouseWheelEvent() {
}

int MouseWheelEvent::delta() const {
    return m_delta;
}

Qt::MouseButtons MouseWheelEvent::buttons() const {
    return m_buttons;
}

Qt::KeyboardModifiers MouseWheelEvent::modifiers() const {
    return m_modifiers;
}

Qt::Orientation MouseWheelEvent::orient() const {
    return m_orient;
}

FocusEvent::FocusEvent(const QString& objectPath, Qt::FocusReason reason) :
    Event(Event::Focus, Event::Widget),
    ObjectPath(objectPath),
    m_reason(reason)
{
}

FocusEvent::~FocusEvent()
{
}

Qt::FocusReason FocusEvent::reason() const
{
    return m_reason;
}

DropEvent::DropEvent(const QString& objectPath, const QPoint& pos) :
    Event(Event::Drop, Event::Widget),
    ObjectPath(objectPath),
    m_pos(pos)
{
}

DropEvent::~DropEvent()
{
}

QPoint DropEvent::pos() const
{
    return m_pos;
}

SleepEvent::SleepEvent(int msec) :
    Event(Event::Sleep),
    m_msec(msec)
{
}

SleepEvent::~SleepEvent()
{
}

int SleepEvent::msec() const
{
    return m_msec;
}


}
