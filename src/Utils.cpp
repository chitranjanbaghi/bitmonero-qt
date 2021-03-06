#include "Utils.h"

#include <QCoreApplication>
#include <QDir>
#include <QUrl>
#include <QString>

#include "Models/InfoWalletModel.h"

Utils::Utils()
{
//    NOP
}


/* Returns first executable pFilenames found in pPaths */
const QStringList Utils::findExecutables(const QStringList& pPaths, const QStringList& pFilenames, bool pFindOne) {

    QStringList lFoundExecutables;

    for( const QString& lPath : pPaths ) {

        for ( const QString& lFilename : pFilenames ) {
            QFileInfo lFile(lPath + "/" + lFilename);

            if ( lFile.exists() && lFile.isFile() ) {
                lFoundExecutables.append(lFile.filePath());
                if (pFindOne) {
                    return lFoundExecutables;
                }
            }

        }
    }

    return lFoundExecutables;

}

const QString Utils::extractWalletAddress(const QString& pWalletPath) {

    QFile lFile( pWalletPath + ".address.txt");
    if ( !lFile.open(QFile::ReadOnly | QFile::Text) ) {
        return "";
    }

    QTextStream lInputStream(&lFile);
    const QString& lAddress = lInputStream.readAll();

    if (lAddress.size() != 95) {
        return "";
    }

    return lAddress;
}


const QStringList Utils::getStandardSearchPaths() {

    const QString& lAppPath = QCoreApplication::applicationDirPath();

    QStringList lSearchPaths;
    lSearchPaths.append( QDir::currentPath() );
    lSearchPaths.append( QDir::homePath() + "/.bitmonero-qt/");
    lSearchPaths.append( lAppPath );
    lSearchPaths.append( lAppPath + "/bitmonero/");

    lSearchPaths.append( "/usr/bin" );
    lSearchPaths.append( "/usr/local/bin" );


    return lSearchPaths;
}


const QStringList Utils::findWalletsKeysFiles(const QUrl& pUrl, const QString& pFileSuffix) {

    QDir lDir(pUrl.toLocalFile());

    qDebug() << "Dir : " << lDir.path();

    QStringList lFoundWalletKeysFiles;

    QStringList lNameFilters;
    lNameFilters.append("*"+pFileSuffix);
    for( const QString& lFileName : lDir.entryList( lNameFilters, QDir::Files) ) {

        lFoundWalletKeysFiles.append(lFileName.mid(0, lFileName.size() - pFileSuffix.size()));

    }

    return lFoundWalletKeysFiles;

}

const QList<QObject*> Utils::fileListToInfoWalletModelList(const QStringList& pWalletFilesList, const QUrl& pFolderUrl) {

    QList<QObject*> lInfoWalletModelList;
    qDebug() << "Found Wallets : ";
    for ( const QString& lWalletName : pWalletFilesList ) {
        qDebug() << "- " << lWalletName;
        const QString& lWalletPath = pFolderUrl.toLocalFile() + QDir::separator() + lWalletName;
        const QString& lAddress = Utils::extractWalletAddress(lWalletPath);
        InfoWalletModel* ptrWalletInfo = new InfoWalletModel(lWalletName, lWalletPath, lAddress, 0);

        lInfoWalletModelList.append(ptrWalletInfo);
    }

    return lInfoWalletModelList;

}

const void Utils::debugQStringList(const QStringList& pList) {
    for( const QString& lItemStr : pList ) {
        qDebug() << "- " << lItemStr;
    }
}
