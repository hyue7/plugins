// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'package:pigeon/pigeon.dart';

@ConfigurePigeon(PigeonOptions(
    dartOut: 'lib/src/messages.dart',
    //cppHeaderOut: 'tizen/src/messages.h',
    //cppSourceOut: 'tizen/src/messages.cc',
    copyrightHeader: 'pigeons/copyright.txt'))
class RequestInfoMessage {
  Map<Object?, Object?>? requestData;
}

@HostApi(dartHostTestHandler: 'TestHostVideoPlayerApi')
abstract class SamsungCheckoutApi {
  void setRequestData(RequestInfoMessage msg);
}
