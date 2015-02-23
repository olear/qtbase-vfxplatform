/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com <http://qt.digia.com/>
**
** This file is part of the Qt Native Client platform plugin.
**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com <http://qt.digia.com/>
**
****************************************************************************/

#include "qpepperwindow.h"

#include "qpeppercompositor.h"
#include "qpepperglcontext.h"
#include "qpeppermodule.h"

#include <qdebug.h>
#include <qpa/qwindowsysteminterface.h>
#include <QtGui/QSurface>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(QT_PLATFORM_PEPPER_WINDOW, "qt.platform.pepper.window")

QPepperWindow::QPepperWindow(QWindow *window)
    : QPlatformWindow(window)
    , m_isVisible(false)
{
    qCDebug(QT_PLATFORM_PEPPER_WINDOW) << "Create QPepperWindow for" << window;

    m_pepperIntegration = QPepperIntegration::getPepperIntegration();

    m_compositor = 0;

    // Set window state (fullscreen windows resize here)
    setWindowState(window->windowState());

    if (m_compositor)
        m_compositor->addRasterWindow(this->window());
}

QPepperWindow::~QPepperWindow()
{
    if (m_compositor)
        m_compositor->removeWindow(this->window());
}

WId QPepperWindow::winId() const { return WId(this); }

void QPepperWindow::setVisible(bool visible)
{
    qCDebug(QT_PLATFORM_PEPPER_WINDOW) << "setVisible" << visible;

    QWindowSystemInterface::handleExposeEvent(window(), visible ? QRect(QPoint(), geometry().size())
                                                                : QRect());
    QPepperInstancePrivate::get()->scheduleWindowSystemEventsFlush();

    if (m_compositor)
        m_compositor->setVisible(this->window(), visible);
}

void QPepperWindow::setWindowState(Qt::WindowState state)
{
    qCDebug(QT_PLATFORM_PEPPER_WINDOW) << "setWindowState" << state;

    switch (state) {
    case Qt::WindowMaximized:
    case Qt::WindowFullScreen: {
        // Set full-scree window geometry
        QRect targetWindowGeometry = QRect(QPoint(), screen()->geometry().size());
        QPlatformWindow::setGeometry(targetWindowGeometry);
        QWindowSystemInterface::handleGeometryChange(window(), targetWindowGeometry);
        QWindowSystemInterface::handleExposeEvent(window(), targetWindowGeometry);
        QPepperInstancePrivate::get()->scheduleWindowSystemEventsFlush();
        break;
    }
    default:
        break;
    }
}

void QPepperWindow::raise()
{
    qCDebug(QT_PLATFORM_PEPPER_WINDOW) << "raise";

    if (m_compositor)
        m_compositor->raise(this->window());
}

void QPepperWindow::lower()
{
    qCDebug(QT_PLATFORM_PEPPER_WINDOW) << "lower";

    if (m_compositor)
        m_compositor->lower(this->window());
}

void QPepperWindow::setGeometry(const QRect &rect)
{
    qCDebug(QT_PLATFORM_PEPPER_WINDOW) << "setGeometry" << rect;

    QPlatformWindow::setGeometry(rect);
    QWindowSystemInterface::handleGeometryChange(window(), rect);
    QPepperInstancePrivate::get()->scheduleWindowSystemEventsFlush();
}

void QPepperWindow::setParent(const QPlatformWindow *parent)
{
    qCDebug(QT_PLATFORM_PEPPER_WINDOW) << "QPepperWindow::setParent" << parent;

    if (m_compositor)
        m_compositor->setParent(this->window(), parent->window());
}

bool QPepperWindow::setKeyboardGrabEnabled(bool grab)
{
    qCDebug(QT_PLATFORM_PEPPER_WINDOW) << "setKeyboardGrabEnabled" << grab;

    Q_UNUSED(grab);
    return false;
}

bool QPepperWindow::setMouseGrabEnabled(bool grab)
{
    qCDebug(QT_PLATFORM_PEPPER_WINDOW) << "setMouseGrabEnabled" << grab;

    Q_UNUSED(grab);
    return false;
}

qreal QPepperWindow::devicePixelRatio() const
{
    return QPepperInstancePrivate::get()->devicePixelRatio();
}

QT_END_NAMESPACE