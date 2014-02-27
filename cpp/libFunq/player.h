#ifndef PLAYER_H
#define PLAYER_H

#include "jsonclient.h"

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
    QtJson::JsonObject model_items(const QtJson::JsonObject & command);
    QtJson::JsonObject model_item_action(const QtJson::JsonObject & command);
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

#endif // PLAYER_H
