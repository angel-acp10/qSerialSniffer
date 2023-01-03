#ifndef SETTINGSFILE_H
#define SETTINGSFILE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>

class SettingsFile
{
public:
    // Constructor that takes the file path as an argument
    SettingsFile(const QString &filePath);

    // Load the configuration from the file
    bool load();

    // Save the configuration to the file
    bool save();

    // Get the value of a key in the JSON object
    QJsonValue value(const QString &key);

    // Set the value of a key in the JSON object
    void setValue(const QString &key, const QJsonValue &value);

private:
    QString m_filePath;
    QJsonObject m_json;
};

#endif // SETTINGSFILE_H
