#ifndef SEARCH_WIDGET_H
#define SEARCH_WIDGET_H

#include <QWidget>
#include <QDialog>

#include "ui_SearchWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SearchWidget; }
QT_END_NAMESPACE

class SearchWidget : public QDialog
{
    Q_OBJECT

public:

    SearchWidget(QDialog *parent = nullptr);
    ~SearchWidget();

signals:

    void signalSearchText(QString text);

private:

    void onSearchText();
    Ui::SearchWidget *ui;

};

#endif // SEARCH_WIDGET_H
