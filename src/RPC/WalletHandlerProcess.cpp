#include "WalletHandlerProcess.h"

#include <QStringList>
#include <QDebug>

#include <QDir>
#include <QUrl>
#include "Models/InfoWalletModel.h"

#include "Utils.h"

const QString cWalletCli = "simplewallet";
const QString cWalletRpc = "rpcwallet";

WalletHandlerProcess::WalletHandlerProcess(WalletHandlerModel& pModel, const WalletSettings& pWalletSettings)
    : WalletHandlerInterface(pModel), settings(pWalletSettings)
{

    const QStringList& lSearchPaths = Utils::getStandardSearchPaths();

    wallet_cli_program = pWalletSettings.getWalletCliProgram();
    if (wallet_cli_program.isEmpty()) {


        const QStringList lWalletSearchFilenames = QStringList() << cWalletCli << cWalletCli+".exe";
        const QStringList lFoundWalletExecutables = Utils::findExecutables(lSearchPaths, lWalletSearchFilenames, true);

        qDebug() << "Found " << lFoundWalletExecutables.size() << " cli wallet executables : ";
        Utils::debugQStringList(lFoundWalletExecutables);


        if (!lFoundWalletExecutables.empty()) {
            wallet_cli_program = lFoundWalletExecutables.first();
        }

    }


    wallet_rpc_program = pWalletSettings.getWalletRpcProgram();
    if (wallet_rpc_program.isEmpty()) {

        const QStringList lWalletSearchFilenames = QStringList() << cWalletRpc << cWalletRpc+".exe";
        const QStringList lFoundWalletExecutables = Utils::findExecutables(lSearchPaths, lWalletSearchFilenames, true);

        qDebug() << "Found " << lFoundWalletExecutables.size() << " rpc wallet executables : ";
        Utils::debugQStringList(lFoundWalletExecutables);

        if (!lFoundWalletExecutables.empty()) {
            wallet_rpc_program = lFoundWalletExecutables.first();
        }

    }


}

WalletHandlerProcess::~WalletHandlerProcess() {

    if(closeWallet()) {

        qWarning() << "Ending WALLET process...";
        if(!main_process.waitForFinished(10000)){
            main_process.kill();
            main_process.waitForFinished(1000);
        }

    }


}


void WalletHandlerProcess::enable() {

    if ( settings.shouldSpawnWallet() ) {

        if (wallet_cli_program.isEmpty() || wallet_rpc_program.isEmpty()) {
            this->onFatalError(5);
            return;
        }

        QObject::connect(this, &WalletHandlerProcess::tryWalletResult, [this] (int pResult) {

            qDebug() << "[OK] Wallet try";
            qDebug() << "With result : " << pResult;
            if (pResult != 0) {
                qDebug() << "Simplewallet try failed. Aborting.";
                if (pResult == -1) {
                    this->onFatalError(1);
                }
                else {
                    this->onFatalError(pResult);
                }
                return;
            }

            if ( !openWallet(settings.getWalletFile(), settings.getWalletPassword(), settings.getWalletIP(), settings.getWalletPort()) ) {
                qDebug() << "Failed to start wallet ("<< main_process.program() << ")";
                this->onFatalError(4);
                return;
            }
            else {

                QObject::disconnect(this, SIGNAL(tryWalletResult(int)));
                this->onReady();
                return;
            }

        });

        tryWalletAsync(settings.getWalletFile(), settings.getWalletPassword());

    }


}

QList<QObject*> WalletHandlerProcess::findWallets(const QString& pPath) {

    const QUrl& lUrl = QUrl::fromUserInput(pPath);


    QStringList lWalletsFilesList =  Utils::findWalletsKeysFiles(lUrl);
    last_found_wallets = Utils::fileListToInfoWalletModelList(lWalletsFilesList, lUrl);

    return last_found_wallets;

}



bool WalletHandlerProcess::openWallet(const QString& pWalletFile, const QString& pWalletPassword, const QString& pBindIP, int pBindPort) {

    if ( main_process.state() == QProcess::Running ) {
        qWarning() << "Wallet is already open. Please close your wallet before opening another one";
        return false;
    }

    main_process.setProgram(wallet_rpc_program);

    QStringList lArguments;

    lArguments.append("--wallet=" + pWalletFile);
    lArguments.append("--pass=" + pWalletPassword);
    lArguments.append("--rpc-bind-ip=" + pBindIP);
    lArguments.append("--rpc-bind-port=" + QString::number(pBindPort));

    main_process.setArguments(lArguments);

    main_process.setProcessChannelMode(QProcess::MergedChannels);
    main_process.start();

    /* Wait 3 seconds to check if program terminates (daemon not running, wrongp password ... */
    bool lFinished = main_process.waitForFinished(3000);
    qDebug() << "Wallet State : " << main_process.state();

    if (lFinished) {
        qWarning() << "Wallet process finished too early with code " << main_process.exitCode() << ", " << main_process.exitStatus();
        return false;
    }

    qDebug() << "Wallet process started on " + pBindIP + ":" + QString::number(pBindPort) + " (" + main_process.program() + ")";

    return true;

}

bool WalletHandlerProcess::closeWallet() {

    if ( main_process.state() == QProcess::Running ) {
        main_process.terminate();
        return true;
    }

    /* Was already closed */
    return false;

}

bool WalletHandlerProcess::walletFileExists(const QString& pFile) {

    if ( pFile.isEmpty() ) {
        return false;
    }

    QFileInfo lFileInfo(pFile);

    qDebug() << "Path : " << lFileInfo.dir().path();

    return lFileInfo.exists() && lFileInfo.isFile();

}


bool WalletHandlerProcess::walletDirectoryExists(const QString& pFile) {

    if ( pFile.isEmpty() ) {
        return false;
    }

    QFileInfo lFileInfo(pFile);

    /* Attempts to create directory if doesn't exists */
    return lFileInfo.dir().exists() || QDir::root().mkpath(lFileInfo.absolutePath());

}



int WalletHandlerProcess::createWallet(const QString& pFile, const QString& pPassword, const QString& pSeed)
{

    if ( !walletDirectoryExists(pFile) ) {
        qWarning() << "Path doesn't exists : " << pFile;
        return 1;
    }

    if ( walletFileExists(pFile) ) {
        qWarning() << "Wallet file " << pFile << "already exists";
        return 1;
    }

    if ( pPassword.isEmpty() ) {
        qWarning() << "No password defined";
        return 2;
    }

    QProcess lCreateWalletProcess;
    lCreateWalletProcess.setProgram(wallet_cli_program);

    QStringList lArguments;

    lArguments.append("--generate-new-wallet=" + pFile);
    lArguments.append("--password="+ pPassword);
    /* Hack for forcing program to exit after generation */
    lArguments.append("--command=address");
    /**/
    lCreateWalletProcess.setArguments(lArguments);

    lCreateWalletProcess.start();

    if ( !lCreateWalletProcess.waitForFinished(5000) ) {
        qWarning() << "createWallet failed : SubProcess doesn't responded";

        qWarning() << "Please ensure that your executable is located at : " << lCreateWalletProcess.program();

        return -1;
    }

    int lExitCode = lCreateWalletProcess.exitCode();
    qDebug() << "PROCESS finished with : " << QString::number(lExitCode);

    return lExitCode == 0 ? 0 : 1;
}


void WalletHandlerProcess::tryWalletResponse(int pCode, QProcess::ExitStatus pExitStatus) {

    qWarning() << "tryWalletAsync returned status : " << pCode << ". " << pExitStatus << ".";
    emit tryWalletResult(pCode);
    this->onTryWalletResult(pCode == 0 ? 0 : -1);

}

void WalletHandlerProcess::tryWalletError(QProcess::ProcessError pError) {
    qDebug() << "Process error" << pError;

    if (pError == QProcess::FailedToStart) {

        emit tryWalletResult(-1);
        this->onTryWalletResult(-1);
    }
}

void WalletHandlerProcess::tryWalletAsync(const QString& pFile, const QString& pPassword) {

    QProcess* lTryWalletProcess = createTryWalletProcess(pFile,pPassword);
    if (!lTryWalletProcess) {
        qWarning() << "TryWalletProcess CANNOT START";
        emit tryWalletResult(2);
        return;
    }

    qDebug() << "Trying wallet : " << pFile;

    QObject::connect(lTryWalletProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(tryWalletResponse(int,QProcess::ExitStatus)));
    QObject::connect(lTryWalletProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(tryWalletError(QProcess::ProcessError)));

    lTryWalletProcess->start();

}


QProcess* WalletHandlerProcess::createTryWalletProcess(const QString& pFile, const QString& pPassword) {


    if ( pPassword.isEmpty() ) {
        qWarning() << "No password defined";
        return NULL;
    }

    qDebug() << "Creating wallet process : " << wallet_cli_program;

    QProcess* lTryWalletProcess = new QProcess();
    lTryWalletProcess->setProgram(wallet_cli_program);

    QStringList lArguments;

    lArguments.append("--wallet=" + pFile);
    lArguments.append("--password="+ pPassword);
    lArguments.append("--command=balance");
    lTryWalletProcess->setArguments(lArguments);


    return lTryWalletProcess;

}
