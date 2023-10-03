#ifndef RESIZEIMAGE_H
#define RESIZEIMAGE_H
#include <QDialog>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QObject>
#include "TextEditor.h"
#include <QLocale>



class ResizeImageDialog : public QDialog
{
    Q_OBJECT

    QLabel                  *m_widthLabel;
    QLabel                  *m_hightLabel;
    QDoubleSpinBox          *m_widthSpinBox;
    QDoubleSpinBox          *m_hightSpinBox;

    QPushButton             *m_okButton;
    QPushButton             *m_cancelButton;

    QHBoxLayout             *m_widthLayout;
    QHBoxLayout             *m_hightLayout;
    QHBoxLayout             *m_buttonLayout;
    QVBoxLayout             *m_generalLayout;

public:
    ResizeImageDialog(TextEditor * parent);

public slots:
    QPair<double, double> getNewSize(TextEditor * parent, double width, double height);

};

#endif // RESIZEIMAGE_H

