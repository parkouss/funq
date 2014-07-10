#ifndef SHORTCUT_RESPONSE_H
#define SHORTCUT_RESPONSE_H

#include "delayedresponse.h"
#include <QKeySequence>
#include <QWidget>

class ShortcutResponse : public DelayedResponse {
    Q_OBJECT
public:
    explicit ShortcutResponse(JsonClient * client, const QtJson::JsonObject & command);

protected:
    virtual void execute(int call);

private slots:
    void on_target_deleted();

private:
    QWidget * m_target;
    QKeySequence m_binding;
};


#endif // SHORTCUT_RESPONSE_H
