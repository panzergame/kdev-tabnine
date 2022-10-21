#include "tabnineplugin.h"

#include "model.h"

#include <language/codecompletion/codecompletion.h>

#include <KPluginFactory>

#include <debug.h>

K_PLUGIN_FACTORY_WITH_JSON(TabninePluginFactory, "kdevtabnine.json", registerPlugin<TabninePlugin>(); )

TabninePlugin::TabninePlugin(QObject *parent, const QVariantList & /*args*/)
    :IPlugin(QStringLiteral("kdevtabnine"), parent)
{
    qInfo(KDEV_TABNINE) << "Started !";
    
    KTextEditor::CodeCompletionModel* codeCompletion = new Tabnine::CodeCompletionModel(this);
    new KDevelop::CodeCompletion(this, codeCompletion, QStringLiteral(""));
}

TabninePlugin::~TabninePlugin()
{
}

// needed for QObject class created from K_PLUGIN_FACTORY_WITH_JSON
#include "tabnineplugin.moc"

