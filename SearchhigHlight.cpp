#include "SearchhigHlight.h"

SearchHighLight::SearchHighLight(QTextDocument* parent)
: BaseClass(parent)
{
    m_format.setBackground(Qt::green);
}

void SearchHighLight::highlightBlock(const QString& text)
{
    QRegularExpressionMatchIterator matchIterator = m_pattern.globalMatch(text);
    while (matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_format);
    }
}

void SearchHighLight::searchText(const QString& text)
{
    m_pattern = QRegularExpression(text);
    rehighlight(); // Перезапускаем подсветку
}
