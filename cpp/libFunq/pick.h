#ifndef PICK_H
#define PICK_H

#include <QObject>
#include <QEvent>
#include <QObject>
#include <QTextStream>

class PickHandler {
public:
    virtual ~PickHandler() {}
    virtual void handle(QObject * object, const QPoint & pos) = 0;
};

class PickFormatter : public PickHandler {
public:
    PickFormatter();
    virtual void handle(QObject * object, const QPoint & pos);
    
    inline bool showProperties() { return m_showProperties; }
    inline void setShowProperties(bool show) { m_showProperties = show; }
private:
    QTextStream m_stream;
    bool m_showProperties;
};

class Pick : public QObject {
public:
    explicit Pick(PickHandler * handler = 0, QObject * parent = 0);
    virtual ~Pick();
    
    virtual bool handleEvent(QObject * receiver, QEvent * event);
private:
    PickHandler * m_handler;
};

#endif // PICK_H
