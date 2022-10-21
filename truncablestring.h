#pragma once

#include <QString>

namespace Tabnine {

class TruncableString
{
public:
    enum class Direction {
        Left,
        Right
    };

    explicit TruncableString(const QString &value, Direction direction, int limit);

    bool truncated() const;
    QStringRef value() const;

private:
    QString m_value;
    Direction m_direction;
    int m_limit;
    bool m_truncated;
};

}
