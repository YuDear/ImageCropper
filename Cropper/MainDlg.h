#ifndef MAINDLG_H
#define MAINDLG_H

#include <QDialog>
#include<QLineEdit>
#include<QComboBox>
#include<QCheckBox>
#include<QSpacerItem>
#include"ImageCropperLabel.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainDlg; }
QT_END_NAMESPACE

class MainDlg : public QDialog
{
    Q_OBJECT

public:
    MainDlg(QWidget *parent = nullptr);
    ~MainDlg();

    void setupLayout(); //添加一些控件，然后在设置一下它们的布局
    void setLabelColor(QLabel *label,const QColor &color); //设置标签背景颜色
private:
    /****************************************
     * 控件变量
     ***************************************/
    ImageCropperLabel* m_imgCropperLabel;   //进行裁剪图片操作对应控件
    QLabel* m_labelPreviewImage;         //显示裁剪后的预览图片（标签）
    QLineEdit*   m_editImagePath;        //图片路径编辑框
    QPushButton* m_btSelectImage;        //选择图片按钮
    QComboBox* m_cbOutputShape;          //输入图片形状选择（组合框）
    QComboBox* m_cbCropperShape;         //Cropper 形状选择（组合框）
    QCheckBox* m_checkShowOpacity;
    QSlider* m_sliderOpacity;            //滑块
    QCheckBox* m_checkShowDragSquare;    //是否显示拖动方块（选择框）
    QPushButton* m_btDragSquareColor;    //拖动方块的颜色选择（按钮）
    QLabel * m_labelDragSquareColor;     //显示选择的颜色
    QCheckBox* m_checkShowRectBorder;    //是否显示矩形边框（选择框）
    QComboBox* m_comboRectBorderType;    //矩形边框类型选择（组合框）
    QPushButton* m_btRectBorderColor;    //矩形边框颜色选择（按钮）
    QLabel * m_labelRectBorderColor;     //显示选择的颜色
    QPushButton* m_savePreviewImage;     //保存预览图片（按钮）

 public slots:
    void OnUpdatePreview();      //更新预览图片（当 Cropper 位置和大小改变时）
    void OnClickedSelectImage(); //选择图片

    void OnOutputShapeChanged(int index); //输出图像形状改变
    void OnCropperShapeChanged(int index);//裁剪框形状改变


    void OnClickedShowOpacity(int state);
    void OnOpacityChanged(int value);

    void OnClickedShowDragSquare(int state);
    void OnClickedDragSquareColor();

    void OnClickedShowRectBorder(int state);
    void OnCropperBorderType(int index);
    void OnClickedRectBorderColor();
    void OnSaveCroppedImage();
};
#endif // MAINDLG_H
