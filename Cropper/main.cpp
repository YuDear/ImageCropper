#include "MainDlg.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainDlg w;

    w.show();
    return a.exec();
}

/*（"Cropping" 是指裁剪图像的操作，"Cropper" 是指用于进行裁剪的工具或软件，而 "Cropped" 是指经过裁剪处理后的图像。）*/
