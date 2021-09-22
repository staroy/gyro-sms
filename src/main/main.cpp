// Copyright (c) 2014-2018, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QStandardPaths>
#include <QIcon>
#include <QDebug>
#include <QDesktopServices>
#include <QObject>
#include <QDesktopWidget>
#include <QScreen>
#include <QRegExp>
#include <QThread>

#include <version.h>

#include "clipboardAdapter.h"
#include "filter.h"
#include "oscursor.h"
#include "oshelper.h"
#include "WalletManager.h"
#include "Wallet.h"
#include "QRCodeImageProvider.h"
#include "PendingTransaction.h"
#include "UnsignedTransaction.h"
#include "TranslationManager.h"
#include "TransactionInfo.h"
#include "TransactionHistory.h"
#include "model/TransactionHistoryModel.h"
#include "model/TransactionHistorySortFilterModel.h"
#include "AddressBook.h"
#include "model/AddressBookModel.h"
#include "model/SMSModel.h"
#include "Subaddress.h"
#include "model/SubaddressModel.h"
#include "SubaddressAccount.h"
#include "model/SubaddressAccountModel.h"
#include "wallet/api/wallet2_api.h"
#include "Logger.h"
#include "MainApp.h"
#include "qt/downloader.h"
#include "qt/ipc.h"
#include "qt/network.h"
#include "qt/updater.h"
#include "qt/utils.h"
#include "qt/TailsOS.h"
#include "qt/KeysFiles.h"
#include "qt/MoneroSettings.h"
#include "qt/NetworkAccessBlockingFactory.h"
#include "cryptonote_config.h"

#define DEFAULT_GYRO_CONF "/var/lib/gyro/gyro.conf"

// IOS exclusions
#ifndef Q_OS_IOS
#include "daemon/DaemonManager.h"
#endif

#if defined(Q_OS_WIN)
#include <QOpenGLContext>
#endif

#ifdef WITH_SCANNER
#include "QR-Code-scanner/QrCodeScanner.h"
#endif

#ifdef MONERO_GUI_STATIC

#include <QtPlugin>
#if defined(Q_OS_OSX)
  Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin);
#elif defined(Q_OS_WIN)
  Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#elif defined(Q_OS_LINUX)
  Q_IMPORT_PLUGIN(QXcbIntegrationPlugin);
  Q_IMPORT_PLUGIN(QXcbGlxIntegrationPlugin);
#endif
Q_IMPORT_PLUGIN(QSvgIconPlugin)
Q_IMPORT_PLUGIN(QICNSPlugin)
Q_IMPORT_PLUGIN(QICOPlugin)
Q_IMPORT_PLUGIN(QJpegPlugin)
Q_IMPORT_PLUGIN(QSvgPlugin)
Q_IMPORT_PLUGIN(QTgaPlugin)
Q_IMPORT_PLUGIN(QTiffPlugin)
Q_IMPORT_PLUGIN(QWbmpPlugin)
Q_IMPORT_PLUGIN(QWebpPlugin)
Q_IMPORT_PLUGIN(QQmlDebuggerServiceFactory)
Q_IMPORT_PLUGIN(QQmlInspectorServiceFactory)
Q_IMPORT_PLUGIN(QLocalClientConnectionFactory)
Q_IMPORT_PLUGIN(QDebugMessageServiceFactory)
Q_IMPORT_PLUGIN(QQmlNativeDebugConnectorFactory)
Q_IMPORT_PLUGIN(QQmlNativeDebugServiceFactory)
Q_IMPORT_PLUGIN(QQmlProfilerServiceFactory)
Q_IMPORT_PLUGIN(QQuickProfilerAdapterFactory)
Q_IMPORT_PLUGIN(QQmlDebugServerFactory)
Q_IMPORT_PLUGIN(QTcpServerConnectionFactory)
Q_IMPORT_PLUGIN(QGenericEnginePlugin)

Q_IMPORT_PLUGIN(QtQuick2Plugin)
Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
Q_IMPORT_PLUGIN(QtGraphicalEffectsPlugin)
Q_IMPORT_PLUGIN(QtGraphicalEffectsPrivatePlugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
Q_IMPORT_PLUGIN(QtQuickControls1Plugin)
Q_IMPORT_PLUGIN(QtQuick2DialogsPlugin)
Q_IMPORT_PLUGIN(QmlFolderListModelPlugin)
Q_IMPORT_PLUGIN(QmlSettingsPlugin)
Q_IMPORT_PLUGIN(QtQuick2DialogsPrivatePlugin)
Q_IMPORT_PLUGIN(QtQuick2PrivateWidgetsPlugin)
Q_IMPORT_PLUGIN(QtQuickControls2Plugin)
Q_IMPORT_PLUGIN(QtQuickTemplates2Plugin)
Q_IMPORT_PLUGIN(QmlXmlListModelPlugin)
#ifdef WITH_SCANNER
Q_IMPORT_PLUGIN(QMultimediaDeclarativeModule)
#endif

#endif

bool isIOS = false;
bool isAndroid = false;
bool isWindows = false;
bool isMac = false;
bool isLinux = false;
bool isTails = false;
bool isDesktop = false;
bool isOpenGL = true;

int main(int argc, char *argv[])
{
  int rc = 0;

  try
  {
    Q_INIT_RESOURCE(translations);

    // platform dependant settings
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    bool isDesktop = true;
#elif defined(Q_OS_LINUX)
    bool isLinux = true;
#elif defined(Q_OS_ANDROID)
    bool isAndroid = true;
#elif defined(Q_OS_IOS)
    bool isIOS = true;
#endif
#ifdef Q_OS_WIN
    bool isWindows = true;
#elif defined(Q_OS_LINUX)
    bool isLinux = true;
    bool isTails = TailsOS::detect();
#elif defined(Q_OS_MAC)
    bool isMac = true;
#endif

    // detect low graphics mode (start-low-graphics-mode.bat)
    if(qgetenv("QMLSCENE_DEVICE") == "softwarecontext")
        isOpenGL = false;

    // disable "QApplication: invalid style override passed" warning
    if (isDesktop) qputenv("QT_STYLE_OVERRIDE", "fusion");
#ifdef Q_OS_LINUX
    // force platform xcb
    if (isDesktop) qputenv("QT_QPA_PLATFORM", "xcb");
#endif

    // enable High DPI scaling
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");

    // Turn off colors in gyrod log output.
    qputenv("TERM", "goaway");

    MainApp app(argc, argv);

#if defined(Q_OS_WIN)
    if (isOpenGL)
    {
        QOpenGLContext ctx;
        isOpenGL = ctx.create() && ctx.format().version() >= qMakePair(2, 1);
        if (!isOpenGL) {
            qputenv("QMLSCENE_DEVICE", "softwarecontext");
        }
    }
#endif

    app.setApplicationName("gyro-core");
    app.setOrganizationDomain("gyro.address");
    app.setOrganizationName("gyro-project");

    // Ask to enable Tails OS persistence mode, it affects:
    // - Log file location
    // - QML Settings file location (monero-core.conf)
    // - Default wallets path
    // Target directory is: ~/Persistent/Monero
    if (isTails) {
        if (!TailsOS::detectDataPersistence())
            TailsOS::showDataPersistenceDisabledWarning();
        else
            TailsOS::askPersistence();
    }

    QString gyroAccountsDir = qEnvironmentVariable("GYRO_ACCOUNTS_DIR");
    if(gyroAccountsDir.isEmpty())
    {
      #if defined(Q_OS_WIN) || defined(Q_OS_IOS)
          QStringList gyroAccountsRootDir = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
      #else
          QStringList gyroAccountsRootDir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
      #endif
      
      if(isTails && TailsOS::usePersistence){
          gyroAccountsDir = QDir::homePath() + "/Persistent/Gyro/wallets";
      } else if (!gyroAccountsRootDir.empty()) {
          gyroAccountsDir = gyroAccountsRootDir.at(0) + "/Gyro/wallets";
      } else {
          qCritical() << "Error: accounts root directory could not be set";
          return 1;
      }
      gyroAccountsDir = QDir::toNativeSeparators(gyroAccountsDir);
    }

#if defined(Q_OS_LINUX)
    if (isDesktop) app.setWindowIcon(QIcon(":/images/appicon.ico"));
#endif

    filter *eventFilter = new filter;
    app.installEventFilter(eventFilter);

    QCommandLineParser parser;
    QCommandLineOption logPathOption(QStringList() << "l" << "log-file",
        QCoreApplication::translate("main", "Log to specified file"),
        QCoreApplication::translate("main", "file"));

    QCommandLineOption configPathOption(QStringList() << "c" << "config-file",
        QCoreApplication::translate("main", "config to specified file"),
        QCoreApplication::translate("main", "file"));

    QCommandLineOption verifyUpdateOption("verify-update", "\
Verify update binary using 'shasum'-compatible (SHA256 algo) output signed by two maintainers.\n\
* Requires 'hashes.txt' - signed 'shasum' output \
(i.e. 'gpg -o hashes.txt --clear-sign <shasum_output>') generated by a maintainer.\n\
* Requires 'hashes.txt.sig' - detached signature of 'hashes.txt' \
(i.e. 'gpg -b hashes.txt') generated by another maintainer.", "update-binary");
    parser.addOption(verifyUpdateOption);

    QCommandLineOption disableCheckUpdatesOption("disable-check-updates", "Disable automatic check for updates.");
    parser.addOption(disableCheckUpdatesOption);
    QCommandLineOption testQmlOption("test-qml");
    testQmlOption.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(logPathOption);
    parser.addOption(configPathOption);
    parser.addOption(testQmlOption);
    parser.addHelpOption();
    parser.process(app);

    Gyro::Utils::onStartup();

    // gyro data dir
    QString gyro_data_dir = parser.value(logPathOption);
    if(gyro_data_dir.isEmpty())
      gyro_data_dir = getDataPath(qEnvironmentVariable("GYRO_DATA_DIR"));
    else
      gyro_data_dir = getDataPath(gyro_data_dir);

    // Log settings
    const QString logPath = QDir::toNativeSeparators(getLogPath(gyro_data_dir));
    Gyro::Wallet::init(argv[0], "gyro-wallet-sms", logPath.toStdString().c_str(), true);

    // gyro config pathfile
    QString config_path = QDir::toNativeSeparators(parser.value(configPathOption));
    if(config_path.isEmpty())
      qEnvironmentVariable("GYRO_CONF");

    if(!config::load_config_file(config_path.toStdString().c_str()))
    {
      if(config::load_config_file(DEFAULT_GYRO_CONF))
        config_path = DEFAULT_GYRO_CONF;
      else
        qWarning().noquote() << "no config file " << config_path;
    }
    qWarning().noquote() << "Config file is " << config_path;

    qInstallMessageHandler(messageHandler);

    // loglevel is configured in main.qml. Anything lower than
    // qWarning is not shown here unless MONERO_LOG_LEVEL env var is set
    bool logLevelOk;
    int logLevel = qEnvironmentVariableIntValue("GYRO_LOG_LEVEL", &logLevelOk);
    if (logLevelOk && logLevel >= 0 && logLevel <= Gyro::WalletManagerFactory::LogLevel_Max){
        Gyro::WalletManagerFactory::setLogLevel(logLevel);
    }
    qWarning().noquote() << "app startd" << "(log: " + logPath + ")";

    if (parser.isSet(verifyUpdateOption))
    {
        const QString updateBinaryFullPath = parser.value(verifyUpdateOption);
        const QFileInfo updateBinaryInfo(updateBinaryFullPath);
        const QString updateBinaryDir = QDir::toNativeSeparators(updateBinaryInfo.absolutePath()) + QDir::separator();
        const QString hashesTxt = updateBinaryDir + "hashes.txt";
        const QString hashesTxtSig = hashesTxt + ".sig";
        try
        {
            const QByteArray updateBinaryContents = fileGetContents(updateBinaryFullPath);
            const QPair<QString, QString> signers = Updater().verifySignaturesAndHashSum(
                fileGetContents(hashesTxt),
                fileGetContents(hashesTxtSig),
                updateBinaryInfo.fileName(),
                updateBinaryContents.data(),
                updateBinaryContents.size());
            qCritical() << "successfully verified, signed by" << signers.first << "and" << signers.second;
            return 0;
        }
        catch (const std::exception &e)
        {
            qCritical() << e.what();
        }
        return 1;
    }

    // Desktop entry
#ifdef Q_OS_LINUX
    registerXdgMime(app);
#endif

    IPC *ipc = new IPC(&app);
    QStringList posArgs = parser.positionalArguments();

    for(int i = 0; i != posArgs.count(); i++){
        QString arg = QString(posArgs.at(i));
        if(arg.isEmpty() || arg.length() >= 512) continue;
        if(arg.contains(reURI)){
            if(!ipc->saveCommand(arg)){
                return 0;
            }
        }
    }

    // start listening
    QTimer::singleShot(0, ipc, SLOT(bind()));

    // screen settings
    // Mobile is designed on 128dpi
    qreal ref_dpi = 128;
    QRect geo = QGuiApplication::primaryScreen()->availableGeometry();
    QRect rect = QGuiApplication::primaryScreen()->geometry();
    qreal dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
    qreal physicalDpi = QGuiApplication::primaryScreen()->physicalDotsPerInch();
    qreal calculated_ratio = physicalDpi/ref_dpi;

    QString GUI_VERSION = "-";
    QFile f(":/version.js");
    if(!f.open(QFile::ReadOnly)) {
        qWarning() << "Could not read qrc:///version.js";
    } else {
        QByteArray contents = f.readAll();
        f.close();

        QRegularExpression re("var GUI_VERSION = \"(.*)\"");
        QRegularExpressionMatch version_match = re.match(contents);
        if (version_match.hasMatch()) {
            GUI_VERSION = version_match.captured(1);  // "v0.13.0.3"
        }
    }

    qWarning().nospace().noquote() << "Qt:" << QT_VERSION_STR << " GUI:" << GUI_VERSION
                                   << " | screen: " << rect.width() << "x" << rect.height()
                                   << " - dpi: " << dpi << " - ratio:" << calculated_ratio;

    // registering types for QML
    qmlRegisterType<clipboardAdapter>("gyroComponents.Clipboard", 1, 0, "Clipboard");
    qmlRegisterType<Downloader>("gyroComponents.Downloader", 1, 0, "Downloader");
    qmlRegisterType<WalletKeysFilesModel>("gyroComponents.WalletKeysFilesModel", 1, 0, "WalletKeysFilesModel");
    qmlRegisterType<WalletManager>("gyroComponents.WalletManager", 1, 0, "WalletManager");

    // Temporary Qt.labs.settings replacement
    qmlRegisterType<MoneroSettings>("gyroComponents.Settings", 1, 0, "MoneroSettings");

    qmlRegisterUncreatableType<Wallet>("gyroComponents.Wallet", 1, 0, "Wallet", "Wallet can't be instantiated directly");


    qmlRegisterUncreatableType<PendingTransaction>("gyroComponents.PendingTransaction", 1, 0, "PendingTransaction",
                                                   "PendingTransaction can't be instantiated directly");

    qmlRegisterUncreatableType<UnsignedTransaction>("gyroComponents.UnsignedTransaction", 1, 0, "UnsignedTransaction",
                                                   "UnsignedTransaction can't be instantiated directly");

    qmlRegisterUncreatableType<TranslationManager>("gyroComponents.TranslationManager", 1, 0, "TranslationManager",
                                                   "TranslationManager can't be instantiated directly");

    qmlRegisterUncreatableType<TransactionHistoryModel>("gyroComponents.TransactionHistoryModel", 1, 0, "TransactionHistoryModel",
                                                        "TransactionHistoryModel can't be instantiated directly");

    qmlRegisterUncreatableType<TransactionHistorySortFilterModel>("gyroComponents.TransactionHistorySortFilterModel", 1, 0, "TransactionHistorySortFilterModel",
                                                        "TransactionHistorySortFilterModel can't be instantiated directly");

    qmlRegisterUncreatableType<TransactionHistory>("gyroComponents.TransactionHistory", 1, 0, "TransactionHistory",
                                                        "TransactionHistory can't be instantiated directly");

    qmlRegisterUncreatableType<TransactionInfo>("gyroComponents.TransactionInfo", 1, 0, "TransactionInfo",
                                                        "TransactionHistory can't be instantiated directly");
#ifndef Q_OS_IOS
    qmlRegisterUncreatableType<DaemonManager>("gyroComponents.DaemonManager", 1, 0, "DaemonManager",
                                                   "DaemonManager can't be instantiated directly");
#endif
    qmlRegisterUncreatableType<AddressBookModel>("gyroComponents.AddressBookModel", 1, 0, "AddressBookModel",
                                                        "AddressBookModel can't be instantiated directly");

    qmlRegisterUncreatableType<AddressBook>("gyroComponents.AddressBook", 1, 0, "AddressBook",
                                                        "AddressBook can't be instantiated directly");

    qmlRegisterUncreatableType<SMSModel>("gyroComponents.SMSModel", 1, 0, "SMSModel",
                                                        "SMSModel can't be instantiated directly");

    qmlRegisterUncreatableType<SubaddressModel>("gyroComponents.SubaddressModel", 1, 0, "SubaddressModel",
                                                        "SubaddressModel can't be instantiated directly");

    qmlRegisterUncreatableType<Subaddress>("gyroComponents.Subaddress", 1, 0, "Subaddress",
                                                        "Subaddress can't be instantiated directly");

    qmlRegisterUncreatableType<SubaddressAccountModel>("gyroComponents.SubaddressAccountModel", 1, 0, "SubaddressAccountModel",
                                                        "SubaddressAccountModel can't be instantiated directly");

    qmlRegisterUncreatableType<SubaddressAccount>("gyroComponents.SubaddressAccount", 1, 0, "SubaddressAccount",
                                                        "SubaddressAccount can't be instantiated directly");

    qRegisterMetaType<PendingTransaction::Priority>();
    qRegisterMetaType<TransactionInfo::Direction>();
    qRegisterMetaType<TransactionHistoryModel::TransactionInfoRole>();

    qRegisterMetaType<NetworkType::Type>();
    qmlRegisterType<NetworkType>("gyroComponents.NetworkType", 1, 0, "NetworkType");

#ifdef WITH_SCANNER
    qmlRegisterType<QrCodeScanner>("gyroComponents.QRCodeScanner", 1, 0, "QRCodeScanner");
#endif

    QQmlApplicationEngine engine;

#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    engine.setNetworkAccessManagerFactory(new NetworkAccessBlockingFactory);
#endif
    OSCursor cursor;
    engine.rootContext()->setContextProperty("globalCursor", &cursor);
    OSHelper osHelper;
    engine.rootContext()->setContextProperty("oshelper", &osHelper);

    engine.addImportPath(":/fonts");

    engine.rootContext()->setContextProperty("gyroAccountsDir", gyroAccountsDir);

    engine.rootContext()->setContextProperty("translationManager", TranslationManager::instance());

    engine.addImageProvider(QLatin1String("qrcode"), new QRCodeImageProvider());

    engine.rootContext()->setContextProperty("mainApp", &app);

    engine.rootContext()->setContextProperty("IPC", ipc);

    engine.rootContext()->setContextProperty("qtRuntimeVersion", qVersion());

    engine.rootContext()->setContextProperty("walletLogPath", logPath);

    engine.rootContext()->setContextProperty("tailsUsePersistence", TailsOS::usePersistence);

// Exclude daemon manager from IOS
#ifndef Q_OS_IOS
    DaemonManager::m_clArgs.append("--config-file");
    DaemonManager::m_clArgs.append(config_path.toStdString().c_str());
    if( !gyro_data_dir.isEmpty() ) {
      DaemonManager::m_clArgs.append("--data-dir");
      DaemonManager::m_clArgs.append(gyro_data_dir);
    }
    DaemonManager daemonManager;
    engine.rootContext()->setContextProperty("daemonManager", &daemonManager);
#endif

    engine.rootContext()->setContextProperty("isWindows", isWindows);
    engine.rootContext()->setContextProperty("isMac", isMac);
    engine.rootContext()->setContextProperty("isLinux", isLinux);
    engine.rootContext()->setContextProperty("isIOS", isIOS);
    engine.rootContext()->setContextProperty("isAndroid", isAndroid);
    engine.rootContext()->setContextProperty("isOpenGL", isOpenGL);
    engine.rootContext()->setContextProperty("isTails", isTails);

    engine.rootContext()->setContextProperty("screenWidth", geo.width());
    engine.rootContext()->setContextProperty("screenHeight", geo.height());

#ifndef Q_OS_IOS
    const QString desktopFolder = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (!desktopFolder.isEmpty())
        engine.rootContext()->setContextProperty("desktopFolder", desktopFolder);
#endif

    // Get default account name
    QString accountName = qgetenv("USER"); // mac/linux
    if (accountName.isEmpty())
        accountName = qgetenv("USERNAME"); // Windows
    if (accountName.isEmpty())
        accountName = "My gyro Account";

    engine.rootContext()->setContextProperty("defaultAccountName", accountName);
    engine.rootContext()->setContextProperty("homePath", QDir::homePath());
    engine.rootContext()->setContextProperty("applicationDirectory", QApplication::applicationDirPath());
    engine.rootContext()->setContextProperty("idealThreadCount", QThread::idealThreadCount());
    engine.rootContext()->setContextProperty("disableCheckUpdatesFlag", parser.isSet(disableCheckUpdatesOption));

    bool builtWithScanner = false;
#ifdef WITH_SCANNER
    builtWithScanner = true;
#endif
    engine.rootContext()->setContextProperty("builtWithScanner", builtWithScanner);

    engine.rootContext()->setContextProperty("moneroVersion", MONERO_VERSION_FULL);

    Network network;
    engine.rootContext()->setContextProperty("Network", &network);

    // Load main window (context properties needs to be defined obove this line)
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    if (engine.rootObjects().isEmpty())
    {
        qCritical() << "Error: no root objects";
        return 1;
    }
    QObject *rootObject = engine.rootObjects().first();
    if (!rootObject)
    {
        qCritical() << "Error: no root objects";
        return 1;
    }

    // QML loaded successfully.
    if (parser.isSet(testQmlOption))
        return 0;

#ifdef WITH_SCANNER
    QObject *qmlCamera = rootObject->findChild<QObject*>("qrCameraQML");
    if (qmlCamera)
    {
        qWarning() << "QrCodeScanner : object found";
        QCamera *camera_ = qvariant_cast<QCamera*>(qmlCamera->property("mediaObject"));
        QObject *qmlFinder = rootObject->findChild<QObject*>("QrFinder");
        qobject_cast<QrCodeScanner*>(qmlFinder)->setSource(camera_);
    }
    else
        qCritical() << "QrCodeScanner : something went wrong !";
#endif

    QObject::connect(eventFilter, SIGNAL(sequencePressed(QVariant,QVariant)), rootObject, SLOT(sequencePressed(QVariant,QVariant)));
    QObject::connect(eventFilter, SIGNAL(sequenceReleased(QVariant,QVariant)), rootObject, SLOT(sequenceReleased(QVariant,QVariant)));
    QObject::connect(eventFilter, SIGNAL(mousePressed(QVariant,QVariant,QVariant)), rootObject, SLOT(mousePressed(QVariant,QVariant,QVariant)));
    QObject::connect(eventFilter, SIGNAL(mouseReleased(QVariant,QVariant,QVariant)), rootObject, SLOT(mouseReleased(QVariant,QVariant,QVariant)));
    QObject::connect(eventFilter, SIGNAL(userActivity()), rootObject, SLOT(userActivity()));
    QObject::connect(eventFilter, SIGNAL(uriHandler(QUrl)), ipc, SLOT(parseCommand(QUrl)));

    rc = app.exec();
  }
  catch(const std::exception& e)
  {
    qCritical() << e.what();
  }
  return rc;
}
