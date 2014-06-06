#ifndef WALLET_H
#define WALLET_H

#include <QObject>
#include <QDebug>
#include <QList>

#include "AbstractModel.h"
//#include "TransactionsListModel.h"
#include "TransactionModel.h"

class WalletInterface;

class WalletModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(double balance READ getBalance WRITE setBalance NOTIFY balanceChanged)
    Q_PROPERTY(double unlocked_balance READ getUnlockedBalance WRITE setUnlockedBalance NOTIFY unlockedBalanceChanged)
    Q_PROPERTY(QString address READ getAddress NOTIFY addressChanged)
    Q_PROPERTY(QList<QObject*> transactions READ getTransactions NOTIFY transactionsChanged)

public:
    WalletModel();
    ~WalletModel();

    void setBalance(double pBalance)
    {

        if ( balance == pBalance ) {
            return;
        }

        balance = pBalance;
        emit balanceChanged();

    }

    double getBalance() const
    {
        return balance;
    }


    void setWalletInterface(WalletInterface* iface)
    {
        wallet_interface = iface;
    }

    void setAddress(const QString& pAddress) {
        if ( pAddress != address ) {

            if ( pAddress.length() != 95 ) {
                qCritical() << "Attempted to assign bad address : " << pAddress;
                return;
            }

            address = pAddress;
            emit addressChanged(address);

        }
    }

    const QString& getAddress() const
    {
        return address;
    }


    void setUnlockedBalance(double pUnlockedBalance)
    {
        if (unlocked_balance != pUnlockedBalance) {
            unlocked_balance = pUnlockedBalance;
            emit unlockedBalanceChanged(pUnlockedBalance);
        }
    }


    double getUnlockedBalance() const
    {
        return unlocked_balance;
    }

    const QList<QObject*>& getTransactions() const
    {
        return transactions;
    }

    /* Not invokable from QML */
    void setTransactions(const QList<QObject*>& pTransactions) {

        /* Basic comparison : TODO */
        if ( transactions.size() != pTransactions.size() ) {
            transactions = pTransactions;
            emit transactionsChanged(pTransactions);
        }
    }

signals:
    void balanceChanged();
    void transferSuccessful(const QString& tx_hash, double amount, const QString& address, int fee);
    void addressChanged(const QString& pAddress);
    void transferError(int error_code, const QString& error_message);

    void unlockedBalanceChanged(double unlocked_balance);

    void transactionsChanged(QList<QObject*> transactions);

public slots:
    Q_INVOKABLE bool transfer(double amount, const QString& address, int pFee, const QString& pPaymentId);



private:
    WalletInterface* wallet_interface;

    double balance;
    QString address;
    double unlocked_balance;
    QList<QObject*> transactions;
};

#endif // WALLET_H
