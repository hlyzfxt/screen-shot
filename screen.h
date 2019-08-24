#ifndef SCREEN_H
#define SCREEN_H
#include <QWidget>
#include "controlwidget.h"
#include "selectwidget.h"
#include "textselectwidget.h"
#include <QPlainTextEdit>
#include <QTextEdit>

typedef enum Type
{
    RECT1,
    RECT2,
    RECT3,
    RECT4,
    LEFT_LINE,
    UP_LINE,
    RIGHT_LINE,
    BOTTOM_LINE,
    RECT,
    NO
}MOUSE_POSITION;

typedef enum DrawSize
{
    SMALL =  1,
    MIDDLE = 3,
    LARGE =  5,

    DRAW_SIZE_MAX_INT
}DRAW_SIZE_E;

typedef enum DrawColor
{
    RED,
    YELLOW,
    BLUE,
    GREEN,
    GRAY,
    WHITE,

    DRAW_COLOR_MAX_INT
}DRAW_COLOR_E;

typedef enum DrawStyle
{
    RECTANGLE,
    ROUND,
    ARROW,
    LINE,
    TEXT,

    DRAW_STYLE_MAX_INT
}DRAW_STYLE_E;

class Screen;

typedef struct myLine{
    QPoint startPoint;
    QPoint endPoint;
}myLine;

typedef struct myRegularShape{
    QPoint startPoint;
    QPoint endPoint;
}myRegularShape;

typedef struct myRectangle{
    QPoint startPoint;
    QPoint endPoint;
}myRectangle;

typedef struct myRound{
    QPoint startPoint;
    QPoint endPoint;
}myRound;
typedef struct myArrow{
    QPoint startPoint;
    QPoint endPoint;
}myArrow;
typedef struct myText{
    QString mText;
    QRect mRect;
}myText;

typedef struct myRecord{
    DRAW_COLOR_E color;
    DRAW_SIZE_E size;
    DRAW_STYLE_E style;

    myLine line;
    QVector<myLine> lines;
    myRegularShape shape;
    myText text;
}myRecord;


//class QSLabel;
class ControlWidget;

class Screen : public QWidget
{
    Q_OBJECT
public:
    explicit Screen(QWidget *parent = nullptr);
    virtual ~Screen();

    DRAW_SIZE_E getDrawSize(DRAW_STYLE_E style);
    int setDrawSize(DRAW_STYLE_E style, DRAW_SIZE_E size);
    DRAW_COLOR_E getDrawColor(DRAW_STYLE_E style);
    int setDrawColor(DRAW_STYLE_E style, DRAW_COLOR_E color);
    DRAW_STYLE_E getDrawStyle();
    int setDrawStyle(DRAW_STYLE_E style);

    QWidget *getSelectControl() { return m_selectControl; }
    QWidget *getTextSelectControl() { return m_textSelectControl; }

    int cancel();
    void snap();
    int savePixmap();

    void Exit();

private:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *e);
    void mouseRightClickedEvent(QMouseEvent *e);

    void reInit();

    Type pointInWhere(QPoint p);
    //返回截取之后的图片
    QPixmap getGrabPixmap();

    QPainterPath getPath();

    int getDefaultEditWidth(DRAW_SIZE_E size);
    int getDefaultEditHeight(DRAW_SIZE_E size);

    int coordinateConvert(QPoint pressed, QPoint move);
    QColor toQColor(DRAW_COLOR_E color);

    void drawLabel(QPainter &painter);
    void drawControlArea(QPainter &);
    void drawLabelRecord(QPainter &painter);
    void drawLabelCurrent(QPainter &painter);
    void drawArrow(QPainter &painter, QPoint start, QPoint end, DRAW_COLOR_E color, DRAW_SIZE_E size);
    void drawCompleteArrow(QPainter &painter, QPoint start, QPoint end, float width);
    void drawHorizontalArrow(QPainter &painter, QPoint start, QPoint end, float width);
    void drawVerticalArrow(QPainter &painter, QPoint start, QPoint end, float width);
    void drawIncompleteArrow(QPainter &painter, QPoint start, QPoint end, float width);
    void drawControlArrow(QPainter &painter,  int direction);
    void drawSizeTips(QPainter &painter);
    void drawMagnifier(QPainter &painter);

    void setMouseCursor(MOUSE_POSITION position);
    void dataUpdate();
    void getRecoderBorder();


    void moveControl();
    int getFontPixelSize(DRAW_SIZE_E size);
    void setTextEditSizeToDefault();
    QString getColorString(DRAW_COLOR_E color);
    float getArrowWidth(double arrowLength,DRAW_SIZE_E size);
    void saveText();
    void setWidgetTop(int toTop);
    bool   pointInRect(QPoint &p, QRectF &r);

public slots:
    void onTextChanged();

private:
    int m_arrowDirection; //0向上，1向下
    int m_controlStartX;
    int m_controlStartY;
    QPixmap m_pixmap;             //全屏
    QPoint m_pressPoint;          //鼠标左键按下后的坐标
    QPoint m_currentPoint;        //鼠标现在的坐标
    QPoint m_oldeMousePoint;      //鼠标之前的位置

    int m_snap;

    QPoint m_startPoint;          //截图窗口矩形的左上角坐标
    QPoint m_endPoint;            //截图窗口矩形的右下角坐标

    QPoint m_oldStartPoint;       //截图窗口矩形的左上角坐标
    QPoint m_oldEndPoint;         //截图窗口矩形的右下角坐标

    QPainterPath m_globalPath;    //保存全局路径
    bool         m_oncePress;       //是不是第一次左键按下
    bool         m_onceRelease;     //是不是第一次左键释放

    //控制选区变化的正方形
    QRectF       m_rect1;           //第一象限的正方形
    QRectF       m_rect2;           //第二象限的正方形
    QRectF       m_rect3;           //第三象限的正方形
    QRectF       m_rect4;           //第四象限的正方形
    QRectF       m_rect5;           //第四象限的正方形
    QRectF       m_rect6;           //第四象限的正方形
    QRectF       m_rect7;           //第四象限的正方形
    QRectF       m_rect8;           //第四象限的正方形

    QRectF       m_rect;            //代表选区的正方形
    Type         m_type;

    QPoint m_lineStartPoint;

    ControlWidget *m_control;
    SelectWidget *m_selectControl;
    TextSelectWidget *m_textSelectControl;

    DRAW_STYLE_E m_drawStyle;
    DRAW_SIZE_E m_drawSize[DRAW_STYLE_MAX_INT];
    DRAW_COLOR_E m_drawColor[DRAW_STYLE_MAX_INT];

    QVector<myLine> m_lines;
    QVector<myRecord> m_records;

    QTextEdit  *m_plaintextedit;

    bool m_isPressedInSnap;
    QRect m_screenRect;
};

#endif // SCREEN_H
