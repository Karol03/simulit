#pragma once

#include <QObject>
#include <QUrl>


namespace controllers
{

class IController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl uiSource READ uiSource CONSTANT)

public:
    using QObject::QObject;

    IController(const IController&) = delete;
    IController& operator=(const IController&) = delete;
    IController(IController&&) = delete;
    IController& operator=(IController&&) = delete;

    virtual QUrl uiSource() const = 0;
};

}  // namespace controllers
