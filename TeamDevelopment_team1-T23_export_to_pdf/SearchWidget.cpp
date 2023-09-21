#include "SearchWidget.h"
#include "ui_SearchWidget.h"

SearchWidget::SearchWidget(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::SearchWidget)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Search"));
    this->setWindowIcon(QIcon(":/res/Icons-file/search"));
    connect(ui->pushButton, &QPushButton::clicked, this, &SearchWidget::onSearchText); // подключаем сигнал кнопки для вызова функции передачи сигнала с текстом поиска
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &SearchWidget::onSearchText); // подключаем сигнал нажатия Enter для вызова функции передачи сигнала с текстом поиска
}

SearchWidget::~SearchWidget()
{
    delete ui;
}

void SearchWidget::onSearchText() // функция передачи сигнала с текстом поиска
{
    emit signalSearchText(ui->lineEdit->text()); // передаем сигнал с текстом для поиска
}
