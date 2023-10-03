#include "ResizeImage.h"

ResizeImageDialog::ResizeImageDialog(TextEditor * parent)
{
    double imageWidth = 30.0;
    double imageHight = 30.0;
    QLocale *resizeImageLanguage = new QLocale;
    resizeImageLanguage=parent->loc;

    QString *imageWidthLabel = new QString;
    QString *imageHeightLabel = new QString;
    QString *imageOkButtom = new QString;
    QString *imageCancelButtom = new QString;
    QString *imageReziseImageLabel = new QString;


    if(resizeImageLanguage->languageToString(resizeImageLanguage->language())== "English"){
        *imageWidthLabel = "Image width:";
        *imageHeightLabel = "Image height:";
        *imageOkButtom = "Ok";
        *imageCancelButtom = "Cancel";
        *imageReziseImageLabel = "Resize image";
    }
    else {
        *imageWidthLabel = "Ширина изображения:";
        *imageHeightLabel = "Высота изображения:";
        *imageOkButtom = "Ок";
        *imageCancelButtom = "Отмена";
        *imageReziseImageLabel = "Изменить размер изображения";
    }


    m_widthLabel = new QLabel(*imageWidthLabel);
    m_hightLabel = new QLabel(*imageHeightLabel);

    m_widthSpinBox = new QDoubleSpinBox;
    m_widthSpinBox->setMaximum(1500);
    m_widthSpinBox->setValue(imageWidth);
    m_hightSpinBox = new QDoubleSpinBox;
    m_hightSpinBox->setMaximum(1500);
    m_hightSpinBox->setValue(imageHight);



    m_widthLayout = new QHBoxLayout;
    m_widthLayout->addWidget(m_widthLabel);
    m_widthLayout->addWidget(m_widthSpinBox);

    m_hightLayout  = new QHBoxLayout;
    m_hightLayout->addWidget(m_hightLabel);
    m_hightLayout->addWidget(m_hightSpinBox);

    m_okButton = new QPushButton(*imageOkButtom);
    connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));

    m_cancelButton = new QPushButton(*imageCancelButtom);
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    m_buttonLayout = new QHBoxLayout;
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_okButton);
    m_buttonLayout->addWidget(m_cancelButton);

    m_generalLayout = new QVBoxLayout;
    m_generalLayout->addLayout(m_widthLayout);
    m_generalLayout->addLayout(m_hightLayout);
    m_generalLayout->addLayout(m_buttonLayout);
    setLayout(m_generalLayout);

    setModal(true);
    setWindowTitle(*imageReziseImageLabel);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowIcon(QIcon(":/res/Icons-file/resize_image"));
    resize(250,100);


}


QPair<double, double> ResizeImageDialog:: getNewSize(TextEditor * parent, double width, double height)
{

    ResizeImageDialog dlg(parent);
    dlg.m_widthSpinBox->setValue(width);
    dlg.m_hightSpinBox->setValue(height);
    dlg.exec();


    QPair<double, double> size;
    size.first = dlg.m_widthSpinBox->value();
    size.second = dlg.m_hightSpinBox->value();
    return size;
}
