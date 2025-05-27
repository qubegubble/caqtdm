/*
 *  This file is part of the caQtDM Framework, developed at the Paul Scherrer Institut,
 *  Villigen, Switzerland
 *
 *  The caQtDM Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The caQtDM Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the caQtDM Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2010 - 2014
 *
 *  Author:
 *    Anton Mezger
 *  Contact details:
 *    anton.mezger@psi.ch
 */
#include <QDebug>
#include <QThread>
#include "opcua_plugin.h"
#include "opcua_core.h"
#include <memory>
#include <QtConcurrent/QtConcurrent>

// as defined in knobDefines.h
//caType {caSTRING	= 0, caINT = 1, caFLOAT = 2, caENUM = 3, caCHAR = 4, caLONG = 5, caDOUBLE = 6};

// this demo plugin just gives you an idea how to use a plugin; for more details you should take a look
// at the epics3 plugin

// gives the plugin name back
#define qasc(x) x.toLatin1().constData()
extern "C"{
MutexKnobData* mutexKnobdataPtr;
MessageWindow *messageWindowPtr;
}

QString OPCUAPlugin::pluginName()
{
    return "opcua";
}

// constructor
OPCUAPlugin::OPCUAPlugin()
{
    qDebug() << "OPCUAPlugin: Create";
}

// initialize our communicationlayer with everything you need
int OPCUAPlugin::initCommunicationLayer(MutexKnobData *data, MessageWindow *messageWindow, QMap<QString, QString> options)
{
    mutexknobdataP = data;
    messagewindowP = messageWindow;
    mutexKnobdataPtr = data;
    messageWindowPtr = messageWindow;
    Channelcache.clear();

    if(messageWindowPtr){
        QString msg= "Info: OPCUA Plugin has been loaded.";
        messageWindowPtr->postMsgEvent(QtInfoMsg,(char *) qasc(msg));
    }

    if (!m_core)
        m_core.reset(new opc::OpcUaCore());

    return true;
}


// in this demo we update our interface here; normally you should update in from your controlsystem
// take a look how monitors are treated in the epics3 plugin
void OPCUAPlugin::updateInterface()
{
    double newValue = 0.0;

    QMutexLocker locker(&mutex);

    // go through our devices
    foreach(int index, listOfIndexes) {
        knobData* kData = mutexknobdataP->GetMutexKnobDataPtr(index);
        if((kData != (knobData *) Q_NULLPTR) && (kData->index != -1)) {
            QString key = kData->pv;

            // find this pv in our internal double values list (assume for now we are only treating doubles)
            // and increment its value
            QMap<QString, double>::iterator i = listOfDoubles.find(key);
            while (i !=listOfDoubles.end() && i.key() == key) {
                newValue = i.value();
                break;
            }
            // update some data
            kData->edata.rvalue = newValue;
            kData->edata.fieldtype = caDOUBLE;
            kData->edata.connected = true;
            kData->edata.accessR = true;
            kData->edata.accessW = false;
            kData->edata.monitorCount++;
            mutexknobdataP->SetMutexKnobData(kData->index, *kData);
            mutexknobdataP->SetMutexKnobDataReceived(kData);
        }
    }
}

// in this demo we update our values here
void OPCUAPlugin::updateValues()
{
    QMutexLocker locker(&mutex);
#ifndef HARDWORK
    QMap<QString, double>::iterator i;
    for (i = listOfDoubles.begin(); i != listOfDoubles.end(); ++i) i.value()++;
#else
    QtConcurrent::run(this, &DemoPlugin::updateHardwork);
#endif
}

#ifdef HARDWORK
void  DemoPlugin::updateHardwork()
{
    qDebug() << "hardwork";
    QMap<QString, double>::iterator i;
    for (i = listOfDoubles.begin(); i != listOfDoubles.end(); ++i) i.value()++;
}
#endif

// caQtDM_Lib will call this routine for defining a monitor
int OPCUAPlugin::pvAddMonitor(int index, knobData *kData, int rate, int skip)
{
    QString raw = QString::fromLatin1(kData->pv);

    int sep = raw.indexOf("::");
    if (sep >= 0) raw = raw.mid(sep + 2);
    if (raw.startsWith("opcua://", Qt::CaseInsensitive)) {
        raw = raw.mid(QString("opcua://").length());
    }

    int splitPos = raw.lastIndexOf("/ns=");
    if (splitPos < 0) {
        if (messageWindowPtr) {
            QString msg = "Invalid OPCUA PV format. Expected <endpoint>/ns=...; got: " + raw;
            messageWindowPtr->postMsgEvent(QtCriticalMsg, (char*)msg.toLatin1().constData());
        }
        return false;
    }

    QString endpoint = raw.left(splitPos);
    QString nodeId = raw.mid(splitPos + 1).trimmed();
    Channelcache.insert(nodeId, index);

    std::shared_ptr<opc::OpcUaCore> core;
    {
        QMutexLocker lock(&m_mutex);
        if (!m_cores.contains(endpoint)) {
            m_cores[endpoint] = std::make_shared<opc::OpcUaCore>();
            m_connectionState[endpoint] = ConnectionState::NotConnected;

            auto core = m_cores[endpoint];
            QObject::connect(core.get(), &opc::OpcUaCore::valueRead, [=](const QString &nodeId, const QVariant &value) {
                auto range = Channelcache.equal_range(nodeId);
                for (auto it = range.first; it != range.second; ++it) {
                    int idx = it.value();
                    knobData kData = mutexKnobdataPtr->GetMutexKnobData(idx);
                    QMutexLocker locker((QMutex *)kData.mutex);
                    kData.edata.rvalue = value.toDouble();
                    kData.edata.connected = 1;
                    kData.edata.fieldtype = caDOUBLE;
                    kData.edata.accessR = true;
                    kData.edata.accessW = false;
                    kData.edata.monitorCount++;
                    mutexknobdataP->SetMutexKnobData(kData.index, kData);
                    mutexknobdataP->SetMutexKnobDataReceived(&kData);
                }

                if (messageWindowPtr) {
                    QString msg = QString("OPCUA: [%1] = %2").arg(nodeId).arg(value.toString());
                    messageWindowPtr->postMsgEvent(QtDebugMsg, (char*)msg.toLatin1().constData());
                }
            });
        }
        core = m_cores[endpoint];
    }

    QMutexLocker lock(&m_mutex);
    ConnectionState state = m_connectionState[endpoint];

    if (state == ConnectionState::Connected) {
        core->subscribeToNode(nodeId);
        if (messageWindowPtr) {
            QString msg = QString("OPCUA: Subscribed %1 on %2 (immediate)").arg(nodeId, endpoint);
            messageWindowPtr->postMsgEvent(QtDebugMsg, (char*)msg.toLatin1().constData());
        }
    } else {
        // Store subscription to do later
        m_pendingSubscriptions[endpoint].append(nodeId);

        if (state == ConnectionState::NotConnected) {
            m_connectionState[endpoint] = ConnectionState::Connecting;

            // Start connection
            core->connectOpc(endpoint, [this, endpoint, core](bool success) {
                QMutexLocker lock(&m_mutex);
                if (!success) {
                    m_connectionState[endpoint] = ConnectionState::NotConnected;
                    if (messageWindowPtr) {
                        QString err = QString("OPCUA: Failed to connect to %1").arg(endpoint);
                        messageWindowPtr->postMsgEvent(QtCriticalMsg, (char*)err.toLatin1().constData());
                    }
                    return;
                }

                m_connectionState[endpoint] = ConnectionState::Connected;

                if (messageWindowPtr) {
                    QString info = QString("OPCUA: Connected to %1").arg(endpoint);
                    messageWindowPtr->postMsgEvent(QtInfoMsg, (char*)info.toLatin1().constData());
                }

                // Now subscribe to all pending nodeIds
                for (const QString& nodeId : m_pendingSubscriptions[endpoint]) {
                    core->subscribeToNode(nodeId);
                    if (messageWindowPtr) {
                        QString msg = QString("OPCUA: Subscribed %1 on %2").arg(nodeId, endpoint);
                        messageWindowPtr->postMsgEvent(QtDebugMsg, (char*)msg.toLatin1().constData());
                    }
                }
                m_pendingSubscriptions[endpoint].clear();
            });
        }
    }

    return true;
}




// caQtDM_Lib will call this routine for getting rid of a monitor
int OPCUAPlugin::pvClearMonitor(knobData *kData) {
    Channelcache.remove(kData->pv, kData->index);
    return true;
}

int OPCUAPlugin::pvFreeAllocatedData(knobData *kData)
{
    QMutexLocker locker((QMutex *)kData->mutex);
    if (kData->edata.dataB) {
        free(kData->edata.dataB);
        kData->edata.dataB = nullptr;
    }
    return true;
}

// caQtDM_Lib will call this routine for setting data (see for more detail the epics3 plugin)
int OPCUAPlugin::pvSetValue(char *pv, double rdata, int32_t idata, char *sdata, char *object, char *errmess, int forceType) {
    // Optional: You can implement write support here using m_core
    qDebug() << "pvSetValue not implemented for OPC UA";
    return false;
}

// caQtDM_Lib will call this routine for setting waveforms data (see for more detail the epics3 plugin)
int OPCUAPlugin::pvSetWave(char *pv, float *fdata, double *ddata, int16_t *data16, int32_t *data32, char *sdata, int nelm, char *object, char *errmess) {
    Q_UNUSED(pv);
    Q_UNUSED(fdata);
    Q_UNUSED(ddata);
    Q_UNUSED(data16);
    Q_UNUSED(data32);
    Q_UNUSED(sdata);
    Q_UNUSED(nelm);
    Q_UNUSED(object);
    Q_UNUSED(errmess);
    QMutexLocker locker(&mutex);
    qDebug() << "OPCUAPlugin:pvSetWave";
    return true;
}

// caQtDM_Lib will call this routine for getting a description of the monitor
int OPCUAPlugin::pvGetTimeStamp(char *pv, char *timestamp) {
    Q_UNUSED(pv);
    Q_UNUSED(timestamp);
    qDebug() << "OPCUAPlugin:pvgetTimeStamp";
    strcpy(timestamp, "timestamp in epics format");
    return true;
}

// caQtDM_Lib will call this routine for getting the timestamp for this monitor
int OPCUAPlugin::pvGetDescription(char *pv, char *description) {
    Q_UNUSED(pv);
    Q_UNUSED(description);
    qDebug() << "OPCUAPlugin:pvGetDescription";
    strcpy(description, "hello, I am a double");
    return true;
}

// next two routines are used to stop and restart the monitoring (used in case of tabWidgets in the display)
int OPCUAPlugin::pvClearEvent(void * ptr) {
    Q_UNUSED(ptr);
    qDebug() << "OPCUAPlugin:pvClearEvent";
    return true;
}

int OPCUAPlugin::pvAddEvent(void * ptr) {
    Q_UNUSED(ptr);
    qDebug() << "OPCUAPlugin:pvAddEvent";
    return true;
}

// next two routines are used to connect and disconnect monitors when the application gest suspended and reactivated
int OPCUAPlugin::pvReconnect(knobData *kData) {
    Q_UNUSED(kData);
    qDebug() << "OPCUAPlugin:pvReconnect";
    return true;
}

int OPCUAPlugin::pvDisconnect(knobData *kData) {
    Q_UNUSED(kData);
    qDebug() << "OPCUAPlugin:pvDisconnect";
    return true;
}

// flush any io is periodically called (1s timer) in order to flush the disconnection and reconnection
// used for pv's that will be hidden and shown in case of tabwidgets
int OPCUAPlugin::FlushIO() {
    //qDebug() << "OPCUAPlugin:FlushIO";
    return true;
}

// termination (in case of epics3, this is used to destroy the context when the application gest deactivated
// otherwise probably no meaning; in this demo, we stop the simulation, however it will not be reactivated
// any more (you may do that through pvReconnect)
int OPCUAPlugin::TerminateIO() {
    //qDebug() << "OPCUAPlugin:TerminateIO";
    timerValues->stop();
    timer->stop();
    return true;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#else
Q_EXPORT_PLUGIN2(DemoPlugin, DemoPlugin)
#endif
