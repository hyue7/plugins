// You have generated a new plugin project without specifying the `--platforms`
// flag. A plugin project with no platform support was generated. To add a
// platform, run `flutter create -t plugin --platforms <platforms> .` under the
// same directory. You can also find a detailed instruction on how to add
// platforms in the `pubspec.yaml` at
// https://flutter.dev/docs/development/packages-and-plugins/developing-packages#plugin-platforms.

import 'src/samsung_checkout_platform_interface.dart';

class ProductListJsonModel {
  List<ItemDetails> itemDetails;

  ProductListJsonModel.fromJson(Map<String, dynamic> json)
      : itemDetails = (List.from(json['ItemDetails']))
            .map((i) => ItemDetails.fromJson(i))
            .toList();
}

class ItemDetails {
  String itemId;
  String itemTitle;
  String itemDesc;
  num price;
  String currencyId;

  ItemDetails.fromJson(Map<String, dynamic> json)
      : itemId = json['ItemID'],
        itemTitle = json['ItemTitle'],
        itemDesc = json['ItemDesc'],
        price = json['Price'],
        currencyId = json['CurrencyID'];
}

class PurchaseListJsonModel {
  List<InvoiceDetails> invoiceDetails;

  PurchaseListJsonModel.fromJson(Map<String, dynamic> json)
      : invoiceDetails = (List.from(json['InvoiceDetails']))
            .map((i) => InvoiceDetails.fromJson(i))
            .toList();
}

class InvoiceDetails {
  String invoiceId;
  String itemId;
  String itemTitle;
  String orderTime;
  num price;
  String orderCurrencyId;
  bool cancelStatus;
  bool appliedStatus;

  InvoiceDetails.fromJson(Map<String, dynamic> json)
      : invoiceId = json['InvoiceID'],
        itemId = json['ItemID'],
        itemTitle = json['ItemTitle'],
        orderTime = json['OrderTime'],
        price = json['Price'],
        orderCurrencyId = json['OrderCurrencyID'],
        cancelStatus = json['CancelStatus'],
        appliedStatus = json['AppliedStatus'];
}

class SamsungCheckout {
  final Map<String, dynamic> requestData;

  SamsungCheckout.setData({this.requestData = const <String, dynamic>{}});

  Future<void> initialize() async {
    SamsungCheckoutPlatform.instance.setRequestData(requestData);
  }

  Future<String?> getProductList() async {
    return SamsungCheckoutPlatform.instance.getProductList();
  }

  Future<String?> getPurchaseList() async {
    return SamsungCheckoutPlatform.instance.getPurchaseList();
  }

  Future<bool?> buyItem(String payDetails) async {
    return SamsungCheckoutPlatform.instance.buyItem(payDetails);
  }
}
