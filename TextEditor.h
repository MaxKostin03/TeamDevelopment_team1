#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class TextEditor; }
QT_END_NAMESPACE

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

private:
    Ui::TextEditor *uiPtr;
    QMenu *menuConfig();
    QMenu *editMenu();
    QMenu *formatMenu();
    QMenu *insertMenu();
    QMenu *viewMenu();

};
#endif // TEXTEDITOR_H
