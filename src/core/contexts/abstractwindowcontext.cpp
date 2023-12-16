#include "abstractwindowcontext_p.h"

#include <QtGui/QPen>
#include <QtGui/QPainter>

#include "qwkglobal_p.h"

namespace QWK {

    AbstractWindowContext::AbstractWindowContext() = default;

    AbstractWindowContext::~AbstractWindowContext() = default;

    bool AbstractWindowContext::setup(QObject *host, WindowItemDelegate *delegate) {
        if (!host || !delegate) {
            return false;
        }

        auto windowHandle = delegate->hostWindow(host);
        if (!windowHandle) {
            return false;
        }

        m_host = host;
        m_delegate.reset(delegate);
        m_windowHandle = windowHandle;

        if (!setupHost()) {
            m_host = nullptr;
            m_delegate.reset();
            m_windowHandle = nullptr;
            return false;
        }
        return true;
    }

    bool AbstractWindowContext::setHitTestVisible(const QObject *obj, bool visible) {
        Q_ASSERT(obj);
        if (!obj) {
            return false;
        }

        if (visible) {
            m_hitTestVisibleItems.insert(obj);
        } else {
            m_hitTestVisibleItems.remove(obj);
        }
        return true;
    }

    bool AbstractWindowContext::setSystemButton(WindowAgentBase::SystemButton button,
                                                QObject *obj) {
        Q_ASSERT(obj);
        Q_ASSERT(button != WindowAgentBase::Unknown);
        if (!obj || (button == WindowAgentBase::Unknown)) {
            return false;
        }

        if (m_systemButtons[button] == obj) {
            return false;
        }
        m_systemButtons[button] = obj;
        return true;
    }

    bool AbstractWindowContext::setTitleBar(QObject *item) {
        Q_ASSERT(item);
        if (!item) {
            return false;
        }

        if (m_titleBar == item) {
            return false;
        }
        m_titleBar = item;
        return true;
    }

    bool AbstractWindowContext::isInSystemButtons(const QPoint &pos,
                                                  WindowAgentBase::SystemButton *button) const {
        *button = WindowAgentBase::Unknown;
        for (int i = WindowAgentBase::WindowIcon; i <= WindowAgentBase::Close; ++i) {
            auto currentButton = m_systemButtons[i];
            if (!currentButton || !m_delegate->isVisible(currentButton) ||
                !m_delegate->isEnabled(currentButton)) {
                continue;
            }
            if (m_delegate->mapGeometryToScene(currentButton).contains(pos)) {
                *button = static_cast<WindowAgentBase::SystemButton>(i);
                return true;
            }
        }
        return false;
    }

    bool AbstractWindowContext::isInTitleBarDraggableArea(const QPoint &pos) const {
        if (!m_titleBar) {
            // There's no title bar at all, the mouse will always be in the client area.
            return false;
        }
        if (!m_delegate->isVisible(m_titleBar) || !m_delegate->isEnabled(m_titleBar)) {
            // The title bar is hidden or disabled for some reason, treat it as there's
            // no title bar.
            return false;
        }
        QRect windowRect = {QPoint(0, 0), m_windowHandle->size()};
        QRect titleBarRect = m_delegate->mapGeometryToScene(m_titleBar);
        if (!titleBarRect.intersects(windowRect)) {
            // The title bar is totally outside the window for some reason,
            // also treat it as there's no title bar.
            return false;
        }

        if (!titleBarRect.contains(pos)) {
            return false;
        }

        for (int i = WindowAgentBase::WindowIcon; i <= WindowAgentBase::Close; ++i) {
            auto currentButton = m_systemButtons[i];
            if (currentButton && m_delegate->isVisible(currentButton) &&
                m_delegate->isEnabled(currentButton) &&
                m_delegate->mapGeometryToScene(currentButton).contains(pos)) {
                return false;
            }
        }

        for (auto widget : m_hitTestVisibleItems) {
            if (widget && m_delegate->isVisible(widget) && m_delegate->isEnabled(widget) &&
                m_delegate->mapGeometryToScene(widget).contains(pos)) {
                return false;
            }
        }

        return true;
    }

    QString AbstractWindowContext::key() const {
        return {};
    }

    QWK_USED static constexpr const struct {
        const quint32 activeLight = MAKE_RGBA_COLOR(210, 233, 189, 226);
        const quint32 activeDark = MAKE_RGBA_COLOR(177, 205, 190, 240);
        const quint32 inactiveLight = MAKE_RGBA_COLOR(193, 195, 211, 203);
        const quint32 inactiveDark = MAKE_RGBA_COLOR(240, 240, 250, 255);
    } kSampleColorSet;

    void AbstractWindowContext::virtual_hook(int id, void *data) {
        switch (id) {
            case CentralizeHook: {
                // TODO: Qt
                break;
            }

            case ShowSystemMenuHook: {
                // TODO: Qt
                break;
            }

            case DefaultColorsHook: {
                auto &map = *static_cast<QMap<QString, QColor> *>(data);
                map.clear();
                map.insert(QStringLiteral("activeLight"), kSampleColorSet.activeLight);
                map.insert(QStringLiteral("activeDark"), kSampleColorSet.activeDark);
                map.insert(QStringLiteral("inactiveLight"), kSampleColorSet.inactiveLight);
                map.insert(QStringLiteral("inactiveDark"), kSampleColorSet.inactiveDark);
                return;
            }

            default:
                break;
        }
    }

    void AbstractWindowContext::showSystemMenu(const QPoint &pos) {
        virtual_hook(ShowSystemMenuHook, &const_cast<QPoint &>(pos));
    }

}