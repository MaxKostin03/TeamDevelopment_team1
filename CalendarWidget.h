#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include <QMainWindow>
#include <QDate>
#include <QDialog>
#include <QTextBrowser>

#include "TextEditor.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class CalendarWidget : public QMainWindow
{
    Q_OBJECT

public:

    CalendarWidget(TextEditor *parent);
    QLocale *local = new QLocale;

public slots:

    void setFontSize(int size); //выбор шрифта
    void setMonth(int month); //выбор месяца
    void setYear(QDate date); //выбор года

private:

    void insertCalendar();
    int fontSize;
    QDate selectedDate;
    QTextBrowser *editor;

};

#endif // CALENDAR_WIDGET_H
