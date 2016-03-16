#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
    classRules();
    keywordRules();
    typeRules();
    symbolRules();
    numberRules();
    commentRules();
}

void Highlighter::highlightBlock(const QString &text) {
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

        int startIndex = 0;
        if (previousBlockState() != 1)
            startIndex = commentStartExpression.indexIn(text);

        while (startIndex >= 0) {
            int endIndex = commentEndExpression.indexIn(text, startIndex);
            int commentLength;
            if (endIndex == -1) {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = endIndex - startIndex
                                + commentEndExpression.matchedLength();
            }
            setFormat(startIndex, commentLength, multiLineCommentFormat);
            startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
        }
}

void Highlighter::classRules(){
    HighlightingRule rule;
    classFormat.setForeground(Qt::darkMagenta);
    classFormat.setFontWeight(QFont::Bold);
    QStringList classPatterns;
    classPatterns << "\\bactive\\b"
                  << "\\binit\\b"
                  << "\\bnever\\b"
                  << "\\btrace\\b"
                  << "\\bnotrace\\b"
                  << "\\bprovided\\b"
                  << "\\bpriority\\b";
    foreach (const QString &pattern, classPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = classFormat;
            highlightingRules.append(rule);
    }
}

void Highlighter::keywordRules(){
    HighlightingRule rule;
    keywordFormat.setForeground(Qt::darkRed);
    classFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bif\\b"
                    << "\\bfi\\b"
                    << "\\bdo\\b"
                    << "\\bod\\b"
                    << "\\b::\\b"
                    << "ltl"
                    << "\\batomic\\b"
                    << "\\bd_step\\b"
                    << "\\belse\\b"
                    << "\\bbreak\\b"
                    << "\\bprint\\b"
                    << "\\bskip\\b"
                    << "\\bgoto\\b"
                    << "\\bassert\\b"
                    << "\\beval\\b"
                    << "\\bhidden\\b"
                    << "\\bshow\\b"
                    << "\\btrue\\b"
                    << "\\bfalse\\b"
                    << "\\blen\\b"
                    << "\\btimeout\\b"
                    << "\\bnp_\\b"
                    << "\\bxr\\b"
                    << "\\bxs\\b";
    foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
    }
}

void Highlighter::commentRules(){
    HighlightingRule rule;
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void Highlighter::typeRules(){
    HighlightingRule rule;
    typeFormat.setForeground(Qt::blue);
    QStringList typePatterns;
    typePatterns << "\\bbit\\b"
                 << "\\bbyte\\b"
                 << "\\bbool\\b"
                 << "\\bpid\\b"
                 << "\\bshort\\b"
                 << "\\bint\\b"
                 << "\\bmtype\\b"
                 << "\\bchan\\b";
    foreach (const QString &pattern, typePatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
    }
}

void Highlighter::symbolRules() {
    HighlightingRule rule;
    symbolFormat.setForeground(Qt::darkBlue);
    QStringList symbolPatterns;
    symbolPatterns << "\\!"
                   << "\\?"
                   << "\\<"
                   << "\\>"
                   << "\\="
                   << "\\+"
                   << "\\-"
                   << "\\|"
                   << "\\*"
                   << "\\/"
                   << "\\%"
                   << "\\&"
                   << "\\^"
                   << "\\~"
                      ;
    foreach (const QString &pattern, symbolPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = symbolFormat;
            highlightingRules.append(rule);
    }
}

void Highlighter::numberRules() {
    HighlightingRule rule;
    numberFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("[0-9]+");
    rule.format = numberFormat;
    highlightingRules.append(rule);
}
