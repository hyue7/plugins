import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'samsung_checkout_platform_interface.dart';
import 'messages.dart';

/// An implementation of [SamsungCheckoutPlatform] that uses method channels.
class MethodChannelSamsungCheckout extends SamsungCheckoutPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('samsung_checkout');
  final SamsungCheckoutApi _api = SamsungCheckoutApi();

  @override
  Future<void> setRequestData(Map<String, dynamic> requestData) {
    final RequestInfoMessage message = RequestInfoMessage();
    message.requestData = requestData;
    return _api.setRequestData(message);
  }

  @override
  Future<String?> getProductList() async {
    print("getProductList");
    return await methodChannel.invokeMethod('getProductList');
  }

  @override
  Future<String?> getPurchaseList() async {
    return await methodChannel.invokeMethod('getPurchaseList');
  }

  @override
  Future<bool?> buyItem(String payDetails) async {
    return await methodChannel
        .invokeMethod('buyItem', {"payDetails": payDetails});
  }
}
