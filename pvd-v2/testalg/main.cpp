#include <QCoreApplication>
#include <pvdprocessor1.h>
#include "cv.h"
//#include <opencv2/core/core.hpp>
static int pos=0;
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
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    PvdC4Processor p;

    static char buf[640*480*3];
    read_rgb(buf,640,480);
    CvMat mt= cvMat(480,640,CV_8UC3,(void *)buf);
    string rst= p.process(mt);
    return a.exec();
}

