#ifndef COLORDIALOGHELPER_H
#define COLORDIALOGHELPER_H

#include <QObject>
#include <QColor>
#include <qqml.h>

class ColorDialogHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    QML_ELEMENT
public:
    explicit ColorDialogHelper(QObject *parent = nullptr);

    QColor color() const;
    Q_INVOKABLE void selectColor();

public slots:
    void setColor(QColor color);

signals:
    void colorChanged(QColor color);

private:
    QColor m_color;
};

#endif // COLORDIALOGHELPER_H
