#ifndef FILEDATABASE_H
#define FILEDATABASE_H
#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "tool.h"
class FileDatabase
{
private:
    void save(QByteArray data)
    {
        QFile *f=new QFile(name);
        bool ret = f->open(QIODevice::ReadWrite|QIODevice::Truncate);
        if(!ret){
            prt(info,"fail to open %s",name.toStdString().data());
            delete f;
        }
        f->write(data);
        f->close();
    }
    bool load(QByteArray &data)
    {

        QFile *f=new QFile(name);
        bool ret = f->open(QIODevice::ReadOnly);
        if(!ret){
            delete f;
            return ret;
        }
        data=f->readAll();
        f->close();
        return ret;
    }
public:
    FileDatabase(QString file_name);
    void save(QJsonObject node)
    {
        QJsonDocument json_doc_new(node);
        save(json_doc_new.toJson());
    }
    void load(QJsonObject &node)
    {
        QByteArray ba;
        QJsonDocument json_doc_new;
        if(load(ba)){
            json_doc_new=QJsonDocument::fromJson(ba);
            node=json_doc_new.object();
        }else{
        }
    }


    void save(string json_data)
    {
        QByteArray ba(json_data.data(),ba.length());
        save(ba);
    }
    void load(string &json_data)
    {
        QByteArray ba;
        load(ba);
        json_data=ba.data();
    }
private:
    QString name;
    QByteArray config;
};

#endif // FILEDATABASE_H
