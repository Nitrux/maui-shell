/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "plugin.h"

#include <QQmlEngine>

#include "client.h"
#include "context.h"
#include "modulemanager.h"
#include "port.h"
#include "profile.h"
#include "pulseaudio.h"
#include "sink.h"
#include "source.h"
#include "volumemonitor.h"

#include "listitemmenu.h"
// #include "microphoneindicator.h"
#include "speakertest.h"
#include "volumefeedback.h"
#include "model/sortfiltermodel.h"
// #include "volumeosd.h"

static QJSValue pulseaudio_singleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)

    QJSValue object = scriptEngine->newObject();
    object.setProperty(QStringLiteral("NormalVolume"), (double)QPulseAudio::Context::NormalVolume);
    object.setProperty(QStringLiteral("MinimalVolume"), (double)QPulseAudio::Context::MinimalVolume);
    object.setProperty(QStringLiteral("MaximalVolume"), (double)QPulseAudio::Context::MaximalVolume);
    return object;
}

void CaskAudioPlugin::registerTypes(const char *uri)
{
    QPulseAudio::Context::setApplicationId(QStringLiteral("maui.cask.audio"));

    qmlRegisterType<SortFilterModel>(uri, 1, 0, "SortFilterModel");

    qmlRegisterType<QPulseAudio::CardModel>(uri, 1, 0, "CardModel");
    qmlRegisterType<QPulseAudio::SinkModel>(uri, 1, 0, "SinkModel");
    qmlRegisterType<QPulseAudio::SinkInputModel>(uri, 1, 0, "SinkInputModel");
    qmlRegisterType<QPulseAudio::SourceModel>(uri, 1, 0, "SourceModel");
    qmlRegisterType<QPulseAudio::ModuleManager>(uri, 1, 0, "ModuleManager");
    qmlRegisterType<QPulseAudio::SourceOutputModel>(uri, 1, 0, "SourceOutputModel");
    qmlRegisterType<QPulseAudio::StreamRestoreModel>(uri, 1, 0, "StreamRestoreModel");
    qmlRegisterType<QPulseAudio::ModuleModel>(uri, 1, 0, "ModuleModel");
    qmlRegisterType<QPulseAudio::VolumeMonitor>(uri, 0, 01, "VolumeMonitor");
    qmlRegisterUncreatableType<QPulseAudio::PulseObject>(uri, 1, 0, "PulseObject", QString());
    qmlRegisterUncreatableType<QPulseAudio::Profile>(uri, 1, 0, "Profile", QString());
    qmlRegisterUncreatableType<QPulseAudio::Port>(uri, 1, 0, "Port", QString());
    qmlRegisterType<ListItemMenu>(uri, 1, 0, "ListItemMenu");
    // qmlRegisterType<VolumeOSD>(uri, 1, 0, "VolumeOSD");
    qmlRegisterType<VolumeFeedback>(uri, 1, 0, "VolumeFeedback");
    qmlRegisterType<SpeakerTest>(uri, 1, 0, "SpeakerTest");
    qmlRegisterSingletonType(uri, 1, 0, "PulseAudio", pulseaudio_singleton);
    // qmlRegisterSingletonType<MicrophoneIndicator>(uri, 1, 0, "MicrophoneIndicator", [](QQmlEngine *engine, QJSEngine *jsEngine) -> QObject * {
    //     Q_UNUSED(engine);
    //     Q_UNUSED(jsEngine);
    //     return new MicrophoneIndicator();
    // });
    qmlRegisterAnonymousType<QPulseAudio::Client>(uri, 1);
    qmlRegisterAnonymousType<QPulseAudio::Sink>(uri, 1);
    qmlRegisterAnonymousType<QPulseAudio::Source>(uri, 1);
    qmlRegisterAnonymousType<QPulseAudio::VolumeObject>(uri, 1);
}
