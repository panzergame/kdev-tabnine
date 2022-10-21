#include "model.h"

#include <debug.h>

#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KTextEditor/Cursor>

namespace Tabnine
{

CodeCompletionModel::CodeCompletionModel(QObject* parent)
    : KTextEditor::CodeCompletionModel(parent)
{

}

KTextEditor::Range CodeCompletionModel::updateCompletionRange(KTextEditor::View *view, const KTextEditor::Range &range)
{
    KTextEditor::Document *document = view->document();

    const KTextEditor::Cursor cursorPosition = range.end();
    m_typedPrefix = document->text(range);
    qInfo(KDEV_TABNINE) << "prefix:" << m_typedPrefix;

    const KTextEditor::Range startToPosition(KTextEditor::Cursor::start(), cursorPosition);
    const KTextEditor::Range positionToEnd(cursorPosition, document->documentEnd());
    
    const QString textBefore = document->text(startToPosition);
    const QString textAfter = document->text(positionToEnd);
    
    constexpr int textLimit = 100000;
    const TruncableString truncableTextBefore(textBefore, TruncableString::Direction::Right, textLimit);
    const TruncableString truncableTextAfter(textAfter, TruncableString::Direction::Left, textLimit);
    
    setLastResult(m_service.resultEntries(truncableTextBefore, truncableTextAfter));

    return range;
}

void CodeCompletionModel::aborted(KTextEditor::View *view)
{
    setLastResult(Result());
}

void CodeCompletionModel::executeCompletionItem(KTextEditor::View *view, const KTextEditor::Range &word, const QModelIndex &index) const
{
    const int itemId = index.row();
    m_lastResult.execute(view, word, itemId);
}

QVariant CodeCompletionModel::data(const QModelIndex& index, int role) const
{
//     qInfo() << "data" << index << role;

    const int itemId = index.row();
    
    switch (role) {
        case Qt::DisplayRole:
        {
            switch (index.column()) {
                case CodeCompletionModel::Name:
                {
                    const QString name = m_lastResult.name(m_typedPrefix, itemId);
                    return name;
                }
                /*default:
                {
                    return QVariant(entry.newPrefix());
                }*/
            }
            break;
        }
            /*case CodeCompletionModel::BestMatchesCount:
            {}
            return QVariant(normalBestMatchesCount);
        case CodeCompletionModel::IsExpandable:
            return QVariant(createsExpandingWidget());
        case CodeCompletionModel::ExpandingWidget: {
            QWidget* nav = createExpandingWidget(model);
            Q_ASSERT(nav);

            QVariant v;
            v.setValue<QWidget*>(nav);
            return v;
        }*/
        case CodeCompletionModel::ScopeIndex:
        {
            return static_cast<int>(reinterpret_cast<quintptr>(this));
        }
        case CodeCompletionModel::CompletionRole:
        {
            return CodeCompletionModel::GlobalScope;
        }
        case CodeCompletionModel::InheritanceDepth:
        {
            return 0;
        }
        case CodeCompletionModel::UnimportantItemRole:
        {
            return 0;
        }
    }

//     qInfo() << "unimplemented" << index << role;
    return QVariant();
}

void CodeCompletionModel::setLastResult(Result &&result)
{
    beginResetModel();

    m_lastResult = result;
    setRowCount(m_lastResult.size());

    endResetModel();
}

}
