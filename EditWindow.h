#ifndef EDITWINDOW_H
#define EDITWINDOW_H

//#include "TextEditor.h"
#include <QTextEdit>

class EditWindow : public QTextEdit
{
    Q_OBJECT

public:

    EditWindow(QWidget *parent = nullptr);
    ~EditWindow();

    QString currentFile() { return curFile; }
    QString userFriendlyCurrentFile();

    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    void newFile();

    /*
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void documentWasModified();
    */

private:

    bool maybeSave();

    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    bool isUntitled;

};

#endif // EDITWINDOW_H
