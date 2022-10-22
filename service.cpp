#include "service.h"

#include <debug.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

namespace Tabnine
{

Service::Service()
{
    setupProcess();
    m_process.start();
    m_process.readAllStandardOutput();
}

Service::~Service()
{
    m_process.terminate();
    m_process.waitForFinished();
}

Result Service::resultEntries(const TruncableString &prefix, const TruncableString& suffix)
{
    if (m_process.state() == QProcess::Running) {
        const int correlationId = m_queryIndex++;
        const QByteArray query = buildQuery(prefix, suffix, correlationId);
        qInfo(KDEV_TABNINE).noquote() << query;
        const QByteArray response = fetchQuery(query);
        qInfo(KDEV_TABNINE).noquote() << response;

        return parseResponseToResult(response, correlationId);
    }

    qCritical(KDEV_TABNINE) << "tabnine process not running";
    return Result();
}

void Service::setupProcess()
{
    m_process.setProgram(findTabNineExecutable());
}

QString Service::findTabNineExecutable() const
{
    const QStringList &sharePaths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    for (const QString& path : sharePaths) {
        const QString searchPath = path + QDir::separator() + "kdevtabnine";
        const QString tabNinePath = QStandardPaths::findExecutable(QStringLiteral("TabNine"), {searchPath});
        if (!tabNinePath.isEmpty()) {
            qInfo(KDEV_TABNINE)  << "TabNine found at" << tabNinePath;
            return tabNinePath;
        }
    }

    qInfo(KDEV_TABNINE) << "TabNine not found";
    return "";
}

QByteArray Service::buildQuery(const TruncableString &prefix, const TruncableString& suffix, int correlationId) const
{
    const QJsonObject autocompleteNode{
        {QStringLiteral("before"), prefix.value().toString()},
        {QStringLiteral("after"), suffix.value().toString()},
        {QStringLiteral("region_includes_beginning"), !prefix.truncated()},
        {QStringLiteral("region_includes_end"), !suffix.truncated()},
        {QStringLiteral("filename"), QStringLiteral("")}, // TODO
        {QStringLiteral("correlation_id"), correlationId} // TODO
    };
    
    const QJsonObject requestNode{
        {QStringLiteral("Autocomplete"), autocompleteNode}
    };

    const QJsonObject rootNode{
        {QStringLiteral("version"), QStringLiteral("1.0.0")},
        {QStringLiteral("request"), requestNode}
    };

    const QJsonDocument doc(rootNode);
    return doc.toJson(QJsonDocument::Compact);
}

QByteArray Service::fetchQuery(const QByteArray &query)
{
    m_process.write(query);
    m_process.write("\n");
    m_process.waitForReadyRead();
    return m_process.readLine();
}

int percentageStringToInt(const QString &repr)
{
    const QString intRepr = repr.chopped(1);
    return intRepr.toInt();
}

int parseEntryScore(const QJsonValue &detailNode, int defaultScore)
{
    return (detailNode.isString()) ? percentageStringToInt(detailNode.toString()) : defaultScore;
}

ResultEntry Service::parseResponseToEntry(const QJsonObject &item) const
{
    const QString newPrefix = item[QStringLiteral("new_prefix")].toString();
    const QString oldSuffix = item[QStringLiteral("old_suffix")].toString();
    const QString newSuffix = item[QStringLiteral("new_suffix")].toString();
    const QJsonValue detailNode = item[QStringLiteral("detail")];
    const int score = parseEntryScore(detailNode, 100);

    return ResultEntry(newPrefix, oldSuffix, newSuffix, score);
}

Result Service::parseResponseToResult(const QByteArray &response, int expectedCorrelationId) const
{
    const QJsonDocument doc = QJsonDocument::fromJson(response);
    const QJsonObject rootNode = doc.object();

    const int correlationId = rootNode[QStringLiteral("correlation_id")].toInt();
    if (correlationId != expectedCorrelationId) {
        qCritical(KDEV_TABNINE) << "correlation id mismatch, got" << correlationId << "expected" << expectedCorrelationId;
        return Result();
    }

    const QString oldPrefix = rootNode[QStringLiteral("old_prefix")].toString();
    
    const QJsonArray resultsArray = rootNode[QStringLiteral("results")].toArray();
    QVector<ResultEntry> entries(resultsArray.size());
    std::transform(resultsArray.cbegin(), resultsArray.cend(), entries.begin(), [this](const QJsonValue &entryValue){
        const QJsonObject &entryNode = entryValue.toObject();
        return parseResponseToEntry(entryNode);
    });

    return Result(oldPrefix, std::move(entries));
}

}
