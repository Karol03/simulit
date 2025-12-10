#pragma once

#include <QObject>
#include <QVariant>
#include <format>

#include "utils.hpp"


namespace api::common
{

class INamedVariable : public QObject
{
    Q_OBJECT

    friend class VariableMap;

public:
    using QObject::QObject;

    virtual const QString& name() const = 0;
    virtual QString fullName() const = 0;
    virtual const QString& description() const = 0;
    virtual QMetaType type() const = 0;
    virtual bool set(const QVariant& value) = 0;
    virtual void reset() = 0;
    virtual QVariant get() const = 0;
    virtual void* dataPointer() const = 0;
};


class IHierarchicalNamedVariable : public INamedVariable
{
    Q_OBJECT

    using Filter = std::function<bool(const IHierarchicalNamedVariable&)>;

public:
    using INamedVariable::INamedVariable;

    virtual const QString& name() const override = 0;
    virtual const QString& description() const override = 0;
    virtual QMetaType type() const override = 0;
    virtual bool set(const QVariant& value) override = 0;
    virtual void reset() override = 0;
    virtual QVariant get() const override = 0;
    virtual void* dataPointer() const override = 0;
    int id() { return m_id; }

    // because of QObject::children, cannot name the method children()
    virtual QList<IHierarchicalNamedVariable*> inner() = 0;

    QString fullName() const override
    {
        if (this->parent())
        {
            if (auto variableParent = dynamic_cast<INamedVariable*>(this->parent()))
                return QString("%1:%2").arg(variableParent->fullName(), name());
        }
        return name();
    }

    template <typename Container, typename Filter>
    void preorderTraversalSquash(Container& result, Filter&& filter)
    {
        if (filter(*this))
            result.push_back(this);
        for (const auto& child : this->inner())
        {
            child->preorderTraversalSquash(result, std::forward<Filter>(filter));
        }
    }

protected:
    void assign(int& newId)
    {
        m_id = newId++;
        for (const auto& child : this->inner())
        {
            child->assign(newId);
        }
    }

private:
    int m_id = 0;
};


class IVariable : public common::IHierarchicalNamedVariable
{
    Q_OBJECT

public:
    IVariable(QString name,
              QString description,
              QObject* parent = nullptr)
        : common::IHierarchicalNamedVariable(parent)
        , m_name{std::move(name)}
        , m_description{std::move(description)}
    {
        if (name.contains(':'))
            throw std::logic_error{QString("Invalid variable name '%1', variable must not contain ':'").arg(name).toStdString()};
    }

    const QString& name() const override { return m_name; }
    const QString& description() const override { return m_description; }

    virtual QMetaType type() const override = 0;
    virtual bool set(const QVariant& value) override = 0;
    virtual void reset() override = 0;
    virtual QVariant get() const override = 0;
    virtual void* dataPointer() const override = 0;
    virtual QList<IHierarchicalNamedVariable*> inner() override = 0;

private:
    const QString m_name;
    const QString m_description;
};


template <typename T>
class Variable : public IVariable
{
public:
    Variable(QString name,
             QString description,
             T defaultValue = T{},
             QObject* parent = nullptr)
        : IVariable(std::move(name), std::move(description), parent)
        , m_data{defaultValue}
        , m_defaultValue{defaultValue}
    {}

    QMetaType type() const override
    {
        return QMetaType::fromType<T>();
    }

    bool set(const QVariant& value) override
    {
        if (utils::convert<T>(value))
        {
            m_data = value.value<T>();
            return true;
        }
        return false;
    }

    void reset() override
    {
        m_data = m_defaultValue;
    }

    QVariant get() const override
    {
        return QVariant{m_data};
    }

    void* dataPointer() const override
    {
        return (void*)&m_data;
    }

    QList<IHierarchicalNamedVariable*> inner() override { return {}; }

protected:
    T m_data;

private:
    T m_defaultValue;
};


template <typename T>
class VariableFiltered : public Variable<T>
{
public:
    using Filter = std::function<bool(const T&)>;

public:
    VariableFiltered(QString name,
                     QString description,
                     T defaultValue = T{},
                     Filter filter = [](const T&) { return true; },
                     QObject* parent = nullptr)
        : Variable<T>(name, description, defaultValue, parent)
        , m_filter{filter}
    {}

    bool set(const QVariant& value) override
    {
        if (utils::convert<T>(value))
        {
            auto newValue = value.value<T>();
            if (m_filter(newValue))
            {
                this->m_data = newValue;
                return true;
            }
        }
        return false;
    }

private:
    Filter m_filter;
};


class VariableGroup : public IVariable
{
    Q_OBJECT

public:
    template <class... Properties>
    VariableGroup(QString name, QObject* parent, Properties&&... properties)
        : IVariable(std::move(name), "", parent)
    {
        m_properties.reserve(sizeof...(Properties));
        (m_properties.emplace_back(
             utils::withParent(std::forward<Properties>(properties), this)
             ), ...);
        if (!parent)
        {
            QSet<QString> uniquenessFullnameCheck;
            int id = 0;
            assign(id);

        }
    }

    QMetaType type() const override { return QMetaType(QMetaType::Nullptr); }
    bool set(const QVariant& value) final { return false; }
    QVariant get() const final { return QVariant{}; }
    void reset() final
    {
        for (auto& child : inner())
        {
            child->reset();
        }
    }
    void* dataPointer() const final { return nullptr; }
    QList<IHierarchicalNamedVariable*> inner() override { return m_properties; }

private:
    QList<IHierarchicalNamedVariable*> m_properties;
};


class VariableMap
{
public:
    class Snapshot
    {
        friend class VariableMap;

        explicit Snapshot(QVariantList data)
        {
            std::swap(m_datas, data);
        }

    public:
        Snapshot(const Snapshot&) = default;
        Snapshot& operator=(const Snapshot&) = default;
        Snapshot(Snapshot&&) noexcept = default;
        Snapshot& operator=(Snapshot&&) noexcept = default;

    private:
        QVariantList m_datas;
    };

    class WatchList
    {
        friend class VariableMap;

        WatchList(const std::vector<IHierarchicalNamedVariable*>& variables)
        {
            m_datas.reserve(variables.size());
            m_fullNames.reserve(variables.size());
            for (int i = 0; i < variables.size(); ++i)
            {
                m_datas.append(variables[i]->get());
                m_fullNames.append(variables[i]->fullName());
            }
        }

    public:
        WatchList(const WatchList& watchlist)
            : m_datas{watchlist.m_datas}
            , m_fullNames{watchlist.m_fullNames}
        {}
        WatchList& operator=(const WatchList& watchlist)
        {
            m_datas = watchlist.m_datas;
            m_fullNames = watchlist.m_fullNames;
            return *this;
        }
        WatchList(WatchList&& watchlist)
        {
            std::swap(m_datas, watchlist.m_datas);
            std::swap(m_fullNames, watchlist.m_fullNames);
        }
        WatchList& operator=(WatchList&& watchlist)
        {
            std::swap(m_datas, watchlist.m_datas);
            std::swap(m_fullNames, watchlist.m_fullNames);
            return *this;
        }
        void update(Snapshot snapshot)
        {
            std::swap(snapshot.m_datas, m_datas);
        }
        void update(const QVariantMap& map)
        {
            for (const auto& [name, value] : map.asKeyValueRange())
            {
                if (auto pos = find(name); pos != -1)
                {
                    m_datas[pos] = value;
                }
            }
        }
        std::size_t size() const
        {
            return m_datas.size();
        }
        bool contains(const QString& name) const
        {
            return find(name) != -1;
        }
        bool contains(const int& id) const
        {
            return (id < m_datas.size());
        }
        template <typename T>
        T get(const QString& name) const
        {
            return operator[](name).value<T>();
        }
        template <typename T>
        T get(const int& id) const
        {
            return operator[](id).value<T>();
        }
        QVariant operator[](const QString& name) const
        {
            if (auto pos = find(name); pos != -1)
                return m_datas[pos];
            throw std::runtime_error{std::format("Variable (name: {}) not on the watch list.\n{}", name.toStdString(), validate(name).toStdString())};
        }
        QVariant operator[](const int& id) const
        {
            if (id < m_datas.size())
                return m_datas[id];
            throw std::runtime_error{std::format("Variable (id: {}) not on the watch list, there is {} elements on list", id, m_datas.size())};
        }
    private:
        int find(const QString& name) const
        {
            auto lastSubname = ':' + name;
            for (int i = 0; i < m_fullNames.size(); ++i)
            {
                if (m_fullNames[i] == name || m_fullNames[i].endsWith(lastSubname))
                {
                    return i;
                }
            }
            return -1;
        }
        QString validate(const QString& name) const
        {
            if (name == ":")
                return QString("Invalid name ':'");
            if (!name.isEmpty() && name.back() == ':')
                return QString("Fullname of variable can't ends on ':");
            for (std::size_t i = 0; i + 1 < name.size(); ++i)
                if (name[i] == ':' && name[i+1] == ':')
                    return QString("Check if you have intentionally used double colons “:”");
            return QString("No variable with the specified name was found");
        }
    private:
        QVariantList m_datas;
        QStringList m_fullNames;
    };

public:
    VariableMap() {}
    VariableMap(common::IHierarchicalNamedVariable* root) { reinitialize(root); }
    VariableMap(const VariableMap&) = delete;
    VariableMap& operator=(const VariableMap&) = delete;
    VariableMap(VariableMap&& vm)
    {
        std::swap(m_variables, vm.m_variables);
        std::swap(m_mapByNames, vm.m_mapByNames);
        vm.m_variables.clear();
        vm.m_mapByNames.clear();
    }
    VariableMap& operator=(VariableMap&& vm)
    {
        std::swap(m_variables, vm.m_variables);
        std::swap(m_mapByNames, vm.m_mapByNames);
        vm.m_variables.clear();
        vm.m_mapByNames.clear();
        return *this;
    }

    void reinitialize(common::IHierarchicalNamedVariable* root)
    {
        if (!root)
        {
            Q_ASSERT_X(false, "VariableMap::reinitialize", "Cannot initialize VariableMap with no root");
            return;
        }
        const auto takeAll = [](const auto&) { return true; };
        m_variables.clear();
        m_mapByNames.clear();
        root->preorderTraversalSquash(m_variables, takeAll);
        fillTheMap();
    }

    template <typename T>
    T& ref(const QString& name)
    {
        auto* variable = named(name);
        if (!variable)
            throw std::runtime_error{std::format("Object (name: {}) from VariableMap cannot be dereferenced", name.toStdString())};
        auto expectedType = QMetaType::fromType<T>();
        if (variable->type() != expectedType)
            throw std::runtime_error{std::format("Object '(name: {}, type: {})' does not match expected type {} in VariableMap", name.toStdString(), variable->type().name(), expectedType.name())};
        return *reinterpret_cast<T*>(variable->dataPointer());
    }

    void reset()
    {
        for (auto& var : m_variables)
        {
            var->reset();
        }
    }

    std::size_t size() const
    {
        return m_variables.size();
    }

    IHierarchicalNamedVariable* named(const QString& name)
    {
        auto it = m_mapByNames.find(name);
        if (it != m_mapByNames.end())
            return it.value();
        auto lastSubname = ':' + name;
        for (const auto& [varName, var] : m_mapByNames.asKeyValueRange())
        {
            if (varName.endsWith(lastSubname))
            {
                return var;
            }
        }
        throw std::runtime_error{std::format("Object (name: {}) not exists in VariableMap", name.toStdString())};
    }

    IHierarchicalNamedVariable* number(const int& id)
    {
        // id is assigned after variables tree squash, so position == id
        if (id < m_variables.size())
            return m_variables[id];
        throw std::runtime_error{std::format("Object (id: {}) not exists in VariableMap", id)};
    }

    inline operator Snapshot() const
    {
        return snapshot();
    }

    inline Snapshot snapshot() const
    {
        auto result = QVariantList{};
        result.reserve(m_variables.size());
        for (const auto& variable : m_variables)
        {
            result.push_back(variable->get());
        }
        return Snapshot{std::move(result)};
    }

    inline WatchList watch() const
    {
        return WatchList{m_variables};
    }

private:
    void fillTheMap()
    {
        for (int i = 0; i < m_variables.size(); ++i)
        {
            const auto& var = m_variables[i];
            m_mapByNames[var->fullName()] = var;
            Q_ASSERT_X(i == var->id(), "api::VariableMap::fillTheMap()",
                       "Variable position after tree squash MUST correspond to variable id()");
        }
    }

private:
    std::vector<IHierarchicalNamedVariable*> m_variables;
    QMap<QString, IHierarchicalNamedVariable*> m_mapByNames;
};

}  // namespace api::common


namespace api
{

template <class T>
concept VariableType =
    (std::is_pointer_v<std::remove_cvref_t<T>> &&
     std::derived_from<std::remove_pointer_t<std::remove_cvref_t<T>>, ::api::common::IVariable>);

template <class... T>
concept VariableTypes = (VariableType<T> && ...);

template <typename T>
inline auto var(QString name, QString description, T defaultValue = T{}, QObject* parent = nullptr)
{
    using DecayedT = std::decay_t<T>;
    if constexpr (std::is_same_v<DecayedT, const char*> ||
                  std::is_same_v<DecayedT, char*> ||
                  std::is_same_v<DecayedT, std::string> ||
                  std::is_same_v<DecayedT, QString>)
        return new ::api::common::Variable<QString>(std::move(name), std::move(description), std::move(defaultValue), parent);
    else
        return new ::api::common::Variable<T>(std::move(name), std::move(description), std::move(defaultValue), parent);
}

template <typename T, typename Filter>
inline auto var(QString name,
                QString description,
                T defaultValue,
                Filter&& filter,
                QObject* parent = nullptr)
{
    using DecayedT = std::decay_t<T>;
    if constexpr (std::is_same_v<DecayedT, const char*> ||
                  std::is_same_v<DecayedT, char*> ||
                  std::is_same_v<DecayedT, std::string> ||
                  std::is_same_v<DecayedT, QString>)
        return new ::api::common::VariableFiltered<QString>(std::move(name),
                                                            std::move(description),
                                                            std::move(defaultValue),
                                                            std::forward<Filter>(filter),
                                                            parent);
    else
        return new ::api::common::VariableFiltered<T>(std::move(name),
                                                      std::move(description),
                                                      std::move(defaultValue),
                                                      std::forward<Filter>(filter),
                                                      parent);
}

template <VariableTypes... Properties>
inline auto var(QObject* parent, Properties&&... childProperties)
{
    auto result = new ::api::common::VariableGroup("", nullptr, std::forward<Properties>(childProperties)...);
    result->setParent(parent);
    return result;
}

template <VariableTypes... Properties>
inline auto var(QString groupName, Properties&&... childProperties)
{
    return new ::api::common::VariableGroup(std::move(groupName), nullptr, std::forward<Properties>(childProperties)...);
}

template <VariableTypes... Properties>
inline auto var(QString groupName, QObject* parent, Properties&&... childProperties)
{
    auto result = new ::api::common::VariableGroup(std::move(groupName), nullptr, std::forward<Properties>(childProperties)...);
    result->setParent(parent);
    return result;
}


using IVariable = ::api::common::IVariable;
using Variables = IVariable*;
template <typename T> using Variable = ::api::common::Variable<T>;
using VariableGroup = ::api::common::VariableGroup;
using VariableMap = ::api::common::VariableMap;
using VariableWatchList = ::api::common::VariableMap::WatchList;
using VariableMapSnapshot = ::api::common::VariableMap::Snapshot;

}  // namespace api
