#include "textselectwidget.h"
#include "screen.h"
#include "QListView"

const int TEXTSELECTWIDGET_SMALL = 0;
const int TEXTSELECTWIDGET_MIDDLE = 1;
const int TEXTSELECTWIDGET_LARGE = 2;

TextSelectWidget::TextSelectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextSelectWidget),
    screen(nullptr)
{
    ui->setupUi(this);

    QListView *view = new QListView(ui->comboBox);
    view->setStyleSheet("\
                        QListView::item:hover{ \
                        background-color: rgb(229, 239, 255); \
                        color:rgb(0, 0, 0); \
                        } \
                        QListView::item:selected { \
                        background-color: rgb(189, 211, 245); \
                        color:rgb(0, 0, 0); \
                        } \
                        QListView::item { \
                        height: 29px; \
                        color:rgb(0, 0, 0); \
                        }");
    ui->comboBox->setView(view);

    ui->comboBox->setItemData(0, Qt::AlignCenter, Qt::TextAlignmentRole);
    ui->comboBox->setItemData(1, Qt::AlignCenter, Qt::TextAlignmentRole);
    ui->comboBox->setItemData(2, Qt::AlignCenter, Qt::TextAlignmentRole);

}

//保存Screen类的引用
void TextSelectWidget::setScreenQuote(Screen *screen)
{
    this->screen = screen;
}

TextSelectWidget::~TextSelectWidget()
{
    delete ui;
}

void TextSelectWidget::uncheckAllColor()
{
    ui->pushButtonRed->setChecked(false);
    ui->pushButtonBlue->setChecked(false);
    ui->pushButtonGreen->setChecked(false);
    ui->pushButtonGray->setChecked(false);
    ui->pushButtonWhite->setChecked(false);
    ui->pushButtonYellow->setChecked(false);
}

void TextSelectWidget::paintEvent(QPaintEvent *e)
{
    uncheckAllColor();

    switch (screen->getDrawSize(screen->getDrawStyle())) {
    case SMALL:
        ui->comboBox->setCurrentIndex(TEXTSELECTWIDGET_SMALL);
        break;
    case MIDDLE:
        ui->comboBox->setCurrentIndex(TEXTSELECTWIDGET_MIDDLE);
        break;
    case LARGE:
        ui->comboBox->setCurrentIndex(TEXTSELECTWIDGET_LARGE);
        break;
    default:
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

void TextSelectWidget::on_pushButtonRed_clicked()
{
    uncheckAllColor();
    ui->pushButtonRed->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), RED);
}

void TextSelectWidget::on_pushButtonYellow_clicked()
{
    uncheckAllColor();
    ui->pushButtonYellow->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), YELLOW);
}

void TextSelectWidget::on_pushButtonGray_clicked()
{
    uncheckAllColor();
    ui->pushButtonGray->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), GRAY);
}

void TextSelectWidget::on_pushButtonBlue_clicked()
{
    uncheckAllColor();
    ui->pushButtonBlue->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), BLUE);
}

void TextSelectWidget::on_pushButtonGreen_clicked()
{
    uncheckAllColor();
    ui->pushButtonGreen->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), GREEN);
}

void TextSelectWidget::on_pushButtonWhite_clicked()
{
    uncheckAllColor();
    ui->pushButtonWhite->setChecked(true);
    screen->setDrawColor(screen->getDrawStyle(), WHITE);
}

void TextSelectWidget::on_comboBox_currentIndexChanged(int index)
{
    if(index == TEXTSELECTWIDGET_SMALL)
    {
        screen->setDrawSize(screen->getDrawStyle(), SMALL);
    }
    else if(index == TEXTSELECTWIDGET_MIDDLE)
    {
        screen->setDrawSize(screen->getDrawStyle(), MIDDLE);
    }
    else if(index == TEXTSELECTWIDGET_LARGE)
    {
        screen->setDrawSize(screen->getDrawStyle(), LARGE);
    }
}
