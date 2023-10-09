     #include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFontDialog>
#include <QTextCharFormat>
#include <QSettings>
#include <QLabel>
#include <QColorDialog>
#include <QLibraryInfo>
#include <QApplication>
#include <QPointer>
#include <QScreen>
#include <QDebug>
#include <QTextTableCell>

#include "TextEditor.h"
#include "CalendarWidget.h"
#include "ResizeImage.h"

QPointer<CalendarWidget> calendarWidget;
QLocale *localLanguage = new QLocale;

// *** class TextEditor

TextEditor::TextEditor(QWidget *parent)
    : QMainWindow(parent)
    , loc(new QLocale)
    , uiPtr(new Ui::TextEditor)
    , searchWidget(new SearchWidget)
    , formulaWidget(new FormulaWidget)
{

    uiPtr->setupUi(this);

    *loc=QLocale::English;
    *localLanguage=QLocale::English;
    slotLightMode();
    qtLanguageTranslator.load(":/QtLanguage_ru.qm", ".");

    setWindowIcon(QIcon(":/res/Icons-file/file")); // добавление иконки приложения

    createMenu();

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
    delete m_searchHighLight;
    delete searchWidget;
    delete formulaWidget;
    delete uiPtr;
    delete loc;
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
    menuFormatPtr->addAction(tr("Font color"), this, &TextEditor::slotFontColor)->setIcon(QIcon(":/res/Icons-file/color-text"));
    menuFormatPtr->addAction(tr("Background color"), this, &TextEditor::slotTextBackgroundColor)->setIcon(QIcon(":/res/Icons-file/background_color"));  //кнопка вызова функции изменения цвета фона текста
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
    menuInsertPtr->addAction(tr("Table"), this, &TextEditor::slotInsertTable)->setIcon(QIcon(":/res/Icons-file/insert-table-icon"));  // кнопка вызова функции добавления формулы
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

    QAction *background_color = toolbar->addAction(QIcon(":/res/Icons-file/background_color"), tr("Background color"));     // кнопка вызова функции цвета фона текста
    connect(background_color, &QAction::triggered, this, &TextEditor::slotTextBackgroundColor);

    toolbar->addSeparator();

    QAction *align_left = toolbar->addAction(QIcon(":/res/Icons-file/AlignLeft"), tr("Align left"));             // кнопка вызова функции выравнивания текста по левому краю
    connect(align_left, &QAction::triggered, this, &TextEditor::slotLeftSide);

    QAction *align_center = toolbar->addAction(QIcon(":/res/Icons-file/AlignCenter"), tr("Align center"));       // кнопка вызова функции выравнивания текста по центру
    connect(align_center, &QAction::triggered, this, &TextEditor::slotInTheCenter);

    QAction *align_right = toolbar->addAction(QIcon(":/res/Icons-file/AlignRight"), tr("Align right"));          // кнопка вызова функции выравнивания текста по правому краю
    connect(align_right, &QAction::triggered, this, &TextEditor::slotRightSide);

    toolbar->addSeparator();

    QAction *table = toolbar->addAction(QIcon(":/res/Icons-file/insert-table-icon"), tr("Insert table"));     // кнопка вызова функции вставки таблицы
    connect(table, &QAction::triggered, this, &TextEditor::slotInsertTable);

    QAction *image = toolbar->addAction(QIcon(":/res/Icons-file/insert-picture-icon"), tr("Insert image"));     // кнопка вызова функции добавления изображения
    connect(image, &QAction::triggered, this, &TextEditor::slotInsertImage);

    QAction *formula = toolbar->addAction(QIcon(":/res/Icons-file/insert-formula-icon"), tr("Insert formula"));     // кнопка вызова функции добавления формулы
    connect(formula, &QAction::triggered, this, &TextEditor::slotInsertFormula);

    QAction *search = toolbar->addAction(QIcon(":/res/Icons-file/search"), tr("Search text"));     // кнопка вызова окна для поиска текста
    connect(search, &QAction::triggered, this, &TextEditor::slotSearch);

    // Добавление текущей даты

    QFont *setDataFont = new QFont;
    setDataFont->setPointSize(15);
    QSizePolicy *policy=new QSizePolicy;
    policy->setHorizontalPolicy(QSizePolicy::Expanding);
    QWidget *Date = new QWidget;
    Date->setSizePolicy(*policy);
    QLabel *currentDate = new QLabel;
    currentDate->setStyleSheet("color: #ffffff");
    currentDate->setFont(*setDataFont);
    currentDate->setText(QDate::currentDate().toString("dd.MM.yyyy"));
    toolbar->addWidget(Date);
    toolbar->addWidget(currentDate);


    QAction *calendar = toolbar->addAction(QIcon(":/res/Icons-file/calendar"), tr("Calendar"));
    connect(calendar,SIGNAL(triggered(bool)),this,SLOT(openCalendar()));

    return toolbar;
}

     // Календарь

void TextEditor::openCalendar()
{
    if(calendarWidget){
        calendarWidget->raise();
        calendarWidget->activateWindow();
    }
    if(!calendarWidget){
    calendarWidget=new CalendarWidget(this);
    calendarWidget->resize(350, 260);
    calendarWidget->setWindowIcon(QIcon(":/res/Icons-file/calendar"));
    calendarWidget->setAttribute(Qt::WA_DeleteOnClose);
    calendarWidget->show();
    }
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
void TextEditor::slotExportToPdf()
{
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

void TextEditor::slotResizeImage(){

    QTextBlock currentBlock = uiPtr->textEdit->textCursor().block();
    QTextBlock::iterator it;

    for (it = currentBlock.begin(); !(it.atEnd()); ++it)
    {
        QTextFragment fragment = it.fragment();
        if (fragment.isValid())
        {
            if(fragment.charFormat().isImageFormat ())
            {
                if(fragment.position()>=uiPtr->textEdit->textCursor().anchor() && (fragment.position()<=uiPtr->textEdit->textCursor().position())){
                ResizeImageDialog *rs = new ResizeImageDialog(this);
                QTextImageFormat newImageFormat = fragment.charFormat().toImageFormat();
                QPair<double, double> size = rs->getNewSize(this,newImageFormat.width(),newImageFormat.height());

                newImageFormat.setWidth(size.first);
                newImageFormat.setHeight(size.second);


                if (newImageFormat.isValid())
                {
                    QTextCursor helper = uiPtr->textEdit->textCursor();
                    helper.setPosition(fragment.position());
                    helper.setPosition(fragment.position() + fragment.length(),
                                       QTextCursor::KeepAnchor);
                    helper.setCharFormat(newImageFormat);
                }
            }
        }
    }
    }
}

void TextEditor::slotDelete()
{
    QTextCursor cursor = uiPtr->textEdit->textCursor();

    if (cursor.hasSelection()) {
        cursor.removeSelectedText();
    }
}

void TextEditor::slotBold()
{
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

void TextEditor::slotItalic()
{
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
    QString *languageSelect = new QString;
    if(localLanguage->languageToString(localLanguage->language())=="English"){
        *languageSelect="en_EN";
    }
    else{
        *languageSelect="ru_RU";
    }

    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + *languageSelect, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        qApp->installTranslator(&qtTranslator);


    bool ok;
    QWidget *WidgetforIcon=new QWidget;
    WidgetforIcon->setWindowIcon(QIcon(":/res/Icons-file/font-adjustment"));
    QFont font = QFontDialog::getFont(&ok, WidgetforIcon);   // вызов программы изменения стиля шрифта

    if (ok)
    {
        uiPtr->textEdit->setCurrentFont(font);      // если ОК - применить стиль
    }
    else return;                                    // если Cansel - выйти
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

QColor TextEditor::selectColor(QString QColorTitleName, QString WindowIconPath)                   //цветовая палитра
{
    QString *languageSelect = new QString;
    if(localLanguage->languageToString(localLanguage->language())=="English"){
        *languageSelect="en_EN";
    }
    else{
        *languageSelect="ru_RU";
    }

    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + *languageSelect, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        qApp->installTranslator(&qtTranslator);

    QColorDialog colorDialog;
    QWidget *WidgetforIcon=new QWidget;
    WidgetforIcon->setWindowIcon(QIcon(WindowIconPath));
    QColor selectedColor = colorDialog.getColor(Qt::red, WidgetforIcon, QColorTitleName);

    return selectedColor;
}

void TextEditor::slotFontColor()
{
    QTextCursor cursor = uiPtr->textEdit->textCursor();

    QTextCharFormat originalFormat = cursor.charFormat();

    QString QColorTitleName = tr("Select text color");
    QString WigdetIconPath = ":/res/Icons-file/color-text";
    QColor textColor = selectColor(QColorTitleName, WigdetIconPath);

    if (!textColor.isValid()) {
        return;
    }

    QTextCharFormat newFormat;
    newFormat.setForeground(QBrush(textColor));
    cursor.mergeCharFormat(newFormat);

    cursor.setPosition(cursor.selectionStart());
    cursor.setPosition(cursor.selectionEnd(), QTextCursor::KeepAnchor);
    cursor.setCharFormat(originalFormat);

    cursor.setPosition(cursor.selectionEnd());

    uiPtr->textEdit->setTextCursor(cursor);
}

void TextEditor::slotTextBackgroundColor()
{
    QTextCursor cursor = uiPtr->textEdit->textCursor();

    QTextCharFormat originalFormat = cursor.charFormat();

    QString selectedText = cursor.selectedText();
    QTextCharFormat selectedTextFormat = cursor.charFormat();

    QString QColorTitleName = tr("Select background color");
    QString WigdetIconPath = ":/res/Icons-file/background_color";
    QColor backgroundColor = selectColor(QColorTitleName, WigdetIconPath);

    if (!backgroundColor.isValid()) {
        return;
    }

    QTextCharFormat newFormat;
    newFormat.setBackground(QBrush(backgroundColor));
    cursor.mergeCharFormat(newFormat);

    cursor.setPosition(cursor.selectionStart());
    cursor.setPosition(cursor.selectionEnd(), QTextCursor::KeepAnchor);
    cursor.setCharFormat(selectedTextFormat);

    cursor.setPosition(cursor.selectionEnd());

    uiPtr->textEdit->setTextCursor(cursor);
}

void TextEditor::slotInsertTable()
{
    bool ok;
    QString *insertTableString = new QString;
    QString *insertNumRowString = new QString;
    QString *insertNumColumnsString = new QString;
    QString *insertWidthString = new QString;
    if(localLanguage->languageToString(localLanguage->language())=="English"){
        *insertTableString = "Insert table";
        *insertNumRowString = "Number of rows:";
        *insertNumColumnsString = "Number of columnes:";
        *insertWidthString = "Width of Table:";
    }
    else{
        *insertTableString = "Вставка таблицы";
        *insertNumRowString = "Количество строк:";
        *insertNumColumnsString = "Количество столбцов:";
        *insertWidthString = "Ширина таблицы:";
    }
    QTextTableFormat tableFormat;       // форматирование таблицы
    tableFormat.setAlignment(Qt::AlignHCenter);
    tableFormat.setBorderStyle( QTextTableFormat::BorderStyle_Solid );
    tableFormat.setCellPadding(0);
    tableFormat.setMargin(0);
    tableFormat.setCellSpacing(0);

    // пользовательский ввод размеров
    int rows = 0, columnes = 0;
    double widthOfTable = 0;
    QWidget *parentTableInsertWidget = new QWidget;
    parentTableInsertWidget->setWindowIcon(QIcon(":/res/Icons-file/insert-table-icon"));
    rows = QInputDialog::getInt(parentTableInsertWidget, *insertTableString, *insertNumRowString, rows, 1, 2147483647 , 1 , &ok);
    columnes = QInputDialog::getInt(parentTableInsertWidget, *insertTableString, *insertNumColumnsString, columnes, 1, 2147483647 , 1, &ok);
    widthOfTable = QInputDialog::getDouble(parentTableInsertWidget, *insertTableString, *insertWidthString, widthOfTable, 1, 2147483647 , 1, &ok);
    QVector<QTextLength> constraints;
    double widthoftextedit=uiPtr->textEdit->width();
    double percetageOfWidth=(widthOfTable/widthoftextedit) *100;
    if (percetageOfWidth>100){
        percetageOfWidth=100;
    }

    for(int i = 0; i<=columnes; i++){
        constraints << QTextLength(QTextLength::PercentageLength, percetageOfWidth/columnes);
    }

    tableFormat.setColumnWidthConstraints(constraints);
    uiPtr->textEdit->textCursor().insertTable(rows, columnes, tableFormat);
}

void TextEditor::slotInsertTableRows(){
    QTextTable *selectedTable;
    selectedTable=uiPtr->textEdit->textCursor().currentTable();
    QTextTableCell *index = new QTextTableCell;
    *index=uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().position());
    int numberOfInsertingRows = uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().position()).row() - uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().anchor()).row();
    if(numberOfInsertingRows < 0){
        numberOfInsertingRows=numberOfInsertingRows*(-1);
        selectedTable->insertRows(index->row()+numberOfInsertingRows+1, numberOfInsertingRows+1); // если идет выделение рядов снизу вверх
    }
    else{
        selectedTable->insertRows(index->row()+1, numberOfInsertingRows+1); // если идет выделение рядов сверху вниз
    }
}

void TextEditor::slotRemoveTableRows(){
    QTextTable *selectedTable;
    selectedTable=uiPtr->textEdit->textCursor().currentTable();
    QTextTableCell *index = new QTextTableCell;
    *index=uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().position());
    int numberOfRemovingRows = uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().position()).row() - uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().anchor()).row();
    if(numberOfRemovingRows <0){
        numberOfRemovingRows=numberOfRemovingRows*(-1);
        selectedTable->removeRows(index->row(), numberOfRemovingRows+1); // если идет выделение рядов снизу вверх
    }
    else
    {
        selectedTable->removeRows(index->row()-numberOfRemovingRows, numberOfRemovingRows+1); // если идет выделение рядов сверху вниз
    }
}

void TextEditor::slotInsertTableColumns(){
    QTextTable *selectedTable;
    selectedTable=uiPtr->textEdit->textCursor().currentTable();
    QTextTableFormat * selectedTableFormat = new QTextTableFormat;
    *selectedTableFormat=selectedTable->format();
    QVector <QTextLength> constraints = selectedTableFormat->columnWidthConstraints();
    double  tableWidth = ((constraints[1].rawValue() * selectedTable->columns())/100)*uiPtr->textEdit->width();
    double  columnWidth = (constraints[1].rawValue() /100)*uiPtr->textEdit->width();

    QTextTableCell *index = new QTextTableCell;
    *index=uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().position());
    int numberOfInsertingColumns = uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().position()).column() - uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().anchor()).column();
    if(numberOfInsertingColumns < 0){
        numberOfInsertingColumns=(numberOfInsertingColumns*(-1)) + 1;
        if((numberOfInsertingColumns * columnWidth < uiPtr->textEdit->width() - tableWidth) && (columnWidth <= uiPtr->textEdit->width() - tableWidth))  {

            selectedTable->insertColumns(index->column()+numberOfInsertingColumns, numberOfInsertingColumns); // если идет выделение столбцов справа налево
        }
    }
    else{
        numberOfInsertingColumns=numberOfInsertingColumns + 1;
        if((numberOfInsertingColumns * columnWidth < uiPtr->textEdit->width() - tableWidth) && (columnWidth <= uiPtr->textEdit->width() - tableWidth))  {
        selectedTable->insertColumns(index->column()+1, numberOfInsertingColumns); // если идет выделение столбцов слева направо
        }
    }
}
void TextEditor::slotRemoveTableColumns(){
    QTextTable *selectedTable;
    selectedTable=uiPtr->textEdit->textCursor().currentTable();
    QTextTableCell *index = new QTextTableCell;
    *index=uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().position());
    int numberOfRemovingColumns = uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().position()).column() - uiPtr->textEdit->textCursor().currentTable()->cellAt(uiPtr->textEdit->textCursor().anchor()).column();
    if(numberOfRemovingColumns <0){
        numberOfRemovingColumns=numberOfRemovingColumns*(-1);
        selectedTable->removeColumns(index->column(), numberOfRemovingColumns+1); // если идет выделение столбцов справа налево
    }
    else
    {
        selectedTable->removeColumns(index->column()-numberOfRemovingColumns, numberOfRemovingColumns+1); // если идет выделение столбцов слева направо
    }

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

void TextEditor::slotSearch()       // функция вызова окна для поиска текста
{
    searchWidget->setModal(true);
    searchWidget->show();   // показать окно
    m_searchHighLight = new SearchHighLight(uiPtr->textEdit->document());
}

void TextEditor::slotInsertFormula()
{
    QTextCursor cursor = uiPtr->textEdit->textCursor();
    QString formattedFormula = formulaWidget->insertFormula();

    QString selectedText = cursor.selectedText();
    if (selectedText.isEmpty())
    {
        cursor.insertHtml(formattedFormula + " ");
        cursor.movePosition(QTextCursor::Right);
    }
    else {
        cursor.removeSelectedText();
        cursor.insertHtml(formattedFormula + " ");
    }
}


void TextEditor::slotContextMenu(const QPoint& pos)
{
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


    if(uiPtr->textEdit->textCursor().charFormat().isImageFormat() && !(uiPtr->textEdit->textCursor().atStart()) && !(uiPtr->textEdit->textCursor().atBlockStart()) && (uiPtr->textEdit->textCursor().position()-uiPtr->textEdit->textCursor().anchor()) == 1) {
    QAction *resizeImage = new QAction(tr("Resize Image"), this);
    contextMenu->addAction(resizeImage);
    connect(resizeImage, &QAction::triggered, this, &TextEditor::slotResizeImage);

    }

    if(uiPtr->textEdit->textCursor().currentTable() != nullptr){
    contextMenu->addSeparator();
    QString *addRowString = new QString;
    QString *removeRowString = new QString;
    QString *addColumnString = new QString;
    QString *removeColumnString = new QString;
    if(localLanguage->languageToString(localLanguage->language())=="English"){
        *addRowString = "Add selected rows";
        *removeRowString = "Remove selected rows";
        *addColumnString = "Add selected columns";
        *removeColumnString = "Remove selected columns";
    }
    else{
        *addRowString = "Добавить выделенные строки";
        *removeRowString = "Удалить выделенные строки";
        *addColumnString = "Добавить выделенные столбцы";
        *removeColumnString = "Удалить выделенные столбцы";
    }
    QAction *addrows = new QAction(*addRowString, this);
    contextMenu->addAction(addrows);
    connect(addrows, &QAction::triggered, this, &TextEditor::slotInsertTableRows);

    QAction *addcolumns = new QAction(*addColumnString, this);
    contextMenu->addAction(addcolumns);
    connect(addcolumns, &QAction::triggered, this, &TextEditor::slotInsertTableColumns);

    QAction *removerows = new QAction(*removeRowString, this);
    contextMenu->addAction(removerows);
    connect(removerows, &QAction::triggered, this, &TextEditor::slotRemoveTableRows);

    QAction *removecolums = new QAction(*removeColumnString, this);
    contextMenu->addAction(removecolums);
    connect(removecolums, &QAction::triggered, this, &TextEditor::slotRemoveTableColumns);
    }

    // Отображаем контекстное меню в указанной позиции
    contextMenu->exec(uiPtr->textEdit->mapToGlobal(pos));
}

void TextEditor::slotSearchText(QString text)       // функция поиска текста
{
    m_searchHighLight->searchText(text);    // поиск текста
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

void TextEditor::slotEnglish()
{
    qApp->removeTranslator(&qtLanguageTranslator);
    uiPtr->retranslateUi(this);
    *loc=QLocale::English;
    *localLanguage=QLocale::English;

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
    connect(searchWidget, SIGNAL(signalSearchText(QString)), this, SLOT(slotSearchText(QString)));
    formulaWidget = new FormulaWidget;
}


void TextEditor::slotRussian()
{
    qApp->installTranslator(&qtLanguageTranslator);
    uiPtr->retranslateUi(this);
    *loc=QLocale::Russian;
    *localLanguage=QLocale::Russian;

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
    connect(searchWidget, SIGNAL(signalSearchText(QString)), this, SLOT(slotSearchText(QString)));
    formulaWidget = new FormulaWidget;
}

void TextEditor::slotHelp()
{   // Функция Help
    QDialog *helpWidget = new QDialog;
    helpWidget->setWindowIcon(QIcon(":/res/Icons-file/question"));
    helpWidget->setWindowTitle(tr("Help"));

    helpWidget->setStyleSheet("background-color: white;");
    QHBoxLayout *layout = new QHBoxLayout;
    QLabel *imageLabel = new QLabel;
    QPixmap image(":/res/Icons-file/question"); // Изображение
    imageLabel->setPixmap(image);
    layout->addWidget(imageLabel);

    QTextEdit *textEdit = new QTextEdit;
    textEdit->setText (tr("New file (Ctrl+N)\n\n"
                         "Open file (Ctrl+O)\n\n"
                         "Save file (Ctrl+S)\n\n"
                         "Save with new file name (Ctrl+Shift+S)\n\n"
                         "Print file (Ctrl+P)\n\n"
                         "Exit (Ctrl+F4)\n\n"
                         "Undo action (Ctrl+Z)\n\n"
                         "Repeat action (Ctrl+Y)\n\n"
                         "Copy (Ctrl+C)\n\n"
                         "Cut (Ctrl+X)\n\n"
                         "Paste (Ctrl+V)\n\n"
                         "Select all (Ctrl+A)\n\n"
                         "Applying bold text (Ctrl+B)\n\n"
                         "Applying italic text (Ctrl+I)\n\n"
                         "Underline text (Ctrl+U)\n"));
    textEdit->setReadOnly(true);
    textEdit->setFrameStyle(QFrame::NoFrame); // Удаление рамок
    layout->addWidget(textEdit);

    helpWidget->setLayout(layout);
    helpWidget->setModal(true);
    helpWidget->resize(500, 530);
    helpWidget->show();
}

void TextEditor::slotAbout()
{   // функция About

    QDialog *aboutWidget = new QDialog;
    aboutWidget->setWindowIcon(QIcon(":/res/Icons-file/info"));
    aboutWidget->setWindowTitle(tr("About"));

    aboutWidget->setStyleSheet("background-color: white;");
    QHBoxLayout *layout = new QHBoxLayout;
    QLabel *imageLabel = new QLabel;
    QPixmap image(":/res/Icons-file/info"); // Изображение
    imageLabel->setPixmap(image);
    layout->addWidget(imageLabel);

    QTextEdit *textEdit = new QTextEdit;
    textEdit->setText (tr("\n\n\nText Editor version 1.0\n\n"
                         "Command 1\n\n"
                         "© 2023 All rights reserved."));
    textEdit->setReadOnly(true);
    textEdit->setFrameStyle(QFrame::NoFrame); // Удаление рамок
    layout->addWidget(textEdit);

    aboutWidget->setLayout(layout);
    aboutWidget->setModal(true);
    aboutWidget->show();
}
void TextEditor::slotFileOpen()
{

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
