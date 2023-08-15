#include "TextEditor.h"
#include "ui_TextEditor.h"

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
    menuFilePtr->addAction(tr("New"));
    menuFilePtr->addAction(tr("Open"));
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
