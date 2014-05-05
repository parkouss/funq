#include <QtTest/QtTest>
#include <QObject>
#include <QBuffer>
#include "protocole.h"
#include "jsonclient.h"
#include <QSignalSpy>

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

 class MyFirstTest: public QObject
 {
     Q_OBJECT
 private slots:
     /* tests de protocole */
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
         
         QVERIFY(protocole.hasMessageToSend());
         
         buffer.emitBytesWritten(24 + 3);
         
         QVERIFY(! protocole.hasMessageToSend());
         
         buffer.seek(0);
         QCOMPARE(QString(buffer.readAll()), QString("24\n{\"1\": 1, \"2\": 2, \"3\": 3}"));
     }
     
     /* tests de jsonclient */
     void test_jsonclient_response() {
         EmittingBuffer buffer;
         QVERIFY(buffer.open(QIODevice::ReadWrite));
         
         TestJsonClient client(&buffer);
         
         buffer.write("35\n{\"action\": \"test_echo\", \"value\": 1}");
         buffer.seek(0);
         buffer.emitReadyRead();
         
         // l'encodage en json de QVariantMap place 4 espaces de plus.
         
         buffer.emitBytesWritten(39 + 3);
         
         buffer.seek(35+3);
         QCOMPARE(QString(buffer.readAll()), QString(
            "39\n{ \"action\" : \"test_echo\", \"value\" : 2 }"
         ));
     }
 };

QTEST_MAIN(MyFirstTest)
#include "test.moc"
