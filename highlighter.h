#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

class Highlighter : public QSyntaxHighlighter {

    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat classFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat typeFormat;
    QTextCharFormat symbolFormat;
    QTextCharFormat numberFormat;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    void classRules();
    void keywordRules();
    void commentRules();
    void typeRules();
    void symbolRules();
    void numberRules();
};

#endif // HIGHLIGHTER_H
