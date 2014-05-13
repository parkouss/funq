#ifndef DELAYEDRESPONSE_H
#define DELAYEDRESPONSE_H

#include <QObject>
#include "jsonclient.h"
#include <QTimer>

/**
  * @brief Objet permettant de fournir une réponse asynchrone à une requète json
  *
  * Un pointeur de DelayedResponse peut être renvoyée par un slot de Player
  * pour fournir une réponse asynchrone.
  *
  * Un QTimer est utilisé pour appeller de manière cyclique la méthode
  * execute(), jusquèà ce que writeResponse() soit appelé. Dès le premier
  * appel de writeResponse(), execute() ne sera plus appelé et l'objet sera
  * détruit automatiquement.
  *
  * Si la méthode writeResponse() n'est pas appellée dans le temps imparti
  * par timerOut lors de la construction de l'objet, une réponse sera automatiquement
  * envoyée pour indiquer une erreur de timeout. par défaut, ce timeout vaut
  * 20000 ms, soit 20s.
  */
class DelayedResponse : public QObject {
    Q_OBJECT
public:
    explicit DelayedResponse(JsonClient * client, const QtJson::JsonObject & command, int interval=0, int timerOut=20000);
    
    /**
      * @ brief Définit l'intervalle en ms entre les appels de execute().
      *
      * Par défaut, l'intervalle vaut 0, ce qui indique que l'appel sera
      * exécuté au prochain tour de la boucle d'évènements QT.
      */
    void setInterval(int interval) { m_timer.setInterval(interval); }

    /**
      * @brief démarre les appels récurrents de execute().
      */
    void start();

protected:
    /**
      * @brief à implémenter pour renvoyer la réponse.
      *
      * Cette méthode doit appeller writeResponse() à un moment donné pour envoyer
      * la réponse et terminer la vie de l'objet.
      */
    virtual void execute(const QtJson::JsonObject & command) = 0;

    /**
      * @brief renvoie une réponse au client json.
      *
      * Cet appel amorcera la destruction automatique de l'objet.
      */
    void writeResponse(const QtJson::JsonObject & result);
    JsonClient * jsonClient() { return m_client; }

private slots:
    void timerCall();
    void onTimerOut();

signals:
    void aboutToWriteResponse(const QtJson::JsonObject &);

private:

    JsonClient * m_client;
    QTimer m_timer;
    QtJson::JsonObject m_command;
    bool m_hasResponded;
};

#endif // DELAYEDRESPONSE_H
