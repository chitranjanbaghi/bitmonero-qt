#ifndef MONEROGUI_H
#define MONEROGUI_H

#include <QObject>

#include <QCoreApplication>
#include <QGuiApplication>

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>

#include <QTimer>

#include "Models/ApplicationModel.h"
#include "Models/MoneroModel.h"
#include "Models/WalletModel.h"
#include "Models/MinerModel.h"
#include "Models/WalletHandlerModel.h"

#include "WalletSettings.h"


#include "DaemonHandler.h"



class MoneroInterface;
class MinerInterface;
class WalletInterface;
class WalletHandlerInterface;


class MoneroGUI : public QObject
{
    Q_OBJECT
public:
    MoneroGUI(QGuiApplication& pApp, const QString& pCustomConfigFile);
    ~MoneroGUI();

    int start();

    int startWizard();
    int startMainWindow();

signals:
    void applicationQuit(int pReturnCode);

private:

    void initModels();

    bool createComponent(QQmlComponent& pComponent) {

        pComponent.create();
        if ( !pComponent.isReady() ) {
            qDebug() << "Component not ready";
            qDebug() << "Error " << pComponent.errors();
            return false;
        }

        return true;

    }

    bool initQmlElement(QQmlEngine& pEngine, const QUrl& pQmlFile) {

        QQmlComponent* lComponent = new QQmlComponent(&pEngine, pQmlFile);

        if ( !createComponent(*lComponent) ) {
            return false;
        }

        return true;

    }

    bool initMainWindow(QQmlEngine& pEngine) {

        return initQmlElement(pEngine, QUrl("qrc:/qml/main.qml"));
    }

    bool initWizard(QQmlEngine& pEngine) {

        return initQmlElement(pEngine, QUrl("qrc:/qml/wizard.qml"));
    }

    bool initSplashScreen(QQmlEngine& pEngine) {

        return initQmlElement(pEngine, QUrl("qrc:/qml/SplashScreen.qml"));
    }

    bool initHappyEnding(QQmlEngine& pEngine) {

        return initQmlElement(pEngine, QUrl("qrc:/qml/HappyEnding.qml"));
    }

    bool isReady() const {
        return settings.areWalletSettingsAcceptable() && !reconfiguration_requested;
    }


    void createMonero();
    void createMiner();
    void createWalletHandler();
    void createWallet();

    int startSplashScreen() {
        if ( initSplashScreen(engine) ) {

            app.exec();
            return 0;
        }

        return 1;
    }

    void closeAllWindows();


public slots:
    void stepEnableDaemon();
    void stepConfigure();
    void stepEnableWalletHandler();
    void stepEnableWallet();
    void stepStartMainGUI();

    void reconfigure();


    /* Handles fatal errors triggered during 'enable' */
    void daemonError(int pCode);
    void walletHandlerError(int pCode);
    void walletError(int pCode);
    void minerError(int pCode);

private:

    void dialogError(int pErrorCode);

private:
    QGuiApplication& app;

    MoneroInterface* monero_interface;
    MinerInterface* miner_interface;
    WalletInterface* wallet_interface;
    WalletHandlerInterface* wallet_handler_interface;

    ApplicationModel application_model;
    MoneroModel monero_model;
    MinerModel miner_model;
    WalletModel wallet_model;
    WalletHandlerModel wallet_handler_model;

    WalletSettings settings;

    QTimer startup_timer;


    QQmlEngine engine;

    int exit_status;

    bool reconfiguration_requested;



};

#endif // MONEROGUI_H
