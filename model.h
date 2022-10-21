#pragma once

#include <language/codecompletion/codecompletionmodel.h>

#include "service.h"

namespace Tabnine {

class CodeCompletionModel : public KTextEditor::CodeCompletionModel, public KTextEditor::CodeCompletionModelControllerInterface
{
    Q_OBJECT
    Q_INTERFACES(KTextEditor::CodeCompletionModelControllerInterface)

public:
    explicit CodeCompletionModel(QObject* parent);

    KTextEditor::Range updateCompletionRange(KTextEditor::View *view, const KTextEditor::Range &range) override;
    void aborted(KTextEditor::View *view) override;
    void executeCompletionItem(KTextEditor::View *view, const KTextEditor::Range &word, const QModelIndex &index) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    void setLastResult(Result &&result);

    Service m_service;
    QString m_typedPrefix;
    Result m_lastResult;
};

}
