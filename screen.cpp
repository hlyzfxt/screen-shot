#include "screen.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPoint>
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>
#include <QFileDialog>
#include <QRectF>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTime>
#include <QClipboard>
#include <QFontMetrics>
#include "math.h"
#include "QDesktopWidget"
#include "iostream"

const int textEditLargeDefaultWidth  = 120;
const int textEditLargeDefaultHeight = 48;

const int textEditMiddleDefaultWidth  = 98;
const int textEditMiddleDefaultHeight = 36;

const int textEditSmallDefaultWidth  = 66;
const int textEditSmallDefaultHeight = 30;

constexpr int controlWidth = 389;
constexpr int controlHeight = 38;
constexpr int selectContrlWidth = 245;
constexpr int textSelectControlWidth = 230;
constexpr int IntervelFromControlToSelectControl = 11;
constexpr int IntervelFromControlToSnap = 8;

Screen::Screen(QWidget *parent)
    : QWidget(parent)
{
    setWindowState(windowState() | Qt::WindowFullScreen);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    QDesktopWidget* desktopWidget = QApplication::desktop();
    m_screenRect = desktopWidget->screenGeometry();  //获取设备屏幕大小

    //截取全屏
    QScreen *scrPix = QGuiApplication::primaryScreen();
    m_pixmap = scrPix->grabWindow(0);

    //全局路径的初始化，一个全屏闭合回路
    m_globalPath.lineTo(m_screenRect.width(), 0);
    m_globalPath.lineTo(m_screenRect.width(), m_screenRect.height());
    m_globalPath.lineTo(0, m_screenRect.height());
    m_globalPath.lineTo(0, 0);

    m_control = nullptr;
    m_selectControl = nullptr;
    m_textSelectControl = nullptr;

    setMouseTracking(true);

    m_plaintextedit = new QTextEdit (this);
    m_plaintextedit->hide();
    m_plaintextedit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plaintextedit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plaintextedit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_plaintextedit->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    connect(m_plaintextedit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    reInit();
}

void Screen::reInit()
{
    m_snap = 0;
    m_onceRelease = true;
    m_oncePress = true;
    m_type = NO;
    m_drawStyle = DRAW_STYLE_MAX_INT;
    m_arrowDirection = 0;
    for(int i = 0; i < DRAW_STYLE_MAX_INT; i++)
    {
        m_drawSize[i] = SMALL;
        m_drawColor[i] = RED;
    }
    m_drawSize[LINE] = MIDDLE;
    m_records.clear();
    m_lines.clear();

    if(m_plaintextedit)
    {
        m_plaintextedit->clear();
        m_plaintextedit->hide();
    }

    if(m_control)
    {
        m_control->hide();
        m_control->unCheckAllButton();
    }
    if(m_selectControl)
        m_selectControl->hide();
    if(m_textSelectControl)
        m_textSelectControl->hide();
    setCursor(Qt::ArrowCursor);
}

int Screen::getDefaultEditWidth(DRAW_SIZE_E size)
{
    if(SMALL == size)
    {
        return textEditSmallDefaultWidth;
    }
    else if (MIDDLE == size)
    {
        return textEditMiddleDefaultWidth;
    }
    else if (LARGE == size)
    {
        return textEditLargeDefaultWidth;
    }
    return textEditSmallDefaultWidth;
}

int Screen::getDefaultEditHeight(DRAW_SIZE_E size)
{
    if(SMALL == size)
    {
        return textEditSmallDefaultHeight;
    }
    else if (MIDDLE == size)
    {
        return textEditMiddleDefaultHeight;
    }
    else if (LARGE == size)
    {
        return textEditLargeDefaultHeight;
    }
    return textEditSmallDefaultHeight;
}

void Screen::onTextChanged()
{
    QFont font = m_plaintextedit->font();
    QFontMetrics  metrics = QFontMetrics(font);
    QSize size = metrics.size(0, m_plaintextedit->toPlainText());

    if(size.width() + 10 > getDefaultEditWidth(getDrawSize(getDrawStyle())))
    {
        m_plaintextedit->resize(size.width() + 10, m_plaintextedit->height());
    }
    if(m_plaintextedit->document()->size().height() > getDefaultEditHeight(getDrawSize(getDrawStyle())))
    {
        m_plaintextedit->resize(m_plaintextedit->width(),
                                m_plaintextedit->document()->size().height());
    }
}

void Screen::setTextEditSizeToDefault()
{
    m_plaintextedit->setDisabled(0);
    QString sheet = QString("QTextEdit{ border: 1px solid #dadada;}"
                        "QTextEdit{font-family:'Microsoft YaHei'; font-size:%1px;border-color:#ff1e10;color:%2;background-color: rgba(255,255,255,255);}"
                        "").arg(QString::number(getFontPixelSize(getDrawSize(getDrawStyle()))))
                        .arg(getColorString(getDrawColor(getDrawStyle())));
    m_plaintextedit->setStyleSheet(sheet);

    m_plaintextedit->setMaximumWidth(m_endPoint.x() - m_pressPoint.x());
    m_plaintextedit->setMaximumHeight(m_endPoint.y() - m_pressPoint.y());
    switch (getDrawSize(getDrawStyle()))
    {
    case SMALL:
        m_plaintextedit->resize(textEditSmallDefaultWidth, textEditSmallDefaultHeight);
        break;
    case MIDDLE:
        m_plaintextedit->resize(textEditMiddleDefaultWidth, textEditMiddleDefaultHeight);
        break;
    case LARGE:
        m_plaintextedit->resize(textEditLargeDefaultWidth, textEditLargeDefaultHeight);
        break;
    default:
        break;
    }
}

void Screen::paintEvent(QPaintEvent *e)//将上一个函数得到的全屏灰色图绘制在painter上，并添加宽高信息矩形和边角拖曳小正方形
{
    QPainter paint(this);
    paint.drawPixmap(0, 0, m_pixmap);

    //初始化画笔操作
    QPen pen;
    pen.setWidth(4);
    pen.setColor(QColor(69,134,255, 255));
    paint.setPen(pen);

    if(m_snap == 0)
    {
        if(true == m_oncePress)
        {
            paint.setBrush(QColor(0, 0, 0, 0));
            paint.drawRect(0, 0, m_screenRect.width(), m_screenRect.height());
        }
        else
        {
            paint.setBrush(QColor(0, 0, 0, 0X66));
            QPainterPath path = getPath();
            pen.setWidth(1);
            pen.setColor(QColor(0, 0, 0, 0));
            paint.setPen(pen);
            paint.drawPath(path);

            pen.setWidth(1);
            pen.setColor(QColor(69,134,255, 255));
            paint.setPen(pen);
            paint.setBrush(QColor(0, 0, 0, 0));
            paint.drawRect(m_startPoint.x(), m_startPoint.y(),
                           m_endPoint.x() - m_startPoint.x(),
                           m_endPoint.y() - m_startPoint.y());
            if(false  == m_oncePress)
            {
                //绘制选取左上角显示宽高的区域
                drawSizeTips(paint);
            }

            //绘制四个小正方形
            drawControlArea(paint);
        }
    }

    //放大镜
    if(m_oncePress == true)
    {
         drawMagnifier(paint);
    }

    //绘制截图区域
    drawLabel(paint);

    if(nullptr != m_selectControl && nullptr != m_textSelectControl)
    {
        if(m_selectControl->isVisible() || m_textSelectControl->isVisible())
            drawControlArrow(paint, m_arrowDirection);
    }
}
//绘制正方形
void Screen::drawControlArea(QPainter &painter)//已看懂
{
    painter.save();

    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(69,134,255, 255));
    painter.setBrush(QColor(69, 134, 255, 255));
    painter.setPen(pen);

    painter.drawRect(m_rect1);
    painter.drawRect(m_rect2);
    painter.drawRect(m_rect3);
    painter.drawRect(m_rect4);
    painter.drawRect(m_rect5);
    painter.drawRect(m_rect6);
    painter.drawRect(m_rect7);
    painter.drawRect(m_rect8);

    painter.restore();
}

void Screen::drawLabel(QPainter &painter)
{
    drawLabelRecord(painter);

    if(m_isPressedInSnap)
        drawLabelCurrent(painter);
}

float Screen::getArrowWidth(double arrowLength,DRAW_SIZE_E size)
{
    float width = 0;
    if (SMALL == size)
    {
        width = 12.0;
    }
    else if(MIDDLE == size)
    {
        if (arrowLength <= 30) {
            width = 12.0;
        }
        else if (arrowLength <= 48) {
            width = 12.0 + (48 - 30) / 3;
        }
        else
            width = 18.0;
    }
    else if (LARGE == size) {
        if (arrowLength <= 36) {
            width = 12.0;
        }
        else if (arrowLength <= 90) {
            width = 12.0 + (90 - 36) / 3;
        }
        else {
            width = 30.0;
        }
    }
    return width;
}

void Screen::drawArrow(QPainter &painter, QPoint start, QPoint end, DRAW_COLOR_E color, DRAW_SIZE_E size)
{
    if(start == end)
        return;
    painter.save();
    QPen pen;
    pen.setWidth(1);
    pen.setColor(toQColor(color));
    painter.setPen(pen);
    painter.setBrush(toQColor(color));
    double arrowLength = sqrt((end.x() - start.x()) * (end.x() - start.x()) +
                             (end.y() - start.y()) * (end.y() - start.y()));

    float width = getArrowWidth(arrowLength, size);

    if (arrowLength < width && start.x() != end.x() && start.y() != end.y())
    {
        drawIncompleteArrow(painter, start, end, width);
    }
    else if (start.x() == end.x())
    {
        drawVerticalArrow(painter, start, end, width);
    }
    else if (start.y() == end.y())
    {
        drawHorizontalArrow(painter, start, end, width);
    }
    else
    {
        drawCompleteArrow(painter, start, end, width);
    }


    painter.restore();
}

void Screen::drawIncompleteArrow(QPainter &painter, QPoint start, QPoint end, float width)
{
    float k, k1;
    // 辅助点 两个边角点的中间点
    float tempOuter_x, tempOuter_y;
    float a = (float)width / 2;

    QPoint aptPoly[4];
    k = (double)(end.y() - start.y()) / (double)(end.x() - start.x());
    k1 = (double)-(1 / k);

    float k_sq = k * k;
    float a_sq = a * a;
    float k1_sq = k1 * k1;
    if (start.x() < end.x())
    {
        tempOuter_x = end.x() - sqrt((4 * a_sq) / (k_sq + 1));
    }
    else
    {
        tempOuter_x = end.x() + sqrt((4 * a_sq) / (k_sq + 1));
    }
    tempOuter_y = (k * (tempOuter_x - end.x())) + end.y();

    float tempPoint_x;

    aptPoly[0].setX(start.x());
    aptPoly[0].setY(start.y());

    tempPoint_x = tempOuter_x - sqrt((a_sq) / (k1_sq + 1));
    aptPoly[1].setX(round(tempPoint_x));
    aptPoly[1].setY(round((k1 * (tempPoint_x - tempOuter_x)) + tempOuter_y));

    aptPoly[2].setX(end.x());
    aptPoly[2].setY(end.y());

    tempPoint_x = tempOuter_x + sqrt((a_sq) / (k1_sq + 1));
    aptPoly[3].setX(round(tempPoint_x));
    aptPoly[3].setY(round((k1 * (tempPoint_x - tempOuter_x)) + tempOuter_y));


    painter.drawPolygon(aptPoly, 4);
}

void Screen::drawVerticalArrow(QPainter &painter, QPoint start, QPoint end, float width)
{
    float a = (float)width / 2;
    QPoint aptPoly[8];

    aptPoly[0].setX(start.x());
    aptPoly[0].setY(start.y());
    aptPoly[3].setX(end.x());
    aptPoly[3].setY(end.y());
    if (end.x() < start.x())
    {
        aptPoly[1].setX(end.x() + 1.8 *a);
        aptPoly[1].setY(end.y() + 1.8*a);
        aptPoly[2].setX(end.x() - a);
        aptPoly[2].setY(end.y() + 2 * a);
        aptPoly[4].setX(end.x() + a);
        aptPoly[4].setY(end.y() + 2 * a);
        aptPoly[5].setX(end.x() + a / 2);
        aptPoly[5].setY(end.y() + 1.8*a);
        aptPoly[6].setX(start.x() - 1);
        aptPoly[6].setY(start.y() + 1);
        aptPoly[7].setX(start.x() - 1);
        aptPoly[7].setY(start.y() - 1);
    }
    else
    {
        aptPoly[1].setX(end.x() - a / 2);
        aptPoly[1].setY(end.y() - 1.8*a);
        aptPoly[2].setX(end.x() - a);
        aptPoly[2].setY(end.y() - 2 * a);
        aptPoly[4].setX(end.x() + a);
        aptPoly[4].setY(end.y() - 2 * a);
        aptPoly[5].setX(end.x() + a / 2);
        aptPoly[5].setY(end.y() - 1.8*a);
        aptPoly[6].setX(start.x() - 1);
        aptPoly[6].setY(start.y() + 1);
        aptPoly[7].setX(start.x() - 1);
        aptPoly[7].setY(start.y() - 1);
    }
    painter.drawPolygon(aptPoly, 8);
}

 void Screen::drawMagnifier(QPainter &painter)
 {
    painter.save();

    constexpr int size = 116 / 3;
    constexpr int tipsHeight = 42;
    int moveWidth = 0;
    int moveHeight = 0;

    if(m_currentPoint.y() + size + size * 3 + tipsHeight > m_screenRect.height())
    {
        moveHeight = size * 3 + tipsHeight + size * 2;
    }
    if(m_currentPoint.x() + size * 3 > m_screenRect.width())
    {
        moveWidth = size * 3;
    }

    QScreen *scrPix = QGuiApplication::primaryScreen();
    QPixmap pix = scrPix->grabWindow(0, m_currentPoint.x() - size / 2, m_currentPoint.y() - size / 2, size, size);
    painter.drawPixmap(QRect(m_currentPoint.x() - moveWidth, m_currentPoint.y() + size - moveHeight, size * 3, size * 3),
                       pix);

    QPen pen;
    pen.setColor(QColor(69, 134, 255, 255));
    pen.setWidth(1);
    painter.setPen(pen);

    QPoint point[4];
    point[0].setX(m_currentPoint.x() - moveWidth);
    point[0].setY(m_currentPoint.y() + size - moveHeight);
    point[1].setX(m_currentPoint.x() + size * 3 - moveWidth);
    point[1].setY(m_currentPoint.y() + size - moveHeight);
    point[2].setX(m_currentPoint.x() + size * 3 - moveWidth);
    point[2].setY(m_currentPoint.y() + size + size * 3 - moveHeight);
    point[3].setX(m_currentPoint.x() - moveWidth);
    point[3].setY(m_currentPoint.y() + size + size * 3 - moveHeight);
    painter.drawPolygon(point, 4);

    pen.setColor(QColor(147, 184, 255, 255));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine((point[0] + point[1]) / 2, (point[2] + point[3]) / 2);
    painter.drawLine((point[0] + point[3]) / 2, (point[1] + point[2]) / 2);

    pen.setColor(QColor(0, 0, 0, 0xD1));
    pen.setWidth(0);
    painter.setBrush(QColor(0, 0, 0, 0xD1));
    painter.setPen(pen);
    painter.drawRect(point[3].x(), point[3].y(), size * 3, tipsHeight);

    QFont font;
    font.setPixelSize(11);
    font.setFamily("SourceHanSansCN-Regular");
    painter.setFont(font);
    pen.setColor(QColor(255, 255, 255, 255));
    painter.setPen(pen);
    QString tips = "坐标:(" + QString::number(m_currentPoint.x()) + "," + QString::number(m_currentPoint.y()) + ")" + "\n";

    QPixmap pixmap = m_pixmap.copy(QRect(m_currentPoint, QSize(1,1)));//截图1个像素
    QRgb rgb = pixmap.toImage().pixel(0, 0);
    QColor color = QColor(rgb);

    tips = tips + "RGB:(" + QString::number(color.red()) + "," +
            QString::number(color.green()) + "," + QString::number(color.blue()) + ")";
    painter.drawText(point[3].x(), point[3].y(), size * 3, tipsHeight, Qt::AlignCenter, tips);

    painter.restore();
 }

void Screen::drawHorizontalArrow(QPainter &painter, QPoint start, QPoint end, float width)
{
    float a = (float)width / 2;
    QPoint aptPoly[8];

    aptPoly[0].setX(start.x());
    aptPoly[0].setY(start.y());
    aptPoly[3].setX(end.x());
    aptPoly[3].setY(end.y());
    if (end.x() < start.x())
    {
        aptPoly[1].setX(end.x() + 1.8 *a);
        aptPoly[1].setY(end.y() - a / 2);
        aptPoly[2].setX(end.x() + 2 * a);
        aptPoly[2].setY(end.y() - a);
        aptPoly[4].setX(end.x() + 2 * a);
        aptPoly[4].setY(end.y() + a);
        aptPoly[5].setX(end.x() + 1.8 *a);
        aptPoly[5].setY(end.y() + a / 2);
        aptPoly[6].setX(start.x() + 1);
        aptPoly[6].setY(start.y() - 1);
        aptPoly[7].setX(start.x() - 1);
        aptPoly[7].setY(start.y() - 1);
    }
    else
    {
        aptPoly[1].setX(end.x() - 1.8 *a);
        aptPoly[1].setY(end.y() - a / 2);
        aptPoly[2].setX(end.x() - 2 * a);
        aptPoly[2].setY(end.y() - a);
        aptPoly[4].setX( end.x() - 2 * a);
        aptPoly[4].setY(end.y() + a);
        aptPoly[5].setX(end.x() - 1.8 *a);
        aptPoly[5].setY(end.y() + a / 2);
        aptPoly[6].setX(start.x() + 1);
        aptPoly[6].setY(start.y() + 1);
        aptPoly[7].setX(start.x() - 1);
        aptPoly[7].setY(start.y() + 1);
    }
    painter.drawPolygon(aptPoly, 8);
}


void Screen::drawCompleteArrow(QPainter &painter, QPoint start, QPoint end, float width)
{
    float k, k1;
    // 辅助点 两个边角点的中间点
    float tempOuter_x, tempOuter_y;
    float tempInner_x, tempInner_y;
    float a = (float)width / 2;

    QPoint aptPoly[8];
    k = (float)(end.y() - start.y()) / (float)(end.x() - start.x());
    k1 = (float)-(1 / k);

    float k_sq = k * k;
    float a_sq = a * a;
    float k1_sq = k1 * k1;
    if (start.x() < end.x()) {
        tempOuter_x = end.x() - sqrt((4 * a_sq) / (k_sq + 1));
        tempInner_x = end.x() - sqrt((3.24 * a_sq) / (k_sq + 1));
    }
    else {
        tempOuter_x = end.x() + sqrt((4 * a_sq) / (k_sq + 1));
        tempInner_x = end.x() + sqrt((3.24 * a_sq) / (k_sq + 1));
    }
    tempOuter_y = (k * (tempOuter_x - end.x())) + end.y();
    tempInner_y = (k * (tempInner_x - end.x())) + end.y();

    float tempPoint_x;
    aptPoly[0].setX(start.x());
    aptPoly[0].setY(start.y());

    tempPoint_x = tempInner_x - sqrt((0.25 * a_sq) / (k1_sq + 1));
    aptPoly[1].setX(round(tempPoint_x));
    aptPoly[1].setY(round((k1 * (tempPoint_x - tempInner_x)) + tempInner_y));

    tempPoint_x = tempOuter_x - sqrt((a_sq) / (k1_sq + 1));
    aptPoly[2].setX(round(tempPoint_x));
    aptPoly[2].setY(round((k1 * (tempPoint_x - tempOuter_x)) + tempOuter_y));

    aptPoly[3].setX(end.x());
    aptPoly[3].setY(end.y());

    tempPoint_x = tempOuter_x + sqrt((a_sq) / (k1_sq + 1));
    aptPoly[4].setX(round(tempPoint_x));
    aptPoly[4].setY(round((k1 * (tempPoint_x - tempOuter_x)) + tempOuter_y));

    tempPoint_x = tempInner_x + sqrt((0.25 * a_sq) / (k1_sq + 1));
    aptPoly[5].setX(round(tempPoint_x));
    aptPoly[5].setY(round((k1 * (tempPoint_x - tempInner_x)) + tempInner_y));

    aptPoly[6].setX(start.x() + 1);
    aptPoly[6].setY(start.y());

    aptPoly[7].setX(start.x() - 1);
    aptPoly[7].setY(start.y());

    painter.drawPolygon(aptPoly, 8);
}

void Screen::drawLabelCurrent(QPainter &painter)
{
    painter.save();
    QPen pen;
    pen.setColor(toQColor(m_drawColor[m_drawStyle]));
    pen.setWidth(m_drawSize[m_drawStyle]);
    painter.setBrush(QColor(0, 0, 0, 0));
    painter.setPen(pen);

    int startX = m_pressPoint.x();
    int startY = m_pressPoint.y();
    int endX = m_currentPoint.x() <= m_endPoint.x() ? m_currentPoint.x() : m_endPoint.x();
    endX = endX >= m_startPoint.x() ? endX : m_startPoint.x();
    int endY = m_currentPoint.y() <= m_endPoint.y() ? m_currentPoint.y() : m_endPoint.y();
    endY = endY >= m_startPoint.y() ? endY : m_startPoint.y();

    if (RECTANGLE == m_drawStyle)
    {
        painter.drawRect(startX, startY, endX - startX, endY - startY);
    }
    else if (ROUND == m_drawStyle)
    {
        painter.drawEllipse(startX, startY, endX - startX, endY - startY);
    }
    else if(ARROW == m_drawStyle)
    {
        QPoint end;
        end.setX(endX);
        end.setY(endY);
        drawArrow(painter, m_pressPoint, end, getDrawColor(m_drawStyle), getDrawSize(m_drawStyle));
    }
    else if(LINE == m_drawStyle)
    {
        for(auto line : m_lines)
            painter.drawLine(line.startPoint, line.endPoint);
    }
    painter.restore();
}

void Screen::drawLabelRecord(QPainter &painter)
{
    painter.save();
    QPen pen;
    painter.setBrush(QColor(0, 0, 0, 0));
    for(auto record : m_records)
    {
        pen.setColor(toQColor(record.color));
        pen.setWidth(record.size);
        painter.setPen(pen);
        switch (record.style) {
        case LINE:
        {
            for(auto line : record.lines)
                painter.drawLine(line.startPoint, line.endPoint);
            break;
        }
        case RECTANGLE:
        {
            painter.drawRect(record.shape.startPoint.x(), record.shape.startPoint.y(),
                             record.shape.endPoint.x() - record.shape.startPoint.x(),
                             record.shape.endPoint.y() - record.shape.startPoint.y());
            break;
        }
        case ROUND:
        {
            painter.drawEllipse(record.shape.startPoint.x(), record.shape.startPoint.y(),
                             record.shape.endPoint.x() - record.shape.startPoint.x(),
                             record.shape.endPoint.y() - record.shape.startPoint.y());
            break;
        }
        case ARROW:
        {
            drawArrow(painter, record.shape.startPoint, record.shape.endPoint, record.color, record.size);
            break;
        }
        case TEXT:
        {
            QFont font;
            font.setPixelSize(getFontPixelSize(record.size));
            painter.setFont(font);
            painter.drawText(record.text.mRect, Qt::TextWrapAnywhere, record.text.mText);
            break;
        }
        default:
            break;
        }
    }
    painter.restore();
}

int Screen::getFontPixelSize(DRAW_SIZE_E size)
{
    constexpr int small_size = 16;
    constexpr int middle_size = 22;
    constexpr int large_size = 32;

    switch(size)
    {
    case SMALL:
        return small_size;
    case MIDDLE:
        return middle_size;
    case LARGE:
        return large_size;
    default:
        return small_size;
    }
    return small_size;
}

int Screen::coordinateConvert(QPoint press, QPoint move)
{
    m_startPoint.setX(press.x() <= move.x() ? press.x() : move.x());
    m_startPoint.setY(press.y() <= move.y() ? press.y() : move.y());

    m_endPoint.setX(press.x() > move.x() ? press.x() : move.x());
    m_endPoint.setY(press.y() > move.y() ? press.y() : move.y());

    return 0;
}
//得到选区之外的路径
QPainterPath Screen::getPath()   //这个函数得到的是开始截图时候得到的白色选区
{
    //选取路径
    QPainterPath path;
    path.moveTo(m_startPoint.x(), m_startPoint.y());
    path.lineTo(m_endPoint.x(), m_startPoint.y());
    path.lineTo(m_endPoint.x(), m_endPoint.y());
    path.lineTo(m_startPoint.x(), m_endPoint.y());
    path.lineTo(m_startPoint.x(), m_startPoint.y());
    return m_globalPath.subtracted(path);    //从灰色全屏路径中扣掉这个白色路径区域，然后作为截取的图片
}

void Screen::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
    {
        return;
    }
    m_oncePress = false;
    if(m_selectControl)
        m_selectControl->hide();
    if(m_textSelectControl)
        m_textSelectControl->hide();
    m_pressPoint = event->pos();
    m_oldeMousePoint = event->pos();

    m_plaintextedit->hide();
    if(m_plaintextedit->toPlainText().size())
    {
        saveText();
    }

    if(false == m_onceRelease &&  RECT == pointInWhere(event->pos()))
    {
        m_isPressedInSnap = true;
        m_lineStartPoint = m_pressPoint;

        if(TEXT == m_drawStyle)
        {
            setTextEditSizeToDefault();
            m_plaintextedit->move(m_pressPoint);
            m_plaintextedit->show();
            m_plaintextedit->clear();
        }
    }

    m_type = pointInWhere(event->pos());
}

void Screen::saveText()
{
    myRecord record;
    record.size = m_drawSize[m_drawStyle];
    record.color = m_drawColor[m_drawStyle];
    record.style = m_drawStyle;
    record.text.mRect = QRect(QPoint(m_plaintextedit->x(), m_plaintextedit->y()),
                              QSize(m_plaintextedit->width(), m_plaintextedit->height()));
    record.text.mText = m_plaintextedit->toPlainText();
    m_records.push_back(record);
    m_control->getUi()->returneditBtn->setDisabled(false);
    update();
}

void Screen::setMouseCursor(MOUSE_POSITION position)
{
    switch(position)
    {
        case RECT1:
        {
            if(m_records.size() == 0)
                setCursor(Qt::SizeBDiagCursor);
            break;
        }
        case RECT2:
        {
            if(m_records.size() == 0)
                setCursor(Qt::SizeFDiagCursor);
            break;
        }
        case RECT3:
        {
            if(m_records.size() == 0)
                setCursor(Qt::SizeBDiagCursor);
            break;
        }
        case RECT4:
        {
            if(m_records.size() == 0)
                setCursor(Qt::SizeFDiagCursor);
            break;
        }
        case LEFT_LINE:
        {
            if(m_records.size() == 0)
                setCursor(Qt::SizeHorCursor);
            break;
        }
        case UP_LINE:
        {
            if(m_records.size() == 0)
                setCursor(Qt::SizeVerCursor);
            break;
        }
        case RIGHT_LINE:
        {
            if(m_records.size() == 0)
                setCursor(Qt::SizeHorCursor);
            break;
        }
        case BOTTOM_LINE:
        {
            if(m_records.size() == 0)
                setCursor(Qt::SizeVerCursor);
            break;
        }
        case RECT:
        {
            if(DRAW_STYLE_MAX_INT == m_drawStyle)
            {
                setCursor(Qt::SizeAllCursor);
            }
            else
            {
                setCursor(Qt::CrossCursor);
            }
            break;
        }
        default:
        {
            setCursor(Qt::ArrowCursor);
            break;
        }
    }
    return;
}

void Screen::drawSizeTips(QPainter &painter)
{
    painter.save();
    painter.setPen(QPen());
    QFont font;
    font.setPointSize(10);
    painter.setFont(font);

    //选取左上角的信息更改
    int x = qAbs(m_endPoint.x() - m_startPoint.x());//movepoint即终点坐标
    int y = qAbs(m_endPoint.y() - m_startPoint.y());
    QString str = tr("%1 * %2").arg(x).arg(y);

    QFontMetrics  metrics = QFontMetrics(font);
    QSize size = metrics.size(0, str);

    painter.setBrush(QColor(0, 0, 0, 150));

    int width = size.width() + 5;
    int height = size.height() + 5;
    int startY = m_startPoint.y() - height >= 0 ? m_startPoint.y() - height : m_startPoint.y();
    painter.drawRect(m_startPoint.x(), startY, width, height);
    //设置白色文字
    painter.setPen(QColor(255, 255, 255));
    //居中画出文字
    painter.drawText(m_startPoint.x(), startY, width, height, Qt::AlignCenter, str);
    painter.restore();

}

void Screen::dataUpdate()
{
    //计算选区矩形
    m_rect.setX(m_startPoint.x());
    m_rect.setY(m_startPoint.y());
    m_rect.setWidth(m_endPoint.x() - m_startPoint.x());
    m_rect.setHeight(m_endPoint.y() - m_startPoint.y());

    constexpr int width = 2;
    constexpr int height = 2;

    //计算顶点四个小正方形
    m_rect1.setX(m_endPoint.x() - width / 2);
    m_rect1.setY(m_startPoint.y() - height / 2);
    m_rect1.setWidth(width);
    m_rect1.setHeight(height);

    m_rect2.setX(m_startPoint.x() - width / 2);
    m_rect2.setY(m_startPoint.y() - height / 2);
    m_rect2.setWidth(width);
    m_rect2.setHeight(height);

    m_rect3.setX(m_startPoint.x() - width / 2);
    m_rect3.setY(m_endPoint.y() - height / 2);
    m_rect3.setWidth(width);
    m_rect3.setHeight(height);

    m_rect4.setX(m_endPoint.x() - width / 2);
    m_rect4.setY(m_endPoint.y() - height / 2);
    m_rect4.setWidth(width);
    m_rect4.setHeight(height);

    m_rect5.setX((m_startPoint.x() + m_endPoint.x()) / 2 - width / 2);
    m_rect5.setY(m_startPoint.y() - height / 2);
    m_rect5.setWidth(width);
    m_rect5.setHeight(height);

    m_rect6.setX((m_startPoint.x() + m_endPoint.x()) / 2 - width / 2);
    m_rect6.setY(m_endPoint.y() - height / 2);
    m_rect6.setWidth(width);
    m_rect6.setHeight(height);

    m_rect7.setX(m_endPoint.x() - width / 2);
    m_rect7.setY((m_startPoint.y() + m_endPoint.y()) / 2 - height / 2);
    m_rect7.setWidth(width);
    m_rect7.setHeight(height);

    m_rect8.setX(m_startPoint.x() - width / 2);
    m_rect8.setY((m_startPoint.y() + m_endPoint.y()) / 2 - height / 2);
    m_rect8.setWidth(width);
    m_rect8.setHeight(height);
}

void Screen::mouseMoveEvent(QMouseEvent *e)
{
    m_currentPoint = e->pos();
    if(false == m_onceRelease &&  RECT == pointInWhere(e->pos()))
    {
        if(m_isPressedInSnap)
        {
            if(LINE == m_drawStyle)
            {
                myLine line;
                line.startPoint = m_lineStartPoint;
                line.endPoint = m_currentPoint;
                m_lines.push_back(line);
                m_lineStartPoint = m_currentPoint;
            }
            update();
        }
    }

    if(e->buttons() & Qt::LeftButton)   //如果左键按下
    {
        if(m_onceRelease)    //如果是第一次按下时候，此时的移动代表选区的变化
        {
            coordinateConvert(m_pressPoint, m_currentPoint);
            update();
        }
        else
        {
            int moveX = m_currentPoint.x() - m_pressPoint.x();  //鼠标移动的x距离
            int moveY = m_currentPoint.y() - m_pressPoint.y();   //鼠标移动的y距离
            switch(m_type)         //选区角落四个小矩形的位置
            {
            case RECT1:               //意思是第一次选区之后，分别拖动四个角落的小矩形时候，截图选区的变化
                if(m_records.size() > 0)
                    break;
                coordinateConvert(QPoint(m_oldStartPoint.x(), m_oldStartPoint.y() + moveY),
                                  QPoint(m_oldEndPoint.x() + moveX, m_oldEndPoint.y()));
                break;
            case RECT2:
                if(m_records.size() > 0)
                    break;
                coordinateConvert(QPoint(m_oldStartPoint.x() + moveX, m_oldStartPoint.y() + moveY),
                                  QPoint(m_oldEndPoint.x(), m_oldEndPoint.y()));
                break;
            case RECT3:
                if(m_records.size() > 0)
                    break;
                coordinateConvert(QPoint(m_oldStartPoint.x() + moveX, m_oldStartPoint.y()),
                                  QPoint(m_oldEndPoint.x(), m_oldEndPoint.y() + moveY));
                break;
            case RECT4:
                if(m_records.size() > 0)
                    break;
                coordinateConvert(QPoint(m_oldStartPoint.x(), m_oldStartPoint.y()),
                                  QPoint(m_oldEndPoint.x() + moveX, m_oldEndPoint.y() + moveY));
                break;
            case LEFT_LINE:
                if(m_records.size() > 0)
                    break;
                coordinateConvert(QPoint(m_oldStartPoint.x() + moveX, m_oldStartPoint.y()),
                                  QPoint(m_oldEndPoint.x(), m_oldEndPoint.y()));
                break;
            case RIGHT_LINE:
                if(m_records.size() > 0)
                    break;
                coordinateConvert(QPoint(m_oldStartPoint.x(), m_oldStartPoint.y()),
                                  QPoint(m_oldEndPoint.x() + moveX, m_oldEndPoint.y()));
                break;
            case UP_LINE:
                if(m_records.size() > 0)
                    break;
                coordinateConvert(QPoint(m_oldStartPoint.x(), m_oldStartPoint.y() + moveY),
                                  QPoint(m_oldEndPoint.x(), m_oldEndPoint.y()));
                break;
            case BOTTOM_LINE:
                if(m_records.size() > 0)
                    break;
                coordinateConvert(QPoint(m_oldStartPoint.x(), m_oldStartPoint.y()),
                                  QPoint(m_oldEndPoint.x(), m_oldEndPoint.y() + moveY));
                break;
            case RECT:                  //指的是当鼠标在截图选区中按下左键然后拖动时候，截图选区的整体位置变化
            {
                if(DRAW_STYLE_MAX_INT == getDrawStyle())
                {
                    moveX = m_currentPoint.x() - m_oldeMousePoint.x();
                    moveY = m_currentPoint.y() - m_oldeMousePoint.y();

                    int startX = m_startPoint.x() + moveX;
                    int startY = m_startPoint.y() + moveY;
                    int endX = m_endPoint.x() + moveX;
                    int endY = m_endPoint.y() + moveY;
                    int width = endX - startX;
                    int height = endY - startY;

                    int deskWidth = m_screenRect.width();
                    int deskHeight = m_screenRect.height();


                    if( startX <= 0)
                    {
                        startX = 0;
                        endX = width;
                    }
                    if( endX >= deskWidth)
                    {
                        endX = deskWidth;
                        startX = deskWidth - width;
                    }
                    if(startY <= 0)
                    {
                        startY = 0;
                        endY = startY + height;
                    }
                    if( endY >= deskHeight)
                    {
                        endY = deskHeight;
                        startY = endY - height;
                    }

                    m_startPoint.setX(startX);    //鼠标在截图区域中拖动结束后，选区的位置
                    m_startPoint.setY(startY);
                    m_endPoint.setX(endX);
                    m_endPoint.setY(endY);
                }
                m_oldeMousePoint = m_currentPoint;
                break;
            }
            case NO:
                break;
            default:
                break;
            }
        }

        dataUpdate();
    }
    else if(false == m_oncePress)
    {
        setMouseCursor(pointInWhere(e->pos()));
    }

    moveControl();

    update();
}

void Screen::getRecoderBorder()
{
    for(auto record :m_records)
    {

    }
}

void Screen::mouseRightClickedEvent(QMouseEvent *e)
{
    if(false == m_oncePress)
    {
        reInit();
        update();
    }
    else
    {
        std::cout << "cancel" << std::endl;
        qApp->quit();
    }
}

void Screen::mouseReleaseEvent(QMouseEvent *e)    //只有已经按下鼠标按键，才会存在鼠标释放行为，释放鼠标后会立刻出现控制面板
{
    if(e->button()==Qt::RightButton)
    {
        mouseRightClickedEvent(e);
        return;
    }
    if(true == m_onceRelease && (m_startPoint.x() == m_endPoint.x() || m_startPoint.y() == m_endPoint.y()))
    {
        reInit();
        return;
    }
    m_oldStartPoint = m_startPoint;
    m_oldEndPoint = m_endPoint;
    m_onceRelease = false;

    if(false == m_onceRelease && true == m_isPressedInSnap)
    {
        myRecord record;
        record.size = m_drawSize[m_drawStyle];
        record.color = m_drawColor[m_drawStyle];
        record.style = m_drawStyle;
        if((RECTANGLE == m_drawStyle) || (ROUND == m_drawStyle) || (ARROW == m_drawStyle))
        {
            record.shape.startPoint = m_pressPoint;

            int endX = m_currentPoint.x() <= m_endPoint.x() ? m_currentPoint.x() : m_endPoint.x();
            endX = endX >= m_startPoint.x() ? endX : m_startPoint.x();
            int endY = m_currentPoint.y() <= m_endPoint.y() ? m_currentPoint.y() : m_endPoint.y();
            endY = endY >= m_startPoint.y() ? endY : m_startPoint.y();
            record.shape.endPoint.setX(endX);
            record.shape.endPoint.setY(endY);

            m_records.push_back(record);
            m_control->getUi()->returneditBtn->setDisabled(false);
        }
        else if ((LINE == m_drawStyle))
        {
            record.lines = m_lines;
            m_records.push_back(record);
            m_control->getUi()->returneditBtn->setDisabled(false);
            m_lines.clear();
        }
        update();
    }
    m_isPressedInSnap = false;

    if(!m_control)        //如果未出现截图操作控件
    {
        m_control = new ControlWidget(this);  //新建控制窗口
        m_control->setScreenQuote(this);
        m_control->setAttribute(Qt::WA_TranslucentBackground);

        m_selectControl = new SelectWidget(this);  //新建控制窗口
        m_selectControl->setScreenQuote(this);
        m_selectControl->setAttribute(Qt::WA_TranslucentBackground);

        m_textSelectControl = new TextSelectWidget(this);  //新建控制窗口
        m_textSelectControl->setScreenQuote(this);
        m_textSelectControl->setAttribute(Qt::WA_TranslucentBackground);
    }

    moveControl();
    setWidgetTop(1);
    m_control->show();
    m_selectControl->hide();
    m_textSelectControl->hide();

    update();
}

void Screen::setWidgetTop(int toTop)
{
    if(toTop)
    {
        m_control->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        m_selectControl->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        m_textSelectControl->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    }
    else
    {
        m_control->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        m_selectControl->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        m_textSelectControl->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    }

}

void Screen::moveControl()
{
    m_arrowDirection = 0;

    int StartX = m_endPoint.x() - controlWidth >= 0 ? m_endPoint.x() - controlWidth : 0;

    int controlStartY = m_endPoint.y() + IntervelFromControlToSnap;
    if(controlStartY + controlHeight > m_screenRect.height())
    {
        controlStartY = m_startPoint.y() - IntervelFromControlToSnap - controlHeight;
    }
    if(controlStartY < 0)
    {
        controlStartY = m_startPoint.y();
    }

    m_controlStartX = StartX;
    m_controlStartY = controlStartY;

    //设置控制面板的位置
    if(m_control)
    {
        m_control->setGeometry(StartX, controlStartY, controlWidth, controlHeight);
    }

    int selectControlStartY = controlStartY - IntervelFromControlToSelectControl - controlHeight;

    if(selectControlStartY < 0)
    {
        selectControlStartY = controlStartY + controlHeight + IntervelFromControlToSelectControl;
        m_arrowDirection = 1;
    }

    if(m_selectControl)
    {
        m_selectControl->setGeometry(StartX, selectControlStartY,
                                   selectContrlWidth, controlHeight);
    }
    if(m_textSelectControl)
    {
        m_textSelectControl->setGeometry(StartX, selectControlStartY,
                                       textSelectControlWidth, controlHeight);
    }
}

void Screen::drawControlArrow(QPainter &painter , int direction)
{
    painter.save();

    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(0, 0, 0, 0xD6));
    painter.setPen(pen);
    painter.setBrush(QColor(0, 0, 0, 0XD6));

    int arrowHeight = 8;
    int arrowWidth = 14;
    constexpr int startx = 28;
    constexpr int intervel = 16;
    constexpr int buttonSize = 20;

    int style = getDrawStyle();

    QPoint point[3];
    point[0].setX(m_controlStartX + startx +
                  (buttonSize - arrowWidth) / 2 + arrowWidth / 2 +
                  (intervel + buttonSize) * style);
    point[1].setX(point[0].x() + arrowWidth / 2);
    point[2].setX((point[0].x() - arrowWidth / 2));

    if(0 == direction)
    {
        point[0].setY(m_controlStartY - IntervelFromControlToSelectControl + arrowHeight);
        point[1].setY(m_controlStartY - IntervelFromControlToSelectControl);
        point[2].setY(m_controlStartY - IntervelFromControlToSelectControl);
    }
    else if (1 == direction)
    {
        point[0].setY(m_controlStartY + controlHeight + arrowHeight);
        point[1].setY(m_controlStartY + controlHeight);
        point[2].setY(m_controlStartY + controlHeight);
    }

    painter.drawPolygon(point, 3);
    painter.restore();
}

QColor Screen::toQColor(DRAW_COLOR_E color)
{
    switch(color){
    case RED:
        return QColor(250, 0, 0, 255);
    case YELLOW:
        return QColor(255, 204, 0, 255);
    case BLUE:
        return QColor(69, 134, 255, 255);
    case GREEN:
        return QColor(116, 224, 0, 255);
    case GRAY:
        return QColor(74, 74, 74, 255);
    case WHITE:
        return QColor(255, 255, 255, 255);
    default:
        return QColor(255, 0, 0, 255);
    }
}

//计算此时鼠标移动在哪个选区上面
Type Screen::pointInWhere(QPoint p)
{
    if(pointInRect(p, m_rect1))
        return RECT1;
    else if( pointInRect(p, m_rect2))
        return RECT2;
    else if( pointInRect(p, m_rect3))
        return RECT3;
    else if( pointInRect(p, m_rect4))
        return RECT4;
    else if(p.x() >= m_startPoint.x() - 1 && p.x() <= m_startPoint.x() + 1 &&
            p.y() >= m_startPoint.y() && p.y() < m_endPoint.y())
        return LEFT_LINE;
    else if(p.x() >= m_endPoint.x() - 1 && p.x() <= m_endPoint.x() + 1 &&
            p.y() >= m_startPoint.y() && p.y() < m_endPoint.y())
        return RIGHT_LINE;
    else if(p.x() >= m_startPoint.x() && p.x() <= m_endPoint.x() &&
            p.y() >= m_startPoint.y() - 1 && p.y() < m_startPoint.y() + 1)
        return UP_LINE;
    else if(p.x() >= m_startPoint.x() && p.x() <= m_endPoint.x() &&
            p.y() >= m_endPoint.y() - 1 && p.y() < m_endPoint.y() + 1)
        return BOTTOM_LINE;
    else if( pointInRect(p, m_rect))
        return RECT;
    else
        return NO;
}

//判断点是否在矩形之内
bool Screen::pointInRect(QPoint &p, QRectF &r)
{
    if( p.x() > r.x() && p.x() < r.x() + r.width() &&
            p.y() > r.y() && p.y() < r.y() + r.height())
        return true;
    return false;
}

void Screen::keyPressEvent(QKeyEvent *e)    //键盘事件，包括esc退出截图，回车键完成截图，返回键完成截图
{
    int key = e->key();
    switch( key )
    {
    case Qt::Key_Escape :
        close();
        Exit();
        break;
    case Qt::Key_Enter:
        if( m_control )
        {
            m_control->finishBtn_slot();
        }
        break;
    case Qt::Key_Return :
        if( m_control )
        {
            m_control->finishBtn_slot();
        }
        break;
    default:
        break;
    }
}

//保存截取出来的图片
int Screen::savePixmap()
{
    setWidgetTop(0);

    //生成图片名称
    QString picName = "fetion";
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyyMMddhhmmss");
    picName += current_date;
    QPixmap pix = getGrabPixmap();
    QString filename = QFileDialog::getSaveFileName(this, QStringLiteral("保存截图"), picName, "JPEG Files(*.jpg)");

    if(filename.length() > 0)
    {
        pix.toImage().save(filename, "jpg");
        return 1;
    }

    setWidgetTop(1);
    m_control->show();
    return 0;
}

QPixmap Screen::getGrabPixmap()   //返回截到的图片
{
    return grab(QRect(QPoint(m_startPoint.x(), m_startPoint.y()), QSize(m_endPoint.x() - m_startPoint.x(), m_endPoint.y() - m_startPoint.y())));
}

void Screen::Exit()
{
}

Screen::~Screen()
{
}

DRAW_SIZE_E Screen::getDrawSize(DRAW_STYLE_E style)
{
    if(DRAW_STYLE_MAX_INT <= style)
        return SMALL;
    return m_drawSize[style];
}
int Screen::setDrawSize(DRAW_STYLE_E style, DRAW_SIZE_E size)
{
    if(DRAW_STYLE_MAX_INT <= style)
        return -1;
    if(DRAW_SIZE_MAX_INT <= size)
        size = SMALL;
    m_drawSize[style] = size;

    return 0;
}
DRAW_COLOR_E Screen::getDrawColor(DRAW_STYLE_E style)
{
    if(DRAW_STYLE_MAX_INT <= style)
        return RED;
    return m_drawColor[style];
}
int Screen::setDrawColor(DRAW_STYLE_E style, DRAW_COLOR_E color)
{
    if(DRAW_STYLE_MAX_INT <= style)
        return -1;
    if(DRAW_COLOR_MAX_INT <= color)
        color = RED;
    m_drawColor[style] = color;

    return 0;
}

QString Screen::getColorString(DRAW_COLOR_E color)
{
    switch (color)
    {
    case RED:
        return "#FA0000";
    case YELLOW:
        return "#FFCC00";
    case BLUE:
        return "#4586FF";
    case GREEN:
        return "#74E000";
    case GRAY:
        return "#4A4A4A";
    case WHITE:
        return "#FFFFFF";
    default:
        return "#FA0000";
    }
}

DRAW_STYLE_E Screen::getDrawStyle()
{
    return m_drawStyle;
}

int Screen::setDrawStyle(DRAW_STYLE_E style)
{
    if(TEXT == style && m_plaintextedit->toPlainText().size())
        saveText();
    m_plaintextedit->hide();
    m_drawStyle = style;

    return 0;
}

int Screen::cancel()
{
    if(m_records.size() > 0)
    {
        m_records.pop_back();
    }
    if(m_records.empty())
    {
        m_control->getUi()->returneditBtn->setDisabled(true);
    }

    update();
    return 0;
}

void Screen::snap()
{
    m_snap = 1;
    QPixmap pix = getGrabPixmap();
    QClipboard *board = QApplication::clipboard();
    board->setImage(pix.toImage());

    std::cout << "ok" << std::endl;

    qApp->quit();
}
