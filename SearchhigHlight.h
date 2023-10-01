#ifndef SEARCH_HIGH_LIGHT_H
#define SEARCH_HIGH_LIGHT_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

// *** class SearchHighLight

class SearchHighLight : public QSyntaxHighlighter
{
    Q_OBJECT
    using BaseClass = QSyntaxHighlighter;

public:

    SearchHighLight(QTextDocument* parent = nullptr);
    void searchText(const QString& text);

protected:

    virtual void highlightBlock(const QString &text) override;

private:

    QRegularExpression m_pattern; // Regular expression to search for, in our case, this word or text
    QTextCharFormat m_format; // Text formatting, highlighting

};

#endif // SEARCH_HIGH_LIGHT_H
