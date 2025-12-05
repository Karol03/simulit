#pragma once

#include <QObject>


namespace adapters
{

class IAdapter : public QObject
{
    Q_OBJECT

public:
    IAdapter(QObject* parent)
        : QObject(parent)
    {}
    IAdapter(const IAdapter&) = delete;
    IAdapter& operator=(const IAdapter&) = delete;
    IAdapter(IAdapter&&) = delete;
    IAdapter& operator=(IAdapter&&) = delete;

    virtual QObject* raw() = 0;
    virtual const QObject* raw() const = 0;
};

using AdapterList = QList<QObject*>;

} // namespace adapters
