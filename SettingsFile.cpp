#include "SettingsFile.h"


SettingsFile::SettingsFile(const QString &filePath) : m_filePath(filePath) {}

bool SettingsFile::load()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QTextStream stream(&file);
    QString jsonString = stream.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    m_json = doc.object();
    return true;
}

bool SettingsFile::save()
{
    QJsonDocument doc(m_json);
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QTextStream stream(&file);
    stream << doc.toJson(QJsonDocument::Indented);
    file.close();
    return true;
}

QJsonValue SettingsFile::value(const QString &key)
{
    return m_json[key];
}

void SettingsFile::setValue(const QString &key, const QJsonValue &value)
{
    m_json[key] = value;
}
