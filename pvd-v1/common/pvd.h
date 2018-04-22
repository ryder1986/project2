#ifndef PVD_H
#define PVD_H
#include <cstring>
#include <QtCore>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
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
    DataPacket(QByteArray data)
    {
        QJsonDocument doc=QJsonDocument::fromJson(data);
        obj=doc.object();
    }
    DataPacket(QJsonObject data)
    {
        obj=data;
    }
    DataPacket()
    {

    }
    QByteArray data()
    {
        QJsonDocument doc(obj);
        return doc.toJson();
    }
    QJsonObject object()
    {
        return obj;
    }

    template <typename tp>
    void set_value(QString name,tp value)
    {
        obj[name]=value;
    }


    QJsonValue get_value(QString name)
    {
        return obj[name];
    }


private:
    QJsonObject obj;
};

#endif // PD_H
