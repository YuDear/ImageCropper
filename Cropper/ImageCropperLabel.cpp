#include "ImageCropperLabel.h"
#include<QPainter>
#include<QPainterPath>
#include <QMouseEvent>
#include<QDebug>
ImageCropperLabel::ImageCropperLabel(int width,int height,QWidget* parent):QLabel(parent)
{
    //设置标签属性
    this->resize(width,height);    //设置标签的大小
    this->setAlignment(Qt::AlignCenter); //设置标签文本水平垂直居中对齐
    this->setMouseTracking(true);        //只有设置这个才能接收到鼠标移动事件

    //初始化画笔的宽度为 1，颜色为白色，和画出的线段样式
    m_borderPen.setWidth(1);
    m_borderPen.setColor(Qt::white);
    m_borderPen.setDashPattern(QVector<qreal>() << 3 << 3);
}

/**********************************
 * 获取裁剪后的图片
**********************************/
QPixmap ImageCropperLabel::getCroppedImage()
{
    // 裁剪操作一般是在原始图像上进行，而不是缩放后的图像上进行，这是因为在缩放后的图像上裁剪会会导致丢失图像的一部分信息。
    // 因为缩放会改变图像的大小和分辨率，从而导致裁剪后得到的图像与原始图像可能存在不一致的情况。此外，放后的图像可能会失
    // 真或产生像素插值等问题，进一步降低图像质量。

    //获取裁剪框在原始图片中的位置和高宽，具体就是获取裁剪框在缩放后图片中的位置和高宽，然后根据缩放比例进行恢复
    int x = int((m_cropperRect.left() - m_imageRect.left()) / m_scaledRate);
    int y = int((m_cropperRect.top() - m_imageRect.top()) / m_scaledRate);
    int width = int(m_cropperRect.width() / m_scaledRate);
    int height = int(m_cropperRect.height() / m_scaledRate);

    QPixmap image = m_originalImage.copy(x, y, width, height); //裁剪


    //Set ellipse mask（mask：在图像处理中，遮罩是一个二进制图像，用于指定哪些区域需要进行特定的操作或处理。遮罩图像的像素值通常为 0 或 1，其中 0 表示该像素被遮挡，1 表示该像素可见。通过将遮罩应用于原始图像，可以实现图像的局部处理或分割）
    if(m_outputShape == OutputShape::ELLIPSE)
    {
        QBitmap mask(width,height);
        QPainter painter(&mask);
        painter.setRenderHint(QPainter::Antialiasing); //使用抗锯齿，提升图像边缘质量
        painter.setRenderHint(QPainter::SmoothPixmapTransform); //使用平滑算法，使图像缩放和旋转更平滑
        painter.fillRect(0, 0, width, height, Qt::white);
        painter.setBrush(QColor(0, 0, 0));
        painter.drawRoundedRect(0, 0, width, height,99, 99,Qt::RelativeSize);

        image.setMask(mask);
    }

    return image;
}


/*************************************************************************
 * 调整图片的大小，使图片能保持其原始比例然后完整的显示到标签中(如果图片比标签大)
 ************************************************************************/
void ImageCropperLabel::adjustImageSize(QPixmap &pixmap)
{

    m_originalImage = pixmap; //保持原始图片

    int ImageWidth  = pixmap.width();
    int ImageHeight = pixmap.height();
    int LabelWidth = this->width();
    int LabelHeight = this->height();
    int imgWidthInLabel;
    int imgHeightInLabel;

    //如果要裁剪的图片小于标签，那么就不进行调整图片的大小
    if(ImageWidth <= LabelWidth && ImageHeight <= LabelHeight)
    {
        imgWidthInLabel = ImageWidth;
        imgHeightInLabel = ImageHeight;
        m_imageRect.setRect((LabelWidth-imgWidthInLabel)/2,(LabelHeight-imgHeightInLabel)/2,
                            imgWidthInLabel,imgHeightInLabel);
    }

    //如果宽度更大，则将图片的宽度拉伸为标签的宽度，然后在计算出比例拉伸图片的高度
    else if(ImageWidth * LabelHeight > LabelWidth * ImageHeight)
    {
        m_scaledRate = LabelWidth / (double)ImageWidth; //计算缩放比例
        imgWidthInLabel = LabelWidth;
        imgHeightInLabel = int(m_scaledRate * ImageHeight); //计算高度
        m_imageRect.setRect(0,(LabelHeight-imgHeightInLabel)/2,imgWidthInLabel,imgHeightInLabel);
    }
    //如果高度更大，将图片的高度拉伸为标签的高度，然后在计算出比例拉伸图片的宽度
    else
    {
        m_scaledRate = LabelHeight / (double)ImageHeight; //计算缩放比例
        imgHeightInLabel = LabelHeight;
        imgWidthInLabel = int(m_scaledRate * ImageWidth); //计算高度
        m_imageRect.setRect((LabelWidth-imgWidthInLabel)/2,0,imgWidthInLabel,imgHeightInLabel);
    }

    //缩放图片，然后显示
    m_scaledImage = pixmap.scaled(imgWidthInLabel,imgHeightInLabel,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    this->setPixmap(m_scaledImage);
    //将cropper的位置和大小设置到默认位置和默认大小
    resetCropperPos();


    /* ImageWidth * LabelHeight > LabelWidth * ImageHeight 的含义
     *
     * 如果 ImageWidth/ImageHeight > LabelWidth/LabelHeight，即图片的比例一定大于标签的比例，则说明 ImageWidth 一定大
     * 于 ImageHeight，如果我们将图片的宽度设为和标签的一样（固定宽度），然后根据比例计算出的高度一定是小于标签的高度。如果是这样
     * 的话我们就可以固定宽度，计算高度，否则不是这种情况，那么就固定高度，计算宽度。
     *
     * 和表达式 ImageWidth * LabelHeight > LabelWidth * ImageHeight 有什么关系？
     *
     * 只不过是将除非换算成乘法，换算过程如下：
     *
     * ImageWidth / ImageHeight > LabelWidth / LabelHeight 俩边同时乘以 ImageHeight =
     * ImageWidth > (LabelWidth * ImageHeight) / LabelHeight 俩边同时乘以 LabelHeight =
     * ImageWidth * LabelHeight > LabelWidth * ImageHeight
     *
    */
}


void ImageCropperLabel::paintEvent(QPaintEvent *event)
{
    //因为重写了基类的 paintEvent ，如果还想要实现基类的 paintEvent 就需要这样调用以下。
    QLabel::paintEvent(event);

    //绘制 cropper 并设置一些效果
    switch (m_cropperShape)
    {
    case CropperShape::RECT: //绘制矩形
        drawRectOpacity();
        drawDragSquare(false); //false 绘制 8 个拖拽方块，ture 绘制 4 个拖拽方块
        break;
    case CropperShape::SQUARE:
        drawRectOpacity();
        drawDragSquare(true);
        break;
    case CropperShape::ELLIPSE:
        //今天该实现这个功能了
        drawEllipseOpacity();
        drawDragSquare(false);
        break;

    case CropperShape::CIRCLE:
        drawEllipseOpacity();
        drawDragSquare(true);
        break;
    }

    //绘制 Cropper 的边框（如果选择绘制）
    if(m_showRectBorder)
    {
        QPainter painter(this);
        painter.setPen(m_borderPen);
        painter.drawRect(m_cropperRect);
    }
}

/*****************************************************
 * 鼠标事件
*****************************************************/
void ImageCropperLabel::mousePressEvent(QMouseEvent *event)
{
    m_pressPos = event->pos(); //获取点击位置
    m_isLBtPress = true;       //更新状态
}

void ImageCropperLabel::mouseReleaseEvent(QMouseEvent *event)
{
    //更新状态
    m_isLBtPress =false;
    m_isLBtPressMove = false;

}

void ImageCropperLabel::mouseMoveEvent(QMouseEvent *event)
{
    QPoint cursorPos = event->pos(); //获取光标所在位置


    // 获取光标在 Cropper 的哪个位置，并根据位置更改光标样式 (如果鼠标没有按下移动,因为按下移动的时候位置一定是固定的)
    if(m_isLBtPressMove == false)
    {
        m_cursorPosInCropperRect = getPosInCropperRect(cursorPos);
        changeCursor();
    }

    //如果鼠标没有按下，则直接返回，后面的代码是移动或拉伸 CropperRect 的代码
    if(m_isLBtPress == false)
        return;

    m_isLBtPressMove = true;

    //得到x,y偏移值
    int xOffset = cursorPos.x() - m_pressPos.x();
    int yOffset = cursorPos.y() - m_pressPos.y();
    m_pressPos = cursorPos;

    switch (m_cursorPosInCropperRect)
    {
    //如果光标在 Cropper 外部，则直接返回
    case RECT_OUTSIZD:
        break;
    //如果光标在 Cropper 内部，则跟随光标移动 Cropper
    case RECT_INSIDE:
    {
        //四个 if 是判断 Cropper 是否超过了图片的边界
        if(m_cropperRect.left() + xOffset < m_imageRect.left())
            xOffset = m_imageRect.left() - m_cropperRect.left();
        if(m_cropperRect.right() + xOffset > m_imageRect.right())
            xOffset = m_imageRect.right() - m_cropperRect.right();
        if(m_cropperRect.top() + yOffset < m_imageRect.top())
            yOffset = m_imageRect.top() - m_cropperRect.top();
        if(m_cropperRect.bottom() + yOffset > m_imageRect.bottom())
            yOffset = m_imageRect.bottom() - m_cropperRect.bottom();

        m_cropperRect.translate(xOffset, yOffset); //偏移
        emit CropperPosChanged();
        break;
    }
    case RECT_LEFT:
        switch (m_cropperShape)
        {
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            stretchCropperLeft(cursorPos.x()); //拉伸 Cropper 左边
            emit CropperPosChanged();
            break;
        }
        break;
    case RECT_RIGHT:
        switch (m_cropperShape)
        {
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            stretchCropperRight(cursorPos.x()); //拉伸 Cropper 右边
            emit CropperPosChanged();
            break;
        }
        break;
    case RECT_TOP:
        switch (m_cropperShape)
        {
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            stretchCropperTop(cursorPos.y()); //拉伸 Cropper 上边
            emit CropperPosChanged();
            break;
        }
        break;
    case RECT_BOTTOM:
        switch (m_cropperShape)
        {
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            stretchCropperBottom(cursorPos.y()); //拉伸 Cropper 下边
            emit CropperPosChanged();
            break;
        }
        break;
    case RECT_LEFT_TOP:
    {
        switch (m_cropperShape)
        {
        case CropperShape::SQUARE:
        case CropperShape::CIRCLE:
            stretchCropLeftTop(cursorPos);
            break;
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            stretchCropperLeft(cursorPos.x()); //拉伸左边
            stretchCropperTop(cursorPos.y());  //拉伸上边
            break;
        }
        emit CropperPosChanged();
        break;
    }
    case RECT_RIGHT_TOP:
    {
        switch (m_cropperShape)
        {
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            stretchCropperRight(cursorPos.x()); //拉伸右边
            stretchCropperTop(cursorPos.y());  //拉伸上边
            emit CropperPosChanged();
            break;
        }
        break;
    }
    case RECT_LEFT_BOTTOM:
    {
        switch (m_cropperShape)
        {
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            stretchCropperLeft(cursorPos.x()); //拉伸左边
            stretchCropperBottom(cursorPos.y());  //拉伸下边
            emit CropperPosChanged();
            break;
        }
        break;
    }
    case RECT_RIGHT_BOTTOM:
    {
        switch (m_cropperShape)
        {
        case CropperShape::SQUARE:
        case CropperShape::CIRCLE:
        {

            break;
        }
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            stretchCropperRight(cursorPos.x()); //拉伸右边
            stretchCropperBottom(cursorPos.y());  //拉伸下边
            emit CropperPosChanged();
            break;
        }
        break;
    }
    }
    repaint(); //触发 QPainter 事件，请求重绘
}

void ImageCropperLabel::resizeEvent(QResizeEvent *event)
{
    if(!m_originalImage.isNull())
        adjustImageSize(m_originalImage);
}

/**********************************************
 * 获取光标在 CopperRect 中的哪个位置
**********************************************/
int ImageCropperLabel::getPosInCropperRect(const QPoint &pt)
{
    if(cursorInDragSquare(pt,m_cropperRect.topLeft()))    //判断光标是否在左上角拖拽方块上面
        return RECT_LEFT_TOP;
    if(cursorInDragSquare(pt,m_cropperRect.topRight()))   //判断光标是否在右上角拖拽方块上面
        return RECT_RIGHT_TOP;
    if(cursorInDragSquare(pt,m_cropperRect.bottomLeft())) //判断光标是否在左下角拖拽方块上面
        return RECT_LEFT_BOTTOM;
    if(cursorInDragSquare(pt,m_cropperRect.bottomRight())) //判断光标是否在右下角拖拽方块上面
        return RECT_RIGHT_BOTTOM;
    if(cursorInDragSquare(pt,QPoint(m_cropperRect.left(),m_cropperRect.center().y())))   //判断光标是否在左边拖拽方块上面
        return RECT_LEFT;
    if(cursorInDragSquare(pt,QPoint(m_cropperRect.right(),m_cropperRect.center().y())))  //判断光标是否在左边拖拽方块上面
        return RECT_RIGHT;
    if(cursorInDragSquare(pt,QPoint(m_cropperRect.center().x(),m_cropperRect.top())))    //判断光标是否在左边拖拽方块上面
        return RECT_TOP;
    if(cursorInDragSquare(pt,QPoint(m_cropperRect.center().x(),m_cropperRect.bottom()))) //判断光标是否在左边拖拽方块上面
        return RECT_BOTTOM;
    if(m_cropperRect.contains(pt,true))
        return RECT_INSIDE;

    //条件都不符合就认为在 CopperRect 外部
    return RECT_OUTSIZD;
}
bool ImageCropperLabel::cursorInDragSquare(const QPoint &p1, const QPoint &p2)
{
    /*
       如果当光标位置在拖拽方块中，那么光标位置减去拖拽方块的中心点位置乘以2必然小于等于拖拽方块的边长，
       所以有以下代码进行判断光标是否在拖拽方块中
    */
    return abs(p1.x() - p2.x()) * 2 <= m_dragPointSize && abs(p1.y() - p2.y()) * 2 <= m_dragPointSize;
}

/**********************************************
 * 根据光标所在位置改变光标的样式
**********************************************/
void ImageCropperLabel::changeCursor()
{
    switch (m_cursorPosInCropperRect)
    {
    case RECT_OUTSIZD:
        setCursor(Qt::ArrowCursor);   //箭头光标样式
        break;
    case RECT_INSIDE:
        setCursor(Qt::SizeAllCursor); //十字光标样式
        break;
    case RECT_LEFT_TOP:
        setCursor(Qt::SizeFDiagCursor);// \ 样式
        break;
    case RECT_LEFT_BOTTOM:
        setCursor(Qt::SizeBDiagCursor);// / 样式
        break;
    case RECT_RIGHT_TOP:
        setCursor(Qt::SizeBDiagCursor);// / 样式
        break;
    case RECT_RIGHT_BOTTOM:
        setCursor(Qt::SizeFDiagCursor);// \ 样式
        break;
    case RECT_LEFT:
        switch (m_cropperShape)
        {
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            setCursor(Qt::SizeHorCursor); // —— 样式
            break;
        }
        break;
    case RECT_RIGHT:
        switch (m_cropperShape)
        {
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            setCursor(Qt::SizeHorCursor); // —— 样式
            break;
        }
        break;
    case RECT_TOP:
        switch (m_cropperShape)
        {
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            setCursor(Qt::SizeVerCursor); // | 样式
            break;
        }
        break;
    case RECT_BOTTOM:
        switch (m_cropperShape)
        {
        case CropperShape::RECT:
        case CropperShape::ELLIPSE:
            setCursor(Qt::SizeVerCursor); // | 样式
            break;
        }
        break;
    }
}

void ImageCropperLabel::stretchCropperLeft(int cursorX)
{
    int distance = m_cropperRect.right() - cursorX; //得到俩个位置之间的距离

    //如果这个距离大于等于 Cropper 的最小宽度，且光标的水平位置大于等于图片左边，则进行缩放
    if( distance >= m_cropperMinWidth && cursorX >= m_imageRect.left())
        m_cropperRect.setLeft(cursorX); //左边跟随光标位置进行缩放

    //处理一些小瑕疵
    if(distance < m_cropperMinWidth)
        m_cropperRect.setLeft(m_cropperRect.right() - m_cropperMinWidth);

    if(cursorX < m_imageRect.left())
        m_cropperRect.setLeft(m_imageRect.left());
}
void ImageCropperLabel::stretchCropperRight(int cursorX)
{
    int distance = cursorX - m_cropperRect.left(); //得到俩个位置之间的距离

    //如果这个距离大于等于 Cropper 的最小宽度，且光标的水平位置小于等于图片左边，则进行缩放
    if( distance >= m_cropperMinWidth && cursorX <= m_imageRect.right())
        m_cropperRect.setRight(cursorX); //右边跟随光标位置进行缩放

    //处理一些小瑕疵
    if(distance < m_cropperMinWidth)
        m_cropperRect.setRight(m_cropperRect.left() + m_cropperMinWidth);

    if(cursorX > m_imageRect.right())
        m_cropperRect.setRight(m_imageRect.right());
}
void ImageCropperLabel::stretchCropperTop(int cursorY)
{
    int distance = m_cropperRect.bottom() - cursorY; //得到俩个位置之间的距离

    //如果这个距离大于等于 Cropper 的最小高度，且光标的水平位置大于等于图片上边，则进行缩放
    if( distance >= m_cropperMinHeight && cursorY >= m_imageRect.top())
        m_cropperRect.setTop(cursorY); //跟随光标位置进行缩放

    //处理一些小瑕疵
    if(distance < m_cropperMinHeight)
        m_cropperRect.setTop(m_cropperRect.bottom() - m_cropperMinHeight);

    if(cursorY < m_imageRect.top())
        m_cropperRect.setTop(m_imageRect.top());
}
void ImageCropperLabel::stretchCropperBottom(int cursorY)
{
    int distance = cursorY - m_cropperRect.top(); //得到俩个位置之间的距离

    //如果这个距离大于等于 Cropper 的最小宽度，且光标的水平位置小于等于图片左边，则进行缩放
    if( distance >= m_cropperMinHeight && cursorY <= m_imageRect.bottom())
        m_cropperRect.setBottom(cursorY); //下边跟随光标位置进行缩放

    //处理一些小瑕疵
    if(distance < m_cropperMinHeight)
        m_cropperRect.setBottom(m_cropperRect.top() + m_cropperMinHeight);

    if(cursorY > m_imageRect.bottom())
        m_cropperRect.setBottom(m_imageRect.bottom());
}

void ImageCropperLabel::stretchCropLeftTop(QPoint &cursorPos)
{
    int distanceX = m_cropperRect.right() - cursorPos.x() ; //得到俩个位置之间的距离
    int distanceY = m_cropperRect.bottom() - cursorPos.y();

    //只有当矩形框的高度大于等于最小宽高，才可能进行跟随鼠标移动缩放
    if(distanceY >= m_cropperMinHeight)
    {
        //当光标的水平位置大于图片左边，垂直位置大于图片上边，则跟随鼠标移动缩放
        if(cursorPos.x() > m_imageRect.left()  && cursorPos.y() > m_imageRect.top())
        {
            //如果
            if (m_cropperRect.right() - distanceY >= m_imageRect.left())
            {
                m_cropperRect.setTop(cursorPos.y());
                m_cropperRect.setLeft(m_cropperRect.right() - distanceY);
            }
        }
        //否则就说明超出图片边界了，直接设置为图片边界
        else
        {
            //没想好怎么处理
        }
    }
    //否则就所明缩放大小，已经小于最小大小，则直接一直设为最小大小
    else if(distanceY < m_cropperMinHeight )
    {
        m_cropperRect.setLeft(m_cropperRect.right() - m_cropperMinWidth);
        m_cropperRect.setTop(m_cropperRect.bottom() - m_cropperMinHeight);
    }
//    if(distanceY > m_imageRect.bottom())
//        m_cropperRect.setBottom(m_imageRect.bottom());
}


void ImageCropperLabel::drawOpacity(const QPainterPath &path)
{
    QPainter painter(this);
    painter.setOpacity(m_opacity);
    painter.fillPath(path,QBrush(Qt::black));
}

void ImageCropperLabel::drawDragSquare(bool onlyFourCorners)
{
    //如果选择不绘制拖拽点，则直接返回不进行绘制
    if(m_showDragSquare == false)
        return;

    // Four corners (绘制 4 个角)

    drawFillRect(m_cropperRect.topLeft(),m_dragPointSize,m_dragPointColor);
    drawFillRect(m_cropperRect.topRight(),m_dragPointSize,m_dragPointColor);
    drawFillRect(m_cropperRect.bottomLeft(),m_dragPointSize,m_dragPointColor);
    drawFillRect(m_cropperRect.bottomRight(),m_dragPointSize,m_dragPointColor);

    // Four edges（在 4 个边上绘制出拖拽点）
    if(onlyFourCorners == false)
    {
        //先得到边的中心点
        int centralX = m_cropperRect.left() + m_cropperRect.width()  / 2;
        int centralY = m_cropperRect.top()  + m_cropperRect.height() / 2;

        drawFillRect(QPoint(centralX,m_cropperRect.top()),m_dragPointSize,m_dragPointColor);    //上
        drawFillRect(QPoint(centralX,m_cropperRect.bottom()),m_dragPointSize,m_dragPointColor); //下
        drawFillRect(QPoint(m_cropperRect.left(),centralY),m_dragPointSize,m_dragPointColor);   //左
        drawFillRect(QPoint(m_cropperRect.right(),centralY),m_dragPointSize,m_dragPointColor);  //右
    }

}

void ImageCropperLabel::drawFillRect(QPoint centralPoint, int edge, QColor color)
{
    QPainter painter(this);
    QRect rect(centralPoint.x() - edge / 2, centralPoint.y() - edge / 2, edge , edge);
    painter.fillRect(rect,QBrush(color));
}


void ImageCropperLabel::drawRectOpacity()
{
    if(m_showOpacity)
    {
        QPainterPath p1,p2;
        p1.addRect(m_imageRect);
        p2.addRect(m_cropperRect);
        drawOpacity(p1.subtracted(p2));
    }
}

void ImageCropperLabel::drawEllipseOpacity()
{
    if(m_showOpacity)
    {
        QPainterPath p1,p2;
        p1.addRect(m_imageRect);
        p2.addEllipse(m_cropperRect);
        drawOpacity(p1.subtracted(p2));
    }
}


/**********************************************
 * 将 cropper 的位置重置到图像中心并调整大小为默认值
**********************************************/
void ImageCropperLabel::resetCropperPos()
{
    int labelWidth = this->width();
    int labelHeight = this->height();

    switch (m_cropperShape)
    {
    case CropperShape::RECT:
    case CropperShape::SQUARE:
    case CropperShape::ELLIPSE:
    case CropperShape::CIRCLE:
    {
        //将 cropper 初始设置为正方形，边长是图片的最小边的3/4
        int imgWidth = m_imageRect.width();
        int imgHeight = m_imageRect.height();
        int edge = (imgWidth > imgHeight ? imgHeight : imgWidth) * 3 / 4;         //得到边长
        m_cropperRect.setRect((labelWidth - edge) / 2, (labelHeight - edge) / 2, edge, edge); //设置大小，并设置到中心
        break;
    }

    }
}

void ImageCropperLabel::setCropperShape(CropperShape shape)
{
    m_cropperShape = shape;
    resetCropperPos();
}

void ImageCropperLabel::setBorderLine(int type)
{
    if(type == 0)
        m_borderPen.setDashPattern(QVector<qreal>() << 3 << 3);
    else
        m_borderPen.setStyle(Qt::SolidLine);
}









































