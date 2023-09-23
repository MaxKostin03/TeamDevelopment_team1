#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "TextEditor.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("Editor #1");
    QCoreApplication::setOrganizationName("Team #1");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    //QApplication::setWindowIcon(QIcon(":/Icons-file/TE_256.png")); // Иконка самого приложения
    app.setDesktopSettingsAware(true);

    QCommandLineParser parser;
    parser.setApplicationDescription("[Qt] Editor #1");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    parser.process(app);

    TextEditor textEditor;
    const QStringList posArgs = parser.positionalArguments();
    for (const QString &fileName : posArgs)
    {
        textEditor.loadFile(fileName);
        break;
    }
    //textEditor.showMaximized(); //полный экран
    textEditor.show(); //полный экран



    return app.exec();
}
