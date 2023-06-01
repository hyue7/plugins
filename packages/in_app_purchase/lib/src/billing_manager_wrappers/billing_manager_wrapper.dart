// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:async';
import 'dart:convert' show json;

import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:in_app_purchase_platform_interface/in_app_purchase_platform_interface.dart';
import 'package:json_annotation/json_annotation.dart';

import '../channel.dart';
import '../in_app_purchase_tizen_platform.dart';

// WARNING: Changes to `@JsonSerializable` classes need to be reflected in the
// below generated file. Run `flutter-tizen packages pub run build_runner watch` to
// rebuild and watch for further changes.
part 'billing_manager_wrapper.g.dart';

/// This class can be used directly to call Billing(Samsung Checkout) APIs.
///
/// Wraps a
/// [`BillingManager`](https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html#BillingManager)
/// instance.
class BillingManager {
  /// Creates a billing manager.
  BillingManager();

  /// Calls
  /// [`BillingManager-isServiceAvailable`](https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html#BillingManager-isServiceAvailable)
  /// to check whether the Billing server is available.
  Future<bool> isAvailable() async {
    final String? isAvailableResult =
        await channel.invokeMethod<String>('isAvailable');
    if (isAvailableResult == null) {
      throw PlatformException(
        code: 'no_response',
        message: 'Failed to get response from platform.',
      );
    }
    final ServiceAvailableAPIResult isAvailable =
        ServiceAvailableAPIResult.fromJson(
            json.decode(isAvailableResult) as Map<String, dynamic>);
    final bool resultRet;
    switch (isAvailable.result) {
      case 'Success':
        resultRet = true;
        break;
      default:
        resultRet = false;
    }
    return resultRet;
  }

  /// Calls
  /// [`BillingManager-getProductsList`](https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html#BillingManager-getProductsList)
  /// to retrieves the list of products registered on the Billing (DPI) server.
  Future<ProductsListApiResult> requestProducts(
      List<String> requestparameters) async {
    final Map<String, dynamic> arguments = <String, dynamic>{
      'appId': requestparameters[0],
      'countryCode': requestparameters[1],
      'itemType': requestparameters[2],
      'pageSize': int.parse(requestparameters[3]),
      'pageNum': int.parse(requestparameters[4]),
      'serverType': requestparameters[5],
      'checkValue': requestparameters[6],
    };

    final String? productResponse =
        await channel.invokeMethod<String?>('getProductList', arguments);
    if (productResponse == null) {
      throw PlatformException(
        code: 'no_response',
        message: 'Failed to get response from platform.',
      );
    }
    return ProductsListApiResult.fromJson(
        json.decode(productResponse) as Map<String, dynamic>);
  }

  /// Calls
  /// [`BillingManager-getUserPurchaseList`](https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html#BillingManager-getUserPurchaseList)
  /// to retrieves the user's purchase list.
  Future<GetUserPurchaseListAPIResult> requestPurchases(
      String? costomId, List<String> requestparameters) async {
    final Map<String, dynamic> arguments = <String, dynamic>{
      'appId': requestparameters[0],
      'customId': costomId,
      'countryCode': requestparameters[1],
      'pageNum': int.parse(requestparameters[4]),
      'serverType': requestparameters[5],
      'checkValue': requestparameters[7],
    };

    final String? purchaseResponse =
        await channel.invokeMethod<String?>('getPurchaseList', arguments);
    if (purchaseResponse == null) {
      throw PlatformException(
        code: 'no_response',
        message: 'Failed to get response from platform.',
      );
    }
    return GetUserPurchaseListAPIResult.fromJson(
        json.decode(purchaseResponse) as Map<String, dynamic>);
  }

  /// Calls
  /// [`BillingManager-buyItem`](https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html#BillingManager-buyItem)
  /// to enables implementing the Samsung Checkout Client module within the application.
  /// After authenticating the purchase information through the application, the user can proceed to purchase payment.
  Future<BillingBuyData> buyItem({
    required String appId,
    required String serverType,
    required String orderItemId,
    required String orderTitle,
    required String orderTotal,
    required String orderCurrencyId,
  }) async {
    final Map<String, dynamic> orderDetails = <String, dynamic>{
      'OrderItemID': orderItemId,
      'OrderTitle': orderTitle,
      'OrderTotal': orderTotal,
      'OrderCurrencyID': orderCurrencyId
    };

    final Map<String, dynamic> arguments = <String, dynamic>{
      'appId': appId,
      'serverType': serverType,
      'payDetails': json.encode(orderDetails)
    };
    final Map<String, dynamic> map =
        await channel.invokeMapMethod<String, dynamic>('buyItem', arguments) ??
            <String, dynamic>{};
    return BillingBuyData.fromJson(map);
  }

  /// Calls
  /// [`BillingManager-verifyInvoice`](https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html#BillingManager-verifyInvoice)
  /// to enables implementing the Samsung Checkout Client module within the application.
  /// Checks whether a purchase, corresponding to a specific "InvoiceID", was successful.
  Future<VerifyInvoiceAPIResult> verifyInvoice(
      {required String appId,
      required String? invoiceId,
      required String? customId,
      required String? countryCode,
      required String? serverType}) async {
    final Map<String, dynamic> arguments = <String, dynamic>{
      'appId': appId,
      'invoiceId': invoiceId,
      'customId': customId,
      'countryCode': countryCode,
      'serverType': serverType
    };
    final String verifyInvoiceResult =
        await channel.invokeMethod<String>('verifyInvoice', arguments) ?? '';

    return VerifyInvoiceAPIResult.fromJson(
        json.decode(verifyInvoiceResult) as Map<String, dynamic>);
  }
}

/// Dart wrapper around [`ServiceAvailableAPIResult`] in (https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html).
///
/// Defines a dictionary for data returned by the IsServiceAvailable API.
/// This only can be used in [BillingManager.isAvailable].
@JsonSerializable()
@immutable
class ServiceAvailableAPIResult {
  /// Creates a [ServiceAvailableAPIResult] with the given purchase details.
  const ServiceAvailableAPIResult({
    required this.status,
    required this.result,
    this.serviceYn,
  });

  /// Constructs an instance of this from a json string.
  ///
  /// The json needs to have named string keys with values matching the names and
  /// types of all of the members on this class.
  factory ServiceAvailableAPIResult.fromJson(Map<String, dynamic> json) =>
      _$ServiceAvailableAPIResultFromJson(json);

  /// Constructs an instance of this to a json string.
  Map<String, dynamic> toJson() => _$ServiceAvailableAPIResultToJson(this);

  /// The result code of connecting to billing server.
  /// Returns "100000" on success and other codes on failure.
  @JsonKey(defaultValue: '')
  final String status;

  /// The result message of connecting to billing server.
  /// Returns "Success" on success.
  @JsonKey(defaultValue: '')
  final String result;

  /// Returns "Y" if the service is available.
  /// It will be null, if disconnect to billing server.
  @JsonKey(defaultValue: '')
  final String? serviceYn;
}

/// Dart wrapper around [`ProductsListApiResult`] in (https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html).
///
/// Defines a dictionary for product list data returned by the getProductsList API.
/// This only can be used in [BillingManager.requestProducts].
@JsonSerializable()
@immutable
class ProductsListApiResult {
  /// Creates a [ProductsListApiResult] with the given purchase details.
  const ProductsListApiResult({
    required this.cPStatus,
    required this.cPResult,
    required this.checkValue,
    required this.totalCount,
    required this.itemDetails,
  });

  /// Constructs an instance of this from a json string.
  ///
  /// The json needs to have named string keys with values matching the names and
  /// types of all of the members on this class.
  factory ProductsListApiResult.fromJson(Map<String, dynamic> json) =>
      _$ProductsListApiResultFromJson(json);

  /// Constructs an instance of this to a json string.
  Map<String, dynamic> toJson() => _$ProductsListApiResultToJson(this);

  /// DPI result code.
  /// Returns "100000" on success and other codes on failure.
  @JsonKey(defaultValue: '', name: 'CPStatus')
  final String cPStatus;

  /// The result message.
  /// "EOF":Last page of the product list.
  /// "hasNext:TRUE" Product list has further pages.
  /// Other error message, depending on the DPI result code.
  @JsonKey(defaultValue: '', name: 'CPResult')
  final String cPResult;

  /// Total number of invoices.
  @JsonKey(defaultValue: 0, name: 'TotalCount')
  final int totalCount;

  /// Security check value.
  @JsonKey(defaultValue: '', name: 'CheckValue')
  final String checkValue;

  /// ItemDetails in JSON format
  @JsonKey(defaultValue: <ItemDetails>[], name: 'ItemDetails')
  final List<ItemDetails> itemDetails;
}

/// Dart wrapper around [`ItemDetails`] in (https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html).
///
/// Defines a dictionary for the ProductsListAPIResult dictionary 'ItemDetails' parameter.
/// This only can be used in [ProductsListApiResult].
@JsonSerializable()
@immutable
class ItemDetails {
  /// Creates a [ItemDetails] with the given purchase details.
  const ItemDetails({
    required this.seq,
    required this.itemID,
    required this.itemTitle,
    required this.itemDesc,
    required this.itemType,
    required this.price,
    required this.currencyID,
  });

  /// Constructs an instance of this from a json string.
  ///
  /// The json needs to have named string keys with values matching the names and
  /// types of all of the members on this class.
  factory ItemDetails.fromJson(Map<String, dynamic> json) =>
      _$ItemDetailsFromJson(json);

  /// Constructs an instance of this to a json string.
  Map<String, dynamic> toJson() => _$ItemDetailsToJson(this);

  /// Sequence number (1 ~ TotalCount).
  @JsonKey(defaultValue: 0, name: 'Seq')
  final int seq;

  /// The ID of Product.
  @JsonKey(defaultValue: '', name: 'ItemID')
  final String itemID;

  /// The name of product.
  @JsonKey(defaultValue: '', name: 'ItemTitle')
  final String itemTitle;

  /// The description of product.
  @JsonKey(defaultValue: '', name: 'ItemDesc')
  final String itemDesc;

  /// The type of product.
  /// "1": CONSUMABLE
  /// "2": NON-CONSUMABLE
  /// "3": LIMITED-PERIOD
  /// "4": SUBSCRIPTION
  @JsonKey(defaultValue: 0, name: 'ItemType')
  final int? itemType;

  /// The price of product, in "xxxx.yy" format.
  @JsonKey(defaultValue: 0, name: 'Price')
  final num price;

  /// The currency code
  @JsonKey(defaultValue: '', name: 'CurrencyID')
  final String currencyID;
}

/// Dart wrapper around [`ProductSubscriptionInfo`] in (https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html).
///
/// Defines a dictionary for the ItemDetails dictionary 'SubscriptionInfo' parameter.
/// This only can be used in [ItemDetails].
@JsonSerializable()
@immutable
class ProductSubscriptionInfo {
  /// Creates a [ProductSubscriptionInfo] with the given purchase details.
  const ProductSubscriptionInfo({
    required this.paymentCycle,
    required this.paymentCycleFrq,
    required this.paymentCyclePeriod,
  });

  /// Constructs an instance of this from a json string.
  ///
  /// The json needs to have named string keys with values matching the names and
  /// types of all of the members on this class.
  factory ProductSubscriptionInfo.fromJson(Map<String, dynamic> json) =>
      _$ProductSubscriptionInfoFromJson(json);

  /// Constructs an instance of this to a json string.
  Map<String, dynamic> toJson() => _$ProductSubscriptionInfoToJson(this);

  /// Subscription payment period:
  /// "D": Days
  /// "W": Weeks
  /// "M": Months
  @JsonKey(defaultValue: '', name: 'PaymentCyclePeriod')
  final String paymentCyclePeriod;

  /// Payment cycle frequency.
  @JsonKey(defaultValue: 0, name: 'PaymentCycleFrq')
  final int paymentCycleFrq;

  /// Number of payment cycles.
  @JsonKey(defaultValue: 0, name: 'PaymentCycle')
  final int paymentCycle;
}

/// The class represents the information of a product as registered in at
/// Samsung Checkout DPI portal.
class SamsungCheckoutProductDetails extends ProductDetails {
  /// Creates a new Samsung Checkout specific product details object with the
  /// provided details.
  SamsungCheckoutProductDetails({
    required super.id,
    required super.title,
    required super.description,
    required super.price,
    required super.currencyCode,
    required this.itemDetails,
    super.rawPrice = 0.0,
    super.currencySymbol,
  });

  /// Generate a [SamsungCheckoutProductDetails] object based on [ItemDetails] object.
  factory SamsungCheckoutProductDetails.fromProduct(ItemDetails itemDetails) {
    return SamsungCheckoutProductDetails(
      id: itemDetails.itemID,
      title: itemDetails.itemTitle,
      description: itemDetails.itemDesc,
      price: itemDetails.price.toString(),
      currencyCode: itemDetails.currencyID,
      itemDetails: itemDetails,
    );
  }

  /// Points back to the [ItemDetails] object that was used to generate
  /// this [SamsungCheckoutProductDetails] object.
  final ItemDetails itemDetails;
}

/// Dart wrapper around [`BillingBuyData`](https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html#BillingBuyData).
///
/// Defines the payment result and information.
@JsonSerializable()
@immutable
class BillingBuyData {
  /// Creates a [BillingBuyData] with the given purchase details.
  const BillingBuyData({required this.payResult, required this.payDetails});

  /// Constructs an instance of this from a json string.
  ///
  /// The json needs to have named string keys with values matching the names and
  /// types of all of the members on this class.
  factory BillingBuyData.fromJson(Map<String, dynamic> json) =>
      _$BillingBuyDataFromJson(json);

  /// Constructs an instance of this to a json string.
  Map<String, dynamic> toJson() => _$BillingBuyDataToJson(this);

  /// The payment result
  @JsonKey(defaultValue: '')
  final String payResult;

  /// The payment information. It is same with paymentDetails param of buyItem.
  @JsonKey(defaultValue: <String, String>{})
  final Map<String, String> payDetails;
}

/// Dart wrapper around [`GetUserPurchaseListAPIResult`] in (https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html).
///
/// Defines a dictionary for data returned by the getUserPurchaseList API.
/// This only can be used in [BillingManager.requestPurchases]
@JsonSerializable()
@immutable
class GetUserPurchaseListAPIResult {
  /// Creates a [GetUserPurchaseListAPIResult] with the given purchase details.
  const GetUserPurchaseListAPIResult(
      {required this.cPResult,
      required this.cPStatus,
      required this.invoiceDetails,
      this.totalCount,
      this.checkValue});

  /// Constructs an instance of this from a json string.
  ///
  /// The json needs to have named string keys with values matching the names and
  /// types of all of the members on this class.
  factory GetUserPurchaseListAPIResult.fromJson(Map<String, dynamic> json) =>
      _$GetUserPurchaseListAPIResultFromJson(json);

  /// Constructs an instance of this to a json string.
  Map<String, dynamic> toJson() => _$GetUserPurchaseListAPIResultToJson(this);

  /// It returns "100000" in success and other codes in failure. Refer to DPI Error Code.
  @JsonKey(defaultValue: '', name: 'CPStatus')
  final String cPStatus;

  /// The result message:
  /// "EOF":Last page of the product list
  /// "hasNext:TRUE" Product list has further pages
  /// Other error message, depending on the DPI result code
  @JsonKey(defaultValue: '', name: 'CPResult')
  final String cPResult;

  /// Total number of invoices.
  @JsonKey(defaultValue: 0, name: 'TotalCount')
  final int? totalCount;

  /// Security check value.
  @JsonKey(defaultValue: '', name: 'CheckValue')
  final String? checkValue;

  /// InvoiceDetailsin JSON format.
  @JsonKey(defaultValue: <InvoiceDetails>[], name: 'InvoiceDetails')
  final List<InvoiceDetails> invoiceDetails;
}

/// Dart wrapper around [`InvoiceDetails`] in (https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html).
///
/// Defines a dictionary for the GetUserPurchaseListAPIResult dictionary 'InvoiceDetails' parameter.
/// This only can be used in [GetUserPurchaseListAPIResult].
@JsonSerializable()
@immutable
class InvoiceDetails {
  /// Creates a [InvoiceDetails] with the given purchase details.
  const InvoiceDetails({
    required this.seq,
    required this.invoiceID,
    required this.itemID,
    required this.itemTitle,
    required this.itemType,
    required this.orderTime,
    required this.price,
    required this.orderCurrencyID,
    required this.appliedStatus,
    required this.cancelStatus,
    this.appliedTime,
    this.period,
    this.limitEndTime,
    this.remainTime,
  });

  /// Constructs an instance of this from a json string.
  ///
  /// The json needs to have named string keys with values matching the names and
  /// types of all of the members on this class.
  factory InvoiceDetails.fromJson(Map<String, dynamic> json) =>
      _$InvoiceDetailsFromJson(json);

  /// Constructs an instance of this to a json string.
  Map<String, dynamic> toJson() => _$InvoiceDetailsToJson(this);

  /// Sequence number (1 ~ TotalCount).
  @JsonKey(defaultValue: 0, name: 'Seq')
  final int seq;

  /// Invoice ID of this purchase history.
  @JsonKey(defaultValue: '', name: 'InvoiceID')
  final String invoiceID;

  /// The ID of product.
  @JsonKey(defaultValue: '', name: 'ItemID')
  final String itemID;

  /// The name of product.
  @JsonKey(defaultValue: '', name: 'ItemTitle')
  final String itemTitle;

  /// The type of product:
  /// "1": CONSUMABLE
  /// "2": NON-CONSUMABLE
  /// "3": LIMITED-PERIOD
  /// "4": SUBSCRIPTION
  @JsonKey(defaultValue: 0, name: 'ItemType')
  final int itemType;

  /// Payment time, in 14-digit UTC time.
  @JsonKey(defaultValue: '', name: 'OrderTime')
  final String orderTime;

  /// Limited period product duration, in minutes.
  @JsonKey(defaultValue: 0, name: 'Period')
  final int? period;

  /// Product price, in "xxxx.yy" format.
  @JsonKey(defaultValue: 0, name: 'Price')
  final num price;

  /// Currency code.
  @JsonKey(defaultValue: '', name: 'OrderCurrencyID')
  final String orderCurrencyID;

  /// Cancellation status:
  /// "true": Sale canceled
  /// "false" : Sale ongoing
  @JsonKey(defaultValue: false, name: 'CancelStatus')
  final bool cancelStatus;

  /// Product application status:
  /// "true": Applied
  /// "false": Not applied
  @JsonKey(defaultValue: false, name: 'AppliedStatus')
  final bool appliedStatus;

  /// Time product applied, in 14-digit UTC time
  @JsonKey(defaultValue: '', name: 'AppliedTime')
  final String? appliedTime;

  /// Limited period product end time, in 14-digit UTC time
  @JsonKey(defaultValue: '', name: 'LimitEndTime')
  final String? limitEndTime;

  /// Limited period product time remaining, in seconds
  @JsonKey(defaultValue: '', name: 'RemainTime')
  final String? remainTime;
}

/// Dart wrapper around [`PurchaseSubscriptionInfo`] in (https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html).
///
/// Defines a dictionary for the InvoiceDetails dictionary 'SubscriptionInfo' parameter.
/// This only can be used in [InvoiceDetails].
@JsonSerializable()
@immutable
class PurchaseSubscriptionInfo {
  /// Creates a [PurchaseSubscriptionInfo] with the given purchase details.
  const PurchaseSubscriptionInfo({
    required this.subscriptionId,
    required this.subsStartTime,
    required this.subsEndTime,
    required this.subsStatus,
  });

  /// Constructs an instance of this from a json string.
  ///
  /// The json needs to have named string keys with values matching the names and
  /// types of all of the members on this class.
  factory PurchaseSubscriptionInfo.fromJson(Map<String, dynamic> json) =>
      _$PurchaseSubscriptionInfoFromJson(json);

  /// Constructs an instance of this to a json string.
  Map<String, dynamic> toJson() => _$PurchaseSubscriptionInfoToJson(this);

  /// ID of subscription history.
  @JsonKey(defaultValue: '', name: 'SubscriptionId')
  final String subscriptionId;

  /// Subscription start time, in 14-digit UTC time.
  @JsonKey(defaultValue: '', name: 'SubsStartTime')
  final String subsStartTime;

  /// Subscription expiry time, in 14-digit UTC time.
  @JsonKey(defaultValue: '', name: 'SubsEndTime')
  final String subsEndTime;

  /// Subscription status:
  /// "00": Active
  /// "01": Subscription expired
  /// "02": Canceled by buyer
  /// "03": Canceled for payment failure
  /// "04": Canceled by CP
  /// "05": Canceled by admin
  @JsonKey(defaultValue: '', name: 'SubsStatus')
  final String subsStatus;
}

/// The class represents the information of a purchase made using Samsung Checkout.
class SamsungCheckoutPurchaseDetails extends PurchaseDetails {
  /// Creates a new Samsung Checkout specific purchase details object with the
  /// provided details.
  SamsungCheckoutPurchaseDetails({
    required super.productID, //itemID
    required super.purchaseID,
    required super.status,
    required super.transactionDate,
    required super.verificationData,
    required this.invoiceDetails, //invoiceID
  });

  /// Generate a [SamsungCheckoutPurchaseDetails] object based on [PurchaseDetails] object.
  factory SamsungCheckoutPurchaseDetails.fromPurchase(
      InvoiceDetails invoiceDetails) {
    final SamsungCheckoutPurchaseDetails purchaseDetails =
        SamsungCheckoutPurchaseDetails(
      purchaseID: invoiceDetails.invoiceID,
      productID: invoiceDetails.itemID,
      verificationData: PurchaseVerificationData(
          localVerificationData: invoiceDetails.invoiceID,
          serverVerificationData: invoiceDetails.invoiceID,
          source: kIAPSource),
      transactionDate: invoiceDetails.orderTime,
      status: const PurchaseStateConverter()
          .toPurchaseStatus(invoiceDetails.cancelStatus),
      invoiceDetails: invoiceDetails,
    );

    if (purchaseDetails.status == PurchaseStatus.error) {
      purchaseDetails.error = IAPError(
        source: kIAPSource,
        code: kPurchaseErrorCode,
        message: '',
      );
    }

    return purchaseDetails;
  }

  /// Points back to the [InvoiceDetails] which was used to generate this
  /// [SamsungCheckoutPurchaseDetails] object.
  final InvoiceDetails invoiceDetails;
}

/// Dart wrapper around [`VerifyInvoiceAPIResult`] in (https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/billing-api.html).
///
/// This only can be used in [BillingManager.verifyInvoice].
@JsonSerializable()
@immutable
class VerifyInvoiceAPIResult {
  /// Creates a [VerifyInvoiceAPIResult] with the given purchase details.
  const VerifyInvoiceAPIResult({
    required this.cpStatus,
    this.cpResult,
    required this.appId,
    required this.invoiceId,
  });

  /// Constructs an instance of this from a json string.
  ///
  /// The json needs to have named string keys with values matching the names and
  /// types of all of the members on this class.
  factory VerifyInvoiceAPIResult.fromJson(Map<String, dynamic> json) =>
      _$VerifyInvoiceAPIResultFromJson(json);

  /// Constructs an instance of this to a json string.
  Map<String, dynamic> toJson() => _$VerifyInvoiceAPIResultToJson(this);

  /// DPI result code. Returns "100000" on success and other codes on failure.
  @JsonKey(defaultValue: '', name: 'CPStatus')
  final String cpStatus;

  /// The result message:
  /// "SUCCESS" and Other error message, depending on the DPI result code.
  @JsonKey(defaultValue: '', name: 'CPResult')
  final String? cpResult;

  /// The application ID.
  @JsonKey(defaultValue: '', name: 'AppID')
  final String appId;

  /// Invoice ID that you want to verify whether a purchase was successful.
  @JsonKey(defaultValue: '', name: 'InvoiceID')
  final String invoiceId;
}

/// Convert bool value to purchase status.ll
class PurchaseStateConverter {
  /// Default const constructor.
  const PurchaseStateConverter();

  /// Converts the purchase state stored in `object` to a [PurchaseStatus].
  PurchaseStatus toPurchaseStatus(bool object) {
    switch (object) {
      case false:
        return PurchaseStatus.purchased;
      case true:
        return PurchaseStatus.canceled;
      default:
        return PurchaseStatus.error;
    }
  }
}
