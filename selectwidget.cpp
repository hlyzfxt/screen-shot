#include "selectwidget.h"
#include "screen.h"

SelectWidget::SelectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectWidget),
    screen(nullptr)
{
    ui->setupUi(this);
}

//保存Screen类的引用
void SelectWidget::setScreenQuote(Screen *screen)
{
    this->screen = screen;
}

SelectWidget::~SelectWidget()
{
    delete ui;
}

void SelectWidget::paintEvent(QPaintEvent *e)
{
    uncheckAllColor();
    uncheckAllSize();
    switch (screen->getDrawSize(screen->getDrawStyle())) {
    case SMALL:
        ui->pushButtonSmall->setChecked(true);
        break;
    case MIDDLE:
        ui->pushButtonMiddle->setChecked(true);
        break;
    case LARGE:
        ui->pushButtonLarge->setChecked(true);
        break;
    default:
        ui->pushButtonSmall->setChecked(true);
        break;
    }

    switch (screen->getDrawColor(screen->getDrawStyle()))
    {
    case RED:
        ui->pushButtonRed->setChecked(true);
        break;
    case YELLOW:
        ui->pushButtonYellow->setChecked(true);
        break;
    case BLUE:
        ui->pushButtonBlue->setChecked(true);
        break;
    case GREEN:
        ui->pushButtonGreen->setChecked(true);
        break;
    case GRAY:
        ui->pushButtonGray->setChecked(true);
        break;
    case WHITE:
        ui->pushButtonWhite->setChecked(true);
        break;
    default:
        ui->pushButtonRed->setChecked(true);
        break;
    }
}

void SelectWidget::uncheckAllColor()
{
    ui->pushButtonRed->setChecked(false);
    ui->pushButtonBlue->setChecked(false);
    ui->pushButtonGreen->setChecked(false);
    ui->pushButtonGray->setChecked(false);
    ui->pushButtonWhite->setChecked(false);
    ui->pushButtonYellow->setChecked(false);
}

void SelectWidget::on_pushButtonRed_clicked()
{
    uncheckAllColor();
    ui->pushButtonRed->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), RED);
}

void SelectWidget::on_pushButtonYellow_clicked()
{
    uncheckAllColor();
    ui->pushButtonYellow->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), YELLOW);
}

void SelectWidget::on_pushButtonGray_clicked()
{
    uncheckAllColor();
    ui->pushButtonGray->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), GRAY);
}

void SelectWidget::on_pushButtonBlue_clicked()
{
    uncheckAllColor();
    ui->pushButtonBlue->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), BLUE);
}

void SelectWidget::on_pushButtonGreen_clicked()
{
    uncheckAllColor();
    ui->pushButtonGreen->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), GREEN);
}

void SelectWidget::on_pushButtonWhite_clicked()
{
    uncheckAllColor();
    ui->pushButtonWhite->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), WHITE);
}

void SelectWidget::uncheckAllSize()
{
    ui->pushButtonSmall->setChecked(false);
    ui->pushButtonMiddle->setChecked(false);
    ui->pushButtonLarge->setChecked(false);
}

void SelectWidget::on_pushButtonLarge_clicked()
{
    uncheckAllSize();
    ui->pushButtonLarge->setChecked(true);
    screen->setDrawSize(screen->getDrawStyle(), LARGE);
}

void SelectWidget::on_pushButtonMiddle_clicked()
{
    uncheckAllSize();
    ui->pushButtonMiddle->setChecked(true);
    screen->setDrawSize(screen->getDrawStyle(), MIDDLE);
}

void SelectWidget::on_pushButtonSmall_clicked()
{
    uncheckAllSize();
    ui->pushButtonSmall->setChecked(true);
    screen->setDrawSize(screen->getDrawStyle(), SMALL);
}

void SelectWidget::mouseMoveEvent(QMouseEvent *e)
{
    setCursor(Qt::ArrowCursor);
}
