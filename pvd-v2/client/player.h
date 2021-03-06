#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QThread>
#include <QJsonValue>
#include <QTimer>
#include <QJsonObject>
#include <videosource.h>
#include <QJsonArray>
#include "tool.h"
#include "playerwidget.h"
class Player : public QThread
{

    typedef struct Camera_config{
        QString url;
        int direction;
        int camera_id;
        QString user_name;
        QString password;
        QString camera_ip;
        int camera_port;
        QJsonValue alg;

    }Camera_config_t;
    Camera_config_t cam_cfg;
    QImage img1;
    int frame_rate;
       Q_OBJECT
public:
    Player(QJsonValue cfg){
        pos=0;
        frame_rate=0;
        jv_2_cfg(cfg);
        src=new VideoSource(cam_cfg.url);
        wgt=NULL;
        QTimer *t=new QTimer();
        connect(t,SIGNAL(timeout()),this,SLOT(check_rate()));
        t->start(1000);
    }

    ~Player()
    {
        delete wgt;
        delete src;
    }
    void set_widget(PlayerWidget *w)
    {
      wgt=w;
    }
    PlayerWidget *get_widget()
    {
        return wgt;
    }

    QList <QPoint> get_v()
    {QList <QPoint>  p;
        p.clear();
      QJsonObject ob=  cam_cfg.alg.toObject();
      QString alg_str=ob["selected_alg"].toString();
      if(alg_str=="pvd_c4"){
           QJsonArray ps= ob["pvd_c4"].toObject()["detect_area"].toArray();
           foreach (QJsonValue v, ps) {
               p.append(QPoint(v.toObject()["x"].toInt(),v.toObject()["y"].toInt()));
           }
      }else if(alg_str=="pvd_hog")
      {
          QJsonArray ps= ob["pvd_hog"].toObject()["detect_area"].toArray();
          foreach (QJsonValue v, ps) {
              p.append(QPoint(v.toObject()["x"].toInt(),v.toObject()["y"].toInt()));
          }
      }
      if(p.size()!=4)
          p.clear();
      return p;
    }
    void set_v( QList <QPoint> )
    {

    }
    void save_rgb(const char* data,int w,int h)
    {
        if(pos++<100){
        QFile f("test.rgb");

        f.open((QIODevice::Append));

        f.write(data,w*h*3);
        }
        else{
            prt(info,"get 100 done");
        }
    }
    void read_rgb(  char* data,int w,int h)
    {
        QFile f("test.rgb");

        f.open((QIODevice::ReadOnly));
        if(pos==30)
            pos=1;
           f.seek(w*h*3*pos++);
       // f.write(data,w*h*3/2);
        f.read(data,w*h*3);
    }
public slots:
    void check_rate()
    {
        //frame_rate++;
      //  prt(info,"frame rate :%d ",frame_rate);
        frame_rate=0;
    }

signals:
private:
    void run()
    {
        Mat bgr_frame;
        Mat rgb_frame;
      //  Mat yuv;
          this->setObjectName("play_thread");
        while(1){

            if(src->get_frame(bgr_frame)){
                src->get_frame(bgr_frame);
                //cvtColor(bgr_frame,yuv,CV_BGR2YUV);


                cvtColor(bgr_frame,rgb_frame,CV_BGR2RGB);


               // save_rgb((const char*)rgb_frame.data,640,480);

     #if    1
                img1=QImage((const uchar*)(rgb_frame.data),
                            rgb_frame.cols,rgb_frame.rows,
                            QImage::Format_RGB888);

#else

                static char buf[640*480*3];
                read_rgb(buf,640,480);
                img1=QImage((const uchar*)buf,
                            rgb_frame.cols,rgb_frame.rows,
                            QImage::Format_RGB888);

#endif
                if(wgt){
                    img1.bits();
                    wgt->set_image(img1);
                    wgt->set_title(cam_cfg.url);
                    if(frame_rate%3==0)
                    wgt->update();
                    frame_rate++;
                }

      //          this->usleep(10);
            }else{
                this->usleep(10);
            }
        }
    }
    virtual QJsonValue cfg_2_jv()
    {
        QJsonValue jv;
        jv.toObject()["url"]= cam_cfg.url;
        jv.toObject()["direction"]=cam_cfg.direction;
        jv.toObject()["camera_id"]= cam_cfg.camera_id;
        jv.toObject()["user_name"]= cam_cfg.user_name;
        jv.toObject()["password"]= cam_cfg.password;
        jv.toObject()["camera_ip"]= cam_cfg.camera_ip;
        jv.toObject()["camera_port"]= cam_cfg.camera_port;
        jv.toObject()["alg"]=cam_cfg.alg;

        return jv;

    }
    virtual void jv_2_cfg(QJsonValue cfg)
    {
        cam_cfg.url = cfg.toObject()["url"].toString();
        cam_cfg.direction=cfg.toObject()["direction"].toInt();
        cam_cfg.camera_id=cfg.toObject()["camera_id"].toInt();
        cam_cfg.user_name=cfg.toObject()["user_name"].toString();
        cam_cfg.password=cfg.toObject()["password"].toString();
        cam_cfg.camera_ip=cfg.toObject()["camera_ip"].toString();
        cam_cfg.camera_port=cfg.toObject()["camera_port"].toInt();
        cam_cfg.alg=cfg.toObject()["alg"];
    }

    VideoSource *src;
    PlayerWidget *wgt;
    int  pos;
};

#endif // PLAYER_H
