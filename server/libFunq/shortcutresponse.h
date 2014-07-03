#ifndef SHORTCUT_RESPONSE_H
#define SHORTCUT_RESPONSE_H

#include "delayedresponse.h"
#include <QKeySequence>
#include <QWidget>

class ShortcutResponse : public DelayedResponse {
public:
    explicit ShortcutResponse(JsonClient * client, const QtJson::JsonObject & command);

protected:
    virtual void execute(int call);

private:
    QWidget * m_target;
    QKeySequence m_binding;
};


#endif // SHORTCUT_RESPONSE_H
