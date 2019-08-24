//#include "qslabel.h"
//#include"controlwidget.h"
//#include<QPalette>
//#include"screen.h"
//#include "math.h"
//using namespace Qt;

//const int textEditLargeDefaultHeight = 120;
//const int textEditLargeDefaultWidth  = 48;

//const int textEditMiddleDefaultHeight = 98;
//const int textEditMiddleDefaultWidth  = 36;

//const int textEditSmallDefaultHeight = 66;
//const int textEditSmallDefaultWidth  = 30;

//QSLabel::QSLabel(QWidget* parent)
//{
//    setAttribute(Qt::WA_TranslucentBackground, true);
//    const QColor default_color = QColor(255,30,16, 1);

//    setMouseTracking(true);
//    isPressedInSnap=false;
//    drawStyle = DRAW_STYLE_MAX_INT;
//    m_plaintextedit=new QTextEdit(this);
//    m_plaintextedit->hide();
//    m_plaintextedit->resize(textEditMiddleDefaultHeight, textEditMiddleDefaultWidth);
//    QPalette p1=m_plaintextedit->palette();
//    p1.setBrush(QPalette::Base,QBrush(default_color));
//    m_plaintextedit->setPalette(p1);
//    connect(m_plaintextedit,SIGNAL(textChanged()), this, SLOT(onTextChanged()));
//    m_plaintextedit->setStyleSheet("QTextEdit{ border: 1px solid #dadada; }"
//                                   "QTextEdit{font-family:'Microsoft YaHei'; font-size:14px;color:#ff0000;}"
//                                   ""
//                                   );
//    m_plaintextedit->setWordWrapMode(QTextOption::WordWrap);
//}

//void QSLabel::onTextChanged()
//{
//    if(m_plaintextedit->toPlainText().size()<10)
//    {
//        m_plaintextedit->resize(m_plaintextedit->toPlainText().size()*10+50,40);
//    }
//    else
//    {
//        m_plaintextedit->resize(m_plaintextedit->toPlainText().size()*15+20,m_plaintextedit->document()->size().rheight()+10);

//    }
//}

//void QSLabel::mouseMoveEvent(QMouseEvent *event)
//{
//    if(DRAW_STYLE_MAX_INT == drawStyle)
//    {
//        setCursor(Qt::SizeAllCursor);
//    }
//    else
//    {
//        setCursor(Qt::CrossCursor);
//    }
//    if(isPressedInSnap)
//    {
//        if(LINE == drawStyle)
//        {
//            endPoint = mapFromGlobal(event->pos());
//            myLine line;
//            line.startPoint=startPoint;
//            line.endPoint=endPoint;
//            lines.push_back(line);
//            startPoint=endPoint;
//        }
//        else if(RECTANGLE == drawStyle || ROUND == drawStyle || ARROW == drawStyle)
//        {
//            endPoint = mapFromGlobal(event->pos());
//        }
//        update();
//    }
//}

//void QSLabel::mousePressEvent(QMouseEvent *event)
//{
//    printf("file:%s, line:%d\n", __FILE__, __LINE__);
//    startPoint = mapFromGlobal(event->pos());
//    endPoint = mapFromGlobal(event->pos());
//    isPressedInSnap = true;
//    if(TEXT == drawStyle)
//    {
//        if(m_plaintextedit->toPlainText().size())
//        {
//            myRecord record;
//            record.size = drawSize[drawStyle];
//            record.color = drawColor[drawStyle];
//            record.style = drawStyle;
//            record.text.mRect = QRect(QPoint(m_plaintextedit->x(),m_plaintextedit->y()),
//                                      QSize(m_plaintextedit->width(),m_plaintextedit->height()));
//            record.text.mText = m_plaintextedit->toPlainText();
//            records.push_back(record);
//            update();
//        }
//        m_plaintextedit->move(startPoint);
//        m_plaintextedit->show();
//        m_plaintextedit->clear();
//    }
//}

//void QSLabel::mouseReleaseEvent(QMouseEvent *event)
//{
//    if(false == isPressedInSnap)
//        return;

//    isPressedInSnap=false;
//    myRecord record;
//    record.size = drawSize[drawStyle];
//    record.color = drawColor[drawStyle];
//    record.style = drawStyle;
//    if((RECTANGLE == drawStyle) || (ROUND == drawStyle) || (ARROW == drawStyle))
//    {
//        endPoint = mapFromGlobal(event->pos());
//        record.shape.startPoint = startPoint;
//        record.shape.endPoint = endPoint;
//        records.push_back(record);
//        update();
//    }
//    else if ((LINE == drawStyle))
//    {
//        record.lines = lines;
//        records.push_back(record);
//        lines.clear();
//        update();
//    }
//}

//QColor QSLabel::toQColor(DRAW_COLOR_E color)
//{
//    switch(color){
//    case RED:
//        return QColor(250, 0, 0, 255);
//    case YELLOW:
//        return QColor(255, 204, 0, 255);
//    case BLUE:
//        return QColor(69, 134, 255, 255);
//    case GREEN:
//        return QColor(116, 224, 0, 255);
//    case GRAY:
//        return QColor(74, 74, 74, 255);
//    case WHITE:
//        return QColor(255, 255, 255, 255);
//    default:
//        return QColor(255, 0, 0, 255);
//    }
//}

//void QSLabel::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);

//    QPen pen;
//    pen.setColor(toQColor(RED));
//    painter.setPen(pen);

//    for(auto record : records)
//    {
//        pen.setColor(toQColor(record.color));
//        pen.setWidth(record.size);
//        painter.setPen(pen);
//        switch (record.style) {
//        case LINE:
//        {
//            for(auto line : record.lines)
//                painter.drawLine(line.startPoint, line.endPoint);
//            break;
//        }
//        case RECTANGLE:
//        {
//            int x1 = record.shape.startPoint.x() < record.shape.endPoint.x() ?
//                        record.shape.startPoint.x() : record.shape.endPoint.x();
//            int y1 = record.shape.startPoint.y() < record.shape.endPoint.y() ?
//                        record.shape.startPoint.y() : record.shape.endPoint.y();
//            painter.drawRect(x1, y1, abs(record.shape.endPoint.x() - record.shape.startPoint.x()),
//                             abs(record.shape.endPoint.y() - record.shape.startPoint.y()));
//            break;
//        }
//        case ROUND:
//        {
//            int x2 = record.shape.startPoint.x() < record.shape.endPoint.x() ?
//                        record.shape.startPoint.x() : record.shape.endPoint.x();
//            int y2 = record.shape.startPoint.y() < record.shape.endPoint.y() ?
//                        record.shape.startPoint.y() : record.shape.endPoint.y();
//            painter.drawEllipse(x2, y2, abs(record.shape.endPoint.x() - record.shape.startPoint.x()),
//                                abs(record.shape.endPoint.y() - record.shape.startPoint.y()));
//            break;
//        }
//        case ARROW:
//        {
//            drawarrow(record.shape.startPoint, record.shape.endPoint, painter);
//            break;
//        }
//        case TEXT:
//        {
//            painter.drawText(record.text.mRect, Qt::TextWrapAnywhere, record.text.mText);
//            break;
//        }
//        default:
//            break;
//        }
//    }

//    pen.setColor(toQColor(drawColor[drawStyle]));
//    pen.setWidth(drawSize[drawStyle]);
//    painter.setPen(pen);

//    int xx = startPoint.x() < endPoint.x() ? startPoint.x() : endPoint.x();
//    int yy = startPoint.y() < endPoint.y() ? startPoint.y() : endPoint.y();
//    if (RECTANGLE == drawStyle)
//    {
//        painter.drawRect(xx, yy, abs(endPoint.x() - startPoint.x()), abs(endPoint.y() - startPoint.y()));
//    }
//    else if (ROUND == drawStyle)
//    {
//        painter.drawEllipse(xx, yy, abs(endPoint.x() - startPoint.x()), abs(endPoint.y() - startPoint.y()));
//    }
//    else if(ARROW == drawStyle)
//    {
//        drawarrow(startPoint, endPoint, painter);
//    }
//    else if(LINE == drawStyle)
//    {
//        for(auto line : lines)
//            painter.drawLine(line.startPoint, line.endPoint);
//    }
//}

//int QSLabel::cancel()
//{
//    if(records.size() > 0)
//    {
//        records.pop_back();
//    }

//    update();
//    return 0;
//}

//void QSLabel::drawarrow(QPoint startpoint, QPoint endpoint, QPainter &p)
//{
//    double x_len = startpoint.x() - endpoint.x();
//    double y_len = startpoint.y() - endpoint.y();
//    int totalLen = static_cast<int>(sqrt(pow(x_len, 2) + pow(y_len, 2)));
//    int arrowLen = 12;
//    printf("x_len:%f, y_len:%f\n", x_len, y_len);
//    printf("x*x:%f, y*y:%f, total:%d\n", pow(x_len, 2), pow(y_len, 2), totalLen);

//    switch (drawSize[drawStyle]) {
//    case SMALL:
//        arrowLen = 12;
//        break;
//    case MIDDLE:
//        if(30 < totalLen)
//            arrowLen = 12;
//        else if(48 < totalLen)
//            arrowLen = 12 + (totalLen - 30) * (18 - 12) / (48 -30);
//        else
//            arrowLen = 18;
//        break;
//    case LARGE:
//        if(36 < totalLen)
//            arrowLen = 12;
//        else if(90 < totalLen)
//            arrowLen = 12 + (totalLen - 30) * (30 - 12) / (90 - 30);
//        else
//            arrowLen = 30;
//        break;
//    default:
//        arrowLen = 12;
//    };

//    double par= arrowLen;
//    double slopy=atan2((endpoint.y()-startpoint.y()),(endpoint.x()-startpoint.x()));
//    double cosy=cos(slopy);
//    double siny=sin(slopy);
//    QPoint point1 = QPoint(endpoint.x() + int(-par*cosy - (par / 2.0*siny)), endpoint.y() + int(-par*siny + (par / 2.0*cosy)));
//    QPoint point2 = QPoint(endpoint.x() + int(-par*cosy + (par / 2.0*siny)), endpoint.y() - int(par / 2.0*cosy + par*siny));
//    QPoint points[3]={endpoint,point1,point2};
//    p.setRenderHint(QPainter::Antialiasing,true);
//    QPen drawtrianglepen;
//    drawtrianglepen.setColor(Qt::red);
//    drawtrianglepen.setWidth(1);
//    p.setPen(drawtrianglepen);
//    p.drawPolygon(points,3);
//    int offsetx=int(par*siny/3);
//    int offsety=int(par*cosy/3);
//    QPoint point3,point4;
//    point3 = QPoint(endpoint.x() + int(-par*cosy - (par / 2.0*siny)) + offsetx, endpoint.y() + int(-par*siny + (par / 2.0*cosy)) - offsety);
//    point4 = QPoint(endpoint.x() + int(-par*cosy + (par / 2.0*siny) - offsetx), endpoint.y() - int(par / 2.0*cosy + par*siny) + offsety);
//    QPoint arrbodypoints[3]={startpoint,point3,point4};
//    p.drawPolygon(arrbodypoints,3);

//    //QPoint points[6] = {startpoint, endpoint};

//}

//void QSLabel::setimagetolabel(const QImage &image)
//{
//    selectimage=image;

//    endPoint=QPoint(0,0);
//    startPoint=QPoint(0,0);
//}

//int QSLabel::setDrawStyle(DRAW_STYLE_E style)
//{
//    if(TEXT == drawStyle && m_plaintextedit->toPlainText().size())
//        saveText();
//    drawStyle = style;
//    m_plaintextedit->hide();

//    return 0;
//}

//void QSLabel::saveText()
//{
//    myRecord record;
//    record.size = drawSize[drawStyle];
//    record.color = drawColor[drawStyle];
//    record.style = drawStyle;
//    record.text.mRect = QRect(QPoint(m_plaintextedit->x(),m_plaintextedit->y()),
//                              QSize(m_plaintextedit->width(),m_plaintextedit->height()));
//    record.text.mText = m_plaintextedit->toPlainText();
//    records.push_back(record);
//    update();
//}

//QImage QSLabel::resultimage()
//{
//    return QImage(QPixmap::grabWidget(this).toImage());
//}


