#pragma once

#include <QVariant>


namespace api::utils
{

template <typename T> inline bool convert(const QVariant& v);
template <> inline bool convert<int>(const QVariant& v)
{ bool ok = false; v.toInt(&ok); return ok; }
template <> inline bool convert<double>(const QVariant& v)
{ bool ok = false; v.toDouble(&ok); return ok; }
template <> inline bool convert<bool>(const QVariant& v)
{ return v.isValid(); }
template <> inline bool convert<QString>(const QVariant& v)
{ return v.isValid(); }

template <typename P>
static P&& withParent(P&& p, QObject* parent)
{
    if (p) p->setParent(parent);
    return std::forward<P>(p);
}

}  // namespace api::utils
