#include "TextEditor.h"
#include "ui_TextEditor.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

TextEditor::TextEditor(QWidget *parent)
    : QMainWindow(parent), uiPtr(new Ui::TextEditor)
{
    uiPtr->setupUi(this);
    uiPtr->menubar->addMenu(menuConfig());
    uiPtr->menubar->addMenu(editMenu());
    uiPtr->menubar->addMenu(formatMenu());
    uiPtr->menubar->addMenu(insertMenu());
    uiPtr->menubar->addMenu(viewMenu());

}

TextEditor::~TextEditor()
{
    delete uiPtr;
}

QMenu *TextEditor::menuConfig()
{
    QFont font("Corbel", 10);
    QMenu *menuFilePtr = new QMenu(this);
    menuFilePtr->setFont(font);
    menuFilePtr->setTitle(tr("File"));
    menuFilePtr->addAction(tr("New"), this, &TextEditor::slotFileNew);
    menuFilePtr->addAction(tr("Open"), this, &TextEditor::slotFileOpen);
    menuFilePtr->addAction(tr("Save"));
    menuFilePtr->addAction(tr("Save as"));
    menuFilePtr->addAction(tr("Print"));
    menuFilePtr->addSeparator();
    menuFilePtr->addAction(tr("Exit"));
    return menuFilePtr;
}

QMenu *TextEditor::editMenu()
{
    QFont font("Corbel", 10);
    QMenu *menuEditPtr = new QMenu(this);
    menuEditPtr->setFont(font);
    menuEditPtr->setTitle(tr("Edit"));
    menuEditPtr->addAction(tr("Undo"));
    menuEditPtr->addAction(tr("Redo"));
    menuEditPtr->addAction(tr("Copy"));
    menuEditPtr->addAction(tr("Cut"));
    menuEditPtr->addAction(tr("Paste"));
    menuEditPtr->addSeparator();
    menuEditPtr->addAction(tr("Select All"));
    return menuEditPtr;
}

QMenu *TextEditor::formatMenu()
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

QMenu *TextEditor::insertMenu()
{
    QFont font("Corbel", 10);
    QMenu *menuInsertPtr = new QMenu(this);
    menuInsertPtr->setFont(font);
    menuInsertPtr->setTitle(tr("Insert"));
    menuInsertPtr->addAction(tr("Image"));
    return menuInsertPtr;
}

QMenu *TextEditor::viewMenu()
{
    QFont font("Corbel", 10);
    QMenu *menuViewPtr = new QMenu(this);
    menuViewPtr->setFont(font);
    menuViewPtr->setTitle(tr("View"));
    menuViewPtr->addAction(tr("Dark mode"));
    menuViewPtr->addAction(tr("Light mode"));
    return menuViewPtr;
}

void TextEditor::slotRenameTitle(QString newName)
{
    if (newName == "")
        setWindowTitle(tr("Text Editor - New document.txt*"));
    else
        setWindowTitle(tr("Text Editor - ") + newName);
}

void TextEditor::slotFileNew()
{
    uiPtr->textEdit->clear();        // if we opened existing file but want to make new file without any changes - we just clear all
    file_path.clear();
    QFileInfo fileInfo(file_path);
    QString titleName = fileInfo.fileName();
    slotRenameTitle(titleName);
}

void TextEditor::slotFileOpen()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(file_name);
    file_path = file_name;
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "File not opened");
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    uiPtr->textEdit->setText(text);      // we show the content of file in the textBrowser
    QFileInfo fileInfo(file_path);
    QString titleName = fileInfo.fileName();
    slotRenameTitle(titleName);
    file.close();
}
