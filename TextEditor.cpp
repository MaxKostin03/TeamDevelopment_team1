#include "TextEditor.h"
#include "ui_TextEditor.h"

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFontDialog>
#include <QTextCharFormat>
#include <QSettings>


// *** class TextEditor

TextEditor::TextEditor(QWidget *parent)
    : QMainWindow(parent)
    , uiPtr(new Ui::TextEditor)
    , searchWidget(new SearchWidget)
{
    uiPtr->setupUi(this);
    //this->setWindowTitle(tr("Text Editor"));

    slotLightMode();
    qtLanguageTranslator.load(":/QtLanguage_ru.qm", ".");

    setWindowIcon(QIcon(":/res/Icons-file/file")); // добавление иконки приложения

    createMenu();
    //searchWidget = new SearchWidget;

    connect(searchWidget, SIGNAL(signalSearchText(QString)), this, SLOT(slotSearchText(QString))); // подключаем сигнал с текстом поиска для вызова функции поиска текста
    connect(uiPtr->textEdit, &QTextEdit::textChanged, this, &TextEditor::slotRenameTitle);

    slotRenameTitle();

    uiPtr->textEdit->clear(); // Очищаем базовое контекстное меню

    // Устанавливаем контекстное меню для QTextEdit
    uiPtr->textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(uiPtr->textEdit, &QTextEdit::customContextMenuRequested, this, &TextEditor::slotContextMenu);

    createStatusBar();
    readSettings();
}

TextEditor::~TextEditor()
{
    delete uiPtr;
    delete searchWidget;
}

void TextEditor::createMenu()
{
    uiPtr->menubar->addMenu(menuConfig());  // добавление в menubar меню File
    uiPtr->menubar->addMenu(editMenu());    // добавление в menubar меню Edit
    uiPtr->menubar->addMenu(formatMenu());  // добавление в menubar меню Format
    uiPtr->menubar->addMenu(insertMenu());  // добавление в menubar меню Insert
    uiPtr->menubar->addMenu(viewMenu());    // добавление в menubar меню View
    uiPtr->menubar->addMenu(languageMenu());// добавление в menubar меню Language
    uiPtr->menubar->addAction(help());      // добавление в menubar Help
    uiPtr->menubar->addAction(about());     // добавление в menubar About
    uiPtr->toolBar->addWidget(toolbar());   // добавление в toolbar блока кнопок управления
}

QMenu *TextEditor::menuConfig()     // заполнение меню File
{
    QFont font("Corbel", 10);
    QMenu *menuFilePtr = new QMenu(this);
    menuFilePtr->setFont(font);
    menuFilePtr->setTitle(tr("File"));
    menuFilePtr->addAction(tr("New"), this, &TextEditor::slotFileNew, QKeySequence::New)->setIcon(QIcon(":/res/Icons-file/file"));              // кнопка вызова функции создания нового файла (Ctrl+N)
    menuFilePtr->addAction(tr("Open"), this, &TextEditor::slotFileOpen, QKeySequence::Open)->setIcon(QIcon(":/res/Icons-file/folder"));         // кнопка вызова функции открытия файла (Ctrl+O)
    menuFilePtr->addAction(tr("Save"), this, &TextEditor::slotFileSave, QKeySequence::Save)->setIcon(QIcon(":/res/Icons-file/diskette"));       // кнопка вызова функции сохранения файла (Ctrl+S)
    menuFilePtr->addAction(tr("Save as"), this, &TextEditor::slotFileSaveAs, QKeySequence(tr("Ctrl+Shift+S")))->setIcon(QIcon(":/res/Icons-file/save-as")); // кнопка вызова функции сохранения с новым именем файла (Ctrl+Shift+S)
    menuFilePtr->addAction(tr("Export to PDF"), this, &TextEditor::slotExportToPdf)->setIcon(QIcon(":/res/Icons-file/PDF")); // кнопка вызова функции экспорта в pdf
    menuFilePtr->addAction(tr("Print"), this, &TextEditor::slotPrintFile, QKeySequence::Print)->setIcon(QIcon(":/res/Icons-file/printer"));     // кнопка вызова функции печати файла (Ctrl+P)

    menuFilePtr->addSeparator();

    recentMenu = menuFilePtr->addMenu(tr("Recent..."));
    connect(recentMenu, &QMenu::aboutToShow, this, &TextEditor::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &TextEditor::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = menuFilePtr->addSeparator();

    setRecentFilesVisible(TextEditor::hasRecentFiles());
    //menuFilePtr->addSeparator();
    //menuFilePtr->addAction(tr("Exit"), this, &TextEditor::slotExitFile, QKeySequence::Close)->setIcon(QIcon(":/res/Icons-file/logout"));        // кнопка вызова функции выхода (Ctrl+F4)

    return menuFilePtr;
}

QMenu *TextEditor::editMenu()       // заполнение меню Edit
{
    QFont font("Corbel", 10);
    QMenu *menuEditPtr = new QMenu(this);
    menuEditPtr->setFont(font);
    menuEditPtr->setTitle(tr("Edit"));
    menuEditPtr->addAction(tr("Undo"), this, &TextEditor::slotUndo, QKeySequence::Undo)->setIcon(QIcon(":/res/Icons-file/turn-left"));                  // кнопка вызова функции отмены действия (Ctrl+Z)
    menuEditPtr->addAction(tr("Redo"), this, &TextEditor::slotRedo, QKeySequence::Redo)->setIcon(QIcon(":/res/Icons-file/forward"));                    // кнопка вызова функции повтора действия (Ctrl+Y)
    menuEditPtr->addAction(tr("Copy"), this, &TextEditor::slotCopy, QKeySequence::Copy)->setIcon(QIcon(":/res/Icons-file/copy"));                       // кнопка вызова функции копирования (Ctrl+C)
    menuEditPtr->addAction(tr("Cut"), this, &TextEditor::slotCut, QKeySequence::Cut)->setIcon(QIcon(":/res/Icons-file/scissors"));                      // кнопка вызова функции вырезать (Ctrl+X)
    menuEditPtr->addAction(tr("Paste"), this, &TextEditor::slotPaste, QKeySequence::Paste)->setIcon(QIcon(":/res/Icons-file/paste"));                   // кнопка вызова функции вставить (Ctrl+V)
    menuEditPtr->addSeparator();
    menuEditPtr->addAction(tr("Select All"), this, &TextEditor::slotSelectAll, QKeySequence::SelectAll)->setIcon(QIcon(":/res/Icons-file/checkbox"));   // кнопка вызова функции выделить все (Ctrl+A)
    return menuEditPtr;
}

QMenu *TextEditor::formatMenu()     // заполнение меню Format
{
    QFont font("Corbel", 10);
    QMenu *menuFormatPtr = new QMenu(this);
    menuFormatPtr->setFont(font);
    menuFormatPtr->setTitle(tr("Format"));
    menuFormatPtr->addAction(tr("Bold"), this, &TextEditor::slotBold, QKeySequence::Bold)->setIcon(QIcon(":/res/Icons-file/bold"));                         // кнопка вызова функции жирного шрифта (Ctrl+B)
    menuFormatPtr->addAction(tr("Italic"), this, &TextEditor::slotItalic, QKeySequence::Italic)->setIcon(QIcon(":/res/Icons-file/italic"));                 // кнопка вызова функции курсивного шрифта (Ctrl+I)
    menuFormatPtr->addAction(tr("Underline"), this, &TextEditor::slotUnderlined, QKeySequence::Underline)->setIcon(QIcon(":/res/Icons-file/underline"));    // кнопка вызова функции подчеркнутого шрифта (Ctrl+U)
    menuFormatPtr->addAction(tr("Crossed"),this, &TextEditor::slotCrossedOut)->setIcon(QIcon(":/res/Icons-file/cross-out"));             // кнопка вызова функции зачеркнутого шрифта
    menuFormatPtr->addSeparator();
    menuFormatPtr->addAction(tr("Font style"), this, &TextEditor::slotFontStyle)->setIcon(QIcon(":/res/Icons-file/font-adjustment"));    // кнопка вызова функции изменения стиля шрифта
    menuFormatPtr->addAction(tr("Font color"), this, &TextEditor::slotFontColor)->setIcon(QIcon(":/res/Icons-file/color-text"));         // кнопка вызова функции изменения цвета шрифта
    menuFormatPtr->addAction(tr("Align left"), this, &TextEditor::slotLeftSide)->setIcon(QIcon(":/res/Icons-file/AlignLeft"));          // кнопка вызова функции выравнивания текста по левому краю
    menuFormatPtr->addAction(tr("Align center"), this, &TextEditor::slotInTheCenter)->setIcon(QIcon(":/res/Icons-file/AlignCenter"));    // кнопка вызова функции выравнивания текста по центру
    menuFormatPtr->addAction(tr("Align right"), this, &TextEditor::slotRightSide)->setIcon(QIcon(":/res/Icons-file/AlignRight"));       // кнопка вызова функции выравнивания текста по правому краю
    return menuFormatPtr;
}

QMenu *TextEditor::insertMenu()     // заполнение меню Insert
{
    QFont font("Corbel", 10);
    QMenu *menuInsertPtr = new QMenu(this);
    menuInsertPtr->setFont(font);
    menuInsertPtr->setTitle(tr("Insert"));
    menuInsertPtr->addAction(tr("Image"), this, &TextEditor::slotInsertImage)->setIcon(QIcon(":/res/Icons-file/insert-picture-icon"));  // кнопка вызова функции добавления изображения
    menuInsertPtr->addAction(tr("Formula"), this, &TextEditor::slotInsertFormula)->setIcon(QIcon(":/res/Icons-file/insert-formula-icon"));  // кнопка вызова функции добавления формулы
    return menuInsertPtr;
}

QMenu *TextEditor::viewMenu()       // заполнение меню View
{
    QFont font("Corbel", 10);
    QMenu *menuViewPtr = new QMenu(this);
    menuViewPtr->setFont(font);
    menuViewPtr->setTitle(tr("View"));
    menuViewPtr->addAction(tr("Dark mode"), this, &TextEditor::slotDarkMode)->setIcon(QIcon(":/res/Icons-file/file_black"));    // кнопка вызова функции темной темы
    menuViewPtr->addAction(tr("Light mode"), this, &TextEditor::slotLightMode)->setIcon(QIcon(":/res/Icons-file/file_white"));  // кнопка вызова функции светлой темы
    return menuViewPtr;
}

QMenu *TextEditor::languageMenu()       // заполнение меню Language
{
    QFont font("Corbel", 10);
    QMenu *menuLanguagePtr = new QMenu(this);
    menuLanguagePtr->setTitle(tr("Language"));
    menuLanguagePtr->addAction(tr("English"), this, &TextEditor::slotEnglish)->setIcon(QIcon(":/res/Icons-file/english"));    // кнопка вызова функции английского языка
    menuLanguagePtr->addAction(tr("Russian"), this, &TextEditor::slotRussian)->setIcon(QIcon(":/res/Icons-file/russian"));  // кнопка вызова функции русского языка
    return menuLanguagePtr;
}

QAction *TextEditor::help()       // заполнение Help
{
    QFont font("Corbel", 10);
    QAction *helpPtr=new QAction(this);
    helpPtr->setText(tr("Help"));
    connect(helpPtr, SIGNAL(triggered(bool)), this, SLOT(slotHelp()));
    return helpPtr;
}

QAction *TextEditor::about()       // заполнение About
{
    QFont font("Corbel", 10);
    QAction *aboutPtr=new QAction(this);
    aboutPtr->setText(tr("About"));
    connect(aboutPtr, SIGNAL(triggered(bool)), this, SLOT(slotAbout()));
    return aboutPtr;
}



QToolBar *TextEditor::toolbar()     // заполнение в toolbar блока кнопок управления
{
    QToolBar *toolbar = new QToolBar(this);

    QAction *newdoc = toolbar->addAction(QIcon(":/res/Icons-file/file"), tr("New document"));           // кнопка вызова функции создания нового файла
    connect(newdoc, &QAction::triggered, this, &TextEditor::slotFileNew);

    QAction *open = toolbar->addAction(QIcon(":/res/Icons-file/folder"), tr("Open file"));              // кнопка вызова функции открытия файла
    connect(open, &QAction::triggered, this, &TextEditor::slotFileOpen);

    QAction *save = toolbar->addAction(QIcon(":/res/Icons-file/diskette"), tr("Save file"));            // кнопка вызова функции сохранения файла
    connect(save, &QAction::triggered, this, &TextEditor::slotFileSave);

    QAction *save_as = toolbar->addAction(QIcon(":/res/Icons-file/save-as"), tr("Save file as"));       // кнопка вызова функции сохранения с новым именем файла
    connect(save_as, &QAction::triggered, this, &TextEditor::slotFileSaveAs);

    QAction *printdoc = toolbar->addAction(QIcon(":/res/Icons-file/printer"), tr("Print document"));    // кнопка вызова функции печати файла
    connect(printdoc, &QAction::triggered, this, &TextEditor::slotPrintFile);

    toolbar->addSeparator();

    QAction *undo = toolbar->addAction(QIcon(":/res/Icons-file/turn-left"), tr("Undo"));    // кнопка вызова функции отмены действия
    connect(undo, &QAction::triggered, this, &TextEditor::slotUndo);

    QAction *redo = toolbar->addAction(QIcon(":/res/Icons-file/forward"), tr("Redo"));      // кнопка вызова функции повтора действия
    connect(redo, &QAction::triggered, this, &TextEditor::slotRedo);

    QAction *copy = toolbar->addAction(QIcon(":/res/Icons-file/copy"), tr("Copy"));         // кнопка вызова функции копирования
    connect(copy, &QAction::triggered, this, &TextEditor::slotCopy);

    QAction *cut = toolbar->addAction(QIcon(":/res/Icons-file/scissors"), tr("Cut"));       // кнопка вызова функции вырезать
    connect(cut, &QAction::triggered, this, &TextEditor::slotCut);

    QAction *paste = toolbar->addAction(QIcon(":/res/Icons-file/paste"), tr("Paste"));      // кнопка вызова функции вставить
    connect(paste, &QAction::triggered, this, &TextEditor::slotPaste);

    toolbar->addSeparator();

    QAction *bold = toolbar->addAction(QIcon(":/res/Icons-file/bold"), tr("Bold"));                                 // кнопка вызова функции жирного шрифта
    connect(bold, &QAction::triggered, this, &TextEditor::slotBold);
    bold->setStatusTip(tr("When selecting text from right to left, the text changes only once when clicked. "
                          "When you select from left to right, the text changes every time."));

    QAction *italic = toolbar->addAction(QIcon(":/res/Icons-file/italic"), tr("Italic"));                           // кнопка вызова функции курсивного шрифта
    connect(italic, &QAction::triggered, this, &TextEditor::slotItalic);
    italic->setStatusTip(tr("When selecting text from right to left, the text changes only once when clicked. "
                            "When you select from left to right, the text changes every time."));

    QAction *underlined = toolbar->addAction(QIcon(":/res/Icons-file/underline"), tr("Underlined"));                // кнопка вызова функции подчеркнутого шрифта
    connect(underlined, &QAction::triggered, this, &TextEditor::slotUnderlined);
    underlined->setStatusTip(tr("When selecting text from right to left, the text changes only once when clicked. "
                                "When you select from left to right, the text changes every time."));

    QAction *crossedOut = toolbar->addAction(QIcon(":/res/Icons-file/cross-out"), tr("Cross"));                     // кнопка вызова функции зачеркнутого шрифта
    connect(crossedOut, &QAction::triggered, this, &TextEditor::slotCrossedOut);
    crossedOut->setStatusTip(tr("When selecting text from right to left, the text changes only once when clicked. "
                                "When you select from left to right, the text changes every time."));

    toolbar->addSeparator();

    QAction *font_style = toolbar->addAction(QIcon(":/res/Icons-file/font-adjustment"), tr("Font style"));      // кнопка вызова функции изменения стиля шрифта
    connect(font_style, &QAction::triggered, this, &TextEditor::slotFontStyle);

    QAction *font_color = toolbar->addAction(QIcon(":/res/Icons-file/color-text"), tr("Font color"));           // кнопка вызова функции изменения цвета шрифта
    connect(font_color, &QAction::triggered, this, &TextEditor::slotFontColor);

    QAction *align_left = toolbar->addAction(QIcon(":/res/Icons-file/AlignLeft"), tr("Align left"));             // кнопка вызова функции выравнивания текста по левому краю
    connect(align_left, &QAction::triggered, this, &TextEditor::slotLeftSide);

    QAction *align_center = toolbar->addAction(QIcon(":/res/Icons-file/AlignCenter"), tr("Align center"));       // кнопка вызова функции выравнивания текста по центру
    connect(align_center, &QAction::triggered, this, &TextEditor::slotInTheCenter);

    QAction *align_right = toolbar->addAction(QIcon(":/res/Icons-file/AlignRight"), tr("Align right"));          // кнопка вызова функции выравнивания текста по правому краю
    connect(align_right, &QAction::triggered, this, &TextEditor::slotRightSide);

    toolbar->addSeparator();

    QAction *image = toolbar->addAction(QIcon(":/res/Icons-file/insert-picture-icon"), tr("Insert image"));     // кнопка вызова функции добавления изображения
    connect(image, &QAction::triggered, this, &TextEditor::slotInsertImage);

    QAction *formula = toolbar->addAction(QIcon(":/res/Icons-file/insert-formula-icon"), tr("Insert formula"));     // кнопка вызова функции добавления формулы
    connect(formula, &QAction::triggered, this, &TextEditor::slotInsertFormula);

    QAction *search = toolbar->addAction(QIcon(":/res/Icons-file/search"), tr("Search text"));     // кнопка вызова окна для поиска текста
    connect(search, &QAction::triggered, this, &TextEditor::slotSearch);

    return toolbar;
}

void TextEditor::slotRenameTitle()       // функция изменения названия файла
{
    QString title {""};

    if (uiPtr->textEdit->getUntitled())
        title = tr("Text Editor - ") + uiPtr->textEdit->currentFile();
    else
        title = tr("Text Editor - ") + uiPtr->textEdit->userFriendlyCurrentFile();

    title += (uiPtr->textEdit->document()->isModified() ? " (*)" : "");
    setWindowTitle(title);
}

void TextEditor::slotFileNew()      // функция создания нового файла
{
    if (uiPtr->textEdit->maybeSave())
    {
        uiPtr->textEdit->newFile();
        slotRenameTitle();
    }
}



void TextEditor::slotFileSave()     // функция сохранения файла
{
    if (uiPtr->textEdit->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void TextEditor::slotFileSaveAs()       //функция сохранения с новым именем файла
{
    if (uiPtr->textEdit->saveAs()) {
        statusBar()->showMessage(tr("File saved"), 2000);
        slotRenameTitle(); // вызов функции изменения названия файла
        TextEditor::prependToRecentFiles(uiPtr->textEdit->currentFile());
    }
}

// Функция экспорта в pdf
void TextEditor::slotExportToPdf() {
    uiPtr->textEdit->printPdf();
}

void TextEditor::slotPrintFile()        // функция печати файла
{

    uiPtr->textEdit->printFile();   // отправка на печать
}

void TextEditor::closeEvent(QCloseEvent *event)     // функция выхода по крестику
{
    if (uiPtr->textEdit->maybeSave()) {
        writeSettings();
        event->accept();
    }
    else {
        event->ignore();
    }
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

void TextEditor::slotDelete(){
    QTextCursor cursor = uiPtr->textEdit->textCursor();

    if (cursor.hasSelection()) {
        cursor.removeSelectedText();
    }
}

void TextEditor::slotBold() {
    QTextCursor cursor = uiPtr->textEdit->textCursor();

    if (!cursor.hasSelection()) return;
    int selectionStart = cursor.selectionStart();
    int selectionEnd = cursor.selectionEnd();

    cursor.beginEditBlock();
    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);

    for (int pos = selectionStart; pos < selectionEnd; ++pos) {
        cursor.setPosition(pos);
        cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);

        QTextCharFormat charFormat = cursor.charFormat();
        if (charFormat.fontWeight() == QFont::Bold) {
            charFormat.setFontWeight(QFont::Normal);
        } else {
            charFormat.merge(boldFormat);
        }

        bool isItalic = charFormat.fontItalic();
        bool isUnderline = charFormat.fontUnderline();
        bool isStrikeOut = charFormat.fontStrikeOut();
        charFormat.setFontItalic(isItalic);
        charFormat.setFontUnderline(isUnderline);
        charFormat.setFontStrikeOut(isStrikeOut);

        cursor.setCharFormat(charFormat);
    }
    cursor.endEditBlock();

    cursor.setPosition(selectionStart);
    cursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);
    uiPtr->textEdit->setTextCursor(cursor);
}

void TextEditor::slotItalic() {
    QTextCursor cursor = uiPtr->textEdit->textCursor();

    if (!cursor.hasSelection()) return;
    int selectionStart = cursor.selectionStart();
    int selectionEnd = cursor.selectionEnd();

    cursor.beginEditBlock();
    QTextCharFormat italicFormat;
    italicFormat.setFontItalic(true);

    for (int pos = selectionStart; pos < selectionEnd; ++pos) {
        cursor.setPosition(pos);
        cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);

        QTextCharFormat charFormat = cursor.charFormat();
        if (charFormat.fontItalic()) {
            charFormat.setFontItalic(false);
        } else {
            charFormat.merge(italicFormat);
        }

        bool isUnderline = charFormat.fontUnderline();
        bool isStrikeOut = charFormat.fontStrikeOut();
        charFormat.setFontWeight(charFormat.fontWeight() == QFont::Bold ? QFont::Bold : QFont::Normal);
        charFormat.setFontUnderline(isUnderline);
        charFormat.setFontStrikeOut(isStrikeOut);

        cursor.setCharFormat(charFormat);
    }
    cursor.endEditBlock();

    cursor.setPosition(selectionStart);
    cursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);
    uiPtr->textEdit->setTextCursor(cursor);
}

void TextEditor::slotUnderlined()
{
    QTextCursor cursor = uiPtr->textEdit->textCursor();

    if (!cursor.hasSelection()) return;
    int selectionStart = cursor.selectionStart();
    int selectionEnd = cursor.selectionEnd();

    cursor.beginEditBlock();
    QTextCharFormat underlineFormat;
    underlineFormat.setFontUnderline(true);

    for (int pos = selectionStart; pos < selectionEnd; ++pos) {
        cursor.setPosition(pos);
        cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);

        QTextCharFormat charFormat = cursor.charFormat();
        if (charFormat.fontUnderline()) {
            charFormat.setFontUnderline(false);
        } else {
            charFormat.merge(underlineFormat);
        }

        bool isItalic = charFormat.fontItalic();
        bool isStrikeOut = charFormat.fontStrikeOut();
        charFormat.setFontWeight(charFormat.fontWeight() == QFont::Bold ? QFont::Bold : QFont::Normal);
        charFormat.setFontItalic(isItalic);
        charFormat.setFontStrikeOut(isStrikeOut);

        cursor.setCharFormat(charFormat);
    }
    cursor.endEditBlock();

    cursor.setPosition(selectionStart);
    cursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);
    uiPtr->textEdit->setTextCursor(cursor);
}

void TextEditor::slotCrossedOut()
{
    QTextCursor cursor = uiPtr->textEdit->textCursor();

    if (!cursor.hasSelection()) return;
    int selectionStart = cursor.selectionStart();
    int selectionEnd = cursor.selectionEnd();

    cursor.beginEditBlock();
    QTextCharFormat strikeoutFormat;
    strikeoutFormat.setFontStrikeOut(true);

    for (int pos = selectionStart; pos < selectionEnd; ++pos) {
        cursor.setPosition(pos);
        cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);

        QTextCharFormat charFormat = cursor.charFormat();
        if (charFormat.fontStrikeOut()) {
            charFormat.setFontStrikeOut(false);
        } else {
            charFormat.merge(strikeoutFormat);
        }

        bool isItalic = charFormat.fontItalic();
        bool isUnderlined = charFormat.fontUnderline();
        charFormat.setFontWeight(charFormat.fontWeight() == QFont::Bold ? QFont::Bold : QFont::Normal);
        charFormat.setFontItalic(isItalic);
        charFormat.setFontUnderline(isUnderlined);

        cursor.setCharFormat(charFormat);
    }
    cursor.endEditBlock();

    cursor.setPosition(selectionStart);
    cursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);
    uiPtr->textEdit->setTextCursor(cursor);
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

    QPoint Pos = mapFromGlobal(QCursor::pos());
    createColorPalette(Pos.x() , Pos.y()-(uiPtr->toolBar->height()));   // вызов функции выбора цветовой палитры

}

void TextEditor::slotLeftSide()                               // функция выравнивания текста по левому краю
{
    QTextCursor cursor = uiPtr->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);
    uiPtr->textEdit->setTextCursor(cursor);
}

void TextEditor::slotInTheCenter()                             // функция выравнивания текста по центру
{
    QTextCursor center = uiPtr->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = center.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignCenter);
    center.mergeBlockFormat(textBlockFormat);
    uiPtr->textEdit->setTextCursor(center);
}

void TextEditor::slotRightSide()                              // функция выравнивания текста по правому краю
{
    QTextCursor cursor = uiPtr->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignRight);
    cursor.mergeBlockFormat(textBlockFormat);
    uiPtr->textEdit->setTextCursor(cursor);
}

void TextEditor::slotInsertImage()      // функция добавления изображения
{
    QString file_path = QFileDialog::getOpenFileName(this, tr("Open the file"));
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

void TextEditor::slotInsertFormula()
{
    bool ok;
    QString introductoryText = tr("This window is intended for entering mathematical formulas. You can use the following operators:\n"
                                  "  - Addition: +;\n"
                                  "  - Subtraction: -;\n"
                                  "  - Multiplication: *;\n"
                                  "  - Division: /;\n"
                                  "  - Degree: a^b (for example, 2^3 for 2 to the power of 3);\n"
                                  "  - Using trigonometric formulas (e.g. sin, cos, tan, con, sec, cosec);\n"
                                  "  - Root: sqrt(a) (for example, sqrt(16) for the square root of 16);\n"
                                  "  - Using the constant π instead of PI.\n"
                                  "Enter your formula:");

    QString formula = QInputDialog::getText(this, tr("Insert formula"), introductoryText, QLineEdit::Normal, QString(), &ok);

    if (ok && !formula.isEmpty())
    {
        // Заменяем базовые операторы на их HTML-эквиваленты
        formula.replace("&", "&amp;"); // Экранируем символ & как &amp;
        formula.replace("+", "&#43;"); // Замена + на HTML-код
        formula.replace("-", "&#8722;"); // Замена - на HTML-код
        formula.replace("*", "&#215;"); // Замена * на HTML-код
        //formula.replace("/", "&#247;"); // Замена / на HTML-код
        formula.replace("PI", "π");

        // Парсинг и форматирование математического выражения
        formula.replace(QRegularExpression("([a-zA-Z0-9]+)\\(([^)]+)\\)\\^(\\d+)"), "\\1(\\2)<sup>\\3</sup>"); // Регулярное выражение для степени после скобок

        formula.replace("sqrt", "<span>&radic;</span>");     // Регулярное выражение для корня
        formula.replace(QRegularExpression("(\\w+)\\^(\\w+)"), "\\1<sup>\\2</sup>");     // Регулярное выражение для степени

        formula.replace(QRegularExpression("([a-zA-Z0-9]+)\\s*/\\s*([a-zA-Z0-9]+)"), "<span class=\"numerator\">\\1</span><span class=\"fraction-line\"></span><span class=\"denominator\">\\2</span>"); // Регулярное выражение для дроби

        QTextCursor cursor = uiPtr->textEdit->textCursor();
        QString selectedText = cursor.selectedText();
        int position = cursor.selectionStart();

        if (selectedText.isEmpty()) // Если нет выделенного текста
        {
            cursor.insertHtml(formula); // Вставляем отформатированное выражение
        }
        else
        {
            // Заменяем выделенный текст на отформатированное выражение
            cursor.removeSelectedText();
            cursor.insertHtml(formula);
        }
    }
}

void TextEditor::slotSearch()       // функция вызова окна для поиска текста
{
    searchWidget->setModal(true);
    searchWidget->show();   // показать окно
    m_searchHighLight = new SearchHighLight(uiPtr->textEdit->document());
}

void TextEditor::slotContextMenu(const QPoint& pos) {
    QMenu *contextMenu = new QMenu(this);

    QAction *undoAction = new QAction(tr("Undo"), this);
    contextMenu->addAction(undoAction);
    connect(undoAction, &QAction::triggered, this, &TextEditor::slotUndo);

    QAction *redoAction = new QAction(tr("Redo"), this);
    contextMenu->addAction(redoAction);
    connect(redoAction, &QAction::triggered, this, &TextEditor::slotRedo);

    contextMenu->addSeparator();

    QAction *cutAction = new QAction(tr("Cut"), this);
    contextMenu->addAction(cutAction);
    connect(cutAction, &QAction::triggered, this, &TextEditor::slotCut);

    QAction *copyAction = new QAction(tr("Copy"), this);
    contextMenu->addAction(copyAction);
    connect(copyAction, &QAction::triggered, this, &TextEditor::slotCopy);

    QAction *pasteAction = new QAction(tr("Paste"), this);
    contextMenu->addAction(pasteAction);
    connect(pasteAction, &QAction::triggered, this, &TextEditor::slotPaste);

    QAction *deleteAction = new QAction(tr("Delete"), this);
    contextMenu->addAction(deleteAction);
    connect(deleteAction, &QAction::triggered, this, &TextEditor::slotDelete);

    contextMenu->addSeparator();

    QAction *selectAllAction = new QAction(tr("Select All"), this);
    contextMenu->addAction(selectAllAction);
    connect(selectAllAction, &QAction::triggered, this, &TextEditor::slotSelectAll);

    // Отображаем контекстное меню в указанной позиции
    contextMenu->exec(uiPtr->textEdit->mapToGlobal(pos));
}

void TextEditor::slotSearchText(QString text)       // функция поиска текста
{
    m_searchHighLight->searchText(text);    // поиск текста
    //searchWidget->hide();   // скрыть окно
}

void TextEditor::slotDarkMode()     // функция темной темы
{
    QFile qssFile(":/res/QSS-file/DarkMode.qss");   // выбрать стиль из ресурсов
    qssFile.open(QFile::ReadOnly);  // открыть файл только для чтения
    if(qssFile.isOpen())
    {
        QString qss = QLatin1String(qssFile.readAll());
        this->setStyleSheet(qss);   // если файл открылся установить данный стиль
        qssFile.close();
    }
}

void TextEditor::slotLightMode()        // функция светлой темы
{
    QFile qssFile(":/res/QSS-file/LightMode.qss");   // выбрать стиль из ресурсов
    qssFile.open(QFile::ReadOnly);  // открыть файл только для чтения
    if(qssFile.isOpen())
    {
        QString qss = QLatin1String(qssFile.readAll());
        this->setStyleSheet(qss);   // если файл открылся установить данный стиль
        qssFile.close();
    }
}

void TextEditor::slotEnglish() {
    qApp->removeTranslator(&qtLanguageTranslator);
    uiPtr->retranslateUi(this);

    this->setWindowTitle(tr("Text Editor"));

    // Настройка перевода динамических элементов

    uiPtr->menubar->clear(); // Очистка динамически созданных меню
    uiPtr->toolBar->clear(); // Очистка динамически созданной панели инструментов

    uiPtr->menubar->addMenu(menuConfig());   // добавление в menubar меню File
    uiPtr->menubar->addMenu(editMenu());     // добавление в menubar меню Edit
    uiPtr->menubar->addMenu(formatMenu());   // добавление в menubar меню Format
    uiPtr->menubar->addMenu(insertMenu());   // добавление в menubar меню Insert
    uiPtr->menubar->addMenu(viewMenu());     // добавление в menubar меню View
    uiPtr->menubar->addMenu(languageMenu()); // добавление в menubar меню Language
    uiPtr->menubar->addAction(help());       // добавление в menubar Help
    uiPtr->menubar->addAction(about());      // добавление в menubar About
    uiPtr->toolBar->addWidget(toolbar());    // Добавить виджет заново
    searchWidget = new SearchWidget;

    hidePalette(window);    // вызов функции скрытия окна кнопок цветовой палитры
    window = NULL;          // очищение окна кнопок цветовой палитры
}


void TextEditor::slotRussian()
{
    qApp->installTranslator(&qtLanguageTranslator);
    uiPtr->retranslateUi(this);

    //Настройка перевода динамических элементов

    uiPtr->menubar->clear(); // Очистка динамически созданных меню
    uiPtr->toolBar->clear(); // Очистка динамически созданной панели инструментов

    uiPtr->menubar->addMenu(menuConfig());   // добавление в menubar меню File
    uiPtr->menubar->addMenu(editMenu());     // добавление в menubar меню Edit
    uiPtr->menubar->addMenu(formatMenu());   // добавление в menubar меню Format
    uiPtr->menubar->addMenu(insertMenu());   // добавление в menubar меню Insert
    uiPtr->menubar->addMenu(viewMenu());     // добавление в menubar меню View
    uiPtr->menubar->addMenu(languageMenu()); // добавление в menubar меню Language
    uiPtr->menubar->addAction(help());       // добавление в menubar Help
    uiPtr->menubar->addAction(about());      // добавление в menubar About
    uiPtr->toolBar->addWidget(toolbar());    // Добавить виджет заново
    searchWidget = new SearchWidget;

    hidePalette(window);    // вызов функции скрытия окна кнопок цветовой палитры
    window = NULL;          // очищение окна кнопок цветовой палитры
}

void TextEditor::slotHelp(){                  //функция Help
    QVBoxLayout *textHelp=new QVBoxLayout;
    QTextEdit *textEdit=new QTextEdit;
    textHelp->addWidget(textEdit);
    QDialog *helpWidget=new QDialog;// исправление бага B8
    helpWidget->setWindowIcon(QIcon(":/res/Icons-file/question"));
    helpWidget->setWindowTitle (tr("Help"));
    textEdit->setText (tr("Create file (Ctrl+N)\n"
                      "Open file (Ctrl+O)\n"
                      "Save file (Ctrl+S)\n"
                      "Save with new file name (Ctrl+Shift+S)\n"
                      "Print file (Ctrl+P)\n"
                      "Exit (Ctrl+F4)\n"
                      "Undo action (Ctrl+Z)\n"
                      "Repeat action (Ctrl+Y)\n"
                      "Copy (Ctrl+C)\n"
                      "Cut (Ctrl+X)\n"
                      "Paste (Ctrl+V)\n"
                      "Select all (Ctrl+A)\n"
                      "Applying bold text (Ctrl+B)\n"
                      "Applying italic text (Ctrl+I)\n"
                      "Underline text (Ctrl+U)\n"));
    textEdit->setReadOnly(true);
    helpWidget->setLayout(textHelp);
    helpWidget->setModal(true);// исправление бага B8
    helpWidget->show();
}

void TextEditor::slotAbout(){                  //функция About
    QVBoxLayout *textAbout=new QVBoxLayout;
    QTextEdit *textEdit=new QTextEdit;
    textAbout->addWidget(textEdit);
    QDialog *aboutWidget=new QDialog;//исправление бага B9
    aboutWidget->setWindowIcon(QIcon(":/res/Icons-file/info"));
    aboutWidget->setWindowTitle (tr("About"));
    textEdit->setText (tr("Text Editor version 1.0\n"));
    textEdit->setReadOnly(true);
    aboutWidget->setLayout(textAbout);
    aboutWidget->setModal(true);//исправление бага B9
    aboutWidget->show();
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
    if(window !=NULL && window->isVisible())
        window->hide();   // если окно видимо - скрыть
}
void TextEditor::showPalette(QWidget *window){      // функция показа окна кнопок цветовой палитры
    if(window !=NULL)
        window->show();       // показать окно
}

void TextEditor::slotFileOpen() {

    if (uiPtr->textEdit->maybeSave())
    {
        QString file_name = QFileDialog::getOpenFileName(this, tr("Open the file"));

        if (uiPtr->textEdit->loadFile(file_name))
        {
            slotRenameTitle();     // вызов функции изменения названия окна
            statusBar()->showMessage(tr("File loaded"), 2000);
            TextEditor::prependToRecentFiles(file_name);
        };
    }

}

bool TextEditor::loadFile(const QString &fileName)
{
    if (fileName.isEmpty())
    {
        return false;
    }
    else if (uiPtr->textEdit->loadFile(fileName))
    {
        QString titleName = uiPtr->textEdit->userFriendlyCurrentFile();
        slotRenameTitle();     // вызов функции изменения названия окна
        return true;
    };

    return false;
}

void TextEditor::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void TextEditor::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        // Размер экрана
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        // ... на весь экран
        QMainWindow::setGeometry(screenGeometry);
    } else {
        restoreGeometry(geometry);
    }
}

void TextEditor::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}



static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

bool TextEditor::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();

    return count > 0;
}

void TextEditor::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

void TextEditor::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
    recentFileSeparator->setVisible(visible);
}

void TextEditor::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

void TextEditor::openRecentFile()
{
    if (uiPtr->textEdit->maybeSave())
    {
        if (const QAction *action = qobject_cast<const QAction *>(sender()))
        {
            QString file_name = action->data().toString();
            if (uiPtr->textEdit->loadFile(file_name))
            {
                slotRenameTitle();     // вызов функции изменения названия окна
                statusBar()->showMessage(tr("File loaded"), 2000);
            };
        };
    };
}



// *** class SearchHighLight

SearchHighLight::SearchHighLight(QTextDocument* parent) : BaseClass(parent)
{
    m_format.setBackground(Qt::green);
}

void SearchHighLight::highlightBlock(const QString& text)
{
    QRegularExpressionMatchIterator matchIterator = m_pattern.globalMatch(text);
    while (matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_format);
    }
}

void SearchHighLight::searchText(const QString& text)
{
    m_pattern = QRegularExpression(text);
    rehighlight(); // Перезапускаем подсветку
}

