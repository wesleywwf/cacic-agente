#ifndef CACIC_COMM_H
#define CACIC_COMM_H

#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QFile>
#include <QUrlQuery>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>

class CacicComm {

private:
    QUrlQuery params;
    QString urlGerente;
    QString urlSsl;
    // FIXME: Get from configuration
    QString usuario = "user";
    QString password = "123456";
    std::string session;

public:
    CacicComm (){

    }

    CacicComm (QString urlGerente,          QString operatingSystem,     QString computerSystem,  QString csCipher,
               QString csDebug,             QString csCompress,          QString httpUserAgent,   QString moduleFolderName,
               QString moduleProgramName,   QString networkConfiguration,QString phpAuthPw,       QString phpAuthUser,
               QString so,                  QString cacicVersion,        QString gercolsVersion)
    {
        this->urlGerente = urlGerente;
        params.addQueryItem("OperatingSystem", operatingSystem);
        params.addQueryItem("ComputerSystem",computerSystem);
        params.addQueryItem("cs_cipher",csCipher);
        params.addQueryItem("cs_debug",csDebug);
        params.addQueryItem("cs_compress",csCompress);
        params.addQueryItem("HTTP_USER_AGENT",httpUserAgent);
        params.addQueryItem("ModuleFolderName",moduleFolderName);
        params.addQueryItem("ModuleProgramName",moduleProgramName);
        params.addQueryItem("NetworkAdapterConfiguration",networkConfiguration);
        params.addQueryItem("PHP_AUTH_PW",phpAuthPw);
        params.addQueryItem("PHP_AUTH_USER",phpAuthUser);
        params.addQueryItem("te_so",so);
        params.addQueryItem("te_versao_cacic",cacicVersion);
        params.addQueryItem("te_versao_gercols",gercolsVersion);


    }

    QJsonObject comm(QString route, const QJsonObject &json = QJsonObject(), bool isSsl = false)
    {
        QByteArray data;
        QNetworkRequest req;
        QUrl url;
        if (isSsl){
            url = urlSsl + route;
            req.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        } else
            url = urlGerente + route;
        req.setUrl(url);
        req.setHeader(QNetworkRequest::LocationHeader, "Cacic Agente");
        if (json.empty())
        {
            //se não for passado QJson, será mandado os valores do get/test antigo. (Será retirado depois)
            req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            data.append(params.toString());
        } else {
            // Add JSON
            QJsonDocument d(json);
            data.append(d.toVariant().toString());
            req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        }
//        qDebug() << QString::fromUtf8(d.toJson().trimmed());
        // cria um event-loop temporario
        QEventLoop eventLoop;
        // "quit()" o event-loop, when the network request "finished()"
        QNetworkAccessManager mgr;
        QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

        // a requisição HTTP

        QNetworkReply *reply = mgr.post(req, data);
        if (!reply->sslConfiguration().isNull()){
//            qDebug() << "Eh ssl";
            reply->ignoreSslErrors();
        }
        eventLoop.exec(); // sai do looping chamando o "finished()".
        QVariantMap replyValue;
        //grava o código de retorno
        replyValue["codestatus"] = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
//        qDebug() << "code: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();

        if (reply->error() == QNetworkReply::NoError) {
            //se não houver erro, grava o retorno;
            QString strReply = (QString)reply->readAll();
            replyValue["reply"] = strReply;
//            qDebug() << "Response:" << strReply;

            delete reply;
        } else {
            //failure
//            qDebug() << "Failure" <<reply->errorString();
            replyValue["error"] = reply->errorString();
            delete reply;


        }
        //retorna o json;
        QJsonObject jsonObj = QJsonObject::fromVariantMap(replyValue);
        return jsonObj;
    }


    /* commStatus
     * execulta um teste do tipo GET na urlGerente;
     * @return retorna o resultado da conexão HTTP:
     *      exemplo: 200 OK
     */
    bool commStatus(){
        // cria um event-loop temporario
        QEventLoop eventLoop;
        // "quit()" o event-loop, when the network request "finished()"
        QNetworkAccessManager mgr;
        QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
        // a requisição HTTP
        QUrl url = urlGerente;
        QNetworkRequest req( url );
        req.setHeader(QNetworkRequest::LocationHeader, "Cacic Agente");
        QNetworkReply *reply = mgr.get(req);
        eventLoop.exec(); // sai do looping chamando o "finished()".

        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        if (reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).isValid()){
//            qDebug() << "Status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << reason;
            return true;
        }else{
            reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
//              qDebug() << "Error:" << reason;
            return false;
        }
    }

    QJsonObject login() {
        // Cria dados de login
        QVariantMap loginMap;
        loginMap["user"] = this->usuario;
        loginMap["password"] = this->password;
        QJsonObject loginJson = QJsonObject::fromVariantMap(loginMap);

        // Cria conexão e retorna Json da sessão
        QJsonObject retorno = this->comm("/ws/get/test", loginJson);

        return retorno;

    }

    void setUrlSsl(QString value){
        this->urlSsl = value;
    }

    QString getUrlSsl (){
        return this->urlSsl;
    }

signals:
    void finished(QNetworkReply* reply);

};
#endif // CACIC_COMM_H