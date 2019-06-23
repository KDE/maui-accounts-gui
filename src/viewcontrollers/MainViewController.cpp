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
#endif

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QTimer>

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

void MainViewController::addOpendesktopAccount(QString protocol,
                                               QString username,
                                               QString password) {
  qDebug() << "Adding Opendesktop Account `" + username + "`";

  QString accountName = username + " - OpenDesktop";
  QString url =
      "https://cloud.opendesktop.cc/remote.php/dav/addressbooks/users/" +
      username + "/contacts";

  addAccount(protocol, url, username, password, accountName);
}

void MainViewController::addCustomAccount(QString protocol, QString server,
                                          QString username, QString password) {
  qDebug() << "Adding Opendesktop Account `" + username + "`";

  addAccount(protocol, server, username, password, username);
}

void MainViewController::getAccountList() {
  QDBusInterface* iface = dbusInterfaceFactory(
      MAUI_ACCOUNTS_DBUS_SERVICE_NAME, MAUI_ACCOUNTS_DBUS_INTERFACE_NAME,
      MAUI_ACCOUNTS_DBUS_INTERFACE_PATH);

  if (iface != nullptr) {
    QList<QString> accountNamesStringList;

    QDBusReply<QList<QVariant> > reply = iface->call("getAccountNames");
    if (reply.isValid()) {
      QList<QVariant> accountNames = reply.value();
      qDebug() << "[DBus] getAccountNames :" << accountNames;

      for (QVariant accountName : accountNames) {
        accountNamesStringList.append(accountName.toString());
      }

      emit accountList(accountNamesStringList);
    } else {
      fprintf(stderr, "Call failed: %s\n", qPrintable(reply.error().message()));
    }
  }
}

void MainViewController::removeAccount(QString accountName) {
  getAccountList();
}

void MainViewController::syncAccount(QString accountName) {}

void MainViewController::showUrl(QString accountName) {}

void MainViewController::addAccount(QString protocol, QString url,
                                    QString username, QString password,
                                    QString accountName) {}

void MainViewController::showToast(QString text) {}

void MainViewController::showIndefiniteProgressDialog(QString message,
                                                      bool isCancelable) {}

void MainViewController::hideIndefiniteProgressDialog() {}

#endif
