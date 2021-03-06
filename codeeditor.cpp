/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include "codeeditor.h"


CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
    lineNumberArea = new LineNumberArea(this);
    colorList << QColor(0, 0, 255, 127)
              << QColor(255, 0, 0, 127)
              << QColor(0, 255, 0, 127)
              << QColor(255,255,0,127)
              << QColor(0,255,255,127)
              << QColor(255,0,255,127);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateHighlights()));

    updateLineNumberAreaWidth(0);
    updateHighlights();
}

int CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 10 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

QList<QTextEdit::ExtraSelection> CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::gray).lighter(140);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    return extraSelections;
}

QList<QTextEdit::ExtraSelection> CodeEditor::errorSelections() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    foreach(QString lineNo, errors) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::red);
        QTextCursor cursor(this->document());

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection,true);

        for (int i=1 ; i<lineNo.toInt() ; i++) {
            cursor.movePosition(QTextCursor::Down);
        }

        selection.cursor = cursor;
        extraSelections.append(selection);
    }
    return extraSelections;
}

void CodeEditor::HighlightErrorLines(QStringList lineNos) {
    errors = lineNos;
    updateHighlights();
}

QList<QTextEdit::ExtraSelection> CodeEditor::procesSelections() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    for (int i = 0 ; i<procList.length() ; i++) {
        if (procList[i].line>0) {
            QTextEdit::ExtraSelection selection;
            QColor lineColor = colorList[procList[i].id%colorList.length()];
            QTextCursor cursor(this->document());

            selection.format.setBackground(lineColor);
            selection.format.setProperty(QTextFormat::FullWidthSelection,true);

            for (int j=1 ; j<procList[i].line ; j++) {
                cursor.movePosition(QTextCursor::Down);
            }

            selection.cursor = cursor;
            extraSelections.append(selection);
        }
    }
    return extraSelections;
}

void CodeEditor::HighlightProcesses(QList<SimulationRun::proc> procs) {
    procList=procs;
    updateHighlights();
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    QColor rectColor = QColor(Qt::gray).lighter(140);
    painter.fillRect(event->rect(), rectColor);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width()-3, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::setWordWrap(bool b){
    if (b)  setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    else    setWordWrapMode(QTextOption::NoWrap);
}

void CodeEditor::updateHighlights(){
    QList<QTextEdit::ExtraSelection> selections = highlightCurrentLine()+procesSelections()+errorSelections();
    setExtraSelections(selections);
}

void CodeEditor::clear() {
    errors.clear();
    procList.clear();
    updateHighlights();
    QPlainTextEdit::clear();
}
