#ifndef GENERICTRIGGER_H
#define GENERICTRIGGER_H

#include <QWidget>
#include <QDebug>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QTimer>

#include <sstream>
//#include <iostream>
#include <iomanip>

//#include <ports/trigger/widgetrigger.h>

class GenericTrigger : public QWidget
{
    Q_OBJECT
public:
    explicit GenericTrigger(QWidget *parent = nullptr);

    virtual void populateConfiguration(QXmlStreamWriter &writer);;
    virtual void loadConfiguration(QXmlStreamReader &reader);;

    static std::map<std::string, std::function<GenericTrigger*(void)>> getAvailableTriggers(){return constructors_;}
    static std::map<std::string, std::function<GenericTrigger*(void)>> getQualifiedConstructors(){return q_constructors_;}

    static void register_qualified_constructor(const std::string &type, std::function<GenericTrigger*(void)> construtor){
       if(q_constructors_.find(type) == q_constructors_.end()){q_constructors_.insert({type, construtor});}
    }

    static void register_constructor(const std::string &type, std::function<GenericTrigger*(void)> construtor){
        if(constructors_.find(type) == constructors_.end()){constructors_.insert({type, construtor});}
    }



signals:
    void goNext();
    void goTo(GenericTrigger *trigger);
    void say(QByteArray msg);
    void tagged(QString name);


public slots:
    virtual void set() = 0;
    virtual void unset(){};
    virtual void heard(QByteArray data){};
    virtual void availableTags(std::map<QString, GenericTrigger*> tags){};

private:
    inline static std::map<std::string, std::function<GenericTrigger*(void)>> constructors_;
    inline static std::map<std::string, std::function<GenericTrigger*(void)>> q_constructors_;

public:
    virtual void nameAlreadyInUse(){}

protected:
    std::string hexDecode(const std::string &data);
    char hexToChar(char val);
    std::string hexEncode(const char *data, size_t num_bytes, std::string leading);
};

#endif // GENERICTRIGGER_H
