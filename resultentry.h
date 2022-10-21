#pragma once

#include <QString>
#include <QVector>

#include <KTextEditor/View>

namespace Tabnine {

class ResultEntry
{
public:
    explicit ResultEntry(const QString &newPrefix, const QString &oldSuffix, const QString &newSuffix, int score);
    ResultEntry() = default;

    const QString &newPrefix() const;
    const QString &oldSuffix() const;
    const QString &newSuffix() const;
    QString newText() const;

    bool operator>(const ResultEntry& other) const;

private:
    QString m_newPrefix;
    QString m_oldSuffix;
    QString m_newSuffix;
    int m_score;
};

class Result
{
public:
    explicit Result(const QString &oldPrefix, QVector<ResultEntry> &&entries);
    Result() = default;

    int size() const;
    QString name(const QString &typedPrefix, int index) const;
    void execute(KTextEditor::View *view, const KTextEditor::Range &word, int index) const;

private:
    QString m_oldPrefix;
    QVector<ResultEntry> m_entries;
};

}
