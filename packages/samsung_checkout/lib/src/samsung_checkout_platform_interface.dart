import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'samsung_checkout_method_channel.dart';

abstract class SamsungCheckoutPlatform extends PlatformInterface {
  /// Constructs a SamsungCheckoutPlatform.
  SamsungCheckoutPlatform() : super(token: _token);

  static final Object _token = Object();

  static SamsungCheckoutPlatform _instance = MethodChannelSamsungCheckout();

  /// The default instance of [SamsungCheckoutPlatform] to use.
  ///
  /// Defaults to [MethodChannelSamsungCheckout].
  static SamsungCheckoutPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [SamsungCheckoutPlatform] when
  /// they register themselves.
  static set instance(SamsungCheckoutPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getProductList() {
    throw UnimplementedError('getProductList() has not been implemented.');
  }

  Future<String?> getPurchaseList() {
    throw UnimplementedError('getPurchaseList() has not been implemented.');
  }

  Future<bool?> buyItem(String payDetails) {
    throw UnimplementedError('buyItem() has not been implemented.');
  }

  Future<void> setRequestData(Map<String, dynamic> requestData) {
    throw UnimplementedError('setRequestData() has not been implemented.');
  }
}
