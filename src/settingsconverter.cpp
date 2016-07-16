/*
 *  Copyright (C) 2008 by ciao <ciao@users.sourceforge.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include "settingsconverter.h"
#include "settings.h"
#include "channelmatcher.h"

static bool isTwoWayFavorites(Settings *settings);
static void convertToTwoWayFavorites(Settings *settings);
static void collectNonGroupItems(QList<ChannelMatcher::Expression *>& items, ChannelMatcher::Expression* group);
static ChannelMatcher::Expression* addTopLevelGroup(ChannelMatcher& matcher, QString name);

void SettingsConverter::convert(Settings *settings)
{
    if (settings->value("General/Version").toString().isEmpty())
        convertToV030(settings);
    if ("0.5.0" > settings->value("General/Version").toString())
        convertToV050(settings);

    if (!isTwoWayFavorites(settings)) {
        convertToTwoWayFavorites(settings);
    }
}

void SettingsConverter::convertToV030(Settings *settings)
{
    // イエローページ
    for (int i = 0; i < 100; ++i) {
        QString key1 = QString("YellowPage/Item%1/").arg(i, 2, 10, QChar('0'));
        QString key2 = QString("YellowPage/Items/%1/").arg(i + 1);
        if (!settings->contains(key1))
            break;
        settings->setValue("YellowPage/Items/size", i + 1);
        settings->copy(key1, key2 + "Enabled");
        settings->copy(key1 + "Name", key2 + "Name");
        settings->copy(key1 + "Type", key2 + "Type");
        settings->copy(key1 + "Url", key2 + "Url");
        settings->copy(key1 + "NameSpaces", key2 + "NameSpaces");
        settings->copy(key1 + "UsePCRawProxy", key2 + "UsePCRawProxy");
        settings->remove(key1);
    }

    // お気に入り
    for (int i = 0; i < 100; ++i) {
        QString key1 = QString("Favorite/Item%1/").arg(i, 2, 10, QChar('0'));
        QString key2 = QString("Favorite/Items/%1/").arg(i + 1);
        if (!settings->contains(key1))
            break;
        settings->setValue("Favorite/Items/size", i + 1);
        settings->copy(key1, key2 + "Enabled");
        settings->copy(key1 + "Pattern", key2 + "Pattern");
        settings->copy(key1 + "MatchFlags", key2 + "MatchFlags");
        settings->copy(key1 + "TargetFlags", key2 + "TargetFlags");
        settings->copy(key1 + "Point", key2 + "Point");
        settings->remove(key1);
    }

    settings->setValue("General/Version", "0.3.0");
}

void SettingsConverter::convertToV050(Settings *settings)
{
    QString program = settings->value("Player/VideoPlayer").toString();
    QString args = settings->value("Player/VideoPlayerArgs").toString();
    QString types = settings->value("Player/VideoTypes").toString();
    settings->beginWriteArray("Player/Items");
    settings->setArrayIndex(0);
    settings->setValue("Program", program);
    settings->setValue("Args", args);
    settings->setValue("Types", types);
    settings->endArray();
    settings->remove("Player/VideoPlayer");
    settings->remove("Player/VideoPlayerArgs");
    settings->remove("Player/VideoTypes");

    program = settings->value("Player/SoundPlayer").toString();
    args = settings->value("Player/SoundPlayerArgs").toString();
    types = settings->value("Player/SoundTypes").toString();
    settings->beginWriteArray("Player/Items");
    settings->setArrayIndex(1);
    settings->setValue("Program", program);
    settings->setValue("Args", args);
    settings->setValue("Types", types);
    settings->endArray();
    settings->remove("Player/SoundPlayer");
    settings->remove("Player/SoundPlayerArgs");
    settings->remove("Player/SoundTypes");
}

static bool isTwoWayFavorites(Settings *settings)
{
    ChannelMatcher matcher(settings);

    QList<ChannelMatcher::Expression *> &list = matcher.rootGroup()->expressions;

    return list.size() == 2 && list[0]->isGroup && list[1]->isGroup;
}

static void collectNonGroupItems(QList<ChannelMatcher::Expression *>& items, ChannelMatcher::Expression* group)
{
    Q_ASSERT(group->isGroup);

    foreach (ChannelMatcher::Expression *exp, group->expressions) {
        if (exp->isGroup) {
            collectNonGroupItems(items, exp);
        } else {
            items.append(exp);
        }
    }
}

static ChannelMatcher::Expression* addTopLevelGroup(ChannelMatcher& matcher, QString name)
{
    ChannelMatcher::Expression *exp = new ChannelMatcher::Expression(matcher.rootGroup());
    exp->isGroup = true;
    exp->pattern = name;
    matcher.rootGroup()->expressions.append(exp);

    return exp;
}

static void convertToTwoWayFavorites(Settings *settings)
{
    ChannelMatcher src(settings);
    ChannelMatcher dst(settings);

    QList<ChannelMatcher::Expression *> items;
    collectNonGroupItems(items, src.rootGroup());

    dst.clear();
    ChannelMatcher::Expression *favGroup = addTopLevelGroup(dst, "お気に入り");
    ChannelMatcher::Expression *ngGroup  = addTopLevelGroup(dst, "NG");

    foreach (ChannelMatcher::Expression *exp, items) {
        if (exp->point >= 0) {
            favGroup->expressions.append(exp);
        } else {
            ngGroup->expressions.append(exp);
        }
    }
    dst.saveExpressions();
}
