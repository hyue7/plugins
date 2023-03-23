import 'package:flutter/material.dart';
import 'dart:async';
import 'package:samsung_checkout/samsung_checkout.dart';
import 'dart:convert' show json;

void main() {
  runApp(
    MaterialApp(
      home: _App(),
    ),
  );
}

class _App extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return DefaultTabController(
      length: 2,
      child: Scaffold(
        key: const ValueKey<String>('home_page'),
        appBar: AppBar(
          title: const Text('samsung_checkout example'),
          bottom: const TabBar(
            isScrollable: true,
            tabs: <Widget>[
              Tab(
                icon: Icon(Icons.cloud),
                text: "ProductList",
              ),
              Tab(
                icon: Icon(Icons.cloud),
                text: "PurchaseList",
              ),
            ],
          ),
        ),
        body: TabBarView(
          children: <Widget>[
            _ProductList(),
            _PurchaseList(),
          ],
        ),
      ),
    );
  }
}

class _ProductList extends StatefulWidget {
  @override
  _ProductListState createState() => _ProductListState();
}

class _ProductListState extends State<_ProductList> {
  late SamsungCheckout _controller;
  List itemDetails = [];

  @override
  void initState() {
    super.initState();

    _controller = SamsungCheckout.setData(requestData: {
      'appId': '3202302029916',
      'countryCode': 'US',
      'itemType': '2',
      'pageSize': 65,
      'pageNum': 1,
      'securityKey': '7db0YeFxHPtajwvVShBC5auI4iSJraKy8yhPZe13yvM=',
    });
    _controller.initialize();
    getProductData();
  }

  Future<void> getProductData() async {
    String? data = await _controller.getProductList();
    print(data);
    var dataList = json.decode(data!);
    ProductListJsonModel productList = ProductListJsonModel.fromJson(dataList);
    setState(() {
      itemDetails = productList.itemDetails;
    });
  }

  Widget listWidget(int index) {
    return InkWell(
      child: Column(
        children: [
          Text(
            itemDetails[index].itemTitle,
            style: const TextStyle(
              fontSize: 20,
              color: Color(0xff333333),
              fontWeight: FontWeight.bold,
            ),
          ),
          Text(
            '\$${itemDetails[index].price}',
            style: const TextStyle(
              color: Color(0xff777777),
              fontSize: 15,
            ),
          ),
          const SizedBox(height: 10),
          const Text(
            'Description',
            style: TextStyle(
              fontSize: 15,
              color: Color(0xff333333),
              fontWeight: FontWeight.bold,
            ),
          ),
          const SizedBox(height: 5),
          Text(
            itemDetails[index].itemDesc,
            style: const TextStyle(
              color: Color(0xff777777),
              fontSize: 15,
            ),
          ),
          const SizedBox(height: 5),
          ElevatedButton(
            onPressed: () async {
              Map<String, dynamic> orderDetails = {
                "OrderItemID": itemDetails[index].itemId,
                "OrderTitle": itemDetails[index].itemTitle,
                "OrderTotal": itemDetails[index].price.toString(),
                "OrderCurrencyID": itemDetails[index].currencyId,
              };
              bool? payResult =
                  await _controller.buyItem(json.encode(orderDetails));
              print(payResult);
            },
            child: const Text("BuyItem"),
          ),
        ],
      ),
    );
  }

  @override
  void dispose() {
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return ListView.separated(
      separatorBuilder: (context, index) =>
          const Divider(height: 1.0, color: Colors.black12),
      itemCount: itemDetails.length,
      itemBuilder: (context, index) {
        return listWidget(index);
      },
    );
  }
}

class _PurchaseList extends StatefulWidget {
  @override
  _PurchaseListState createState() => _PurchaseListState();
}

class _PurchaseListState extends State<_PurchaseList> {
  late SamsungCheckout _controller;
  List invoiceDetails = [];

  @override
  void initState() {
    super.initState();
    _controller = SamsungCheckout.setData(requestData: {
      'appId': '3202302029916',
      'countryCode': 'US',
      'itemType': '2',
      'pageSize': 65,
      'pageNum': 1,
      'securityKey': '7db0YeFxHPtajwvVShBC5auI4iSJraKy8yhPZe13yvM=',
    });
    _controller.initialize();
    getPurchaseData();
  }

  Future<void> getPurchaseData() async {
    String? data = await _controller.getPurchaseList();
    var dataList = json.decode(data!);
    PurchaseListJsonModel purchaseList =
        PurchaseListJsonModel.fromJson(dataList);
    setState(() {
      invoiceDetails = purchaseList.invoiceDetails;
    });
  }

  Widget listWidget(int index) {
    return InkWell(
      child: Column(
        children: [
          Text(
            invoiceDetails[index].itemTitle,
            style: const TextStyle(
              fontSize: 20,
              color: Color(0xff333333),
              fontWeight: FontWeight.bold,
            ),
          ),
          Text(
            '\$${invoiceDetails[index].price}',
            style: const TextStyle(
              color: Color(0xff777777),
              fontSize: 15,
            ),
          ),
          const SizedBox(height: 10),
          const Text(
            'orderTime',
            style: TextStyle(
              fontSize: 15,
              color: Color(0xff333333),
              fontWeight: FontWeight.bold,
            ),
          ),
          const SizedBox(height: 5),
          Text(
            invoiceDetails[index].orderTime,
            style: const TextStyle(
              color: Color(0xff777777),
              fontSize: 15,
            ),
          ),
        ],
      ),
    );
  }

  @override
  void dispose() {
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return ListView.separated(
      separatorBuilder: (context, index) =>
          const Divider(height: 1.0, color: Colors.black12),
      itemCount: invoiceDetails.length,
      itemBuilder: (context, index) {
        return listWidget(index);
      },
    );
  }
}
