#ifndef FORMULAWIDGET_H
#define FORMULAWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QRegularExpression>

class FormulaWidget : public QDialog
{
    Q_OBJECT

public:
    FormulaWidget(QWidget *parent = nullptr);
    ~FormulaWidget();

    QString insertFormula();

private slots:
    void acceptFormula();

private:
    QLineEdit *formulaLineEdit;
    QString formatFormula(const QString &inputFormula);
};
#endif // FORMULAWIDGET_H
