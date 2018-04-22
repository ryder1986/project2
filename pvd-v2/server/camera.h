#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include "filedatabase.h"
#include "QJsonObject"
#include "videosource.h"
#include "pddemoprocessor.h"
#include "processeddatasender.h"
//#include "pdprocessor.h"
#include "../PvdProcessor/pvdprocessor.h"
#include <mutex>
using namespace cv;
class Camera : public QThread
{
    Q_OBJECT
    typedef struct alg{
        QJsonValue pvd_c4;
        QJsonValue pvd_hog;
        QString selected_alg;
    }alg_t;

    typedef struct Camera_config{
        QString url;
        int direction;
        int camera_id;
        QString user_name;
        QString password;
        QString camera_ip;
        int camera_port;
        alg_t alg;
    }Configture_t;
    Rect detect_rect;
public:
    Camera(QJsonValue jv):processor(NULL)
    {
        frame_rate=0;
        quit=false;
        jv_2_cfg(jv);
        start_cam();
        tmr=new QTimer;
        connect (tmr,SIGNAL(timeout()),this,SLOT(handle_time_up()));
        tmr->start(1000);


    }
    ~Camera()
    {
        stop_cam();
    }

    Rect area_2_rect(QJsonValue area)
    {
        int x_min=10000;
        int y_min=10000;
        int x_max=0;
        int y_max=0;
        foreach (QJsonValue v, area.toArray()) {
            int x= v.toObject()["x"].toInt();
            int y= v.toObject()["y"].toInt();
            if(x<x_min)
                x_min=x;
            if(x>x_max)
                x_max=x;
            if(y<y_min)
                y_min=y;
            if(y>y_max)
                y_max=y;
        }
        return Rect(x_min,y_min,x_max-x_min,y_max-y_min);
    }
    bool modify_alg(QJsonValue jv)
    {

        DataPacket pkt(jv.toObject());
        QJsonValue jv_selected_alg= pkt.get_value("selected_alg");
        QJsonValue jv_pvd_c4=  pkt.get_value("pvd_c4");
        QJsonValue jv_pvd_hog= pkt.get_value("pvd_hog");
        if(jv_selected_alg.isNull()||jv_pvd_c4.isNull()||jv_pvd_hog.isNull()){
            return false;
        }
        cam_cfg.alg.selected_alg= jv_selected_alg.toString();
        cam_cfg.alg.pvd_c4= jv_pvd_c4;
        cam_cfg.alg.pvd_hog= jv_pvd_hog;
        restart_processor();
        return true;
    }
    void modify_attr(QJsonValue v)
    {
        DataPacket pkt(v.toObject());
        int di= pkt.get_value("direction").toInt();
        int no= pkt.get_value("camera_id").toInt();
        cam_cfg.direction=di;
        cam_cfg.camera_id=no;
    }

    QJsonValue config()
    {
        return cfg_2_jv();
    }

    void add_watcher(QString ip)
    {
        if(ip_list.contains(ip)){

        }else{
            prt(info,"client %s require output",ip.toStdString().data());
            ip_list.append(ip);
        }

    }
    void del_watcher(QString ip)
    {
        if(ip_list.contains(ip)){
            prt(info,"client %s stop output",ip.toStdString().data());
            ip_list.removeOne(ip);
        }else{
        }
    }

private:
    void restart_processor()
    {
        mtx.lock();
        QString str=cam_cfg.alg.selected_alg;
        if(processor)
            delete processor;
        if(str=="pvd_c4"){
            processor=new PvdC4Processor(cam_cfg.alg.pvd_c4);
        }else if(str=="pvd_hog"){
            processor=new PvdHogProcessor(cam_cfg.alg.pvd_hog);
        }
        mtx.unlock();
    }

    void start_cam()
    {
        src=new VideoSource(cam_cfg.url);
        restart_processor();
        start();
    }

    void stop_cam()
    {
        delete tmr;
        quit=true;
        this->wait();
        delete src;
        delete processor;
        src=NULL;
        processor=NULL;
    }

    virtual QJsonValue cfg_2_jv()
    {
        QJsonObject cfg;
        QJsonValue jv;
        DataPacket pkt(cfg);
        pkt.set_value("url",cam_cfg.url);
        pkt.set_value("direction",cam_cfg.direction);
        pkt.set_value("camera_id",cam_cfg.camera_id);
        pkt.set_value("user_name",cam_cfg.user_name);
        pkt.set_value("password",cam_cfg.password);
        pkt.set_value("camera_ip",cam_cfg.camera_ip);
        pkt.set_value("camera_port",cam_cfg.camera_port);
        QJsonObject alg;
        DataPacket pkt_alg(alg);
        pkt_alg.set_value("selected_alg",cam_cfg.alg.selected_alg);
        pkt_alg.set_value("pvd_c4",cam_cfg.alg.pvd_c4);
        pkt_alg.set_value("pvd_hog",cam_cfg.alg.pvd_hog);
        pkt.set_value("alg",pkt_alg.object());
        jv= pkt.object();
        return jv;
    }

    virtual void jv_2_cfg(QJsonValue cfg)
    {
        DataPacket pkt(cfg.toObject());
        cam_cfg.url = pkt.get_value("url").toString();
        cam_cfg.direction=pkt.get_value("direction").toInt();
        cam_cfg.camera_id=pkt.get_value("camera_id").toInt();
        cam_cfg.user_name=pkt.get_value("user_name").toString();
        cam_cfg.password=pkt.get_value("password").toString();
        cam_cfg.camera_ip=pkt.get_value("camera_ip").toString();
        cam_cfg.camera_port=pkt.get_value("camera_port").toInt();
        QJsonValue alg=pkt.get_value("alg");
        DataPacket pkt_alg(alg.toObject());
        cam_cfg.alg.selected_alg=pkt_alg.get_value("selected_alg").toString();
        cam_cfg.alg.pvd_c4=pkt_alg.get_value("pvd_c4");
        cam_cfg.alg.pvd_hog=pkt_alg.get_value("pvd_hog");
    }

    void send_out(QByteArray ba)
    {
        //  emit output(ba);
        ProcessedDataSender *s=ProcessedDataSender::get_instance();
        foreach (QString ip, ip_list) {
            s->send(ba,QHostAddress(ip));
        }
    }

protected:
    void run()
    {
        int i=0;
        Mat frame;
        threadid=(int)QThread::currentThread();
        QByteArray rst;
        while(!quit){
            //   prt(info,"runing %s",cam_cfg.url.toStdString().data());
            // lock.lock();
            mtx.lock();
            if(src->get_frame(frame)&&frame.cols>0&&frame.rows>0){
                frame_rate++;
                // bool ret=process(frame,rst);
                bool ret=processor->process(frame);
                send_out(processor->get_rst());
                //  if(ret){
                //                if(true){
                //                    ba.clear();
                //                    ba.append(rst);
                //                    //  emit output(ba);
                //                    send_out(ba);
                //                }
            }else{
                //prt(info,"get no frame");
            }
            //  lock.unlock();

            mtx.unlock();
            QThread::msleep(1);
        }
        // QThread::msleep(10);
    }

signals:
    void output(QByteArray ba);
public slots:
    void handle_time_up()
    {
        // prt(info,"%s framerate:%d,(id:%x)",cam_cfg.url.toStdString().data(),frame_rate,threadid);
        frame_rate=0;
    }

private:
    QList <QString> ip_list;
    int frame_rate;
    int threadid;
    QTimer *tmr;
    VideoSource *src;
    VideoProcessor *processor;
    Configture_t cam_cfg;
    bool quit;
    QMutex lock;
    mutex mtx;
};

#endif // CAMERA_H
