#include "cacicd.h"

cacicD::cacicD(int argc, char **argv) : QtService<QCoreApplication>(argc, argv, "CacicDaemon")
{
    try{
        this->createApplication(argc, argv);
        Ocacictimer = new CacicTimer(application()->applicationDirPath());
        ccacic = new CCacic();
        setServiceDescription("Cacic Daemon");
        setServiceFlags(QtService::Default);
        OcheckModules = new CheckModules(application()->applicationDirPath());
        logManager = QLogger::QLoggerManager::getInstance();
        logManager->addDestination(application()->applicationDirPath() + "/Logs/cacicLog.txt","Cacic Daemon",QLogger::InfoLevel);
        logManager->addDestination(application()->applicationDirPath() + "/Logs/cacicLog.txt","Cacic Daemon",QLogger::ErrorLevel);
    } catch (...){
        qCritical() << "Error desconhecido no construtor.";
        QLogger::QLog_Error("Cacic Daemon", QString("Erro desconhecido no construtor."));
    }
}

cacicD::~cacicD()
{
    try{
        logManager->closeLogger();
        application()->exit();
    } catch (...){
        QLogger::QLog_Error("Cacic Daemon", QString("Erro desconhecido no destrutor."));
    }
}

void cacicD::start() {
    try{
        if(Ocacictimer->comunicarGerente()){
            OcheckModules->start();
            QLogger::QLog_Info("Cacic Daemon", QString("Servico iniciado em " + application()->applicationDirPath() + "."));
            QJsonObject result = ccacic->getJsonFromFile(application()->applicationDirPath() + "/getConfig.json");
            if(!result.contains("error") && !result.isEmpty()){

                QJsonObject agenteConfigJson = result["agentcomputer"].toObject();
                QJsonObject configuracoes = agenteConfigJson["configuracoes"].toObject();

                //o valor nu_intervalo_exec vem em minutos. O valor que o timer aceita é em milisegundos,por isso 60000
                Ocacictimer->setPeriodicidadeExecucao(configuracoes["nu_intervalo_exec"].toString().toInt() * 60000);
                Ocacictimer->iniciarTimer(true);
            }
<<<<<<< HEAD
            //delete OcheckModules;
=======
            delete OcheckModules;
>>>>>>> 3bba07f0e727b7333683c2d175316686f95d8b9e
        }else{
            QLogger::QLog_Error("Cacic Daemon", QString("Problemas com o arquivo getConfig.json"));
            QLogger::QLog_Info("Cacic Daemon", QString("Inicializando periodicidade de execução do serviço com tempo padrão."));
            Ocacictimer->setPeriodicidadeExecucao(this->periodicidadeExecucaoPadrao * 60000); // em minutos !!!!
            Ocacictimer->iniciarTimer(false);
        }
    }catch (...){
        QLogger::QLog_Error("Cacic Daemon", QString("Erro desconhecido ao iniciar o serviço."));
    }
}

void cacicD::pause()
{
    try{
        QLogger::QLog_Info("Cacic Daemon", QString("Serviço pausado."));
    } catch (...){
        QLogger::QLog_Error("Cacic Daemon", QString("Erro desconhecido ao pausar o serviço."));
    }
}

void cacicD::resume()
{
    try{
        QLogger::QLog_Info("Cacic Daemon", QString("Serviço resumido."));
    } catch (...){
        QLogger::QLog_Error("Cacic Daemon", QString("Erro desconhecido ao resumir o serviço."));
    }
}

void cacicD::stop()
{
    try{
        QLogger::QLog_Info("Cacic Daemon", QString("Serviço parado."));
        this->application()->quit();
    } catch (...){
        QLogger::QLog_Error("Cacic Daemon", QString("Erro desconhecido ao parar o serviço."));
    }
}


