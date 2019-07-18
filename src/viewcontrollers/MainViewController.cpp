#include "MainViewController.hpp"

#ifdef ANDROID
#include "../libccdav/lib/CardDAV.hpp"
#include "../libccdav/lib/utils/CardDAVReply.hpp"

#include <jni.h>
#include <QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>
#else
#include <QDBusReply>
#include <QFile>
#include <QQmlExtensionPlugin>
#include <QTimer>
#include <QtConcurrent>
#endif

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QStandardPaths>

#ifdef ANDROID
void MainViewController::addOpendesktopAccount(QString protocol,
                                               QString username,
                                               QString password) {
  QString accountName = username + " - OpenDesktop";
  QString url =
      "https://cloud.opendesktop.cc/remote.php/dav/addressbooks/users/" +
      username + "/contacts";

  addAccount(protocol, url, username, password, accountName);
}

void MainViewController::addCustomAccount(QString protocol, QString url,
                                          QString username, QString password) {
  QString accountName = username + " - " + QUrl(url).host();

  addAccount(protocol, url, username, password, accountName);
}

void MainViewController::getAccountList() {
  QAndroidJniEnvironment env;
  QList<QString> accounts;

  QAndroidJniObject accountsArrayJniObject =
      QAndroidJniObject::callStaticObjectMethod(
          "org/mauikit/accounts/MainActivity", "getAccounts",
          "()[[Ljava/lang/String;");

  jobjectArray accountsJniArray = accountsArrayJniObject.object<jobjectArray>();
  int len = env->GetArrayLength(accountsJniArray);

  for (int i = 0; i < len; i++) {
    jobjectArray array = static_cast<jobjectArray>(
        env->GetObjectArrayElement(accountsJniArray, i));
    jstring jAccountName =
        static_cast<jstring>(env->GetObjectArrayElement(array, 0));
    jstring jCount = static_cast<jstring>(env->GetObjectArrayElement(array, 1));
    const char *accountName = env->GetStringUTFChars(jAccountName, 0);
    const char *count = env->GetStringUTFChars(jCount, 0);

    qDebug() << accountName << count;
    accounts.append(QString(accountName).append(" (" + QString(count) + ")"));

    env->ReleaseStringUTFChars(jAccountName, accountName);
    env->ReleaseStringUTFChars(jCount, count);
  }

  emit accountList(accounts);
}

void MainViewController::removeAccount(QString accountName) {
  showToast("Deleting Account");

  QAndroidJniObject::callStaticMethod<void>(
      "org/mauikit/accounts/MainActivity", "removeAccount",
      "(Ljava/lang/String;)V",
      QAndroidJniObject::fromString(accountName).object<jstring>());

  QTimer::singleShot(1000, this, [=]() { this->getAccountList(); });
}

void MainViewController::syncAccount(QString accountName) {
  showToast("Syncing Account");

  QAndroidJniObject::callStaticMethod<void>(
      "org/mauikit/accounts/MainActivity", "syncAccount",
      "(Ljava/lang/String;)V",
      QAndroidJniObject::fromString(accountName).object<jstring>());
}

void MainViewController::showUrl(QString accountName) {
  QAndroidJniObject::callStaticMethod<void>(
      "org/mauikit/accounts/MainActivity", "showUrl", "(Ljava/lang/String;)V",
      QAndroidJniObject::fromString(accountName).object<jstring>());
}

void MainViewController::addAccount(QString protocol, QString url,
                                    QString username, QString password,
                                    QString accountName) {
  CardDAV *m_CardDAV = new CardDAV(url, username, password);

  //  showToast("Checking Server Credentials");
  showIndefiniteProgressDialog("Checking Server Credentials", false);

  CardDAVReply *reply = m_CardDAV->testConnection();
  this->connect(
      reply, &CardDAVReply::testConnectionResponse, [=](bool isSuccess) {
        if (isSuccess) {
          QAndroidJniObject::callStaticMethod<void>(
              "org/mauikit/accounts/MainActivity", "createSyncAccount",
              "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;"
              "Ljava/lang/String;)V",
              QAndroidJniObject::fromString(accountName).object<jstring>(),
              QAndroidJniObject::fromString(username).object<jstring>(),
              QAndroidJniObject::fromString(password).object<jstring>(),
              QAndroidJniObject::fromString(url).object<jstring>());

          qDebug() << "Account Added to System";
          emit accountAdded();

          this->getAccountList();

          hideIndefiniteProgressDialog();
          showToast("Account Added to System");
        } else {
          qDebug() << "Invalid Username or Password";
          showToast("Invalid Username or Password");
        }
      });
  this->connect(reply, &CardDAVReply::error,
                [=](QNetworkReply::NetworkError err) {
                  qDebug() << "Unknown Error Occured." << err;
                  //                  showToast("Unknown Error Occured");
                });
}

void MainViewController::showToast(QString text) {
  QAndroidJniObject::callStaticMethod<void>(
      "org/mauikit/accounts/MainActivity", "showToast", "(Ljava/lang/String;)V",
      QAndroidJniObject::fromString(text).object<jstring>());
}

void MainViewController::showIndefiniteProgressDialog(QString message,
                                                      bool isCancelable) {
  QAndroidJniObject::callStaticMethod<void>(
      "org/mauikit/accounts/MainActivity", "showIndefiniteProgressDialog",
      "(Ljava/lang/String;Z)V",
      QAndroidJniObject::fromString(message).object<jstring>(),
      isCancelable ? JNI_TRUE : JNI_FALSE);
}

void MainViewController::hideIndefiniteProgressDialog() {
  QAndroidJniObject::callStaticMethod<void>("org/mauikit/accounts/MainActivity",
                                            "hideIndefiniteProgressDialog");
}
#else
QDBusInterface* MainViewController::dbusInterfaceFactory(QString service,
                                                         QString interface,
                                                         QString path) {
  if (!QDBusConnection::systemBus().isConnected()) {
    fprintf(stderr, "Cannot connect to the D-Bus system bus.\n");
    return nullptr;
  }

  QDBusInterface* iface = new QDBusInterface(service, path, interface,
                                             QDBusConnection::systemBus());
  if (iface->isValid()) {
    return iface;
  }

  return nullptr;
}

MainViewController::MainViewController() {
  mauiAccountsDBusInterface = dbusInterfaceFactory(
      MAUI_ACCOUNTS_DBUS_SERVICE_NAME, MAUI_ACCOUNTS_DBUS_INTERFACE_NAME,
      MAUI_ACCOUNTS_DBUS_INTERFACE_PATH);

  if (mauiAccountsDBusInterface == nullptr) {
    qDebug()
        << "Could not connect `org.mauikit.accounts` System DBus.. Exiting..";

    QCoreApplication::exit(1);
  }
}

void MainViewController::addOpendesktopAccount(QString appId, QString protocol,
                                               QString username,
                                               QString password) {
  qDebug() << "Adding Opendesktop Account `" + username + "`";

  QString accountName = username + " - OpenDesktop";
  QString url =
      "https://cloud.opendesktop.cc/remote.php/dav/addressbooks/users/" +
      username + "/contacts";

  addAccount(appId, protocol, url, username, password);
}

void MainViewController::addCustomAccount(QString appId, QString protocol,
                                          QString server, QString username,
                                          QString password) {
  qDebug() << "Adding Opendesktop Account `" + username + "`";

  addAccount(appId, protocol, server, username, password);
}

void MainViewController::getAccountList() {
  QJsonArray accountsArray;

  QDBusReply<QList<QVariant> > reply =
      mauiAccountsDBusInterface->call("getAccountIds");

  QList<QVariant> accountIds = reply.value();

  for (QVariant accountId : accountIds) {
    QDBusReply<QMap<QString, QVariant> > reply =
        mauiAccountsDBusInterface->call("getAccount", accountId);
    QMap<QString, QVariant> accountData = reply.value();

    QJsonObject accountObj = {
        {"id", accountData.value("id").toString()},
        {"appId", accountData.value("appId").toString()},
        {"type", accountData.value("type").toString()},
        {"username", accountData.value("username").toString()},
        {"url", accountData.value("url").toString()},
        {"host", QUrl(accountData.value("url").toString()).host()}};

    accountsArray.append(accountObj);
  }

  emit accountList(accountsArray);
}

void MainViewController::removeAccount(QString id) {
  qDebug() << "Removing Account with id `" + id + "`";

  QDBusReply<bool> reply = mauiAccountsDBusInterface->call("removeAccount", id);

  if (reply.value()) {
    getAccountList();
  }
}

void MainViewController::syncAccount(QString appId) {
  qDebug() << "Syncing Account with appId `" + appId + "`";

  QFile manifestJsonFile(getManifestPath(appId));

  if (!manifestJsonFile.open(QIODevice::ReadWrite)) {
    qWarning("Couldn't open config file.");
  }

  QJsonObject accountsJsonObject =
      QJsonDocument::fromJson(manifestJsonFile.readAll()).object();
  QString syncCommand = accountsJsonObject["syncCommand"].toString();
  manifestJsonFile.close();

  QtConcurrent::run([=]() {
    QEventLoop loop;
    QProcess process;

    qDebug().noquote() << "[SYNC_START]";

    process.start(syncCommand);
    connect(&process, &QProcess::readyReadStandardOutput, [&process]() {
      qDebug().noquote() << "[SYNC_OUTPUT]" << process.readAllStandardOutput();
    });
    connect(&process, &QProcess::readyReadStandardError, [&process]() {
      qDebug().noquote() << "[SYNC_ERROR]" << process.readAllStandardError();
    });
    connect(&process,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=, &loop](int exitCode, QProcess::ExitStatus exitStatus) {
              qDebug().noquote() << "[SYNC_END]" << exitStatus;
              QTimer::singleShot(0, &loop, &QEventLoop::quit);
            });
    connect(&process, &QProcess::errorOccurred,
            [=, &loop](QProcess::ProcessError err) {
              qDebug().noquote() << "[SYNC_ERROR]" << err;
              qDebug().noquote() << "[SYNC_END]";
              QTimer::singleShot(0, &loop, &QEventLoop::quit);
            });

    loop.exec();
  });
}

void MainViewController::showUrl(QString url) {
  qDebug() << "Show url :" << url;
}

void MainViewController::addAccount(QString appId, QString protocol,
                                    QString url, QString username,
                                    QString password) {
  QDBusReply<QString> reply = mauiAccountsDBusInterface->call(
      "createCardDAVAccount", appId, username, password, url);

  if (reply.isValid()) {
    QString createAccountReply = reply.value();
    qDebug() << "[DBus] createCardDAVAccount :" << createAccountReply;

    emit accountAdded(createAccountReply);
  } else {
    qDebug() << mauiAccountsDBusInterface->lastError();
  }
}

QString MainViewController::getManifestPath(QString appId) {
  return "/usr/share/maui-accounts/manifests/" + appId + ".json";
}

QString MainViewController::generateAccountName(QString username,
                                                QString host) {
  return username + " - " + host;
}

QString MainViewController::getHostFromAccountName(QString accountName) {
  return accountName.split(" - ").at(1);
}

QString MainViewController::getUsernameFromAccountName(QString accountName) {
  return accountName.split(" - ").at(0);
}

#endif
