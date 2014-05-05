#ifndef PLAYER_H
#define PLAYER_H

#include "jsonclient.h"
#include <QWidget>
class DelayedResponse;

class Player : public JsonClient
{
    Q_OBJECT
public:
    explicit Player(QIODevice * device, QObject *parent = 0);
    
    qulonglong registerObject(QObject * object);
    QObject * registeredObject(const qulonglong & id);

signals:
    
public slots:
    QtJson::JsonObject list_actions(const QtJson::JsonObject & command);

    QtJson::JsonObject widget_by_path(const QtJson::JsonObject & command);
    QtJson::JsonObject object_properties(const QtJson::JsonObject & command);
    QtJson::JsonObject object_set_properties(const QtJson::JsonObject & command);
    QtJson::JsonObject widgets_list(const QtJson::JsonObject & command);
    QtJson::JsonObject widget_click(const QtJson::JsonObject & command);
    DelayedResponse * drag_n_drop(const QtJson::JsonObject & command);
    QtJson::JsonObject model_items(const QtJson::JsonObject & command);
    QtJson::JsonObject model_item_action(const QtJson::JsonObject & command);
    QtJson::JsonObject model_gitem_action(const QtJson::JsonObject & command);
    QtJson::JsonObject desktop_screenshot(const QtJson::JsonObject & command);
    QtJson::JsonObject widget_keyclick(const QtJson::JsonObject & command);
    QtJson::JsonObject shortcut(const QtJson::JsonObject & command);
    QtJson::JsonObject tabbar_list(const QtJson::JsonObject & command);
    QtJson::JsonObject graphicsitems(const QtJson::JsonObject & command);
    QtJson::JsonObject gitem_properties(const QtJson::JsonObject & command);

    QtJson::JsonObject quit(const QtJson::JsonObject & command);

private slots:
    void objectDeleted(QObject * object);

private:
    QHash<qulonglong, QObject*> m_registeredObjects;
};

class ObjectLocatorContext {
public:
    ObjectLocatorContext(Player * player,
                         const QtJson::JsonObject & command,
                         const QString & objKey);
    virtual ~ObjectLocatorContext() {}

    qulonglong id;
    QObject * obj;
    QtJson::JsonObject lastError;
    inline bool hasError() { return ! lastError.isEmpty(); }
};

template <class T = QWidget>
class WidgetLocatorContext : public ObjectLocatorContext {
public:
    WidgetLocatorContext(Player * player,
                         const QtJson::JsonObject & command,
                         const QString & objKey) : ObjectLocatorContext(player, command, objKey) {

        if (! hasError()) {
            widget = qobject_cast<T *>(obj);
            if (!widget) {
                lastError = player->createError("NotAWidget",
                                                QString::fromUtf8("L'objet (id:%1) n'est pas un %2").arg(id).arg(T::staticMetaObject.className()));
            }
        }
    }
    T * widget;
};

#endif // PLAYER_H
