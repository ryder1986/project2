#ifndef PVD_H
#define PVD_H
#include <cstring>
#include <QtCore>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <json/json.h>
#include <json/value.h>
using namespace std;
typedef  QJsonValue JsonValue;
class Pvd{
public:
    enum LENGTH_FIXED_VALUE{
        STR_LENGTH=100,
        PATH_LENGTH=1000,
        BUFFER_LENGTH=1000,
        BUFFER_MAX_LENGTH=3000
    };
    enum CMD{
        GET_CONFIG=1,
        SET_CONFIG,
        INSERT_CAMERA,
        DELETE_CAMERA,
        MOD_CAMERA_ATTR,
        MOD_CAMERA_ALG,
        CAM_OUTPUT_OPEN,
        CAM_OUTPUT_CLOSE,
        HEART,
        REBOOT,
        MOD_DEVICE_ATTR
    };
    enum RETURN{
        OK=0,
        PARSE_ERROR,
        NEED_UPDATE,
        INVALID_VALUE
    };
    enum PORTS{
        SERVER_PORT=12345,
        SERVER_DATA_OUTPUT_PORT=12346,
        CLIENT_REPORTER_PORT=12347,
        SERVER_REPORTER_PORT=12348
    };
};

class DataPacket{
public:
    DataPacket(string data)
    {
        QJsonDocument doc=QJsonDocument::fromJson(QByteArray(data.data(),data.size()));

        jv=doc.object();

    }
    DataPacket()
    {

    }
    DataPacket(JsonValue v)
    {
        jv=v;
    }
    string data()
    {
        QJsonObject obj=jv.toObject();
        QJsonDocument doc(obj);
        return doc.toJson().data();
    }
    template <typename tp>
    void set_value(QString name,tp value)
    {
        QJsonObject obj=jv.toObject();
        obj[name]=value;
        jv=obj;

    }
    int get_int(QString name)
    {
        return get_value(name).toInt();
    }


    bool get_bool(QString name)
    {
        return get_value(name).toBool();
    }


    string get_string(QString name)
    {
        return string(get_value(name).toString().toStdString().data());
    }

    JsonValue get_value(QString name)
    {
        QJsonObject obj=jv.toObject();
        return obj[name];
    }

    JsonValue value()
    {
        return jv;
    }
private:

    //  QJsonObject obj;
    QJsonValue jv;
};
using  namespace Json ;
class JsonPacket{
public:
    JsonPacket(string json_data)
    {
        Value v;
        Reader r;
        r.parse(json_data,v);
        val=v;
    }
    template <typename tp>
    void set_value(string name,tp value)
    {
        val[name]=value;
    }

    Value get_value(string name)
    {
        return val[name];
    }
private:
    Value val;
};
#endif // PD_H
