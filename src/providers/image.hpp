#pragma once

#include <QQuickItem>
#include <QQmlEngine>
#include <QImage>


// no namespace, as it is registered as QML_ELEMENT
// we could use it in .qml just as ImageProvider


class ImageProvider : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

public:
    explicit ImageProvider(QQuickItem* parent = nullptr);

    QImage image() const;

public slots:
    void setImage(const QImage& img);

signals:
    void imageChanged();

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode,
                             UpdatePaintNodeData* data) override;

private:
    QImage m_image;
    bool m_dirty = false;
};
