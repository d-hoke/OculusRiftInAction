#include <QtCommon.h>
#include "CodeEditor.h"

CodeEditor::CodeEditor(QQuickItem* parent) : QQuickItem(parent) {
}

CodeEditor::~CodeEditor() {
}

void CodeEditor::setHighlighter(QSyntaxHighlighter* highlighter) {
    _highlighter = highlighter;
    //if (editorControl) {
    //    highlighter.setDocument(
    //        editorControl->property("textDocument").value<QQuickTextDocument*>()->textDocument());
    //}
}

void CodeEditor::setText(const QString& text) {
    getTextArea()->setProperty("text", text);
}

void CodeEditor::setErrorText(const QString& text) {
    getErrorTextArea()->setProperty("text", text);
}

QQuickItem* CodeEditor::getTextArea() {
    QQuickItem* textArea = findChild<QQuickItem*>("textEdit");
    return textArea;
}

QQuickItem* CodeEditor::getErrorTextArea() {
    QQuickItem* textArea = findChild<QQuickItem*>("compileErrors");
    return textArea;
}
