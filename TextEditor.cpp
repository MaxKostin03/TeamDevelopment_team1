#include "TextEditor.h"
#include "ui_TextEditor.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QCloseEvent>

TextEditor::TextEditor(QWidget *parent)
    : QMainWindow(parent), uiPtr(new Ui::TextEditor)
{
    uiPtr->setupUi(this);
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
    menuFilePtr->addAction(tr("Print"));
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
    menuFormatPtr->addAction(tr("Bold"));
    menuFormatPtr->addAction(tr("Italic"));
    menuFormatPtr->addAction(tr("Underline"));
    menuFormatPtr->addAction(tr("Crossed"));
    menuFormatPtr->addSeparator();
    menuFormatPtr->addAction(tr("Font style"));
    menuFormatPtr->addAction(tr("Font color"));
    return menuFormatPtr;
}

QMenu *TextEditor::insertMenu()     // заполнение меню Insert
{
    QFont font("Corbel", 10);
    QMenu *menuInsertPtr = new QMenu(this);
    menuInsertPtr->setFont(font);
    menuInsertPtr->setTitle(tr("Insert"));
    menuInsertPtr->addAction(tr("Image"));
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
    QString text = uiPtr->textEdit->toHtml(); // чтобы сохранить форматирование и изображения, мы меняем "toPlainText" yfна "toHtml"
    out << text;
    file.flush();
    file.close();
    isFileSaved = true;
}

void TextEditor::slotFileSaveAs()       //функция сохранения с новым именем файла
{
    QString file_name = QFileDialog::getSaveFileName(this, "Save the file", "", "Text Files (*.txt)");  // охраняет в формате txt
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
    QString text = uiPtr->textEdit->toHtml(); // чтобы сохранить форматирование и изображения, мы меняем "toPlainText" yfна "toHtml"
    out << text;
    file.flush();
    file.close();
    isFileSaved = true;
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
