#pragma once

#include <QApplication>
#include <QMouseEvent>
#include <QWidget>
#include <QEvent>

class DynamicMouseWidget : public QWidget
{
    Q_OBJECT

public:
    DynamicMouseWidget(QWidget* parent = nullptr);
    virtual ~DynamicMouseWidget();

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
};

