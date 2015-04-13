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
  * @brief Objet that represent a non blocking answer.
  *
  * A pointer of DelayedResponse may be returned by a Player's slot to return
  * an asynchron answer (that is, non blocking the main Qt loop).
  *
  * A QTimer is used to call multiple times the execute() method, until
  * writeResponse() is called. After the writeResponse() call, execute()
  * won't be called again and the object will be deleted automatically.
  *
  * If the writeResponse() method is not called in the given time (timerOut, given
  * in the constructor), an automatic error response will be sent. Default timeout
  * is 20 seconds.
  */
class DelayedResponse : public QObject {
    Q_OBJECT
public:
    explicit DelayedResponse(JsonClient * client, const QtJson::JsonObject & command, int interval=0, int timerOut=20000);
    
    /**
      * @brief Define the intervall between each execute() call.
      *
      * By default, interval is 0, meaning that it will be called as soon
      * as possible by Qt.
      */
    void setInterval(int interval) { m_timer.setInterval(interval); }

    /**
      * @brief start the execute() calls.
      */
    void start();

protected:
    /**
      * @brief This needs to be implemented, this is the entry point for answering.
      *
      * This method MUST call writeResponse() at a given time to answer and
      * ends the life cycle of this object.
      */
    virtual void execute(int call) = 0;

    /**
      * @brief Returns the response.
      *
      * This call will automatically ask for object deletion.
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
