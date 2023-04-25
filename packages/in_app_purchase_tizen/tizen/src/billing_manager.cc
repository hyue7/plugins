// Copyright 2023 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "billing_manager.h"

#include <flutter/standard_method_codec.h>

#include <cassert>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "log.h"

const char *kInvalidArgument = "Invalid argument";

static server_type ConvertServerType(const char *server_type_string) {
  if (strcasecmp("DEV", server_type_string) == 0) {
    return SERVERTYPE_DEV;
  } else if (strcasecmp("OPERATE", server_type_string) == 0) {
    return SERVERTYPE_OPERATE;
  } else if (strcasecmp("WORKING", server_type_string) == 0) {
    return SERVERTYPE_WORKING;
  } else if (strcasecmp("DUMMY", server_type_string) == 0) {
    return SERVERTYPE_DUMMY;
  } else
    return SERVERTYPE_NONE;
}

template <typename T>
static bool GetValueFromEncodableMap(const flutter::EncodableMap *map,
                                     const char *key, T &out) {
  auto iter = map->find(flutter::EncodableValue(key));
  if (iter != map->end() && !iter->second.IsNull()) {
    if (auto *value = std::get_if<T>(&iter->second)) {
      out = *value;
      return true;
    }
  }
  return false;
}

template <typename T>
static T GetRequiredArg(const flutter::EncodableMap *arguments,
                        const char *key) {
  T value;
  if (GetValueFromEncodableMap(arguments, key, value)) {
    return value;
  }
  std::string message =
      "No " + std::string(key) + " provided or has invalid type or value.";
  throw std::invalid_argument(message);
}

bool BillingManager::Init() {
  billing_api_handle_ = OpenBillingApi();
  if (billing_api_handle_ == nullptr) {
    LOG_ERROR("Fail to open billing api");
    return false;
  }

  int init_billing_api = InitBillingApi(billing_api_handle_);
  if (init_billing_api == 0) {
    LOG_ERROR("Fail to init billing api");
    return false;
  }
  return true;
}

bool BillingManager::GetProductList(const flutter::EncodableMap *encodables) {
  LOG_INFO("start get product list");
  std::string app_id = GetRequiredArg<std::string>(encodables, "appId");
  std::string country_code =
      GetRequiredArg<std::string>(encodables, "countryCode");
  std::string item_type = GetRequiredArg<std::string>(encodables, "itemType");
  int page_size = GetRequiredArg<int>(encodables, "pageSize");
  int page_num = GetRequiredArg<int>(encodables, "pageNum");
  std::string check_value =
      GetRequiredArg<std::string>(encodables, "checkValue");
  std::string server_type =
      GetRequiredArg<std::string>(encodables, "serverType");

  bool bRet = service_billing_get_products_list(
      app_id.c_str(), country_code.c_str(), page_size, page_num,
      check_value.c_str(), ConvertServerType(server_type.c_str()), OnProducts,
      (void *)this);
  if (!bRet) {
    LOG_ERROR("service_billing_get_products_list failed");
    return false;
  }
  return true;
}

bool BillingManager::GetPurchaseList(const flutter::EncodableMap *encodables) {
  LOG_INFO("start get purchase list");
  std::string app_id = GetRequiredArg<std::string>(encodables, "appId");
  std::string custom_id = GetRequiredArg<std::string>(encodables, "customId");
  std::string country_code =
      GetRequiredArg<std::string>(encodables, "countryCode");
  int page_num = GetRequiredArg<int>(encodables, "pageNum");
  std::string check_value =
      GetRequiredArg<std::string>(encodables, "checkValue");
  std::string server_type =
      GetRequiredArg<std::string>(encodables, "serverType");

  bool bRet = service_billing_get_purchase_list(
      app_id.c_str(), "810000047372", country_code.c_str(), page_num,
      check_value.c_str(), ConvertServerType(server_type.c_str()), OnPurchase,
      (void *)this);
  if (!bRet) {
    LOG_ERROR("service_billing_get_purchase_list failed");
    return false;
  }
  return true;
}

bool BillingManager::BuyItem(const flutter::EncodableMap *encodables) {
  LOG_INFO("start buy item");
  std::string pay_details =
      GetRequiredArg<std::string>(encodables, "payDetails");
  std::string app_id = GetRequiredArg<std::string>(encodables, "appId");
  std::string server_type =
      GetRequiredArg<std::string>(encodables, "serverType");

  LOG_INFO("BuyItem detail:%s", pay_details.c_str());
  bool bRet = service_billing_buyitem(app_id.c_str(), server_type.c_str(),
                                      pay_details.c_str());
  service_billing_set_buyitem_cb(OnBuyItem, this);
  if (!bRet) {
    LOG_ERROR("service_billing_buyitem failed");
    return false;
  }
  return true;
}

BillingManager::BillingManager(flutter::PluginRegistrar *plugin_registrar)
    : plugin_registrar_(plugin_registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          plugin_registrar->messenger(),
          "plugins.flutter.tizen.io/in_app_purchase",
          &flutter::StandardMethodCodec::GetInstance());

  channel->SetMethodCallHandler(
      [this](const flutter::MethodCall<flutter::EncodableValue> &call,
             std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>>
                 result) { this->HandleMethodCall(call, std::move(result)); });
}

void BillingManager::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  LOG_INFO("HandleMethodCall");

  const auto *encodables =
      std::get_if<flutter::EncodableMap>(method_call.arguments());
  try {
    const auto &method_name = method_call.method_name();
    if (method_name == "getProductList") {
      if (!encodables) {
        result->Error(kInvalidArgument, "No arguments provided");
        return;
      }
      if (GetProductList(encodables)) {
        method_result_ = std::move(result);
      } else {
        result->Error("getProductList failed");
        return;
      }
    } else if (method_name == "getPurchaseList") {
      if (!encodables) {
        result->Error(kInvalidArgument, "No arguments provided");
        return;
      }
      if (GetPurchaseList(encodables)) {
        method_result_ = std::move(result);
      } else {
        result->Error("getPurchaseList failed");
        return;
      }
    } else if (method_name == "buyItem") {
      if (!encodables) {
        result->Error(kInvalidArgument, "No arguments provided");
        return;
      }
      if (BuyItem(encodables)) {
        method_result_ = std::move(result);
      } else {
        result->Error("buyItem failed");
        return;
      }
    } else if (method_name == "isAvailable") {
      if (BillingIsAvailable()) {
        method_result_ = std::move(result);
      } else {
        result->Error("isAvailable failed");
        return;
      }
    } else {
      result->NotImplemented();
    }
  } catch (const std::invalid_argument &error) {
    result->Error(kInvalidArgument, error.what());
  }
}

void BillingManager::SendResult(const flutter::EncodableValue &result) {
  if (method_result_) {
    method_result_->Success(result);
    method_result_ = nullptr;
  }
}

bool BillingManager::BillingIsAvailable() {
  bool bRet =
      service_billing_is_service_available(SERVERTYPE_DEV, OnAvailable, this);
  if (!bRet) {
    LOG_ERROR("service_billing_is_service_available failed");
    return false;
  }
  return true;
}

void BillingManager::OnAvailable(const char *detailResult, void *pUser) {
  LOG_INFO("OnAvailable detailResult:%s", detailResult);
  BillingManager *billing = reinterpret_cast<BillingManager *>(pUser);

  billing->SendResult(flutter::EncodableValue(std::string(detailResult)));
}

void BillingManager::OnProducts(const char *detailResult, void *pUser) {
  LOG_INFO("productlist:%s", detailResult);
  BillingManager *billing = reinterpret_cast<BillingManager *>(pUser);

  billing->SendResult(flutter::EncodableValue(std::string(detailResult)));
}

void BillingManager::OnPurchase(const char *detailResult, void *pUser) {
  LOG_INFO("purchaselist:%s", detailResult);
  BillingManager *billing = reinterpret_cast<BillingManager *>(pUser);

  billing->SendResult(flutter::EncodableValue(std::string(detailResult)));
}

bool BillingManager::OnBuyItem(const char *payResult, const char *detailInfo,
                               void *pUser) {
  LOG_INFO("OnBuyItem result:[%s] [%s]", payResult, detailInfo);
  BillingManager *billing = reinterpret_cast<BillingManager *>(pUser);
  flutter::EncodableMap result_map = {
      {flutter::EncodableValue("PayResult"),
       flutter::EncodableValue(payResult)},
  };
  billing->SendResult(flutter::EncodableValue(result_map));
}

void BillingManager::Dispose() {
  LOG_INFO("dispose");
  // close dlopen handle.
  CloseBillingApi(billing_api_handle_);
  billing_api_handle_ = nullptr;
}
