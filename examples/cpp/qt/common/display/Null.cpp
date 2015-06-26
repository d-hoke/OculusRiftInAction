#include "Window.h"
#include <QWindow>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QCoreApplication>
#include <QTimer>

using namespace Plugins::Display;


class NullDisplayPlugin : public Plugins::Display::Plugin {
public:
    NullDisplayPlugin() {}
    virtual bool supported() const override { return true; }
    virtual const char* name() const override { return "Null"; }
    virtual bool init() override { return true; }
    virtual void destroy() override { }
    virtual bool start() override { return true; }
    virtual void stop() override { }
 
    virtual void setShareContext(QOpenGLContext* context) override { }
    virtual QOpenGLContext* context() override { return nullptr; }
    virtual Plugins::Display::Type type() const override { return Type::_2D; }

    virtual glm::uvec2 preferredSurfaceSize() const override;
    virtual glm::uvec2 preferredUiSize() const override;

    virtual void preRender() override {}
    virtual void render(
        uint32_t sceneTexture, const glm::uvec2& textureSize,
        uint32_t uiTexture, const glm::uvec2& uiSize, const glm::mat4& uiView) override {
    }
    virtual void postRender() override { }
};

glm::uvec2 NullDisplayPlugin::preferredSurfaceSize() const {
    return glm::uvec2(100, 100);
}

glm::uvec2 NullDisplayPlugin::preferredUiSize() const {
    return preferredSurfaceSize();
}

Plugins::Display::Plugin* buildNullPlugin() {
    return new NullDisplayPlugin();
}
