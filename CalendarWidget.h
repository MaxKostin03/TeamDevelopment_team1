#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QMainWindow>
#include <QDate>
#include <Qdialog>
#include <TextEditor.h>

class QTextBrowser;

class CalendarWidget : public QMainWindow {
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

#endif
