#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H
#include <QObject>
#include <QTcpSocket>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include "processeddatasender.h"
#include "tool.h"
#include "pvd.h"
class ClientSession:public QObject{
    QByteArray tmp_msg;
    Q_OBJECT
public:
    bool need_sync;
    ClientSession(QTcpSocket *client_skt):skt(client_skt),focus_index(0){
        tmp_msg.clear();
        need_sync=true;
        focus_index=0;
        connect(skt,SIGNAL(readyRead()),this,SLOT(handle_msg()));
        connect(skt,SIGNAL(disconnected()),this,SLOT(deleteLater()));
        connect(skt,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error_happened()));
        client_addr=skt->peerAddress();
    }
    ~ClientSession()
    {
    }
    void set_valid(bool valid)
    {
        need_sync=!valid;

        if(!valid){
            QByteArray rt;
            rt.clear();
            QJsonObject obj;
            obj["type"]=Pvd::NEED_UPDATE;
            QJsonDocument doc(obj);
            rt=doc.toJson();
            skt->write(rt,rt.size());
        }
    }

    bool is_valid()
    {
        return !need_sync;
    }

public slots:
    void handle_alg_out(QByteArray out)
    {
        QString str(out.data());
        prt(info,"sending %s",str.toStdString().data());
        ProcessedDataSender *sender=ProcessedDataSender::get_instance();
        sender->send(out,client_addr);
    }

    void error_happened()
    {
        emit socket_error(this);
    }
    int count_begin_symbol(QByteArray ba)
    {
        char *tmp=ba.data();
        int sz=ba.size();
        int ret=0;
        int i;
        for( i=0;i<sz;i++){
            if(tmp[i]=='{'){
                ret++;
            }
        }
        return ret;
    }
    int count_end_symbol(QByteArray ba)
    {
        char *tmp=ba.data();
        int sz=ba.size();
        int ret=0;
        int i;
        for( i=0;i<sz;i++){
            if(tmp[i]=='}'){
                ret++;
            }
        }
        return ret;
    }
    void find_pos(QByteArray ba,int &endpos,int &left)
    {

    }
    bool try_get_obj_buf(QByteArray src,QByteArray &dst)
    {
        int ret=false;
        int stack=0;
        char *p_tmp=src.data();
        bool flg=false;
        dst.clear();
        dst.append(src);
        int i;
        if(count_begin_symbol(src)>0){
            for(i=0;i<src.size();i++){
                if(p_tmp[i]=='{')
                {
                    stack++;
                    flg=true;
                }
                if(p_tmp[i]=='}')
                    stack--;


                if(stack==0&&flg)
                {

                    break;
                }

            }
            if(i<src.size()){
                ret=true;
                if(src[i+1]=='\n')
                    dst.truncate(i+2);
                else
                    dst.truncate(i+i);
            }
        }
        return ret;
    }

    //dst:a sting which contain a compelete json object
    //src:a slice ofstream buffer
    //tmp_msg:last slice of buffer(maybe begining of json string)
    bool get_valid_buf(QByteArray &src,QByteArray &dst)
    {
        if(try_get_obj_buf(src,dst)){
            src.remove(0,dst.size());
            return true;
        }
        return false;
    }

    void handle_msg(){
        int writes_num=0;
        QByteArray client_buf=skt->readAll();
        QByteArray valid_buf;
        valid_buf.clear();
        tmp_msg.append(client_buf);
        while(get_valid_buf(tmp_msg,valid_buf)) {//Get valid json object, TODO:we only check {} matches, we should check json grammar
            string rt;
            rt.clear();
            QString str_input(valid_buf);
            prt(info,"get %d bytes:",str_input.length());
            printf("%s\n",str_input.toStdString().data());
            string str=valid_buf.data();
            emit client_request(str,rt,(void *)this);
           // f_client_request(valid_buf,rt,this);
            writes_num=skt->write(rt.data(),rt.size());
            QString str_output(QByteArray(rt.data(),rt.size()));
            prt(info,"reply %d bytes:",writes_num);
            printf("%s\n",str_output.toStdString().data());
        }
    }

    void displayError(QAbstractSocket::SocketError socketError)
    {
        switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            prt(info,"err");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            prt(info,"err");
            break;
        default:break;

        }
    }
    QString ip()
    {
        return client_addr.toString();
    }

signals :
    int get_server_config(char *buf);
    void socket_error(ClientSession *c);
    int client_request(string request,string &ret,void *);

//    void session_operation(int req,void *addr, int len,int cam_index,char *buf,int &ret_size);

public:

//    function<int(QByteArray request,QByteArray &ret,void *)> f_client_request=[](QByteArray request,QByteArray &ret,void *){prt(info," need funtion defination");return 0;};
private:
    char *rcv_buf;
    char send_buf[Pvd::BUFFER_LENGTH];
    QTcpSocket *skt;
    QTimer *timer;
    QHostAddress client_addr;
    int focus_index;
};

#endif // CLIENTSESSION_H
