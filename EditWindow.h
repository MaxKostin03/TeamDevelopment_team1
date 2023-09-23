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

    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    void newFile();

private slots:

    void documentWasModified();

public:

    QString currentFile() const { return curFile; };
    QString userFriendlyCurrentFile();

    bool maybeSave();
    bool getUntitled () const { return isUntitled; };

    void printFile();
    void printPdf();

private:

    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    bool isUntitled;

};

#endif // EDITWINDOW_H
