#include "controlwidget.h"
#include <QPoint>
#include "screen.h"
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include "iostream"

ControlWidget::ControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlWidget),
    screen(nullptr)
{
    ui->setupUi(this);
    ui->returneditBtn->setDisabled(true);
    connectSignalAndSlot();
}

Ui::ControlWidget* ControlWidget::getUi()
{
    return ui;
}

void ControlWidget::connectSignalAndSlot()
{
    connect(ui->cancelBtn, SIGNAL(clicked()), this, SLOT(cancelBtn_slot()));
    connect(ui->saveBtn, SIGNAL(clicked()), this, SLOT(saveBtn_slot()));
    connect(ui->finishBtn, SIGNAL(clicked()), this, SLOT(finishBtn_slot()));
}
//撤销截图按钮
void ControlWidget::cancelBtn_slot()
{
    if( screen )
    {
        std::cout << "cancel" << std::endl;
        screen->close();
        screen->Exit();
    }
}
//保存截图按钮
void ControlWidget::saveBtn_slot()
{
    if(1 == screen->savePixmap())
    {
        cancelBtn_slot();
    }
}

//完成按钮将截图保存到剪贴板
void ControlWidget::finishBtn_slot()
{
    screen->snap();
}

//保存Screen类的引用
void ControlWidget::setScreenQuote(Screen *screen)
{
    this->screen = screen;
}

void ControlWidget::unCheckAllButton()
{
    ui->rectangleBtn->setChecked(false);
    ui->drawRoundBtn->setChecked(false);
    ui->arrowBtn->setChecked(false);
    ui->drawLineBtn->setChecked(false);
    ui->textEditBtn->setChecked(false);
}

ControlWidget::~ControlWidget()
{
    delete ui;
}

void ControlWidget::on_drawLineBtn_clicked()
{
    unCheckAllButton();
    ui->drawLineBtn->setChecked(true);
    screen->getTextSelectControl()->hide();
    screen->getSelectControl()->show();
    screen->setDrawStyle(LINE);
    screen->update();
}

void ControlWidget::on_textEditBtn_clicked()
{
    unCheckAllButton();
    ui->textEditBtn->setChecked(true);
    screen->getSelectControl()->hide();
    screen->getTextSelectControl()->show();
    screen->setDrawStyle(TEXT);
    screen->update();
}

void ControlWidget::on_rectangleBtn_clicked()
{
    unCheckAllButton();
    ui->rectangleBtn->setChecked(true);
    screen->getTextSelectControl()->hide();
    screen->getSelectControl()->show();
    screen->setDrawStyle(RECTANGLE);
    screen->update();
}

void ControlWidget::on_drawRoundBtn_clicked()
{
    unCheckAllButton();
    ui->drawRoundBtn->setChecked(true);
    screen->getTextSelectControl()->hide();
    screen->getSelectControl()->show();
    screen->setDrawStyle(ROUND);
    screen->update();
}

void ControlWidget::on_arrowBtn_clicked()
{
    unCheckAllButton();
    ui->arrowBtn->setChecked(true);
    screen->getTextSelectControl()->hide();
    screen->getSelectControl()->show();
    screen->setDrawStyle(ARROW);
    screen->update();
}

void ControlWidget::on_returneditBtn_clicked()
{
    screen->cancel();
}

