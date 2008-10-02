/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "process.h"
#include "application.h"
#include "settings.h"
#include "channel.h"

bool Process::start(const QString &program, Channel *channel)
{
    QString p = expandVars(program, channel);
    qDebug() << p;
    if (startDetached(p))
        return true;
    QMessageBox::warning(QApplication::activeWindow(),
            tr("エラー"), tr("プログラムの実行に失敗しました。\n%1").arg(p));
    return false;
}

QString Process::expandVars(const QString &str, Channel *channel)
{
    QString s = str;
    QRegExp rxVar("\\$\\{?([\\w\\d_]+)(?:\\(([\\w\\d_]+)\\))?\\}?");
    int pos = 0;
    while ((pos = rxVar.indexIn(s, pos)) != -1) {
        QString name = rxVar.cap(1).toUpper();
        QString arg = rxVar.cap(2).toUpper();
        QString value;
        if (name == "PEERCAST_SERVER") {
            QUrl pcserv = qApp->settings()->value("PeerCast/ServerUrl").toString();
            if (arg.isEmpty())
                value = pcserv.toString();
            else if (arg == "HOST")
                value = pcserv.host();
            else if (arg == "PORT")
                value = QString::number(pcserv.port());
        }
        if (channel) {
            if (name == "STREAM_URL") {
                value = channel->streamUrl(arg.isEmpty() ? "http" : arg.toLower()).toEncoded();
            } else if (name == "CHANNEL" and !arg.isEmpty()) {
                if (arg == "STREAM_URL")
                    value = channel->streamUrl().toEncoded();
                else if (arg == "LONG_DESC")
                    value = channel->longDescription();
                else
                    value = channel->property(arg.toLower().toAscii()).toString();
            }
        }
        if (value.isNull()) {
            pos += rxVar.matchedLength();
        } else {
            s.replace(pos, rxVar.matchedLength(), value);
            pos += value.length();
        }
    }
    return s;
}

