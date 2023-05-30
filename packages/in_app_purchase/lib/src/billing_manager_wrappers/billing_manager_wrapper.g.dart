// GENERATED CODE - DO NOT MODIFY BY HAND

part of 'billing_manager_wrapper.dart';

// **************************************************************************
// JsonSerializableGenerator
// **************************************************************************

ServiceAvailableAPIResult _$ServiceAvailableAPIResultFromJson(
        Map<String, dynamic> json) =>
    ServiceAvailableAPIResult(
      status: json['status'] as String? ?? '',
      result: json['result'] as String? ?? '',
      serviceYn: json['serviceYn'] as String? ?? '',
    );

Map<String, dynamic> _$ServiceAvailableAPIResultToJson(
        ServiceAvailableAPIResult instance) =>
    <String, dynamic>{
      'status': instance.status,
      'result': instance.result,
      'serviceYn': instance.serviceYn,
    };

ProductsListApiResult _$ProductsListApiResultFromJson(
        Map<String, dynamic> json) =>
    ProductsListApiResult(
      cPStatus: json['CPStatus'] as String? ?? '',
      cPResult: json['CPResult'] as String? ?? '',
      checkValue: json['CheckValue'] as String? ?? '',
      totalCount: json['TotalCount'] as int? ?? 0,
      itemDetails: (json['ItemDetails'] as List<dynamic>?)
              ?.map((e) => ItemDetails.fromJson(e as Map<String, dynamic>))
              .toList() ??
          [],
    );

Map<String, dynamic> _$ProductsListApiResultToJson(
        ProductsListApiResult instance) =>
    <String, dynamic>{
      'CPStatus': instance.cPStatus,
      'CPResult': instance.cPResult,
      'TotalCount': instance.totalCount,
      'CheckValue': instance.checkValue,
      'ItemDetails': instance.itemDetails,
    };

ItemDetails _$ItemDetailsFromJson(Map<String, dynamic> json) => ItemDetails(
      seq: json['Seq'] as int? ?? 0,
      itemID: json['ItemID'] as String? ?? '',
      itemTitle: json['ItemTitle'] as String? ?? '',
      itemDesc: json['ItemDesc'] as String? ?? '',
      itemType: json['ItemType'] as int? ?? 0,
      price: json['Price'] as num? ?? 0,
      currencyID: json['CurrencyID'] as String? ?? '',
    );

Map<String, dynamic> _$ItemDetailsToJson(ItemDetails instance) =>
    <String, dynamic>{
      'Seq': instance.seq,
      'ItemID': instance.itemID,
      'ItemTitle': instance.itemTitle,
      'ItemDesc': instance.itemDesc,
      'ItemType': instance.itemType,
      'Price': instance.price,
      'CurrencyID': instance.currencyID,
    };

ProductSubscriptionInfo _$ProductSubscriptionInfoFromJson(
        Map<String, dynamic> json) =>
    ProductSubscriptionInfo(
      paymentCycle: json['PaymentCycle'] as int? ?? 0,
      paymentCycleFrq: json['PaymentCycleFrq'] as int? ?? 0,
      paymentCyclePeriod: json['PaymentCyclePeriod'] as String? ?? '',
    );

Map<String, dynamic> _$ProductSubscriptionInfoToJson(
        ProductSubscriptionInfo instance) =>
    <String, dynamic>{
      'PaymentCyclePeriod': instance.paymentCyclePeriod,
      'PaymentCycleFrq': instance.paymentCycleFrq,
      'PaymentCycle': instance.paymentCycle,
    };

BillingBuyData _$BillingBuyDataFromJson(Map<String, dynamic> json) =>
    BillingBuyData(
      payResult: json['payResult'] as String? ?? '',
      payDetails: (json['payDetails'] as Map<String, dynamic>?)?.map(
            (k, e) => MapEntry(k, e as String),
          ) ??
          {},
    );

Map<String, dynamic> _$BillingBuyDataToJson(BillingBuyData instance) =>
    <String, dynamic>{
      'payResult': instance.payResult,
      'payDetails': instance.payDetails,
    };

GetUserPurchaseListAPIResult _$GetUserPurchaseListAPIResultFromJson(
        Map<String, dynamic> json) =>
    GetUserPurchaseListAPIResult(
      cPResult: json['CPResult'] as String? ?? '',
      cPStatus: json['CPStatus'] as String? ?? '',
      invoiceDetails: (json['InvoiceDetails'] as List<dynamic>?)
              ?.map((e) => InvoiceDetails.fromJson(e as Map<String, dynamic>))
              .toList() ??
          [],
      totalCount: json['TotalCount'] as int? ?? 0,
      checkValue: json['CheckValue'] as String? ?? '',
    );

Map<String, dynamic> _$GetUserPurchaseListAPIResultToJson(
        GetUserPurchaseListAPIResult instance) =>
    <String, dynamic>{
      'CPStatus': instance.cPStatus,
      'CPResult': instance.cPResult,
      'TotalCount': instance.totalCount,
      'CheckValue': instance.checkValue,
      'InvoiceDetails': instance.invoiceDetails,
    };

InvoiceDetails _$InvoiceDetailsFromJson(Map<String, dynamic> json) =>
    InvoiceDetails(
      seq: json['Seq'] as int? ?? 0,
      invoiceID: json['InvoiceID'] as String? ?? '',
      itemID: json['ItemID'] as String? ?? '',
      itemTitle: json['ItemTitle'] as String? ?? '',
      itemType: json['ItemType'] as int? ?? 0,
      orderTime: json['OrderTime'] as String? ?? '',
      price: json['Price'] as num? ?? 0,
      orderCurrencyID: json['OrderCurrencyID'] as String? ?? '',
      appliedStatus: json['AppliedStatus'] as bool? ?? false,
      cancelStatus: json['CancelStatus'] as bool? ?? false,
      appliedTime: json['AppliedTime'] as String? ?? '',
      period: json['Period'] as int? ?? 0,
      limitEndTime: json['LimitEndTime'] as String? ?? '',
      remainTime: json['RemainTime'] as String? ?? '',
    );

Map<String, dynamic> _$InvoiceDetailsToJson(InvoiceDetails instance) =>
    <String, dynamic>{
      'Seq': instance.seq,
      'InvoiceID': instance.invoiceID,
      'ItemID': instance.itemID,
      'ItemTitle': instance.itemTitle,
      'ItemType': instance.itemType,
      'OrderTime': instance.orderTime,
      'Period': instance.period,
      'Price': instance.price,
      'OrderCurrencyID': instance.orderCurrencyID,
      'CancelStatus': instance.cancelStatus,
      'AppliedStatus': instance.appliedStatus,
      'AppliedTime': instance.appliedTime,
      'LimitEndTime': instance.limitEndTime,
      'RemainTime': instance.remainTime,
    };

PurchaseSubscriptionInfo _$PurchaseSubscriptionInfoFromJson(
        Map<String, dynamic> json) =>
    PurchaseSubscriptionInfo(
      subscriptionId: json['SubscriptionId'] as String? ?? '',
      subsStartTime: json['SubsStartTime'] as String? ?? '',
      subsEndTime: json['SubsEndTime'] as String? ?? '',
      subsStatus: json['SubsStatus'] as String? ?? '',
    );

Map<String, dynamic> _$PurchaseSubscriptionInfoToJson(
        PurchaseSubscriptionInfo instance) =>
    <String, dynamic>{
      'SubscriptionId': instance.subscriptionId,
      'SubsStartTime': instance.subsStartTime,
      'SubsEndTime': instance.subsEndTime,
      'SubsStatus': instance.subsStatus,
    };
