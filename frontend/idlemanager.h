#ifndef IDLEMANAGER_H
#define IDLEMANAGER_H

#include <QObject>
#include <QTimer>

class IdleManager : public QObject
{
    Q_OBJECT
public:
    static IdleManager* instance();   
    void start(int timeoutMs);
    void stop();

signals:
    void idleTimeout();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    explicit IdleManager(QObject *parent = nullptr);
    static IdleManager* m_instance;

    QTimer timer;
    int m_timeoutMs;
};

#endif

