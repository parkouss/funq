#ifndef PLAYER_H
#define PLAYER_H

#include "jsonclient.h"
#include <QWidget>
class DelayedResponse;

/**
  * @brief Player est une spécialisation de JsonClient pour la gestion des objets manipuler à distance.
  *
  * Gère le référencement des QWidget que l'on veut manipulable, et définit les actions
  * possible par un client de test.
  */
class Player : public JsonClient
{
    Q_OBJECT
public:
    explicit Player(QIODevice * device, QObject *parent = 0);
    
    qulonglong registerObject(QObject * object);
    QObject * registeredObject(const qulonglong & id);

signals:
    void emit_object_set_properties(QObject * object, const QVariantMap & props);
    
public slots:
    /*
      Ces slots sont automatiquement transformés en commande disponibles
      pour les clients.

      Ils peuvent être de deux formes différentes :

      - QtJson::JsonObject commande(const QtJson::JsonObject & command);
      - DelayedResponse * commande(const QtJson::JsonObject & command);

      La première forme est une réponse directe, synchrone à une demande
      (pas de possibilité d'attente sans bloquage de l'application). La
      deuxième forme permet de passer outre cette limitation.

      */
    QtJson::JsonObject list_actions(const QtJson::JsonObject & command);

    QtJson::JsonObject widget_by_path(const QtJson::JsonObject & command);
    QtJson::JsonObject active_window(const QtJson::JsonObject & command);
    QtJson::JsonObject object_properties(const QtJson::JsonObject & command);
    QtJson::JsonObject object_set_properties(const QtJson::JsonObject & command);
    QtJson::JsonObject widgets_list(const QtJson::JsonObject & command);
    QtJson::JsonObject widget_click(const QtJson::JsonObject & command);
    QtJson::JsonObject widget_close(const QtJson::JsonObject & command);
    DelayedResponse * drag_n_drop(const QtJson::JsonObject & command);
    QtJson::JsonObject model_items(const QtJson::JsonObject & command);
    QtJson::JsonObject model_item_action(const QtJson::JsonObject & command);
    QtJson::JsonObject model_gitem_action(const QtJson::JsonObject & command);
    QtJson::JsonObject desktop_screenshot(const QtJson::JsonObject & command);
    QtJson::JsonObject widget_keyclick(const QtJson::JsonObject & command);
    DelayedResponse * shortcut(const QtJson::JsonObject & command);
    QtJson::JsonObject tabbar_list(const QtJson::JsonObject & command);
    QtJson::JsonObject graphicsitems(const QtJson::JsonObject & command);
    QtJson::JsonObject gitem_properties(const QtJson::JsonObject & command);
    QtJson::JsonObject call_slot(const QtJson::JsonObject & command);

    QtJson::JsonObject quit(const QtJson::JsonObject & command);

private slots:
    void objectDeleted(QObject * object);
    void _object_set_properties(QObject * object, const QVariantMap & props);

private:
    QHash<qulonglong, QObject*> m_registeredObjects;
};

/**
  * @brief Permet de retrouver un objet précédemment référencé par le Player.
  */
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

/**
  * @brief Permet de retrouver un widget (de type T) précédemment référencé par le Player.
  */
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
