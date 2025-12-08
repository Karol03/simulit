#pragma once

#include <QObject>


namespace api
{

class NumberGenerator : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief operator ()
     * @return random integer number
     */
    virtual int operator()() = 0;

    /**
     * @brief operator ()
     * @param to upper bound of random value
     * @return random integer in range <0, to)
     */
    virtual int operator()(int to) = 0;

    /**
     * @brief operator ()
     * @param from lower bound of random value
     * @param to upper bound of random value
     * @return random integer in range <from, to)
     */
    virtual int operator()(int from, int to) = 0;
};

}  // namespace api
