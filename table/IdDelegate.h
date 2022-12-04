#ifndef IDDELEGATE_H
#define IDDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>
#include <QColor>
#include <QPainter>

class IdDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit IdDelegate(QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QString getId2() const;
    void setId2(const QString id2);
    QString getId3() const;
    void setId3(const QString id3);

    QColor getColor2() const;
    void setColor2(const QColor color2);
    QColor getColor3() const;
    void setColor3(const QColor color3);
    

private:
    QString m_id2;
    QString m_id3;

    QColor m_color2;
    QColor m_color3;
};

#endif // IDDELEGATE_H
