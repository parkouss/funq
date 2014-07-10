#include "shortcutresponse.h"
#include "player.h"
#include <QApplication>
#include <QKeyEvent>

ShortcutResponse::ShortcutResponse(JsonClient * client, const QtJson::JsonObject & command)
        : DelayedResponse(client, command), m_target(NULL) {
    
    if (command.contains("oid")) {
        WidgetLocatorContext<QWidget> ctx(static_cast<Player *>(jsonClient()), command, "oid");
        if (ctx.hasError()) { writeResponse(ctx.lastError); return; }
        m_target = ctx.widget;
    } else {
        m_target = qApp->activeWindow();
    }
    if (m_target) {
        connect(m_target, SIGNAL(destroyed()), this, SLOT(on_target_deleted()));
    }
    m_binding = QKeySequence::fromString(command["keysequence"].toString());
}

void ShortcutResponse::on_target_deleted() {
    m_target = NULL;
}

void ShortcutResponse::execute(int call) {
    if (!m_target) {
        // this can happen when target is deleted for example in
        // step 2, after a press event has been sent. We do not
        // want it to be an error.
        writeResponse(QtJson::JsonObject());
        return;
    }
    if (call == 0) {
        m_target->repaint();
        setInterval(100);
    } else if (call == 1) {
        m_target->grabKeyboard();
    } else if (call == 2) {
        // taken from
        // http://stackoverflow.com/questions/14283764/how-can-i-simulate-emission-of-a-standard-key-sequence
        for (uint i = 0; i < m_binding.count(); ++i) {
            uint key = m_binding[i];
            Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(key & Qt::KeyboardModifierMask);
            key = key & ~Qt::KeyboardModifierMask;

            qApp->postEvent(m_target, new QKeyEvent(QKeyEvent::KeyPress, key, modifiers));
        }
    } else if (call == 3) {
        for (uint i = 0; i < m_binding.count(); ++i) {
            uint key = m_binding[i];
            Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(key & Qt::KeyboardModifierMask);
            key = key & ~Qt::KeyboardModifierMask;

            qApp->postEvent(m_target, new QKeyEvent(QKeyEvent::KeyRelease, key, modifiers));
        }
    } else if (call == 4) {
        m_target->releaseKeyboard();
        writeResponse(QtJson::JsonObject());
    }
}
