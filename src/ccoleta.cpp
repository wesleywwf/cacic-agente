#include "ccoleta.h"

CColeta::CColeta(QObject *parent)
{
    logcacic = new LogCacic(LOG_GERCOLS, Identificadores::ENDERECO_PATCH_CACIC+"/Logs");
}

void CColeta::coletaHardware()
{
    oHardware.iniciaColeta();
    logcacic->escrever(LogCacic::InfoLevel, "Coleta de hardware finalizada.");
    emit hardwareFinish();
}


void CColeta::coletaSoftware()
{
    oSoftware.iniciaColeta();
    logcacic->escrever(LogCacic::InfoLevel, QString("Coleta de software finalizada."));
    emit softwareFinish();
}

/********************************************************
 * Conecta os sinais com os slots corretos, ou seja,
 * sempre que um sinal for emitido, o slot será executado
 ********************************************************/
void CColeta::configuraColetas(){
    QObject::connect(this, SIGNAL(beginHardware()), this, SLOT(coletaHardware()));
    QObject::connect(this, SIGNAL(beginSoftware()), this, SLOT(coletaSoftware()));
    QObject::connect(this, SIGNAL(softwareFinish()), this, SLOT(softwareReady()));
    QObject::connect(this, SIGNAL(hardwareFinish()), this, SLOT(hardwareReady()));
}

bool CColeta::waitToCollect()
{
    int timeout = 0;
    while(!(this->softwareIsFinish && this->hardwareIsFinish) && timeout != 30){
        QThread::sleep(1);
        timeout++;
    }
    return true;
}

void CColeta::run()
{
    QJsonObject coleta = oCacic.getJsonFromFile("getConfig.json");
    QJsonObject actions = coleta["agentcomputer"].toObject()["actions"].toObject();

    //se for pra coletar hardware...
    if( actions.contains("col_hard") && actions["col_hard"].toBool()){
        logcacic->escrever(LogCacic::InfoLevel, QString("Iniciando coleta de hardware."));
        this->hardwareIsFinish = false;
        emit beginHardware();
    } else
        this->hardwareIsFinish = true;

    //se for pra coletar software...
    if ( actions.contains("col_soft") && actions["col_soft"].toBool()){
        logcacic->escrever(LogCacic::InfoLevel, QString("Iniciando coleta de software."));
        this->softwareIsFinish = false;
        emit beginSoftware();
    } else
        this->softwareIsFinish = true;

}

/*********************************************
 * Transforma todos os dados da classe em json
 *********************************************/
QJsonObject CColeta::toJsonObject()
{
    QJsonObject coletaJson;
    QJsonObject hardware = oHardware.toJsonObject();
    hardware["NetworkAdapterConfiguration"] = oComputer.toJsonObject()["networkDevices"].toArray();
    coletaJson["computador"] = oComputer.toJsonObject();
    coletaJson["hardware"] = hardware;
    coletaJson["software"] = oSoftware.toJsonObject();
    return coletaJson;
}

void CColeta::hardwareReady()
{
    this->hardwareIsFinish = true;
}

void CColeta::softwareReady()
{
    this->softwareIsFinish = true;
}

CACIC_Computer CColeta::getOComputer() const
{
    return oComputer;
}

cacic_software CColeta::getOSoftware() const
{
    return oSoftware;
}

cacic_hardware CColeta::getOHardware() const
{
    return oHardware;
}
