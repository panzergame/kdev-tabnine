#include "resultentry.h"

#include <debug.h>

namespace Tabnine
{

ResultEntry::ResultEntry(const QString& newPrefix, const QString& oldSuffix, const QString& newSuffix, int score)
    :m_newPrefix(newPrefix),
    m_oldSuffix(oldSuffix),
    m_newSuffix(newSuffix),
    m_score(score)
{
}

const QString &ResultEntry::newPrefix() const
{
    return m_newPrefix;
}

const QString &ResultEntry::oldSuffix() const
{
    return m_oldSuffix;
}

const QString &ResultEntry::newSuffix() const
{
    return m_newSuffix;
}

QString ResultEntry::newText() const
{
    return m_newPrefix + m_newSuffix;
}

bool ResultEntry::operator>(const ResultEntry& other) const
{
    return m_score > other.m_score;
}

Result::Result(const QString& oldPrefix, QVector<ResultEntry> &&entries)
    :m_oldPrefix(oldPrefix),
    m_entries(entries)
{
    std::sort(m_entries.begin(), m_entries.end(), std::greater<ResultEntry>());
}

int Result::size() const
{
    return m_entries.size();
}

QString Result::name(const QString& typedPrefix, int index) const
{
    const int chopSize = std::min(m_oldPrefix.size(), typedPrefix.size());
    const ResultEntry &entry = m_entries[index];
    return typedPrefix.chopped(chopSize) + entry.newText();
}

constexpr KTextEditor::Cursor columnOffsetedCursor(const KTextEditor::Cursor &origin, int offset)
{
    return origin + KTextEditor::Cursor(0, offset);
}

void Result::execute(KTextEditor::View* view, const KTextEditor::Range& word, int index) const
{
    KTextEditor::Document *document = view->document();
    const ResultEntry &entry = m_entries[index];

    const KTextEditor::Cursor oldWordEndCursor = word.end();

    const KTextEditor::Cursor startCursor = columnOffsetedCursor(oldWordEndCursor, -m_oldPrefix.size());
    const KTextEditor::Cursor endCursor = columnOffsetedCursor(oldWordEndCursor, entry.oldSuffix().size());
    const KTextEditor::Range replacedRange(startCursor, endCursor);
    document->replaceText(replacedRange, entry.newText());

    const int cursorOffsetFromStart = entry.newPrefix().size();
    const KTextEditor::Cursor newWordEndCursor = columnOffsetedCursor(endCursor, cursorOffsetFromStart);
    view->setCursorPosition(newWordEndCursor);
}

}
