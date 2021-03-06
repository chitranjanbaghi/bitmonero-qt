#include "RPCMiner.h"

RPCMiner::RPCMiner(MinerModel& pMinerModel, const WalletSettings& pSettings)
    : MinerInterface(pMinerModel), rpc(pSettings.getMinerUri(), pSettings.getMinerPort())
{

}

RPCMiner::~RPCMiner() {

    miningstatus_timer.stop();

}

void RPCMiner::enable() {

    getMiningStatus();

    QObject::connect(&miningstatus_timer,SIGNAL(timeout()), this, SLOT(getMiningStatus()));
    miningstatus_timer.start(5000);

}

void RPCMiner::startMining(const QString& pMoneroAddress, unsigned int pNbThreads) {

    QJsonObject lObj;
    lObj["miner_address"] = pMoneroAddress;
    lObj["threads_count"] = (int) pNbThreads;

    JsonRPCRequest* lReq = rpc.sendRequest("start_mining",lObj, true);
    QObject::connect(lReq,&JsonRPCRequest::jsonResponseReceived,[this](const QJsonObject pJsonResponse) {

        const QString& lStatus = pJsonResponse["status"].toString();

        if ( lStatus == "OK" ) {
            this->onStartMiningSucceeded();
        }
        else {
            qWarning() << "Bad status for mining start : " << lStatus;
            this->onStartMiningFailed(lStatus);
        }
    });

}

void RPCMiner::stopMining() {

    JsonRPCRequest* lReq = rpc.sendRequest("stop_mining", QJsonObject(), true);
    QObject::connect(lReq,&JsonRPCRequest::jsonResponseReceived,[this](const QJsonObject pJsonResponse) {

        const QString& lStatus = pJsonResponse["status"].toString();

        if ( lStatus == "OK" ) {
            this->onStopMiningSucceeded();
        }
        else {
            qWarning() << "Bad status for mining stop : " << lStatus;
            this->onStopMiningFailed(lStatus);
        }

    });

}

void RPCMiner::getMiningStatus() {
    JsonRPCRequest* lReq = rpc.sendRequest("mining_status", QJsonObject(), true);

    QObject::connect(lReq,&JsonRPCRequest::jsonResponseReceived,[this](const QJsonObject pJsonResponse) {

//        qDebug() << "'mining_status' Response : " << pJsonResponse;
        const QString& lStatus = pJsonResponse["status"].toString();

        if ( lStatus == "OK" ) {

            if ( !pJsonResponse["active"].isBool() ||
                 !pJsonResponse["threads_count"].isDouble() ||
                 !pJsonResponse["address"].isString() ||
                 !pJsonResponse["speed"].isDouble() ) {

                qCritical() << "Invalid data format for 'mining_status' response. Is your Daemon up to date ?";
            }

            this->onGetMiningStatusResponse(
                    pJsonResponse["active"].toBool(),
                    pJsonResponse["threads_count"].toDouble(),
                    pJsonResponse["address"].toString(),
                    pJsonResponse["speed"].toDouble()
                    );
        }
        else {
            qWarning() << "Bad status for mining status : " << lStatus;
        }

    });
}

