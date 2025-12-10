#pragma once

#include <QObject>
#include <QUrl>

#include "api/variable.hpp"
#include "providers/iprovider.hpp"


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
    virtual api::Variables properties() = 0;
    virtual providers::IProvider* statistics() = 0;
};

}  // namespace controllers
