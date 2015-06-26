/************************************************************************************

Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
Copyright   :   Copyright Bradley Austin Davis. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#include "QtCommon.h"
#include "MainWindow.h"
#include <QGuiApplication>
#include <QQuickView>

#ifndef _DEBUG
#include "TrackerbirdConfig.h"
#endif

#ifdef TRACKERBIRD_PRODUCT_ID
#include <Trackerbird.h>
#endif

const char * ORG_NAME = "Oculus Rift in Action";
const char * ORG_DOMAIN = "oculusriftinaction.com";
const char * APP_NAME = "ShadertoyVR";

QDir CONFIG_DIR;
QSharedPointer<QFile> LOG_FILE;
QtMessageHandler ORIGINAL_MESSAGE_HANDLER;
static const QUrl SOURCE{ "qrc:layouts/DesktopWindow.qml" };
QQuickView* MAIN_WINDOW{ nullptr };


void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    ORIGINAL_MESSAGE_HANDLER(type, context, msg);
    QByteArray localMsg = msg.toLocal8Bit();
    QString now = QDateTime::currentDateTime().toString("yyyy.dd.MM_hh:mm:ss");
    switch (type) {
    case QtDebugMsg:
        LOG_FILE->write(QString().sprintf("%s Debug:    %s (%s:%u, %s)\n", now.toLocal8Bit().constData(), localMsg.constData(), context.file, context.line, context.function).toLocal8Bit());
        break;
    case QtWarningMsg:
        LOG_FILE->write(QString().sprintf("%s Warning:  %s (%s:%u, %s)\n", now.toLocal8Bit().constData(), localMsg.constData(), context.file, context.line, context.function).toLocal8Bit());
        break;
    case QtCriticalMsg:
        LOG_FILE->write(QString().sprintf("%s Critical: %s (%s:%u, %s)\n", now.toLocal8Bit().constData(), localMsg.constData(), context.file, context.line, context.function).toLocal8Bit());
        break;
    case QtFatalMsg:
        LOG_FILE->write(QString().sprintf("%s Fatal:    %s (%s:%u, %s)\n", now.toLocal8Bit().constData(), localMsg.constData(), context.file, context.line, context.function).toLocal8Bit());
        LOG_FILE->flush();
        abort();
}
    LOG_FILE->flush();
}

void initTracker() {
#if (!defined(_DEBUG) && defined(TRACKERBIRD_PRODUCT_ID))
    tbCreateConfig(TRACKERBIRD_URL, TRACKERBIRD_PRODUCT_ID,
        TRACKERBIRD_PRODUCT_VERSION, TRACKERBIRD_BUILD_NUMBER,
        TRACKERBIRD_MULTISESSION_ENABLED);
    tbStart();
#endif
}

void shutdownTracker() {
#if (!defined(_DEBUG) && defined(TRACKERBIRD_PRODUCT_ID))
    tbStop(TRUE);
#endif
}

void initAppInformation() {
    QCoreApplication::setOrganizationName(ORG_NAME);
    QCoreApplication::setOrganizationDomain(ORG_DOMAIN);
    QCoreApplication::setApplicationName(APP_NAME);
#if (!defined(_DEBUG) && defined(TRACKERBIRD_PRODUCT_ID))
    QCoreApplication::setApplicationVersion(QString::fromWCharArray(TRACKERBIRD_PRODUCT_VERSION));
#endif
}

void initResources() {
    Q_INIT_RESOURCE(ShadertoyVR);
}

void initSettings() {
    CONFIG_DIR = QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
}

void initLogging() {
    QString currentLogName = CONFIG_DIR.absoluteFilePath("ShadertoyVR.log");
    LOG_FILE = QSharedPointer<QFile>(new QFile(currentLogName));
    if (LOG_FILE->exists()) {
        QFile::rename(currentLogName,
            CONFIG_DIR.absoluteFilePath("ShadertoyVR_" +
            QDateTime::currentDateTime().toString("yyyy.dd.MM_hh.mm.ss") + ".log"));
    }
    if (!LOG_FILE->open(QIODevice::WriteOnly | QIODevice::Append)) {
        qWarning() << "Could not open log file";
    }
    ORIGINAL_MESSAGE_HANDLER = qInstallMessageHandler(messageHandler);
}

void shutdownLogging() {
    qInstallMessageHandler(ORIGINAL_MESSAGE_HANDLER);
    LOG_FILE->close();
}

void initTypes() {
    qmlRegisterType<MainWindow>("ShadertoyVR", 1, 0, "MainWindow");
}

void initUi() {
    static QQmlEngine engine;
    MAIN_WINDOW = new QQuickView;
    MAIN_WINDOW->setSource(SOURCE);
    MAIN_WINDOW->show();
}

MAIN_DECL {
  try {

#if !defined(_DEBUG)
    qputenv("QT_QPA_PLATFORM_PLUGIN_PATH", "./plugins");
#endif
    QT_APP_WITH_ARGS(QGuiApplication);

    initTracker();
    initAppInformation();
    initResources();
    initSettings();
    initLogging();
    initTypes();
    initUi();

    int result = app.exec();

    shutdownTracker();
    shutdownLogging();

    //MainWindow * riftRenderWidget = new MainWindow();
    //riftRenderWidget->start();
    //riftRenderWidget->requestActivate();
    //riftRenderWidget->stop();
    //riftRenderWidget->makeCurrent();
    //Platform::runShutdownHooks();
    //delete riftRenderWidget;
    return result;
  } catch (std::exception & error) {
    qFatal(error.what());
  } catch (const std::string & error) {
    qFatal(error.c_str());
  }
  return -1;
}

#if 0
SiHdl si;
SiGetEventData getEventData;
bool nativeEventFilter(const QByteArray & eventType, void * message, long * result) {
  // On Windows, eventType is set to "windows_generic_MSG" for messages sent
  // to toplevel windows, and "windows_dispatcher_MSG" for system - wide
  // messages such as messages from a registered hot key.In both cases, the
  // message can be casted to a MSG pointer.The result pointer is only used
  // on Windows, and corresponds to the LRESULT pointer.
  MSG & msg = *(MSG*)message;
  SiSpwEvent siEvent;
  vec3 tr, ro;
  long * md = siEvent.u.spwData.mData;
  SiGetEventWinInit(&getEventData, msg.message, msg.wParam, msg.lParam);
  if (SiGetEvent(si, 0, &getEventData, &siEvent) == SI_IS_EVENT) {
    switch (siEvent.type) {
    case SI_MOTION_EVENT:
      emit sixDof(
        vec3(md[SI_TX], md[SI_TY], -md[SI_TZ]),
        vec3(md[SI_RX], md[SI_RY], md[SI_RZ]));
      break;

    default:
      break;
    }
    return true;
  }
  return false;
}

SpwRetVal result = SiInitialize();
int cnt = SiGetNumDevices();
SiDevID devId = SiDeviceIndex(0);
SiOpenData siData;
SiOpenWinInit(&siData, (HWND)riftRenderWidget.effectiveWinId());
si = SiOpen("app", devId, SI_NO_MASK, SI_EVENT, &siData);
installNativeEventFilter(this);

#endif



/*
yes:
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/Xlf3D8.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/XsjXR1.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/XslXW2.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/ld2GRz.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/ldSGRW.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/ldfGzr.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/4slGzn.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/ldj3Dm.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/lsl3W2.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/lts3Wn.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/XdBSzd.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/MsBGRh.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/4ds3zn.json"

maybe:

Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/Xll3Wn.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/Xsl3Rl.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/ldSGzR.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/ldX3Ws.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/ldsGWB.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/lsBXD1.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/Xds3zN.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/4sl3zn.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/4sjXzG.json"

debug:
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/ldjXD3.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/XdfXDB.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/Msl3Rr.json"
Loading shader from  "c:/Users/bdavis/AppData/Local/Oculus Rift in Action/ShadertoyVR/shadertoy/4sXGDs.json"

*/
