#include "colordialoghelper.h"
#include <QColorDialog>

ColorDialogHelper::ColorDialogHelper(QObject *parent)
    : QObject(parent)
{
    m_color = QColor(41, 41, 41);
}

QColor ColorDialogHelper::color() const
{
    return m_color;
}

void ColorDialogHelper::selectColor()
{
    const QColor color = QColorDialog::getColor();
    m_color = color;
    emit colorChanged(m_color);
}

void ColorDialogHelper::setColor(QColor color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(m_color);
}
