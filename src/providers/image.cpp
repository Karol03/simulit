#include "image.hpp"

#include <QSGSimpleTextureNode>
#include <QQuickWindow>


ImageProvider::ImageProvider(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

QImage ImageProvider::image() const
{
    return m_image;
}

void ImageProvider::setImage(const QImage& img)
{
    m_image = img;
    m_dirty = true;
    emit imageChanged();
    update();
}

QSGNode* ImageProvider::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
    QSGSimpleTextureNode* node = static_cast<QSGSimpleTextureNode*>(oldNode);
    if (m_image.isNull())
    {
        delete node;
        return nullptr;
    }

    if (!node)
    {
        node = new QSGSimpleTextureNode();
    }

    if (m_dirty && window())
    {
        m_dirty = false;
        QSGTexture* texture = window()->createTextureFromImage(m_image);
        node->setTexture(texture);
        node->setOwnsTexture(true);
    }
    node->setRect(boundingRect());
    return node;
}
