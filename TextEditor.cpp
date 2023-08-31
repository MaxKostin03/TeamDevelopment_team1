#include "TextEditor.h"
#include "ui_TextEditor.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFontDialog>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>

TextEditor::TextEditor(QWidget *parent)
    : QMainWindow(parent), uiPtr(new Ui::TextEditor)
{
    uiPtr->setupUi(this);
    this->setWindowTitle("Text Editor");

    QFile qssFile(":/QSS/QSS-file/WordOffice.qss");   // выбрать стиль из ресурсов
    qssFile.open(QFile::ReadOnly);  // открыть файл только для чтения
    if(qssFile.isOpen())
    {
        QString qss = QLatin1String(qssFile.readAll());
        this->setStyleSheet(qss);   // если файл открылся установить данный стиль
        qssFile.close();
    }

    uiPtr->menubar->addMenu(menuConfig());  // добавление в menubar меню File
    uiPtr->menubar->addMenu(editMenu());    // добавление в menubar меню Edit
    uiPtr->menubar->addMenu(formatMenu());  // добавление в menubar меню Format
    uiPtr->menubar->addMenu(insertMenu());  // добавление в menubar меню Insert
    uiPtr->menubar->addMenu(viewMenu());    // добавление в menubar меню View

}

TextEditor::~TextEditor()
{
    delete uiPtr;
}

QMenu *TextEditor::menuConfig()     // заполнение меню File
{
    QFont font("Corbel", 10);
    QMenu *menuFilePtr = new QMenu(this);
    menuFilePtr->setFont(font);
    menuFilePtr->setTitle(tr("File"));
    menuFilePtr->addAction(tr("New"), this, &TextEditor::slotFileNew);          // кнопка вызова функции создания нового файла
    menuFilePtr->addAction(tr("Open"), this, &TextEditor::slotFileOpen);        // кнопка вызова функции открытия файла
    menuFilePtr->addAction(tr("Save"), this, &TextEditor::slotFileSave);        // кнопка вызова функции сохранения файла
    menuFilePtr->addAction(tr("Save as"), this, &TextEditor::slotFileSaveAs);   // кнопка вызова функции сохранения с новым именем файла
    menuFilePtr->addAction(tr("Print"), this, &TextEditor::slotPrintFile);      // кнопка вызова функции печати файла
    menuFilePtr->addSeparator();
    menuFilePtr->addAction(tr("Exit"), this, &TextEditor::slotExitFile);        // кнопка вызова функции выхода
    return menuFilePtr;
}

QMenu *TextEditor::editMenu()       // заполнение меню Edit
{
    QFont font("Corbel", 10);
    QMenu *menuEditPtr = new QMenu(this);
    menuEditPtr->setFont(font);
    menuEditPtr->setTitle(tr("Edit"));
    menuEditPtr->addAction(tr("Undo"), this, &TextEditor::slotUndo);            // кнопка вызова функции отмены действия
    menuEditPtr->addAction(tr("Redo"), this, &TextEditor::slotRedo);            // кнопка вызова функции повтора действия
    menuEditPtr->addAction(tr("Copy"), this, &TextEditor::slotCopy);            // кнопка вызова функции копирования
    menuEditPtr->addAction(tr("Cut"), this, &TextEditor::slotCut);              // кнопка вызова функции вырезать
    menuEditPtr->addAction(tr("Paste"), this, &TextEditor::slotPaste);          // кнопка вызова функции вставить
    menuEditPtr->addSeparator();
    menuEditPtr->addAction(tr("Select All"), this, &TextEditor::slotSelectAll); // кнопка вызова функции выделить все
    return menuEditPtr;
}

QMenu *TextEditor::formatMenu()     // заполнение меню Format
{
    QFont font("Corbel", 10);
    QMenu *menuFormatPtr = new QMenu(this);
    menuFormatPtr->setFont(font);
    menuFormatPtr->setTitle(tr("Format"));
    menuFormatPtr->addAction(tr("Bold"), this, &TextEditor::slotBold);              // кнопка вызова функции жирного шрифта
    menuFormatPtr->addAction(tr("Italic"), this, &TextEditor::slotItalic);          // кнопка вызова функции курсивного шрифта
    menuFormatPtr->addAction(tr("Underline"), this, &TextEditor::slotUnderlined);   // кнопка вызова функции подчеркнутого шрифта
    menuFormatPtr->addAction(tr("Crossed"),this, &TextEditor::slotCrossedOut);      // кнопка вызова функции зачеркнутого шрифта
    menuFormatPtr->addSeparator();
    menuFormatPtr->addAction(tr("Font style"), this, &TextEditor::slotFontStyle);   // кнопка вызова функции изменения стиля шрифта
    menuFormatPtr->addAction(tr("Font color"), this, &TextEditor::slotFontColor);   // кнопка вызова функции изменения цвета шрифта
    return menuFormatPtr;
}

QMenu *TextEditor::insertMenu()     // заполнение меню Insert
{
    QFont font("Corbel", 10);
    QMenu *menuInsertPtr = new QMenu(this);
    menuInsertPtr->setFont(font);
    menuInsertPtr->setTitle(tr("Insert"));
    menuInsertPtr->addAction(tr("Image"), this, &TextEditor::slotInsertImage);  // кнопка вызова функции добавления изображения
    return menuInsertPtr;
}

QMenu *TextEditor::viewMenu()       // заполнение меню View
{
    QFont font("Corbel", 10);
    QMenu *menuViewPtr = new QMenu(this);
    menuViewPtr->setFont(font);
    menuViewPtr->setTitle(tr("View"));
    menuViewPtr->addAction(tr("Dark mode"));
    menuViewPtr->addAction(tr("Light mode"));
    return menuViewPtr;
}

void TextEditor::slotRenameTitle(QString newName)       // функция изменения названия файла
{
    if (newName == "")
        setWindowTitle(tr("Text Editor - New document.txt*"));
    else
        setWindowTitle(tr("Text Editor - ") + newName);
}

void TextEditor::slotFileNew()      // функция создания нового файла
{
    if (hasUnsavedChanges()) {      // вызов функции проверки сохранения текущего файла
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Unsaved changes", "You have unsaved changes. Do you want to save them?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            slotFileSaveAs();               // если мы создали новый файл и выбираем сохранить изменения - сохраняем файл, область открытого текста, путь, имя файла в шапке
            uiPtr->textEdit->clear();
            file_path.clear();
            QFileInfo fileInfo(file_path);
            QString titleName = fileInfo.fileName();
            slotRenameTitle(titleName);     // вызов функции изменения названия файла
        }
        else
        {
            uiPtr->textEdit->clear();    // если мы решили не сохранять изменения - мы просто очищаем все (область текста, путь, имя)
            file_path.clear();
            QFileInfo fileInfo(file_path);
            QString titleName = fileInfo.fileName();
            slotRenameTitle(titleName);     // вызов функции изменения названия файла
        }
    }
    else
    {
        uiPtr->textEdit->clear();        // если мы открыли существующий файл, но хотим создать новый файл без каких-либо изменений - мы просто очищаем все
        file_path.clear();
        QFileInfo fileInfo(file_path);
        QString titleName = fileInfo.fileName();
        slotRenameTitle(titleName);     // вызов функции изменения названия файла
    }
}

void TextEditor::slotFileOpen()     // функция открытия файла
{
    if (hasUnsavedChanges())        // вызов функции проверки сохранения текущего файла
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Unsaved changes", "You have unsaved changes. Do you want to save them?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            slotFileSave();     // если мы открыли новый файл и выбираем сохранить изменения - сохраняем файл
        }
        else
        {
            QString file_name = QFileDialog::getOpenFileName(this, "Open the file");
            QFile file(file_name);
            file_path = file_name;
            if (!file.open(QFile::ReadOnly | QFile::Text))
            {
                QMessageBox::warning(this, "Warning", "File not opened");  // если файл не открылся - выводится сообщение
                return;
            }
            QTextStream in(&file);
            QString text = in.readAll();
            uiPtr->textEdit->setText(text);
            QFileInfo fileInfo(file_path);
            QString titleName = fileInfo.fileName();
            slotRenameTitle(titleName);     // вызов функции изменения названия файла
            file.close();
        }
    }
    QString file_name = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(file_name);
    file_path = file_name;
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "File not opened"); // если файл не открылся - выводится сообщение
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    uiPtr->textEdit->setText(text);      // вывод содержимого файла в textBrowser
    QFileInfo fileInfo(file_path);
    QString titleName = fileInfo.fileName();
    isFileSaved = true;
    slotRenameTitle(titleName);     // вызов функции изменения названия файла
    file.close();
}

void TextEditor::slotFileSave()     // функция сохранения файла
{
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        slotFileSaveAs();                   // если файл еще не существует, мы сохраняем его функцией "Save As", если он уже существует, просто сохраняем изменения
    }
    QFileInfo fileInfo(file_path);
    QString titleName = fileInfo.fileName();
    slotRenameTitle(titleName);     // вызов функции изменения названия файла
    QTextStream out(&file);
    QString text = uiPtr->textEdit->toHtml(); // чтобы сохранить форматирование и изображения, мы меняем "toPlainText" на "toHtml"
    out << text;
    file.flush();
    file.close();
    isFileSaved = true;
}

void TextEditor::slotFileSaveAs()       //функция сохранения с новым именем файла
{
    QString file_name = QFileDialog::getSaveFileName(this, "Save the file", "", "Text Files (*.txt)");  // сохраняет в формате txt
    QFile file(file_name);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot save the file"); // если файл не сохранен - выводится сообщение
        return;
    }
    file_path = file_name;
    QFileInfo fileInfo(file_path);
    QString titleName = fileInfo.fileName();
    slotRenameTitle(titleName);     // вызов функции изменения названия файла
    QTextStream out(&file);
    QString text = uiPtr->textEdit->toHtml(); // чтобы сохранить форматирование и изображения, мы меняем "toPlainText" на "toHtml"
    out << text;
    file.flush();
    file.close();
    isFileSaved = true;
}

void TextEditor::slotPrintFile()        // функция печати файла
{
    QPrinter printer;
    QPrintDialog dlg(&printer, this);
    dlg.setWindowTitle("Print");
    if (dlg.exec() != QDialog::Accepted)
    {
        return;     // если отмена печати - выйти из функции
    }
    uiPtr->textEdit->print(&printer);   // отправка на печать
}

void TextEditor::slotExitFile()     // функция выхода
{
    if (hasUnsavedChanges())        // вызов функции проверки сохранения текущего файла
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Unsaved changes", "You have unsaved changes. Do you really want to save them?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
        {
            QApplication::exit();       // если мы выбираем не сохранять изменения - выход
        }
        else
        {
            slotFileSave();     // если мы выбираем сохранить изменения - сохраняем файл
            QApplication::exit();
        }
    }
    else
    {
        QApplication::exit();       // если текущий файл сохранен или пуст - выход
    }
}

void TextEditor::closeEvent(QCloseEvent *event)     // функция выхода по крестику
{
    event->ignore();
    slotExitFile();     // вызов функции выхода
}

void TextEditor::slotUndo()     // функция отмены действия
{
    uiPtr->textEdit->undo();
}

void TextEditor::slotRedo()     // функция повтора действия
{
    uiPtr->textEdit->redo();
}

void TextEditor::slotCopy()     // функция копирования
{
    uiPtr->textEdit->copy();
}

void TextEditor::slotCut()      // функция вырезать
{
    uiPtr->textEdit->cut();
}

void TextEditor::slotPaste()        // функция вставить
{
    uiPtr->textEdit->paste();
}

void TextEditor::slotSelectAll()        // функция выделить все
{
    uiPtr->textEdit->selectAll();
}

void TextEditor::slotBold()     // функция жирного шрифта
{
    if( uiPtr->textEdit->fontWeight() == QFont::Normal) {   // если текст не жирным шрифтом - делаем жирным
        uiPtr->textEdit->setFontWeight(QFont::Bold);
    }
    else{                                                   // если текст жирным шрифтом - делаем не жирным
        uiPtr->textEdit->setFontWeight(QFont::Normal);
    }
}

void TextEditor::slotItalic()       // функциия курсивного шрифта
{
    if(uiPtr->textEdit->fontItalic() == false) {   // если текст не курсивным шрифтом - делаем курсив
        uiPtr->textEdit->setFontItalic(true);
    }
    else{                                           // если текст курсивным шрифтом - делаем не курсивным
        uiPtr->textEdit->setFontItalic(false);
    }
}

void TextEditor::slotUnderlined()       // функция подчеркнутого шрифта
{
    if( uiPtr->textEdit->fontUnderline() == false) {    // если текст не подчерунцтым шрифтом - делаем подчеркнутым
        uiPtr->textEdit->setFontUnderline(true);
    }
    else{                                               // если текст подчерунцтым шрифтом - делаем не подчеркнутым
        uiPtr->textEdit->setFontUnderline(false);
    }
}

void TextEditor::slotCrossedOut()       // функция зачеркинутого шрифта
{
    QFont font_ = uiPtr->textEdit->currentFont();

    if( uiPtr->textEdit->currentFont().strikeOut() == false) {  // если текст не зачеркнутым шрифтом - делаем зачеркнутым
        font_.setStrikeOut(true);
        uiPtr->textEdit->setCurrentFont(font_);
    }
    else{                                                       // если текст зачеркнутым шрифтом - делаем не зачеркнутым
        font_.setStrikeOut(false);
        uiPtr->textEdit->setCurrentFont(font_);
    }

}

void TextEditor::slotFontStyle()        // функция изменения стиля шрифта
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);   // вызов программы изменения стиля шрифта
    if (ok)
    {
        uiPtr->textEdit->setCurrentFont(font);      // если ОК - применить стиль
    }
    else return;                                    // если Cansel - выйти
}

void TextEditor::slotFontColor()        // функция изменения цвета шрифта
{

    QPoint Pos =mapFromGlobal(QCursor::pos());
    createColorPalette(Pos.x() , Pos.y()-(uiPtr->toolBar->height()));   // вызов функции выбора цветовой палитры

}


void TextEditor::slotInsertImage()      // функция добавления изображения
{
    QString file_path = QFileDialog::getOpenFileName(this, "Open the file");
    if (file_path.isEmpty())
    {
        return;             // если отменить вставку изображения - выход из функции
    }

    QTextImageFormat img_fmt;
    img_fmt.setName(file_path);
    img_fmt.setHeight(30);  // задание начальных размеров изображения
    img_fmt.setWidth(30);   //
    uiPtr->textEdit->textCursor().insertImage(img_fmt); // вставка изображения
}

bool TextEditor::hasUnsavedChanges()        // функция проверки сохранения текущего файла
{
    if(uiPtr->textEdit->toPlainText().length() > 0 && file_path.isEmpty()) {
        return true;
    }


    QFile file(file_path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }

    QTextStream in(&file);
    QString fileContent = in.readAll();
    QString textContent = uiPtr->textEdit->toHtml();

    return (textContent != fileContent);
}

void TextEditor::setPaletteColors(){        // функция установки цвета отображаемых кнопок

    redColorButton->setStyleSheet("background:red;");
    orangeColorButton->setStyleSheet("background:orange;");
    yellowColorButton->setStyleSheet("background:yellow;");
    greenColorButton->setStyleSheet("background:green;");
    whiteColorButton->setStyleSheet("background:white;");
    blueColorButton->setStyleSheet("background:blue;");
    purpleColorButton->setStyleSheet("background:purple;");
    blackColorButton->setStyleSheet("background:black;");


}

void TextEditor::onRedColorButtonClicked()      // функция установки красного цвета шрифта
{
    uiPtr->textEdit->setTextColor(Qt::red);
    hidePalette(window);    // вызов функции скрытия окна кнопок цветовой палитры
}


void TextEditor::onOrangeColorButtonClicked()       // функция установки оранжевого цвета шрифта
{
    uiPtr->textEdit->setTextColor(QColorConstants::Svg::orange);
    hidePalette(window);    // вызов функции скрытия окна кнопок цветовой палитры
}


void TextEditor::onYellowColorButtonClicked()       // функция установки желтого цвета шрифта
{
    uiPtr->textEdit->setTextColor(Qt::yellow);
    hidePalette(window);    // вызов функции скрытия окна кнопок цветовой палитры
}


void TextEditor::onGreenColorButtonClicked()        // функция установки зеленого цвета шрифта
{
    uiPtr->textEdit->setTextColor(Qt::green);
    hidePalette(window);    // вызов функции скрытия окна кнопок цветовой палитры
}


void TextEditor::onWhiteColorButtonClicked()        // функция установки белого цвета шрифта
{
    uiPtr->textEdit->setTextColor(Qt::white);
    hidePalette(window);    // вызов функции скрытия окна кнопок цветовой палитры
}


void TextEditor::onBlueColorButtonClicked()     // функция установки синего цвета шрифта
{
    uiPtr->textEdit->setTextColor(Qt::blue);
    hidePalette(window);    // вызов функции скрытия окна кнопок цветовой палитры
}


void TextEditor::onPurpleColorButtonClicked()       // функция установки пурпурного цвета шрифта
{
    uiPtr->textEdit->setTextColor(QColorConstants::Svg::purple);
    hidePalette(window);    // вызов функции скрытия окна кнопок цветовой палитры
}


void TextEditor::onBlackColorButtonClicked()        // функция установки черного цвета шрифта
{
    uiPtr->textEdit->setTextColor(Qt::black);
    hidePalette(window);    // вызов функции скрытия окна кнопок цветовой палитры
}

void TextEditor::createColorPalette(qint32 x ,qint32 y , qint32 height , qint32 width){     // функция выбора цветовой палитры

    if(window == NULL)  window = new QWidget(uiPtr->centralwidget);     // создание нового окна для кнопок цветовой палитры
    window->setMaximumSize(height,width);
    window->setGeometry(QRect(x,y,height,width));                       // установка положения и размеров окна для кнопок цветовой палитры

    redColorButton = new QPushButton(this);
    QObject::connect(redColorButton,&QPushButton::clicked, this, &TextEditor::onRedColorButtonClicked);         // кнопка вызова функции красного цвета шрифта

    orangeColorButton= new QPushButton(this);
    QObject::connect(orangeColorButton,&QPushButton::clicked, this, &TextEditor::onOrangeColorButtonClicked);   // кнопка вызова функции оранжевого цвета шрифта

    yellowColorButton= new QPushButton(this);
    QObject::connect(yellowColorButton,&QPushButton::clicked, this, &TextEditor::onYellowColorButtonClicked);   // кнопка вызова функции желтого цвета шрифта

    greenColorButton= new QPushButton(this);
    QObject::connect(greenColorButton,&QPushButton::clicked, this, &TextEditor::onGreenColorButtonClicked);     // кнопка вызова функции зеленого цвета шрифта

    whiteColorButton= new QPushButton(this);
    QObject::connect(whiteColorButton,&QPushButton::clicked, this, &TextEditor::onWhiteColorButtonClicked);     // кнопка вызова функции белого цвета шрифта

    blueColorButton= new QPushButton(this);
    QObject::connect(blueColorButton,&QPushButton::clicked, this, &TextEditor::onBlueColorButtonClicked);       // кнопка вызова функции синего цвета шрифта

    purpleColorButton= new QPushButton(this);
    QObject::connect(purpleColorButton,&QPushButton::clicked, this, &TextEditor::onPurpleColorButtonClicked);   // кнопка вызова функции пурпурного цвета шрифта

    blackColorButton= new QPushButton(this);
    QObject::connect(blackColorButton,&QPushButton::clicked, this, &TextEditor::onBlackColorButtonClicked);     // кнопка вызова функции черного цвета шрифта


    gridGroupBox = new QGroupBox(tr("Palette"),window);     // создание группы для обьединения кнопок окна цветовой палитры
    colorPalette = new QGridLayout;                         // создание макета сетки для группировки кнопок окна цветовой палитры
    gridGroupBox->setLayout(colorPalette);                  // добавление макета сетки в группу

    // расстановка кнопок цветовой палитры по сетке
    colorPalette->addWidget(redColorButton,0,0,Qt::AlignCenter);
    colorPalette->addWidget(orangeColorButton,0,1,Qt::AlignCenter);
    colorPalette->addWidget(yellowColorButton,0,2,Qt::AlignCenter);
    colorPalette->addWidget(greenColorButton,0,3,Qt::AlignCenter);
    colorPalette->addWidget(whiteColorButton,1,0,Qt::AlignCenter);
    colorPalette->addWidget(blueColorButton,1,1,Qt::AlignCenter);
    colorPalette->addWidget(purpleColorButton,1,2,Qt::AlignCenter);
    colorPalette->addWidget(blackColorButton,1,3,Qt::AlignCenter);

    setPaletteColors();     // вызов функции установки цвета отображаемых кнопок

    if(window->isVisible()){
        hidePalette(window);    // вызов функции скрытия окна кнопок цветовой палитры
    }
    else{
        showPalette(window);    // вызов функции показа окна кнопок цветовой палитры
    }


}

void TextEditor::hidePalette(QWidget *window){      // функция скрытия окна кнопок цветовой палитры
    if(window !=NULL && window->isVisible())  window->hide();   // если окно видимо - скрыть
}
void TextEditor::showPalette(QWidget *window){      // функция показа окна кнопок цветовой палитры
    if(window !=NULL) window->show();       // показать окно
}
