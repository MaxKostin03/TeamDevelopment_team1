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
//#include <QtWebView>


#include "ui_TextEditor.h"
#include "SearchHighLight.h"
#include "SearchWidget.h"
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


    // bool hasUnsavedChanges(); // не требуется

private slots:

    void slotRenameTitle();
    void slotFileNew();
    void slotFileOpen();
    void slotFileSave();
    void slotFileSaveAs();
    void slotExportToPdf();
    void slotPrintFile();
    //void slotExitFile(); // не требуется
    void slotUndo();
    void slotRedo();
    void slotCopy();
    void slotCut();
    void slotPaste();
    void slotSelectAll();
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


    void onRedColorButtonClicked();
    void onOrangeColorButtonClicked();
    void onYellowColorButtonClicked();
    void onGreenColorButtonClicked();
    void onWhiteColorButtonClicked();
    void onBlueColorButtonClicked();
    void onPurpleColorButtonClicked();
    void onBlackColorButtonClicked();

    void updateRecentFileActions();
    void openRecentFile();
    void openCalendar();

private:

    enum { MaxRecentFiles = 5 };

    Ui::TextEditor *uiPtr;

    // QString file_path = ""; // не требуется
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

    // bool isFileSaved = true; // не требуется

    QWidget *window = NULL;
    QGroupBox *gridGroupBox;
    QGridLayout *colorPalette;
    QPushButton *redColorButton;
    QPushButton *orangeColorButton;
    QPushButton *yellowColorButton;
    QPushButton *greenColorButton;
    QPushButton *whiteColorButton;
    QPushButton *blueColorButton;
    QPushButton *purpleColorButton;
    QPushButton *blackColorButton;

    void createStatusBar();
    void readSettings();
    void writeSettings();

    QColor selectColor(QString QColorTitleName, QString WindowIconPath);
    void setPaletteColors();
    void createColorPalette(qint32 x ,qint32 y , qint32 width=200 , qint32 height=100);
    void hidePalette(QWidget *window);
    void showPalette(QWidget *window);


    QTranslator qtLanguageTranslator;

    SearchHighLight* m_searchHighLight;
    SearchWidget *searchWidget;
    //CalendarWidget *calendarWidget;
};



#endif // TEXT_EDITOR_H
