#include "idlemanager.h"
#include <QApplication>
#include <QEvent>
#include <QDebug>

IdleManager* IdleManager::m_instance = nullptr;

IdleManager* IdleManager::instance()
{
    if (!m_instance)
        m_instance = new IdleManager(qApp);
    return m_instance;
}

IdleManager::IdleManager(QObject *parent)
    : QObject(parent)
{
    connect(&timer, &QTimer::timeout, this, &IdleManager::onTimerTimeout);
}

void IdleManager::start(int timeoutMs)
{
    m_timeoutMs = timeoutMs;
    timer.start(m_timeoutMs);
    qApp->installEventFilter(this);
}

void IdleManager::stop()
{
    timer.stop();
    qApp->removeEventFilter(this);
}

void IdleManager::onTimerTimeout()
{
    qDebug() << "IDLE TIMEOUT TRIGGERED";
    emit idleTimeout();
}

bool IdleManager::eventFilter(QObject *, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::KeyPress:
        timer.start(m_timeoutMs);
        break;
    default:
        break;
    }
    return false;
}


