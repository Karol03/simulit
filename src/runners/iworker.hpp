#pragma once

#include <QObject>


namespace runners
{

class IWorker : public QObject
{
    Q_OBJECT

public:
    IWorker(QObject* parent)
        : QObject(parent)
    {}
    IWorker(const IWorker&) = delete;
    IWorker& operator=(const IWorker&) = delete;
    IWorker(IWorker&&) = delete;
    IWorker& operator=(IWorker&&) = delete;

public:
    virtual void start() = 0;
};

}  // namespace runners
