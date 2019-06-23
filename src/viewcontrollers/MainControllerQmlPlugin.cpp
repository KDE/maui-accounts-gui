#include "MainControllerQmlPlugin.hpp"

#include <qqml.h>
#include <QDebug>

MainViewController* mainviewcontroller = nullptr;

static QObject* mainviewcontroller_singleton_provider(QQmlEngine* engine,
                                                      QJSEngine* scriptEngine) {
  Q_UNUSED(engine)
  Q_UNUSED(scriptEngine)

  if (mainviewcontroller == nullptr) {
    mainviewcontroller = new MainViewController();
  }

  return mainviewcontroller;
}

void MainControllerQmlPlugin::registerTypes(const char* uri) {
  Q_ASSERT(uri == QLatin1String("org.mauikit.accounts"));
  qmlRegisterSingletonType<MainViewController>(
      uri, 1, 0, "Controller", mainviewcontroller_singleton_provider);
}
