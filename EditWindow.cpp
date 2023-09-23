#include <QtWidgets>
#include <QFileInfo>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>

#include "EditWindow.h"

EditWindow::EditWindow(QWidget *parent)
    : QTextEdit(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    //isUntitled = true;
    newFile();

    connect(document(), &QTextDocument::contentsChanged,
            this, &EditWindow::documentWasModified);
}

EditWindow::~EditWindow()
{
    // Выполняем необходимые действия перед уничтожением объекта
}

void EditWindow::documentWasModified()
{
    setWindowModified(document()->isModified());
}

QString EditWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

QString EditWindow::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void EditWindow::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    //setWindowTitle(userFriendlyCurrentFile() + "[*]");
}


bool EditWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Text editor"),
                             tr("Cannot read file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    setText(in.readAll());
    QGuiApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

//    connect(document(), &QTextDocument::contentsChanged,
//            this, &EditWindow::documentWasModified);

    return true;
}


bool EditWindow::maybeSave()
{
    if (!document()->isModified())
        return true;
    QMessageBox customBox(QMessageBox::Warning, tr("Text editor"),
                          tr("'%1' has been modified.\nDo you want to save your changes?")
                              .arg(userFriendlyCurrentFile()), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                          this);

    customBox.setButtonText(QMessageBox::Save, tr("Save"));
    customBox.setButtonText(QMessageBox::Discard, tr("Discard"));
    customBox.setButtonText(QMessageBox::Cancel, tr("Cancel"));

    customBox.setDefaultButton(QMessageBox::Save);
    int ret = customBox.exec();
    switch (ret)
    {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }

    return true;
}

bool EditWindow::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }

}

bool EditWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool EditWindow::saveFile(const QString &fileName)
{
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << toHtml(); // toPlainText();
        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                               .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("Text editor"), errorMessage);
        return false;
    }

    setCurrentFile(fileName);

    return true;
}


void EditWindow::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.txt").arg(sequenceNumber++);
    //setWindowTitle(curFile + "[*]");


}

void EditWindow::printFile() {

    QPrinter printer;
    QPrintDialog dlg(&printer, this);

    dlg.setWindowTitle("Print");
    if (dlg.exec() != QDialog::Accepted)
    {
        return;     // если отмена печати - выйти из функции
    }
    print(&printer);
}

void EditWindow::printPdf() {

    QString fileName = QFileDialog::getSaveFileName((QWidget*)0, tr("Export PDF"), QString(), "*.pdf");

    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName.append(".pdf");
    }

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A4);
    printer.setOutputFileName(fileName);

    print(&printer);
}
