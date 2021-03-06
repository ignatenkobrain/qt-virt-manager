#include "domain_monitor_thread.h"

DomainMonitorThread::DomainMonitorThread(
        QObject *parent, virConnectPtr conn, QString _domainName) :
    QThread(parent), currWorkConn(conn), domainName(_domainName)
{
    domain = virDomainLookupByName(
                currWorkConn, domainName.toUtf8().data());
    virNodeInfo NodeInfo;
    if ( virNodeGetInfo(currWorkConn, &NodeInfo)+1 ) {
        nr_cores = NodeInfo.cores;
        //qDebug()<<nr_cores<<"cores";
    };
}

/* public slots */
void DomainMonitorThread::stop()
{
    if ( NULL!=domain ) {
        virDomainFree(domain);
    }
}

/* private slots */
void DomainMonitorThread::run()
{
    if ( NULL!=domain && nr_cores!=0 ) {
        virDomainInfo info;
        uint curr_cpuTime, cpu_time_diff, curr_time_mark,
                _time_diff, CPU_percent, MEM_percent, MEM;
        curr_time_mark = time(0);
        _time_diff = curr_time_mark - prev_time_mark;
        if ( virDomainGetInfo(domain, &info)+1 ) {
            curr_cpuTime = info.cpuTime;
            cpu_time_diff = curr_cpuTime - prev_cpuTime;
            CPU_percent = 100 * cpu_time_diff / (_time_diff * nr_cores * 1000000000);
            MEM = info.memory;
            MEM_percent = 100*MEM / info.maxMem;
            prev_cpuTime = curr_cpuTime;
            prev_time_mark = curr_time_mark;
        };
        //qDebug()<<info.state
        //        <<info.memory << info.maxMem
        //        <<QString("%1%").arg(MEM_percent)
        //        <<QString("%1%").arg(CPU_percent);
        if ( info.state == VIR_DOMAIN_RUNNING || info.state == VIR_DOMAIN_PAUSED ) {
            emit dataChanged(info.state, CPU_percent, MEM_percent, MEM);
        } else
            emit dataChanged(info.state, 0, 0, 0);
    };
}
