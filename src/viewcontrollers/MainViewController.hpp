#ifndef VIEWCONTROLLERS_MAINVIEWCONTROLLER_HPP
#define VIEWCONTROLLERS_MAINVIEWCONTROLLER_HPP

#include <QJsonObject>
#include <QObject>

#ifndef ANDROID
#include <QDBusInterface>
#include <QJsonArray>
#include <QList>
#endif

class MainViewController : public QObject {
  Q_OBJECT

#ifndef ANDROID
 public:
  MainViewController();

 public slots:
  void addOpendesktopAccount(QString appId, QString protocol, QString username,
                             QString password);
  void addCustomAccount(QString appId, QString protocol, QString url,
                        QString username, QString password);
  void syncAccount(QString appId);

 signals:
  void accountAdded(QString secret);
  void accountList(QJsonArray accounts);

 private:
  const QString MAUI_ACCOUNTS_DBUS_SERVICE_NAME = "org.mauikit.accounts";
  const QString MAUI_ACCOUNTS_DBUS_INTERFACE_NAME = "org.mauikit.accounts";
  const QString MAUI_ACCOUNTS_DBUS_INTERFACE_PATH = "/";
  QDBusInterface* mauiAccountsDBusInterface;

  QDBusInterface* dbusInterfaceFactory(QString service, QString interface,
                                       QString path);
  void addAccount(QString appId, QString protocol, QString url,
                  QString username, QString password);

  QString getManifestPath(QString appId);
  QString generateAccountName(QString username, QString host);
  QString getHostFromAccountName(QString accountName);
  QString getUsernameFromAccountName(QString accountName);
#else
 public slots:
  void addOpendesktopAccount(QString protocol, QString username,
                             QString password);
  void addCustomAccount(QString protocol, QString url, QString username,
                        QString password);
  void syncAccount(QString accountName);

 signals:
  void accountAdded();
  void accountList(QList<QString> accounts);

 private:
  void addAccount(QString protocol, QString url, QString username,
                  QString password, QString accountName);
  void showToast(QString text);
  void showIndefiniteProgressDialog(QString message, bool isCancelable);
  void hideIndefiniteProgressDialog();
#endif

 public slots:
  void getAccountList();
  void removeAccount(QString accountName);
  void showUrl(QString accountName);
};

#endif
