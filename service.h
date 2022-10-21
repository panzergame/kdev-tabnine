#pragma once

#include "resultentry.h"
#include "truncablestring.h"

#include <QProcess>

namespace Tabnine {

class Service
{
public:
    explicit Service();

    Result resultEntries(const TruncableString &prefix, const TruncableString& suffix);
    
private:
    void setupProcess();

    QByteArray buildQuery(const TruncableString &prefix, const TruncableString& suffix, int correlationId) const;
    QByteArray fetchQuery(const QByteArray &query);
    ResultEntry parseResponseToEntry(const QJsonObject &item) const;
    Result parseResponseToResult(const QByteArray &response, int expectedCorrelationId) const;

    QProcess m_process;
    int m_queryIndex = 0;
};

}
