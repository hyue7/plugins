#include "samsung_checkout_plugin.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <string>

#include "log.h"
#include "messages.h"
#include "billing_manager.h"



class SamsungCheckoutPlugin : public flutter::Plugin, public SamsungCheckoutApi {
public:
  static void
  RegisterWithRegistrar(FlutterDesktopPluginRegistrarRef registrar_ref,
                        flutter::PluginRegistrar *plugin_registrar);
  SamsungCheckoutPlugin(FlutterDesktopPluginRegistrarRef registrar_ref,
                 flutter::PluginRegistrar *plugin_registrar);
  virtual ~SamsungCheckoutPlugin() { DisposeAll(); }
  std::optional<FlutterError>
  SetRequestData(const RequestInfoMessage &msg) override;

private:
  void DisposeAll();
  std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel_;
  FlutterDesktopPluginRegistrarRef registrar_ref_;
  flutter::PluginRegistrar *plugin_registrar_;
};

void SamsungCheckoutPlugin::DisposeAll() {

}

void SamsungCheckoutPlugin::RegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar_ref,
    flutter::PluginRegistrar *plugin_registrar) {
  auto plugin =
      std::make_unique<SamsungCheckoutPlugin>(registrar_ref, plugin_registrar);
  plugin_registrar->AddPlugin(std::move(plugin));
}

SamsungCheckoutPlugin::SamsungCheckoutPlugin(FlutterDesktopPluginRegistrarRef registrar_ref,
                               flutter::PluginRegistrar *plugin_registrar)
    : registrar_ref_(registrar_ref), plugin_registrar_(plugin_registrar) {
  SamsungCheckoutApi::SetUp(plugin_registrar_->messenger(), this);
}

std::optional<FlutterError>
SamsungCheckoutPlugin::SetRequestData(const RequestInfoMessage &msg) {
  std::unique_ptr<BillingManager> billing =
      std::make_unique<BillingManager>(registrar_ref_, msg);
  billing->Init();
  return {};
}


void SamsungCheckoutPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  SamsungCheckoutPlugin::RegisterWithRegistrar(
       registrar, flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrar>(registrar));
}
