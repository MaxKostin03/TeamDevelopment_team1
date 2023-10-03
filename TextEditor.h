#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <QMainWindow>
#include <QMenu>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QTranslator>
#include <QLocale>


#include "ui_TextEditor.h"
#include "SearchHighLight.h"
#include "SearchWidget.h"
#include "formulawidget.h"
//#include "CalendarWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TextEditor; }
//class CalendarWidget;
QT_END_NAMESPACE


// *** class TextEditor

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

    bool loadFile(const QString&);
    QLocale *loc;

private slots:

    void slotRenameTitle();
    void slotFileNew();
    void slotFileOpen();
    void slotFileSave();
    void slotFileSaveAs();
    void slotExportToPdf();
    void slotPrintFile();
    void slotUndo();
    void slotRedo();
    void slotCopy();
    void slotCut();
    void slotPaste();
    void slotSelectAll();
    void slotResizeImage();
    void slotDelete();
    void slotBold();
    void slotItalic();
    void slotUnderlined();
    void slotCrossedOut();
    void slotFontStyle();
    void slotFontColor();
    void slotLeftSide();
    void slotInTheCenter();
    void slotRightSide();
    void slotTextBackgroundColor();
    void slotInsertImage();
    void slotInsertFormula();
    void slotDarkMode();
    void slotLightMode();
    void slotEnglish();
    void slotRussian();
    void slotHelp();
    void slotAbout();
    void slotSearch();
    void slotContextMenu(const QPoint& pos);
    void slotSearchText(QString text);
    void closeEvent(QCloseEvent *event);

    void updateRecentFileActions();
    void openRecentFile();
    void openCalendar();

private:

    enum { MaxRecentFiles = 5 };

    Ui::TextEditor *uiPtr;

    void createMenu();
    QMenu *menuConfig();
    QMenu *recentMenu;
    QMenu *editMenu();
    QMenu *formatMenu();
    QMenu *insertMenu();
    QMenu *viewMenu();
    QMenu *languageMenu();
    QAction *help();
    QAction *about();
    QToolBar *toolbar();

    QAction *recentFileActs[MaxRecentFiles];
    QAction *recentFileSeparator;
    QAction *recentFileSubMenuAct;

    static bool hasRecentFiles();
    void prependToRecentFiles(const QString& );
    void setRecentFilesVisible(bool );
    void createStatusBar();
    void readSettings();
    void writeSettings();

    QColor selectColor(QString QColorTitleName, QString WindowIconPath);

    QTranslator qtLanguageTranslator;

    SearchHighLight* m_searchHighLight;
    SearchWidget *searchWidget;
    FormulaWidget *formulaWidget;
};



#endif // TEXT_EDITOR_H
