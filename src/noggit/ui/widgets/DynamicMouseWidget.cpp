#include "DynamicMouseWidget.h"

DynamicMouseWidget::DynamicMouseWidget(QWidget* parent)
	: QWidget(parent)
{ }

DynamicMouseWidget::~DynamicMouseWidget()
{ }

void DynamicMouseWidget::enterEvent(QEnterEvent* event)
{
	QApplication::setOverrideCursor(Qt::PointingHandCursor);
}

void DynamicMouseWidget::leaveEvent(QEvent* event)
{
	QApplication::restoreOverrideCursor();
}

void DynamicMouseWidget::mousePressEvent(QMouseEvent* event)
{
	QApplication::setOverrideCursor(Qt::CursorShape::ClosedHandCursor);
}

void DynamicMouseWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QApplication::restoreOverrideCursor();
}