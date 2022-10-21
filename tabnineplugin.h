#pragma once

#include <interfaces/iplugin.h>
#include <QVariantList>

class TabninePlugin : public KDevelop::IPlugin
{
    Q_OBJECT
public:
    explicit TabninePlugin(QObject *parent, const QVariantList &args = QVariantList());
    ~TabninePlugin() override;
};
