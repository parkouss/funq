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

#include <QtTest/QtTest>
#include <QObject>
#include <QBuffer>
#include "protocole.h"
#include "jsonclient.h"
#include "delayedresponse.h"
#include <QSignalSpy>
/*
 * QBuffer by default does not emit readyRead and bytesWritten. But we need it for testing :)
 */
class EmittingBuffer : public QBuffer {
    Q_OBJECT
public:
    void emitReadyRead() {
        emit readyRead();
    }
    void emitBytesWritten ( qint64 bytes ) {
        emit bytesWritten ( bytes );
    }
};

class TestJsonClient : public JsonClient {
    Q_OBJECT
    
public:
    TestJsonClient(QIODevice * device, QObject *parent = 0) : JsonClient(device, parent) {}
    
public slots:
    QtJson::JsonObject test_echo(const QtJson::JsonObject & command) {
        QtJson::JsonObject result(command);
        result["value"] = 2;
        return result;
    }
};

class TestDelayedResponse : public DelayedResponse {
public:
    TestDelayedResponse(JsonClient * client, const QtJson::JsonObject & command, int nb_exec_before_response=0, int interval=0, int timerOut=20000) :
                    DelayedResponse(client, command, interval, timerOut),
                    nb_executed(0), 
                    _nb_exec_before_response(nb_exec_before_response) {}
    
    void execute(int) {
        nb_executed += 1;
        if (_nb_exec_before_response > 0) {
            _nb_exec_before_response -= 1;
            return;
        }
        QtJson::JsonObject result;
        result["result"] = 1;
        writeResponse(result);
    }
    
    int nb_executed;
private:
    int _nb_exec_before_response;
};

 class MyFirstTest: public QObject
 {
     Q_OBJECT
 private slots:
     /* protocole tests */
     void test_protocole_simple_read() {
         EmittingBuffer buffer;
         Protocole protocole;
         QSignalSpy spy(&protocole, SIGNAL(messageReceived()));
         
         QVERIFY(buffer.open(QIODevice::ReadWrite));
         buffer.write("8\n{\"1\": 2}");
         buffer.seek(0);
         
         protocole.setDevice(&buffer);
         buffer.emitReadyRead();
         
         QCOMPARE(spy.count(), 1);
         QCOMPARE(QString(protocole.nextAvailableMessage()), QString("{\"1\": 2}"));
     }
     
     void test_protocole_read_bad_network() {
         EmittingBuffer buffer;
         Protocole protocole;
         QSignalSpy spy(&protocole, SIGNAL(messageReceived()));
         
         QVERIFY(buffer.open(QIODevice::ReadWrite));
         buffer.write("2");
         buffer.seek(0);
         
         protocole.setDevice(&buffer);
         buffer.emitReadyRead();
         
         QCOMPARE(spy.count(), 0);
         QCOMPARE(protocole.nextAvailableMessage(), QByteArray());
         
         buffer.write("24\n{\"1\": 1, \"2\": 2, \"3\": 3}");
         buffer.seek(0);
         buffer.emitReadyRead();
         
         QCOMPARE(spy.count(), 1);
         QCOMPARE(QString(protocole.nextAvailableMessage()), QString("{\"1\": 1, \"2\": 2, \"3\": 3}"));
     }
     
     void test_protocole_simple_write() {
         EmittingBuffer buffer;
         Protocole protocole;
         
         QVERIFY(buffer.open(QIODevice::ReadWrite));
         protocole.setDevice(&buffer);
         
         protocole.sendMessage("{\"1\": 1, \"2\": 2, \"3\": 3}");
         
         buffer.emitBytesWritten(24 + 3);
         
         buffer.seek(0);
         QCOMPARE(QString(buffer.readAll()), QString("24\n{\"1\": 1, \"2\": 2, \"3\": 3}"));
     }
     
     /* jsonclient tests */
     void test_jsonclient_response() {
         EmittingBuffer buffer;
         QVERIFY(buffer.open(QIODevice::ReadWrite));
         
         TestJsonClient client(&buffer);
         
         buffer.write("35\n{\"action\": \"test_echo\", \"value\": 1}");
         buffer.seek(0);
         buffer.emitReadyRead();
         
         // encoding QVariantMap in json put 4 more spaces.
         
         buffer.emitBytesWritten(35 + 4 + 3);
         
         buffer.seek(35+3);
         QCOMPARE(QString(buffer.readAll()), QString(
            "39\n{ \"action\" : \"test_echo\", \"value\" : 2 }"
         ));
     }
     
     /* delayedresponse tests */
     void test_delayedresponse_simple() {
         EmittingBuffer buffer;
         QVERIFY(buffer.open(QIODevice::ReadWrite));
         
         TestJsonClient client(&buffer);
         
         TestDelayedResponse response(&client, QtJson::JsonObject());
         
         response.start();
         qApp->processEvents();
         
         QCOMPARE(response.nb_executed, 1);
         buffer.seek(0);
         
         QByteArray data = buffer.readAll();
         
         QString resultStr = data.mid(data.indexOf('\n')+1); // remove the message header
         QtJson::JsonObject result = QtJson::parse(resultStr).toMap();
         
         QCOMPARE(result["result"].toInt(), 1);
     }
     
     void test_delayedresponse_timeout() {
         EmittingBuffer buffer;
         QVERIFY(buffer.open(QIODevice::ReadWrite));
         
         TestJsonClient client(&buffer);
         
         TestDelayedResponse response(&client, QtJson::JsonObject(), 0, 1000, 0);
         
         response.start();
         qApp->processEvents();
         
         QCOMPARE(response.nb_executed, 0);
         buffer.seek(0);
         
         QByteArray data = buffer.readAll();
         
         QString resultStr = data.mid(data.indexOf('\n')+1); // remove the message header
         QtJson::JsonObject result = QtJson::parse(resultStr).toMap();
         
         QVERIFY(result.contains("errName"));
         QVERIFY(result.contains("errDesc"));
         QVERIFY(result.contains("success"));
         QCOMPARE(result["success"].toBool(), false);
     }
     
     void test_multi_pass() {
         EmittingBuffer buffer;
         QVERIFY(buffer.open(QIODevice::ReadWrite));
         
         TestJsonClient client(&buffer);
         
         TestDelayedResponse response(&client, QtJson::JsonObject(), 1);
         
         response.start();
         qApp->processEvents();
         qApp->processEvents();
         
         QCOMPARE(response.nb_executed, 2);
         buffer.seek(0);
         
         QByteArray data = buffer.readAll();
         
         QString resultStr = data.mid(data.indexOf('\n')+1); // remove the message header
         QtJson::JsonObject result = QtJson::parse(resultStr).toMap();
         
         QCOMPARE(result["result"].toInt(), 1);
     }
     
     void test_multi_pass_one_response() {
         EmittingBuffer buffer;
         QVERIFY(buffer.open(QIODevice::ReadWrite));
         
         TestJsonClient client(&buffer);
         
         TestDelayedResponse response(&client, QtJson::JsonObject());
         
         response.start();
         qApp->processEvents();
         qApp->processEvents();
         
         QCOMPARE(response.nb_executed, 1);
         buffer.seek(0);
         
         QByteArray data = buffer.readAll();
         
         QString resultStr = data.mid(data.indexOf('\n')+1); // remove the message header
         QtJson::JsonObject result = QtJson::parse(resultStr).toMap();
         
         QCOMPARE(result["result"].toInt(), 1);
     }
 };

QTEST_MAIN(MyFirstTest)
#include "test.moc"
