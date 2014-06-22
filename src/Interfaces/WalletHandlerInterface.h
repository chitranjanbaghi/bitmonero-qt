#ifndef WALLETHANDLERINTERFACE_H
#define WALLETHANDLERINTERFACE_H

#include <QObject>
#include <QDir>

#include "Models/WalletHandlerModel.h"

class WalletHandlerInterface : public QObject {

    Q_OBJECT

public:

    WalletHandlerInterface(WalletHandlerModel& pModel) : wallet_handler_model(pModel), self_ready(false) {
        wallet_handler_model.setWalletHandlerInterface(this);
        wallet_handler_model.setDefaultWalletLocation(QDir::homePath() + "/.bitmonero/");
    }

    virtual ~WalletHandlerInterface() {}


    virtual bool createWallet(const QString& pFile, const QString& pPassword, const QString& pSeed) = 0;

    virtual bool tryWalletAsync(const QString& pFile, const QString& pPassword) = 0;

    virtual bool walletFileExists(const QString& pFile) = 0;

    virtual QList<QObject*> findWallets(const QString& pPath) = 0;


    virtual int enable() = 0;



    void onTryWalletResult(bool pResult) {
        emit wallet_handler_model.tryWalletResult(pResult);
    }

    void onSeedGenerated(const QString& pSeed) {
        wallet_handler_model.setSeed(pSeed);
    }

    void onReady() {

        if ( !self_ready ) {
            self_ready = true;
            emit ready();
        }

    }

    void setSeedAvailable() {
        wallet_handler_model.setSeedAvailable(true);
    }


signals:
    void ready();


private:

    WalletHandlerModel& wallet_handler_model;
    bool self_ready;
};

#endif // WALLETHANDLERINTERFACE_H
