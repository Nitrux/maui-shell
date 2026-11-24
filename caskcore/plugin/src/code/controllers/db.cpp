/*
 *   Copyright 2018 Camilo Higuita <milo.h@aol.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "db.h"

#include <QDebug>
#include <QDir>
#include <QList>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QUuid>

DB::DB(QObject *parent)
    : QObject(parent)
{
    QDir collectionDBPath_dir(TAG::TaggingPath);
    if (!collectionDBPath_dir.exists())
        collectionDBPath_dir.mkpath(QStringLiteral("."));

    this->name = QUuid::createUuid().toString();
    if (!FMH::fileExists(QUrl::fromLocalFile(TAG::TaggingPath + TAG::DBName))) {
        this->openDB(this->name);
        qDebug() << "Collection doesn't exists, trying to create it" << TAG::TaggingPath + TAG::DBName;
        this->prepareCollectionDB();
    } else
        this->openDB(this->name);
}

DB::~DB()
{
    qDebug() << "CLOSING THE TAGGING DATA BASE";
    this->m_db.close();
}

void DB::openDB(const QString &name)
{
    if (!QSqlDatabase::contains(name)) {
        this->m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), name);
        this->m_db.setDatabaseName(TAG::TaggingPath + TAG::DBName);
    }

    if (!this->m_db.isOpen()) {
        if (!this->m_db.open())
            qDebug() << "ERROR OPENING DB" << this->m_db.lastError().text() << m_db.connectionName();
    }
    auto query = this->getQuery(QStringLiteral("PRAGMA synchronous=OFF"));
    query.exec();
}

void DB::prepareCollectionDB() const
{
    QSqlQuery query(this->m_db);

    QFile file(QStringLiteral(":/db/code/controllers/script.sql"));

    if (!file.exists()) {
        QString log = QStringLiteral("Fatal error on build database. The file '");
        log.append(file.fileName() + QStringLiteral("' for database and tables creation query cannot be not found!"));
        qDebug() << log;
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << QStringLiteral("Fatal error on try to create database! The file with sql queries for database creation cannot be opened!");
        return;
    }

    bool hasText;
    QString line;
    QByteArray readLine;
    QString cleanedLine;
    QStringList strings;

    while (!file.atEnd()) {
        hasText = false;
        line = QStringLiteral("");
        readLine = QByteArrayLiteral("");
        cleanedLine = QStringLiteral("");
        strings.clear();
        while (!hasText) {
            readLine = file.readLine();
            cleanedLine = QString::fromStdString(readLine.trimmed().toStdString());
            strings = cleanedLine.split(QStringLiteral("--"));
            cleanedLine = strings.at(0);
            if (!cleanedLine.startsWith(QStringLiteral("--")) && !cleanedLine.startsWith(QStringLiteral("DROP")) && !cleanedLine.isEmpty())
                line += cleanedLine;
            if (cleanedLine.endsWith(QStringLiteral(";")))
                break;
            if (cleanedLine.startsWith(QStringLiteral("COMMIT")))
                hasText = true;
        }
        if (!line.isEmpty()) {
            if (!query.exec(line)) {
                qDebug() << "exec failed" << query.lastQuery() << query.lastError();
            }

        } else
            qDebug() << "exec wrong" << query.lastError();
    }
    file.close();
}

bool DB::checkExistance(const QString &tableName, const QString &searchId, const QString &search) const
{
    QSqlQuery query(this->m_db);
    query.prepare(QStringLiteral("SELECT %1 FROM %2 WHERE %3 = ?").arg(searchId, tableName, searchId));
    query.addBindValue(search);

    if (query.exec() && query.next())
        return true;

    if (query.lastError().isValid())
        qDebug() << query.lastError().text();

    return false;
}

bool DB::checkExistance(const QString &queryStr) const
{
    qDebug() << "CHECKIGN QUERY TAG" << queryStr;
    auto query = this->getQuery(queryStr);

    if (query.exec()) {
        if (query.next())
            return true;
    } else
        qDebug() << query.lastError().text();

    return false;
}

QSqlQuery DB::getQuery(const QString &queryTxt) const
{
    QSqlQuery query(queryTxt, this->m_db);
    return query;
}

QSqlQuery DB::getQuery() const
{
    QSqlQuery query(this->m_db);
    return query;
}

bool DB::insert(const QString &tableName, const QVariantMap &insertData) const
{
    if (tableName.isEmpty()) {
        qDebug() << QStringLiteral("Fatal error on insert! The table name is empty!");
        return false;

    } else if (insertData.isEmpty()) {
        qDebug() << QStringLiteral("Fatal error on insert! The insertData is empty!");
        return false;
    }

    QStringList strValues;
    QStringList fields = insertData.keys();
    QVariantList values = insertData.values();
    int totalFields = fields.size();
    for (int i = 0; i < totalFields; ++i)
        strValues.append(QStringLiteral("?"));

    QString sqlQueryString = QStringLiteral("INSERT INTO ") + tableName + QStringLiteral(" (") + QString(fields.join(QStringLiteral(","))) + QStringLiteral(") VALUES(") + QString(strValues.join(QStringLiteral(","))) + QStringLiteral(")");
    QSqlQuery query(this->m_db);
    query.prepare(sqlQueryString);

    int k = 0;
    for (const QVariant &value : values)
        query.bindValue(k++, value);

    return query.exec();
}

bool DB::update(const QString &tableName, const FMH::MODEL &updateData, const QVariantMap &where) const
{
    if (tableName.isEmpty()) {
        qDebug() << QStringLiteral("Fatal error on insert! The table name is empty!");
        return false;
    } else if (updateData.isEmpty()) {
        qDebug() << QStringLiteral("Fatal error on insert! The insertData is empty!");
        return false;
    }

    QStringList set;
    QVariantList values;
    const auto updateKeys = updateData.keys();
    for (const auto &key : updateKeys)
    {
        set.append(FMH::MODEL_NAME[key] + QStringLiteral(" = ?"));
        values << updateData[key];
    }

    QStringList condition;
    const auto whereKeys = where.keys();
    for (const auto &key : whereKeys)
    {
         condition.append(key + QStringLiteral(" = ?"));
         values << where[key];
    }

    const QString sql = QStringLiteral("UPDATE ") + tableName
            + QStringLiteral(" SET ") + set.join(QStringLiteral(","))
            + QStringLiteral(" WHERE ") + condition.join(QStringLiteral(","));

    QSqlQuery query(this->m_db);
    query.prepare(sql);

    int i = 0;
    for (const auto &value : values)
        query.bindValue(i++, value);

    return query.exec();
}

bool DB::update(const QString &table, const QString &column, const QVariant &newValue, const QVariant &op, const QString &id) const
{
    QSqlQuery query(this->m_db);
    query.prepare(QStringLiteral("UPDATE %1 SET %2 = ? WHERE %3 = ?").arg(table, column, op.toString()));
    query.addBindValue(newValue);
    query.addBindValue(id);
    return query.exec();
}

bool DB::remove(const QString &tableName, const FMH::MODEL &removeData) const
{
    if (tableName.isEmpty()) {
        qDebug() << QStringLiteral("Fatal error on removing! The table name is empty!");
        return false;

    } else if (removeData.isEmpty()) {
        qDebug() << QStringLiteral("Fatal error on insert! The removeData is empty!");
        return false;
    }

    QStringList conditions;
    QVariantList values;
    const auto keys = removeData.keys();
    for (const auto &key : keys) {
        conditions.append(FMH::MODEL_NAME[key] + QStringLiteral(" = ?"));
        values << removeData[key];
    }

    QSqlQuery query(this->m_db);
    query.prepare(QStringLiteral("DELETE FROM ") + tableName + QStringLiteral(" WHERE ") + QString(conditions.join(QStringLiteral(" AND "))));

    int i = 0;
    for (const auto &value : values)
        query.bindValue(i++, value);

    return query.exec();
}

const QSqlDatabase & DB::db() const
{
    return this->m_db;
}
