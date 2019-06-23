#ifndef VIEWCONTROLLERS_MAINCONTROLLERQMLPLUGIN_HPP
#define VIEWCONTROLLERS_MAINCONTROLLERQMLPLUGIN_HPP

#include "MainViewController.hpp"

#include <QObject>
#include <QQmlExtensionPlugin>

class MainControllerQmlPlugin : public QQmlExtensionPlugin {
 Q_OBJECT
 Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

 public:
   void registerTypes(const char *uri) override;
};

#endif
