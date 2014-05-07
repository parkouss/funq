#include <QtTest/QtTest>
#include <QObject>
#include "objectpath.h"

class LibFunqTest: public QObject
{
    Q_OBJECT
private slots:
    void test_objectPath_objectName_noname() {
        QObject obj;
        
        QString name = ObjectPath::objectName(&obj);
        QCOMPARE(name, QString("QObject"));
    }
    void test_objectPath_objectName_named() {
        QObject obj;
        obj.setObjectName("NAMEd");
        
        QString name = ObjectPath::objectName(&obj);
        QCOMPARE(name, QString("NAMEd"));
    }
};

QTEST_MAIN(LibFunqTest)
#include "test.moc"
