#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>
#include <QMenu>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QTranslator>

QT_BEGIN_NAMESPACE
namespace Ui { class TextEditor; }
QT_END_NAMESPACE

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

private slots:
    void slotRenameTitle(QString newName);
    void slotFileNew();
    void slotFileOpen();
    void slotFileSave();
    void slotFileSaveAs();
    void slotPrintFile();
    void slotExitFile();
    void slotUndo();
    void slotRedo();
    void slotCopy();
    void slotCut();
    void slotPaste();
    void slotSelectAll();
    void slotBold();
    void slotItalic();
    void slotUnderlined();
    void slotCrossedOut();
    void slotFontStyle();
    void slotFontColor();
    void slotInsertImage();
    void slotDarkMode();
    void slotLightMode();
    void slotEnglish();
    void slotRussian();
    void slotHelp();
    void slotAbout();
    void closeEvent(QCloseEvent *event);
    bool hasUnsavedChanges();

    void onRedColorButtonClicked();
    void onOrangeColorButtonClicked();
    void onYellowColorButtonClicked();
    void onGreenColorButtonClicked();
    void onWhiteColorButtonClicked();
    void onBlueColorButtonClicked();
    void onPurpleColorButtonClicked();
    void onBlackColorButtonClicked();


private:
    Ui::TextEditor *uiPtr;
    QString file_path = "";
    QMenu *menuConfig();
    QMenu *editMenu();
    QMenu *formatMenu();
    QMenu *insertMenu();
    QMenu *viewMenu();
    QMenu *languageMenu();
    QAction *help();
    QAction *about();
    QToolBar *toolbar();

    bool isFileSaved = true;

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

    void setPaletteColors();
    void createColorPalette(qint32 x ,qint32 y , qint32 width=200 , qint32 height=100);
    void hidePalette(QWidget *window);
    void showPalette(QWidget *window);


    QTranslator qtLanguageTranslator;
};
#endif // TEXTEDITOR_H
