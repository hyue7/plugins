#include "billing_manager.h"

#include <flutter/method_channel.h>
#include <flutter/standard_method_codec.h>
#include <json-glib/json-glib.h>

#include <cassert>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "log.h"

void BillingManager::Init() {
  billing_api_handle_ = OpenBillingApi();
  if (billing_api_handle_ == nullptr) {
    LOG_ERROR("Fail to open billing api");
  }

  int init_billing_api = InitBillingApi(billing_api_handle_);
  LOG_INFO("init_billing_api:%d", init_billing_api);
  if (init_billing_api == 0) {
    LOG_ERROR(" Fail to init billing api");
  }

  sso_api_handle_ = OpenSsoApi();
  if (sso_api_handle_ == nullptr) {
    LOG_ERROR("Fail to open sso api");
  }

  int init_sso_api = InitSsoApi(sso_api_handle_);
  LOG_INFO("init_sso_api:%d", init_sso_api);
  if (init_sso_api == 0) {
    LOG_ERROR(" Fail to init sso api");
  }
}

void BillingManager::ParseRequestMessage(
    const RequestInfoMessage &request_message) {
  LOG_INFO("ParseRequestMessage");
  const flutter::EncodableMap *request_data = request_message.request_data();
  auto request_data_iter = request_data->find(flutter::EncodableValue("appId"));
  if (request_data_iter != request_data->end()) {
    if (std::holds_alternative<std::string>(
            request_data->at(flutter::EncodableValue("appId")))) {
      app_id_ = std::get<std::string>(
          request_data->at(flutter::EncodableValue("appId")));
    }
  }
  request_data_iter =
      request_data->find(flutter::EncodableValue("countryCode"));
  if (request_data_iter != request_data->end()) {
    if (std::holds_alternative<std::string>(
            request_data->at(flutter::EncodableValue("countryCode")))) {
      country_code_ = std::get<std::string>(
          request_data->at(flutter::EncodableValue("countryCode")));
    }
  }
  request_data_iter = request_data->find(flutter::EncodableValue("itemType"));
  if (request_data_iter != request_data->end()) {
    if (std::holds_alternative<std::string>(
            request_data->at(flutter::EncodableValue("itemType")))) {
      item_type_ = std::get<std::string>(
          request_data->at(flutter::EncodableValue("itemType")));
    }
  }
  request_data_iter = request_data->find(flutter::EncodableValue("pageSize"));
  if (request_data_iter != request_data->end()) {
    if (std::holds_alternative<int>(
            request_data->at(flutter::EncodableValue("pageSize")))) {
      page_size_ =
          std::get<int>(request_data->at(flutter::EncodableValue("pageSize")));
    }
  }
  request_data_iter = request_data->find(flutter::EncodableValue("pageNum"));
  if (request_data_iter != request_data->end()) {
    if (std::holds_alternative<int>(
            request_data->at(flutter::EncodableValue("pageNum")))) {
      page_num_ =
          std::get<int>(request_data->at(flutter::EncodableValue("pageNum")));
    }
  }
  request_data_iter =
      request_data->find(flutter::EncodableValue("securityKey"));
  if (request_data_iter != request_data->end()) {
    if (std::holds_alternative<std::string>(
            request_data->at(flutter::EncodableValue("securityKey")))) {
      security_key_ = std::get<std::string>(
          request_data->at(flutter::EncodableValue("securityKey")));
    }
  }
  LOG_INFO(
      "app_id_:%s, country_code_:%s, item_type_:%s,page_size_:%d, "
      "page_num_:%d, security_key_:%s",
      app_id_.c_str(), country_code_.c_str(), item_type_.c_str(), page_size_,
      page_num_, security_key_.c_str());
}

void BillingManager::GetProductList() {
  LOG_INFO("GetProductList");
  bool bRet;
  // std::string product_check_value = HMac::base64Encode(
  //     HMac::cHMacSha256(app_id_ + country_code_, security_key_, true), true);
  // bRet = service_billing_get_products_list(
  //     app_id_.c_str(), country_code_.c_str(), page_size_, page_num_,
  //     product_check_value.c_str(), SERVERTYPE_DEV, OnProducts, (void *)this);
  bRet = service_billing_get_products_list(
      "3201504002021", "US", 20, 1,
      "sbKzQJrFdeLkfXTQUdjjq4jt7ykx79QmJno9ZLam+SM=", SERVERTYPE_DEV,
      OnProducts, (void *)this);
  LOG_INFO("bRet:%d", bRet);
}

void BillingManager::GetPurchaseList() {
  LOG_INFO("GetPurchaseList");
  sso_login_info_s login_info;
  sso_get_login_info(&login_info);
  custom_id_ = login_info.uid;
  LOG_INFO("custom_id:%s", custom_id_.c_str());
  bool bRet;
  std::string purchase_check_value = HMac::base64Encode(
      HMac::cHMacSha256(app_id_ + custom_id_ + country_code_ + item_type_ +
                            std::to_string(page_num_),
                        security_key_, true),
      true);
  bRet = service_billing_get_purchase_list(
      app_id_.c_str(), custom_id_.c_str(), country_code_.c_str(), page_num_,
      purchase_check_value.c_str(), SERVERTYPE_DEV, OnPurchase, (void *)this);
}

BillingManager::BillingManager(FlutterDesktopPluginRegistrarRef registrar_ref,
                               const RequestInfoMessage &request_message)
    : registrar_ref_(registrar_ref) {
  flutter::PluginRegistrar *plugin_registrar =
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrar>(registrar_ref_);

  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          plugin_registrar->messenger(), "samsung_checkout",
          &flutter::StandardMethodCodec::GetInstance());

  channel->SetMethodCallHandler(
      [this](const flutter::MethodCall<flutter::EncodableValue> &call,
             std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>>
                 result) { this->HandleMethodCall(call, std::move(result)); });

  ParseRequestMessage(request_message);
}

void BillingManager::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  LOG_INFO("HandleMethodCall");
  const auto &method_name = method_call.method_name();
  method_result_ = std::move(result);
  if (method_name == "getProductList") {
    GetProductList();
  } else if (method_name == "getPurchaseList") {
    GetPurchaseList();
  } else if (method_name == "buyItem") {
    const flutter::EncodableValue *args = method_call.arguments();
    std::string pay_details;
    if (std::holds_alternative<flutter::EncodableMap>(*args)) {
      flutter::EncodableMap encodables = std::get<flutter::EncodableMap>(*args);
      flutter::EncodableValue &pay_details_value =
          encodables[flutter::EncodableValue("payDetails")];
      if (std::holds_alternative<std::string>(pay_details_value)) {
        pay_details = std::get<std::string>(pay_details_value);
        LOG_INFO("pay_details:%s", pay_details.c_str());
      }
    }
    BuyItem(pay_details);
  } else {
    result->NotImplemented();
  }
}

void BillingManager::BuyItem(std::string detail) {
  LOG_INFO("BuyItem detail:%s", detail.c_str());
  service_billing_set_buyitem_cb(OnBuyItem, this);
  bool bRet = service_billing_buyitem("3201504002021", "DEV", detail.c_str());
  method_result_->Success(flutter::EncodableValue(true));
}

void BillingManager::OnProducts(const char *detailResult, void *pUser) {
  LOG_INFO("productlist:%s", detailResult);
  BillingManager *billing = reinterpret_cast<BillingManager *>(pUser);

  if (billing->method_result_ != nullptr) {
    LOG_INFO("billing->method_result_");
    billing->method_result_->Success(
        flutter::EncodableValue(std::string(detailResult)));
  }
}

void BillingManager::OnPurchase(const char *detailResult, void *pUser) {
  LOG_INFO("purchaselist:%s", detailResult);
  BillingManager *billing = reinterpret_cast<BillingManager *>(pUser);

  if (billing->method_result_) {
    billing->method_result_->Success(
        flutter::EncodableValue(std::string(detailResult)));
  }
}

bool BillingManager::OnBuyItem(const char *payResult, const char *detailInfo,
                               void *pUser) {
  LOG_INFO("[%s] [%s]", payResult, detailInfo);
}

void BillingManager::Dispose() {
  LOG_INFO("dispose");
  method_result_ = nullptr;
}
