#include <QtWidgets>
#include <QDialog>
#include "CalendarWidget.h"

CalendarWidget::CalendarWidget(TextEditor *parent) { //конструктор
    selectedDate = QDate::currentDate();
    fontSize = 10;
    local=parent->loc;
    QDialog *centralWidget = new QDialog;
    QLabel *dateLabel = new QLabel;
    QString *dateLang = new QString;
    if(local->languageToString(local->language())=="English")
        *dateLang="Date:";
    else
        *dateLang="Дата:";
    dateLabel->setText(*dateLang);
    QComboBox *monthCombo = new QComboBox;
    for (int month = 1; month <= 12; ++month)
        monthCombo->addItem(parent->loc->monthName(month,QLocale::LongFormat));
    //monthCombo->addItem(QLocale(QLocale::English).monthName(month,QLocale::LongFormat));
    QDateTimeEdit *yearEdit = new QDateTimeEdit;
    yearEdit->setDisplayFormat("yyyy");
    yearEdit->setDateRange (QDate(1918, 1, 1), QDate(9998, 1, 1));
    monthCombo->setCurrentIndex (selectedDate.month() - 1);
    yearEdit->setDate (selectedDate);
    QString *fontlabel = new QString;
    if(local->languageToString(local->language())=="English")
        *fontlabel="Font:";
    else
        *fontlabel="Шрифт:";
    QLabel *fontSizeLabel = new QLabel;
    fontSizeLabel->setText(*fontlabel);
    QSpinBox *fontSizeSpinBox = new QSpinBox;
    fontSizeSpinBox->setRange(6, 72);
    fontSizeSpinBox->setValue(10);

    editor = new QTextBrowser; //компонента для календаря
    insertCalendar(); //главный метод

    connect(monthCombo, SIGNAL(activated(int)), this, SLOT(setMonth(int)));
    connect(yearEdit, SIGNAL(dateChanged(QDate)), this, SLOT(setYear(QDate)));
    connect(fontSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setFontSize(int)));

    QHBoxLayout *horizontalLayout = new QHBoxLayout; //панель навигации
    horizontalLayout->addWidget(dateLabel);
    horizontalLayout->addWidget(monthCombo);
    horizontalLayout->addWidget(yearEdit);
    horizontalLayout->addSpacing(20);
    horizontalLayout->addWidget(fontSizeLabel);
    horizontalLayout->addWidget(fontSizeSpinBox);
    horizontalLayout->addStretch(1);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addLayout(horizontalLayout);
    centralLayout->addWidget(editor, 1);

    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);
}

void CalendarWidget::insertCalendar() {
    editor->clear(); //очистить
    QTextCursor cursor = editor->textCursor();
    cursor.beginEditBlock(); //создать курсор и поставить в начало

    QDate date(selectedDate.year(), selectedDate.month(), 1);
        //создать дату - 1-е число выбранного в панели месяца

    QTextTableFormat tableFormat; //создать формат таблицы
    tableFormat.setAlignment(Qt::AlignHCenter); //выравнивание
    tableFormat.setBackground(QColor("#EEEEEE")); //фоновый цвет
    tableFormat.setCellPadding(4); //отступ от края ячейки
    tableFormat.setCellSpacing(0); //промежуток между ячейками
    QVector <QTextLength> constraints; //7 столбцов по 15% шириной
    constraints << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14);
    tableFormat.setColumnWidthConstraints(constraints);

    QTextTable *table = cursor.insertTable(1, 7, tableFormat);
        //создать таблицу из 1 строки и 7 описанных выше столбцов

    QTextCharFormat format = cursor.charFormat(); //общий формат
    format.setFontPointSize(fontSize); //поставить размер шрифта

    QTextCharFormat boldFormat = format;
    boldFormat.setFontWeight(QFont::Bold); //жирный

    QTextCharFormat highlightedFormat = boldFormat;
    highlightedFormat.setBackground(Qt::yellow); //подсвеченный жёлтым

    QTextBlockFormat centerAlignment; //равнять по центру
    centerAlignment.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    QTextBlockFormat rightAlignment; //равнять направо
    rightAlignment.setAlignment(Qt::AlignRight);

    for (int weekDay = 0; weekDay < 7; weekDay++) { //подписи дней недели
        QTextTableCell cell = table->cellAt(0, weekDay); //в нулевой строке
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.setBlockFormat(centerAlignment);
        cellCursor.insertText(QString("%1").
                              arg(local->dayName(weekDay+1,QLocale::LongFormat)),boldFormat);
    }

    table->insertRows(table->rows(), 1); //вставить подписи

    while (date.month() == selectedDate.month()) { //для текущего месяца
        int weekDay = date.dayOfWeek(); //1=Пн, ...,  7 =Вс
        QTextTableCell cell = table->cellAt(table->rows()-1, weekDay-1);
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.setBlockFormat(rightAlignment);
        //текущую дату - желтым, иначе обычным
        if (date == QDate::currentDate())
            cellCursor.insertText(QString("%1").arg(date.day()), highlightedFormat);
        else
            cellCursor.insertText(QString("%1").arg(date.day()), format);
        date = date.addDays(1); //следующий день
        if (weekDay == 7 && date.month() == selectedDate.month())
            table->insertRows(table->rows(), 1); //добавить строчку, если пора
    }

    cursor.endEditBlock(); //завершить редактирование
    QString *calendarTitle = new QString;
    if(local->languageToString(local->language())=="English")
        *calendarTitle="Calendar";
    else
        *calendarTitle=("Календарь");
    setWindowTitle(*calendarTitle);
}

void CalendarWidget::setFontSize(int size) {
    fontSize = size;
    insertCalendar();
}

void CalendarWidget::setMonth(int month) {
    selectedDate = QDate(selectedDate.year(), month + 1, selectedDate.day());
    insertCalendar();
}

void CalendarWidget::setYear(QDate date) {
    selectedDate = QDate(date.year(), selectedDate.month(), selectedDate.day());
    insertCalendar();
}
