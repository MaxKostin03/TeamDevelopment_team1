#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include <QDialog>

namespace Ui {
class SearchWidget;
}

class SearchWidget : public QDialog
{
    Q_OBJECT

public:
    explicit SearchWidget(QDialog *parent = nullptr);
    ~SearchWidget();

signals:
    void signalSearchText(QString text);

private:
    void onSearchText();

    Ui::SearchWidget *ui;

};

#endif // SEARCHWIDGET_H
