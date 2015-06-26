#include "OculusWin32.h"
#include <OVR_CAPI.h>

using namespace Plugins::Display;

class OculusWin32DisplayPlugin : public Plugins::Display::Plugin {
    virtual bool supported() const override;
    virtual const char* name() const override;
    virtual bool init() override;
    virtual void destroy() override;
    virtual bool start() override;
    virtual void stop() override;
    virtual Plugins::Display::Type type() const override;
    virtual glm::uvec2 preferredSurfaceSize() const override;
    virtual glm::uvec2 preferredUiSize() const override;
    virtual glm::mat4 projection(Plugins::Display::Eye eye, const glm::mat4& baseProjection) const override;
    virtual glm::mat4 pose(Plugins::Display::Eye eye) const override;

    virtual void preRender() override;
    virtual void render(
            uint32_t sceneTexture, const glm::uvec2& textureSize,
            uint32_t uiTexture, const glm::uvec2& uiSize, const glm::mat4& uiView) override;
    virtual void postRender() override;
};



bool OculusWin32DisplayPlugin::supported() const {
    return false;
};
const char* OculusWin32DisplayPlugin::name() const {
    return "Oculus SDK 0.6.x";
}

bool OculusWin32DisplayPlugin::init() {
    return false;
}

void OculusWin32DisplayPlugin::destroy() {
}

bool OculusWin32DisplayPlugin::start() {
    return false;
}

void OculusWin32DisplayPlugin::stop() {
}

Type OculusWin32DisplayPlugin::type() const {
    return Type::HMD;
}

glm::uvec2 OculusWin32DisplayPlugin::preferredSurfaceSize() const {
    return glm::uvec2(100, 100);
}

glm::uvec2 OculusWin32DisplayPlugin::preferredUiSize() const {
    return glm::uvec2(1920, 1080);
}

glm::mat4 OculusWin32DisplayPlugin::projection(Plugins::Display::Eye eye, const glm::mat4& baseProjection) const {
    return baseProjection;
}

glm::mat4 OculusWin32DisplayPlugin::pose(Plugins::Display::Eye eye) const {
    return glm::mat4();
}

void OculusWin32DisplayPlugin::preRender() {
}

void OculusWin32DisplayPlugin::render(
    uint32_t sceneTexture, const glm::uvec2& textureSize,
    uint32_t uiTexture, const glm::uvec2& uiSize, const glm::mat4& uiView) {
}

void OculusWin32DisplayPlugin::postRender() {
}

Plugins::Display::Plugin* buildOculusPlugin() {
    return new OculusWin32DisplayPlugin();
}
