#include "MainDlg.h"
#include"ImageCropperLabel.h"
#include<QPalette>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QFormLayout>
#include<QFileDialog>
#include<QMessageBox>
#include<QColorDialog>
MainDlg::MainDlg(QWidget *parent)
    : QDialog(parent)
{

    this->setMinimumSize(QSize(1050, 500));

    setupLayout(); //添加一些控件，然后在设置一下它们的布局

    connect(m_imgCropperLabel,&ImageCropperLabel::CropperPosChanged,this,&MainDlg::OnUpdatePreview);

    connect(m_btSelectImage,&QPushButton::clicked,this,&MainDlg::OnClickedSelectImage);

    connect(m_cbOutputShape,&QComboBox::currentIndexChanged,this,&MainDlg::OnOutputShapeChanged);
    connect(m_cbCropperShape,&QComboBox::currentIndexChanged,this,&MainDlg::OnCropperShapeChanged);

    connect(m_checkShowOpacity,&QCheckBox::clicked,this,&MainDlg::OnClickedShowOpacity);
    connect(m_sliderOpacity,&QSlider::valueChanged,this,&MainDlg::OnOpacityChanged);

    connect(m_checkShowDragSquare,&QCheckBox::clicked,this,&MainDlg::OnClickedShowDragSquare);
    connect(m_btDragSquareColor,&QPushButton::clicked,this,&MainDlg::OnClickedDragSquareColor);

    connect(m_checkShowRectBorder,&QCheckBox::clicked,this,&MainDlg::OnClickedShowRectBorder);
    connect(m_comboRectBorderType,&QComboBox::currentIndexChanged,this,&MainDlg::OnCropperBorderType);
    connect(m_btRectBorderColor,&QPushButton::clicked,this,&MainDlg::OnClickedRectBorderColor);

    connect(m_savePreviewImage,&QPushButton::clicked,this,&MainDlg::OnSaveCroppedImage);
}

MainDlg::~MainDlg()
{
}

void MainDlg::setupLayout()
{
    m_imgCropperLabel = new ImageCropperLabel(600,500,this);
    m_imgCropperLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_imgCropperLabel->setCropperShape(CropperShape::RECT);

    //预览图片标签
    m_labelPreviewImage = new QLabel(this);
    m_labelPreviewImage->setFixedSize(100, 100);  //览图片标签固定大小为100x100
    m_labelPreviewImage->setAlignment(Qt::AlignCenter); //内容居中对齐
    m_labelPreviewImage->setFrameStyle(QFrame::Panel | QFrame::Sunken); //样式

    // 编辑框 按钮，
    m_editImagePath = new QLineEdit(this);
    m_btSelectImage = new QPushButton("选择图片",this);
    m_editImagePath->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); //设置控件大小策略为固定值
    QHBoxLayout* hLayout1 = new QHBoxLayout;
    hLayout1->addWidget(m_editImagePath);
    hLayout1->addWidget(m_btSelectImage);

    //输出图片形状选择（组合框）和 Cropper 形状选择（组合框）
    m_cbOutputShape  = new QComboBox(this);
    m_cbOutputShape->setFixedWidth(215);
    m_cbOutputShape->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); //设置控件大小策略为固定值
    m_cbOutputShape->addItem("正方形");
    m_cbOutputShape->addItem("圆形");
    m_cbOutputShape->setEnabled(false);

    m_cbCropperShape = new QComboBox(this);
    m_cbCropperShape->setFixedWidth(215);
    m_cbCropperShape->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);//设置控件大小策略为固定值
    m_cbCropperShape->addItem("矩形");
    m_cbCropperShape->addItem("正方形");
    m_cbCropperShape->addItem("椭圆");
    m_cbCropperShape->addItem("圆形");
    m_cbCropperShape->setEnabled(false);


    //阴影贴图显示选择框，阴影贴图透明度滑块
    m_checkShowOpacity = new QCheckBox(this);
    m_checkShowOpacity->setCheckState(Qt::Checked);
    m_checkShowOpacity->setEnabled(false);

    m_sliderOpacity = new QSlider(Qt::Horizontal, this);
    m_sliderOpacity->setFixedWidth(215);
    m_sliderOpacity->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); //设置控件大小策略为固定值
    m_sliderOpacity->setRange(0, 100); //设置范围
    m_sliderOpacity->setValue(60);     //设置当前值
    m_sliderOpacity->setEnabled(false);


    //是否显示拖拽方块选择框，拖拽方块颜色选择按钮，显示选择的颜色的标签
    m_checkShowDragSquare  = new QCheckBox(this);
    m_checkShowDragSquare->setCheckState(Qt::Checked);
    m_checkShowDragSquare->setEnabled(false);

    m_btDragSquareColor    = new QPushButton("选择颜色",this);
    m_btDragSquareColor->setEnabled(false);

    m_labelDragSquareColor = new QLabel(this);
    setLabelColor(m_labelDragSquareColor,Qt::white);

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    hLayout2->addWidget(m_labelDragSquareColor);
    hLayout2->addWidget(m_btDragSquareColor);


    //是否显示矩形边框选择框，矩形边框类型选择组合框，矩形边框颜色选择按钮，显示选择的颜色的标签
    m_checkShowRectBorder  = new QCheckBox(this);
    m_checkShowRectBorder->setCheckState(Qt::Checked);
    m_checkShowRectBorder->setEnabled(false);

    m_comboRectBorderType  = new QComboBox(this);
    m_comboRectBorderType->setFixedWidth(215);
    m_comboRectBorderType->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); //设置控件大小策略为固定值
    m_comboRectBorderType->addItem("虚线");
    m_comboRectBorderType->addItem("实线");
    m_comboRectBorderType->setEnabled(false);

    m_btRectBorderColor    = new QPushButton("选择颜色",this);
    m_btRectBorderColor->setEnabled(false);

    m_labelRectBorderColor = new QLabel(this);
    m_labelRectBorderColor->setFixedWidth(25);
    setLabelColor(m_labelRectBorderColor,Qt::white);

    QHBoxLayout* hLayout3 = new QHBoxLayout();
    hLayout3->addWidget(m_labelRectBorderColor);
    hLayout3->addWidget(m_btRectBorderColor);

    //添加静态文本
    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("预览",m_labelPreviewImage);
    formLayout->addRow("裁剪图片",hLayout1);
    formLayout->addRow("输出图片形状",m_cbOutputShape);
    formLayout->addRow("裁剪框样式",m_cbCropperShape);
    formLayout->addRow("Show Opacity",m_checkShowOpacity);
    formLayout->addRow("Opacity",m_sliderOpacity);
    formLayout->addRow("显示拖拽方块",m_checkShowDragSquare);
    formLayout->addRow("拖拽方块颜色",hLayout2);
    formLayout->addRow("显示矩形边框",m_checkShowRectBorder);
    formLayout->addRow("矩形边框样式",m_comboRectBorderType);
    formLayout->addRow("矩形边框颜色",hLayout3);

    //保存预览图片按钮
    m_savePreviewImage = new QPushButton("保存",this);
    m_savePreviewImage->setFixedSize(50,25);
    m_savePreviewImage->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); //设置控件大小策略为固定值
    m_savePreviewImage->setEnabled(false);

    formLayout->addRow(m_savePreviewImage);



    QSpacerItem *spacer = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_imgCropperLabel);
    mainLayout->addItem(spacer);
    mainLayout->addLayout(formLayout);
}

void MainDlg::setLabelColor(QLabel* label,const QColor &color)
{
    QPixmap pixmap(QSize(25, 25));
    pixmap.fill(color);
    label->setPixmap(pixmap);
}

/************************************************
 * 更新预览图片（当 Cropper 位置和大小改变时）
************************************************/
void MainDlg::OnUpdatePreview()
{
    QPixmap imgPreview = m_imgCropperLabel->getCroppedImage();

    imgPreview = imgPreview.scaled(m_labelPreviewImage->width(),m_labelPreviewImage->height(),
                                   Qt::KeepAspectRatio,Qt::SmoothTransformation);

    m_labelPreviewImage->setPixmap(imgPreview);
}


void MainDlg::OnClickedSelectImage()
{
    QString ImagePath = QFileDialog::getOpenFileName(this,"选择图片","","图片文件(*.jpg *.png *.bmp)");

    if(ImagePath.isEmpty())
        return;

    QPixmap pixmap;
    if (!pixmap.load(ImagePath))
    {
        QMessageBox::critical(this, "Error", "Load image failed", QMessageBox::Ok);
        return;
    }

    m_editImagePath->setText(ImagePath);

    m_imgCropperLabel->adjustImageSize(pixmap);

    OnUpdatePreview();

    m_labelPreviewImage->setFrameStyle(0);

    m_cbOutputShape->setEnabled(true);
    m_cbCropperShape->setEnabled(true);
    m_checkShowOpacity->setEnabled(true);
    m_sliderOpacity->setEnabled(true);
    m_checkShowDragSquare->setEnabled(true);
    m_btDragSquareColor->setEnabled(true);
    m_checkShowRectBorder->setEnabled(true);
    m_comboRectBorderType->setEnabled(true);
    m_btRectBorderColor->setEnabled(true);
    m_savePreviewImage->setEnabled(true);
}

void MainDlg::OnOutputShapeChanged(int index)
{
    if(index == 0)
        m_imgCropperLabel->setOutputShape(OutputShape::RECT);
    else
        m_imgCropperLabel->setOutputShape(OutputShape::ELLIPSE);

    OnUpdatePreview(); //更新预览图片
}

void MainDlg::OnCropperShapeChanged(int index)
{
    switch (index)
    {

    case 0: //RECT
        m_imgCropperLabel->setCropperShape(CropperShape::RECT);
        break;
    case 1: //SQUARE
        m_imgCropperLabel->setCropperShape(CropperShape::SQUARE);
        break;
    case 2: // ELLIPSE
        m_imgCropperLabel->setCropperShape(CropperShape::ELLIPSE);
        break;
    case 3: // CIRCLE
        m_imgCropperLabel->setCropperShape(CropperShape::CIRCLE);
        break;
    }
    m_imgCropperLabel->update();
    OnUpdatePreview();
}

void MainDlg::OnClickedShowOpacity(int state)
{
    if(state == 1)
    {
        m_sliderOpacity->setEnabled(true);
        m_imgCropperLabel->showShowOpacity(true);
    }
    else
    {
        m_sliderOpacity->setEnabled(false);
        m_imgCropperLabel->showShowOpacity(false);
    }
    m_imgCropperLabel->update();
}

void MainDlg::OnOpacityChanged(int value)
{
    m_imgCropperLabel->setOpacity(value/100.0);
    m_imgCropperLabel->update();
}

void MainDlg::OnClickedShowDragSquare(int state)
{
    if(state == 1)
    {
        m_imgCropperLabel->showDragSquare(true);
        m_btDragSquareColor->setEnabled(true);
    }
    else
    {
        m_imgCropperLabel->showDragSquare(false);
        m_btDragSquareColor->setEnabled(false);
    }
    m_imgCropperLabel->update();
}

void MainDlg::OnClickedDragSquareColor()
{
    QColor color = QColorDialog::getColor(Qt::white,this);
    if (color.isValid())
    {
        //将颜色显示到标签上
        setLabelColor(m_labelDragSquareColor,color);

        m_imgCropperLabel->setDragSquareColor(color);
        m_imgCropperLabel->update();
    }
}

void MainDlg::OnClickedShowRectBorder(int state)
{
    if(state == 1)
    {
        m_imgCropperLabel->showRectBorder(true);
        m_btRectBorderColor->setEnabled(true);
        m_comboRectBorderType->setEnabled(true);
    }
    else
    {
        m_imgCropperLabel->showRectBorder(false);
        m_btRectBorderColor->setEnabled(false);
        m_comboRectBorderType->setEnabled(false);
    }
    m_imgCropperLabel->update();
}

void MainDlg::OnCropperBorderType(int index)
{
    m_imgCropperLabel->setBorderLine(index);
    m_imgCropperLabel->update();
}

void MainDlg::OnClickedRectBorderColor()
{
    QColor color = QColorDialog::getColor(Qt::white,this);
    if (color.isValid())
    {
        //将颜色显示到标签上
        setLabelColor(m_labelRectBorderColor,color);

        m_imgCropperLabel->setBorderLineColor(color);
        m_imgCropperLabel->update();
    }

}

void MainDlg::OnSaveCroppedImage()
{
    //得到裁剪后的 Image
    const QPixmap pixmap = m_labelPreviewImage->pixmap();
    if (!pixmap)
    {
        QMessageBox::information(this, "Error", "There is no cropped image to save.", QMessageBox::Ok);
        return ;
    }

    //保存
    QString filename = QFileDialog::getSaveFileName(this, "Save cropped image", "", "PNG(*.png)");
    if (!filename.isNull())
        m_imgCropperLabel->getCroppedImage().save(filename);
}



















