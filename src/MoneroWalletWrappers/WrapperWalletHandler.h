#ifndef WRAPPERWALLETHANDLER_H
#define WRAPPERWALLETHANDLER_H

#include "Interfaces/WalletHandlerInterface.h"
#include "Models/WalletHandlerModel.h"
#include "WalletSettings.h"

class WrapperWalletHandler : public WalletHandlerInterface
{
public:
    WrapperWalletHandler(WalletHandlerModel& pModel, const WalletSettings& pSettings);

    int enable();

    bool createWallet(const QString& pFile, const QString& pPassword);

    bool tryWalletAsync(const QString& pFile, const QString& pPassword);

    bool walletFileExists(const QString& pFile);

    QList<QObject*> findWallets(const QString& pPath);


private:

    const WalletSettings& settings;
};

#endif // WRAPPERWALLETHANDLER_H