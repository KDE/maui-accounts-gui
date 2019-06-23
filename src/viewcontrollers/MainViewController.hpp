#ifndef VIEWCONTROLLERS_MAINVIEWCONTROLLER_HPP
#define VIEWCONTROLLERS_MAINVIEWCONTROLLER_HPP

#include <QJsonObject>
#include <QObject>

#ifndef ANDROID
#include <QDBusInterface>
#endif

class MainViewController : public QObject {
  Q_OBJECT

#ifndef ANDROID
 private:
  const QString MAUI_ACCOUNTS_DBUS_SERVICE_NAME = "org.mauikit.accounts";
  const QString MAUI_ACCOUNTS_DBUS_INTERFACE_NAME = "org.mauikit.accounts";
  const QString MAUI_ACCOUNTS_DBUS_INTERFACE_PATH = "/";

  QDBusInterface *dbusInterfaceFactory(QString service, QString interface,
                                       QString path);
#endif

 public slots:
  void addOpendesktopAccount(QString protocol, QString username,
                             QString password);
  void addCustomAccount(QString protocol, QString url, QString username,
                        QString password);
  void getAccountList();
  void removeAccount(QString accountName);
  void syncAccount(QString accountName);
  void showUrl(QString accountName);

 signals:
  void accountList(QList<QString> accounts);
  void accountAdded();

 private:
  void addAccount(QString protocol, QString url, QString username,
                  QString password, QString accountName);
  void showToast(QString text);
  void showIndefiniteProgressDialog(QString message, bool isCancelable);
  void hideIndefiniteProgressDialog();
};

#endif
