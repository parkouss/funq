/*
Copyright: SCLE SFE
Contributor: Julien Pagès <j.parkouss@gmail.com>

This software is a computer program whose purpose is to test graphical
applications written with the QT framework (http://qt.digia.com/).

This software is governed by the CeCILL v2.1 license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL v2.1 license and that you accept its terms.
*/

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
    virtual void execute(int call) = 0;

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
    QString m_action;
    bool m_hasResponded;
    int m_nbCall;
};

#endif // DELAYEDRESPONSE_H
