#include <QtTest/QtTest>
#include <QObject>
#include <QBuffer>
#include "protocole.h"
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

 class MyFirstTest: public QObject
 {
     Q_OBJECT
 private slots:
     void test_simple_read() {
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
     
     void test_read_bad_network() {
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
     
     void test_simple_write() {
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
 };

QTEST_MAIN(MyFirstTest)
#include "test.moc"
