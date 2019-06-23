#include "entities/SyncManager.hpp"
// #include "viewcontrollers/MainViewController.hpp"

#ifdef ANDROID
#include <jni.h>
#include <QtAndroidExtras/QAndroidJniObject>
#endif

#include <QEventLoop>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QThread>
#include <QTimer>

#ifdef ANDROID
extern "C" {
JNIEXPORT void JNICALL
Java_org_mauikit_accounts_syncadapter_ContactsSyncAdapter_performSync(
    JNIEnv* env, jobject obj, jstring username, jstring password, jstring url) {
  QEventLoop* qEventLoop = new QEventLoop();

  const char* username_str = env->GetStringUTFChars(username, 0);
  const char* password_str = env->GetStringUTFChars(password, 0);
  const char* url_str = env->GetStringUTFChars(url, 0);

  //  SyncManager* syncManager =
  //      new SyncManager(username_str, password_str, url_str);
  //  syncManager->doSyncAndroid();
  //  syncManager->deleteLater();

  //  SyncThread* s = new SyncThread(username_str, password_str, url_str);
  //  s->start();

  QTimer* t = new QTimer();
  QObject::connect(t, &QTimer::timeout, [=]() {
    qDebug() << "timeout() called";
    qEventLoop->exit(0);
  });
  t->start(2000);

  //  QObject::connect(syncManager, &SyncManager::syncComplete,
  //                   [=]() { qEventLoop->quit(); });

  env->ReleaseStringUTFChars(username, username_str);
  env->ReleaseStringUTFChars(password, password_str);
  env->ReleaseStringUTFChars(url, url_str);

  qEventLoop->exec();
}
}
#endif

int main(int argc, char* argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QLoggingCategory::setFilterRules("default.debug=true");

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  qDebug() << QCoreApplication::arguments();

  app.setApplicationName("Accounts");
  app.setApplicationVersion("0.4.0");
  app.setApplicationDisplayName("Accounts");

  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty()) return -1;

#ifdef ANDROID
  QAndroidJniObject::callStaticMethod<void>("org/mauikit/accounts/MainActivity",
                                            "init");
#endif

  return app.exec();
}
