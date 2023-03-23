#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>

#include "billing_service_proxy.h"
#include "hmac.h"
#include "messages.h"


class BillingManager {

public:
  BillingManager(FlutterDesktopPluginRegistrarRef registrar_ref,
                 const RequestInfoMessage &request_message);
  ~BillingManager(){};
  void Init();
  void ParseRequestMessage(const RequestInfoMessage &request_message);
  void GetProductList();
  void GetPurchaseList();
  void BuyItem(std::string detail);
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
  void Dispose();

private:
  static void OnProducts(const char *detailResult, void *pUser);
  static void OnPurchase(const char *detailResult, void *pUser);
  static bool OnBuyItem(const char *payResult, const char *detailInfo,
                        void *pUser);

  void *billing_api_handle_ = nullptr;
  void *sso_api_handle_ = nullptr;
  std::string product_list_;
  std::shared_ptr<flutter::MethodResult<flutter::EncodableValue>>
      method_result_ = nullptr;
  FlutterDesktopPluginRegistrarRef registrar_ref_;
  std::string app_id_;
  std::string custom_id_;
  std::string country_code_;
  std::string item_type_;
  int page_size_;
  int page_num_;
  std::string security_key_;
};
