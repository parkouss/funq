#include <QtTest/QtTest>
#include <QObject>
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QBuffer>
#include "objectpath.h"
#include "player.h"

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
    void test_objectPath_objectName_noname_with_siblings() {
        QObject parent;
        QObject obj(&parent);
        QObject obj2(&parent);
        
        QCOMPARE(ObjectPath::objectName(&obj), QString("QObject"));
        QCOMPARE(ObjectPath::objectName(&obj2), QString("QObject-1"));
    }
    void test_objectPath_objectName_named_with_siblings() {
        QObject parent;
        QObject obj(&parent);
        QObject obj2(&parent);
        
        obj.setObjectName("NAMEd");
        obj2.setObjectName("NAMEd2");
        
        QCOMPARE(ObjectPath::objectName(&obj), QString("NAMEd"));
        QCOMPARE(ObjectPath::objectName(&obj2), QString("NAMEd2"));
    }
    void test_objectPath_objectName_named_same_with_siblings() {
        QObject parent;
        QObject obj(&parent);
        QObject obj2(&parent);
        
        obj.setObjectName("NAMEd");
        obj2.setObjectName("NAMEd");
        
        QCOMPARE(ObjectPath::objectName(&obj), QString("NAMEd"));
        QCOMPARE(ObjectPath::objectName(&obj2), QString("NAMEd-1"));
    }
    void test_objectPath_objectPath_simple() {
        QObject parent;
        QObject obj(&parent);
        QObject obj2(&parent);
        
        obj.setObjectName("NAMEd");
        obj2.setObjectName("NAMEd");
        
        QCOMPARE(ObjectPath::objectPath(&obj), QString("QObject::NAMEd"));
        QCOMPARE(ObjectPath::objectPath(&obj2), QString("QObject::NAMEd-1"));
    }
    void test_objectPath_objectPath_simple_with_sep() {
        QObject parent;
        QObject obj(&parent);
        QObject obj2(&parent);
        
        obj.setObjectName("::NAMEd");
        obj2.setObjectName("NAMEd");
        
        QCOMPARE(ObjectPath::objectPath(&obj), QString("QObject:::_:NAMEd"));
        QCOMPARE(ObjectPath::objectPath(&obj2), QString("QObject::NAMEd"));
    }
    void test_objectPath_findObject_simple() {
        QMainWindow parent;
        QObject obj(&parent);
        QObject obj2(&parent);
        
        obj2.setObjectName("NAMEd");
        
        QCOMPARE(ObjectPath::findObject("QMainWindow::NAMEd"), &obj2);
    }
    void test_objectPath_findObject_with_sep() {
        QMainWindow parent;
        QObject obj(&parent);
        QObject obj2(&parent);
        
        obj2.setObjectName("::NAMEd");
        
        QCOMPARE(ObjectPath::findObject("QMainWindow:::_:NAMEd"), &obj2);
    }
    void test_objectpath_graphicsItemPos_toplevel() {
        QGraphicsView view;
        QGraphicsScene scene;
        view.setScene(&scene);
        
        QGraphicsRectItem item;
        
        scene.addItem(&item);
        
        QCOMPARE(ObjectPath::graphicsItemPos(&item), 0);
    }
    void test_objectpath_graphicsItemPos_multi_toplevel() {
        QGraphicsView view;
        QGraphicsScene scene;
        view.setScene(&scene);
        
        QGraphicsRectItem item;
        QGraphicsRectItem item2;
        
        scene.addItem(&item);
        scene.addItem(&item2);
        
        item2.setZValue(-1200);
        
        QCOMPARE(ObjectPath::graphicsItemPos(&item), 0);
        QCOMPARE(ObjectPath::graphicsItemPos(&item2), 1);
    }
    void test_objectpath_graphicsItemPos_child() {
        QGraphicsView view;
        QGraphicsScene scene;
        view.setScene(&scene);
        
        QGraphicsRectItem parent;
        QGraphicsRectItem item(&parent);
        
        scene.addItem(&parent);
        
        QCOMPARE(ObjectPath::graphicsItemPos(&item), 0);
    }
    void test_objectpath_graphicsItemPos_children() {
        QGraphicsView view;
        QGraphicsScene scene;
        view.setScene(&scene);
        
        QGraphicsRectItem parent;
        QGraphicsRectItem item(&parent);
        QGraphicsRectItem item2(&parent);
        
        item.setZValue(-1);
        item2.setZValue(0);
        
        scene.addItem(&parent);
        
        QCOMPARE(ObjectPath::graphicsItemPos(&item), 0);
        QCOMPARE(ObjectPath::graphicsItemPos(&item2), 1);
    }
    void test_objectpath_graphicsItemPath_child() {
        QGraphicsView view;
        QGraphicsScene scene;
        view.setScene(&scene);
        
        QGraphicsRectItem parent;
        QGraphicsRectItem item(&parent);
        
        scene.addItem(&parent);
        
        QCOMPARE(ObjectPath::graphicsItemPath(&parent), QString("0"));
        QCOMPARE(ObjectPath::graphicsItemPath(&item), QString("0/0"));
    }
    void test_objectpath_graphicsItemPath_children() {
        QGraphicsView view;
        QGraphicsScene scene;
        view.setScene(&scene);
        
        QGraphicsRectItem parent;
        QGraphicsRectItem item(&parent);
        QGraphicsRectItem item2(&parent);
        
        item.setZValue(-1);
        item2.setZValue(0);
        
        scene.addItem(&parent);
        
        QCOMPARE(ObjectPath::graphicsItemPath(&parent), QString("0"));
        QCOMPARE(ObjectPath::graphicsItemPath(&item), QString("0/0"));
        QCOMPARE(ObjectPath::graphicsItemPath(&item2), QString("0/1"));
    }
    void test_objectpath_graphicsItemFromPath() {
        QGraphicsView view;
        QGraphicsScene scene;
        view.setScene(&scene);
        
        QGraphicsRectItem parent;
        QGraphicsRectItem item(&parent);
        QGraphicsRectItem item2(&parent);
        
        item.setZValue(-1);
        item2.setZValue(0);
        
        scene.addItem(&parent);
        
        QCOMPARE(ObjectPath::graphicsItemFromPath(&view, "0"), &parent);
        QCOMPARE(ObjectPath::graphicsItemFromPath(&view, "0/0"), &item);
        QCOMPARE(ObjectPath::graphicsItemFromPath(&view, "0/1"), &item2);
    }
    /*
     * 
     * TESTS de player.cpp
     * 
     */
     void test_player_widget_by_path() {
         QMainWindow w;
         QObject o(&w);
         
         QBuffer buffer;
         
         Player player(&buffer);
         
         QtJson::JsonObject command;
         command["path"] = "QMainWindow::QObject";
         
         QtJson::JsonObject result = player.widget_by_path(command);
         
         QVERIFY(result["oid"].value<qulonglong>() != 0);
         QCOMPARE(player.registeredObject(result["oid"].value<qulonglong>()), &o);
     }
};

QTEST_MAIN(LibFunqTest)
#include "test.moc"
