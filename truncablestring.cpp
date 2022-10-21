#include "truncablestring.h"

namespace Tabnine {

TruncableString::TruncableString(const QString &value, Direction direction, int limit)
    :m_value(value),
    m_direction(direction),
    m_limit(limit),
    m_truncated(limit < value.length())
{
}

bool TruncableString::truncated() const
{
    return m_truncated;
}

QStringRef TruncableString::value() const
{
    if (m_truncated) {
        switch (m_direction) {
            case Direction::Left:
            {
                return m_value.leftRef(m_limit);
            }
            case Direction::Right:
            {
                return m_value.rightRef(m_limit);
            }
        }
    }

    return QStringRef(&m_value);
}

}
