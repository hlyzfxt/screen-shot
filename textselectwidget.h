#ifndef TEXTSELECTWIDGET_H
#define TEXTSELECTWIDGET_H

#include <QObject>
#include <QWidget>
#include "ui_textselectwidget.h"

class Screen;

class TextSelectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TextSelectWidget(QWidget *parent = nullptr);
    virtual ~TextSelectWidget();
    void setScreenQuote(Screen *screen);
    void paintEvent(QPaintEvent *e);

private:
    void uncheckAllColor();
    Ui::TextSelectWidget *ui;
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
    void on_comboBox_currentIndexChanged(int index);
};

#endif // TEXTSELECTWIDGET_H
