#ifndef SELECTWIDGET_H
#define SELECTWIDGET_H

#include <QObject>
#include <QWidget>
#include "ui_selectwidget.h"

class Screen;

class SelectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SelectWidget(QWidget *parent = nullptr);
    virtual  ~SelectWidget();
    void setScreenQuote(Screen *screen);
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private:
    void uncheckAllColor();
    void uncheckAllSize();
    Ui::SelectWidget *ui;
    Screen *screen;

signals:

public slots:
private slots:
    void on_pushButtonRed_clicked();
    void on_pushButtonYellow_clicked();
    void on_pushButtonBlue_clicked();
    void on_pushButtonGray_clicked();
    void on_pushButtonGreen_clicked();
    void on_pushButtonWhite_clicked();

    void on_pushButtonLarge_clicked();
    void on_pushButtonMiddle_clicked();
    void on_pushButtonSmall_clicked();
};

#endif // SELECTWIDGET_H
