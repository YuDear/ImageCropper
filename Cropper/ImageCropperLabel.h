#ifndef IMAGECROPPERLABEL_H
#define IMAGECROPPERLABEL_H

#include <QLabel>
#include<QPixmap>
#include<QPen>

/***************************************************
 * Cropper Shape Type
***************************************************/
enum class CropperShape{
    RECT          = 1,
    SQUARE        = 2,
    ELLIPSE       = 3,
    CIRCLE        = 4,
};

/***************************************************
 * Output Image Shape
***************************************************/
enum OutputShape{
    RECT    = 0,
    ELLIPSE = 1
};

class ImageCropperLabel : public QLabel
{
    Q_OBJECT

public:
    ImageCropperLabel(int width,int height,QWidget* parent = nullptr);

private:
    CropperShape m_cropperShape = CropperShape::RECT;
    QRect        m_cropperRect;

    OutputShape  m_outputShape  = OutputShape::RECT;

    QPixmap      m_originalImage; //原始图片
    QPixmap      m_scaledImage;   //缩放过后的图片
    QRect        m_imageRect;     //缩放过后的图片的大小（根据标签大小缩放过后的图片大小）

    double       m_scaledRate = 1.0;  //图片缩放比例

    bool m_isLBtPress = false;      //鼠标左键是否按下
    bool m_isLBtPressMove = false;  //鼠标是否按下移动中
    QPoint m_pressPos;              //鼠标按下的位置

    //可调整参数
    bool   m_showOpacity    = true;     //是否显示透明贴图（图片上面覆盖一层黑色透明矩形）
    bool   m_showRectBorder = true;     //是否显示 cropper 的矩形边框，默认为显示
    bool   m_showDragSquare = true;     //是否显示 cropper 的拖拽方块，默认为显示

    double m_opacity        = 0.6;      //黑色透明矩形的透明度[0,1]，0.0完全透明，1.0完全不透明
    int    m_dragPointSize  = 8;        //拖拽方块的大小（指定边长），默认边长是 8
    QColor  m_dragPointColor = Qt::white;//拖拽方块的颜色，默认为白色

    int    m_cropperMinWidth = m_dragPointSize * 2;  // cropper 的最小宽度，默认是拖拽方块的俩倍
    int    m_cropperMinHeight = m_dragPointSize * 2; // cropper 的最小高度，默认是拖拽方块的俩倍



    QPen m_borderPen; //绘制 Cropper 边框的画笔

    int m_cursorPosInCropperRect = RECT_OUTSIZD; //光标在 CopperRect 中的哪个位置，默认光标在 CopperRect 外部

    enum {
        RECT_OUTSIZD = 0, //外部
        RECT_INSIDE  = 1, //内部
        RECT_LEFT_TOP,    //左上拖动形状
        RECT_TOP,         //上边拖动形状
        RECT_RIGHT_TOP,   //右上拖动形状
        RECT_LEFT,        //左边拖动形状
        RECT_RIGHT,       //右边拖动形状
        RECT_LEFT_BOTTOM, //左下拖动形状
        RECT_BOTTOM,      //下边拖动形状
        RECT_RIGHT_BOTTOM,//右下拖动形状
    };

public:
    /*****************************************
     * 对外提供的功能函数
    *****************************************/
    QPixmap getCroppedImage(); //获取裁剪后的图片
    void setCropperShape(CropperShape shape); // 该函数可以设置不同形状的 Cropper
    void setOutputShape(OutputShape shape){m_outputShape = shape;} //设置输出的图片的形状
    void showShowOpacity(bool b){m_showOpacity = b;}; //设置是否显示透明贴图
    void setOpacity(double value){m_opacity = value ;};
    void showDragSquare(bool b){m_showDragSquare = b;};
    void showRectBorder(bool b){m_showRectBorder = b;};
    void setBorderLine(int type);
    void setBorderLineColor(const QColor &color){m_borderPen.setColor(color);};
    void setDragSquareColor(QColor color){m_dragPointColor = color;};

    void adjustImageSize(QPixmap &pixmap);    //调整图片的大小，使图片能保持其原始比例然后完整的显示到标签中(如果图片比标签大)


protected:
    /*****************************************
     * 事件
    *****************************************/
    virtual void paintEvent(QPaintEvent *event) override;       //绘图事件（类试MFC OnPaint）
    virtual void mousePressEvent(QMouseEvent *event) override;  //鼠标按下事件
    virtual void mouseReleaseEvent(QMouseEvent *event) override;//鼠标释放事件
    virtual void mouseMoveEvent(QMouseEvent *event) override;   //鼠标移动事件（处理CropperRect 的移动和拉伸）
    virtual void resizeEvent(QResizeEvent *event) override;     //大小改变事件

private:
    /*****************************************
     * 在 paintEvent 事件中做的主要功能，用到的函数
    *****************************************/
    void drawRectOpacity();                     //得到一个矩形的 Cropper path
    void drawEllipseOpacity();                  //得到一个椭圆的 Cropper path
    void drawOpacity(const QPainterPath &path); //绘制透明贴图（图片上面覆盖一层黑色的透明path（paht是上面俩个函数得到的））

    void drawDragSquare(bool onlyFourCorners);                          //绘制拖拽方块
    void drawFillRect(QPoint centralPoint, int edge, QColor color);     //绘制矩形拖拽方块

    /*********************************************
     * 在 mouseMoveEvent 事件中做的主要功能，用到的函数
    *********************************************/
    int  getPosInCropperRect(const QPoint &pt);                 //获取光标在 CopperRect 中的哪个位置
    bool cursorInDragSquare(const QPoint &p1,const QPoint &p2); //判断光标是否在拖拽方块中，是的话返回 true
    void changeCursor();                                        //改变光标样式
    void stretchCropperLeft(int cursorX);  //拉伸 Cropper 左边
    void stretchCropperRight(int cursorX); //拉伸 Cropper 右边
    void stretchCropperTop(int cursorY);   //拉伸 Cropper 上边
    void stretchCropperBottom(int cursorY);//拉伸 Cropper 下边
    void stretchCropLeftTop(QPoint &cursorPos); //拉伸 Cropper 左上角
    void stretchCropRingTop(QPoint &cursorPos); //拉伸 Cropper 右上角
    void stretchCropLeftBottom(QPoint &cursorPos); //拉伸 Cropper 左下角
    void stretchCropRightBottom(QPoint &cursorPos); //拉伸 Cropper 右下角

    void resetCropperPos(); //重置 Cropper 位置

signals:
    void CropperPosChanged(); //当裁剪位置或大小改变时，可以选择触发此信号
};

#endif // IMAGECROPPERLABEL_H





















