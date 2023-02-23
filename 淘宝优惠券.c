/* Note:Your choice is C IDE */
#include "stdio.h"
void main()
{
   
(function() {
	'use strict';
	const host = window.location.host;
	const href = window.location.href;
	const isDebug = false;
	
	/**
	 * 公共功能
	 */
	function commonFunctionObject(){
		this.selectParamterQueryUrl = function(text, tag) { //查询GET请求url中的参数
			if(text.indexOf("?")!=-1){ //选取?后面的字符串,兼容window.location.search，前面的?不能去掉
				var textArray = text.split("?");
				text = "?"+textArray[textArray.length-1];
			}
			var t = new RegExp("(^|&)" + tag + "=([^&]*)(&|$)");
			var a = text.substr(1).match(t);
			if (a != null){
				return a[2];
			}
			return "";
		};
		this.getEndHtmlIdByUrl = function(url) { //获得以html结束的ID
			if(url.indexOf("?")!=-1){
				url = url.split("?")[0]
			}
			if(url.indexOf("#")!=-1){
				url = url.split("#")[0]
			}
			var splitText = url.split("/");
			var idText = splitText[splitText.length-1];
			idText = idText.replace(".html","");
			return idText;
		};
		this.filterStr = function(str){ //去掉空格等
			if(!str) return "";
			str = str.replace(/\t/g,"");
			str = str.replace(/\r/g,"");
			str = str.replace(/\n/g,"");
			str = str.replace(/\+/g,"%2B");//"+"
			str = str.replace(/\&/g,"%26");//"&"
			str = str.replace(/\#/g,"%23");//"#"
			return encodeURIComponent(str)
		};
	}
	const commonFuncObject = new commonFunctionObject();
	
	
	function searchPageObject(){
		this.intervalIsRunComplete = true;  //定时任务一个循环是否完成
		
		/**
		 * 收集列表的元素
		 */
		this.pickupSearchElements = function(){
			var selectorElementList = new Array();
			var url = location.href;
			if(url.indexOf("://s.taobao.com/search")!=-1 || url.indexOf("://s.taobao.com/list")!=-1) { //淘宝搜索页面
				selectorElementList.push({"element":".items >.item", "page":"taobao"});
			}else if(url.indexOf("taobao.com/category")!=-1){ //淘宝店铺分类
				selectorElementList.push({"element":"dl.item", "page":"taobao_category"});
			}else if(url.indexOf("taobao.com/search.htm")!=-1){ //淘宝店铺搜索
				selectorElementList.push({"element":"dl.item", "page":"taobao_shop_search"});
			}
			else if (url.indexOf("://list.tmall.com/search_product.htm")!=-1 || url.indexOf("://list.tmall.com//search_product.htm")!=-1
				|| url.indexOf("://list.tmall.com/coudan/search_product.htm")!=-1) { //天猫、天猫超市
				selectorElementList.push({"element":".product", "page":"tmall_search"});  //天猫
				selectorElementList.push({"element":".product", "page":"tmall_chaoshi"}); //天猫超市
			}else if (url.indexOf("//list.tmall.hk/search_product")!=-1) { //天猫国际
				selectorElementList.push({"element":"#J_ItemList .product", "page":"tmall_hk"});
			}else if(url.indexOf("tmall.com/category-")!=-1){ //天猫分类
				selectorElementList.push({"element":"#J_ShopSearchResult dl.item", "page":"tmall_category"});
			}else if(url.indexOf("tmall.com/category-")!=-1){ //天猫旗舰店分类
				selectorElementList.push({"element":"#J_ShopSearchResult dl.item", "page":"tmall_category"});
			}else if(url.indexOf(".tmall.com/shop/view_shop.htm")!=-1){ //天猫旗舰全部展示
				selectorElementList.push({"element":"dl.item", "page":"tmall_shop_01"});
			}else if(url.indexOf(".tmall.com/search.htm")!=-1){
				selectorElementList.push({"element":"dl.item", "page":"tmall_shop_search"});
			}
			else if(url.indexOf("//maiyao.liangxinyao.com/shop/view_shop.htm")!=-1 
				|| url.indexOf("//maiyao.liangxinyao.com/search.htm")!=-1
				|| url.indexOf("//maiyao.liangxinyao.com/category")!=-1){ //阿里大药房
				selectorElementList.push({"element":"#J_ShopSearchResult dl.item", "page":"ali_yao"});
			}
			else if (url.indexOf("//search.jd.com/Search")!=-1) { //京东搜索
				selectorElementList.push({"element":"#J_goodsList li.gl-item", "page":"jd"});
			}else if(url.indexOf("//search.jd.hk/Search")!=-1){ //京东国际
				selectorElementList.push({"element":"#plist li.gl-item", "page":"jd_hk"});
			}else if(url.indexOf("//mall.jd.com/view_search")!=-1){ //京东大药房
				selectorElementList.push({"element":"li.jSubObject", "page":"jd_yao"});
			}
			return selectorElementList;
		};
		
		/**
		 * 分析链接，提取id和平台信息
		 * @param {Object} url
		 * @param {Object} tag
		 */
		this.analysisUrl = function(url, tag){
			if(!!url){
				var id = commonFuncObject.selectParamterQueryUrl(url, tag);
				if(!!id){
					var platform = "taobao";
					if(url.indexOf("jd.com")!=-1 || url.indexOf("yiyaojd.com")!=-1){
						platform = "jd";
					}else if(url.indexOf("taobao.com")!=-1 || url.indexOf("maiyao.liangxinyao.com")!=-1){
						platform = "taobao";
					}else if(url.indexOf("tmall.com")!=-1 || url.indexOf("tmall.hk")!=-1){
						platform = "tmall";
					}
					return {"id":id, "platform":platform};
				}
			}
			return null;
		};
		
		/**
		 * 为所有的商品创建提示
		 * @param {Object} elementData
		 */
		this.createAllElementHtml = function(elementData){
			let that = this;
			$(elementData.element).each(function(){  //遍历所有的商品框
				that.createOneElementHtml($(this), elementData);
			});
			that.intervalIsRunComplete = true;
		};
		
		/**
		 * 为商品box添加有券提醒
		 * @param {Object} $element  每一个商品box
		 * @param {Object} elementData id and page
		 */
		this.createOneElementHtml = function($element, elementData){  //查询到每个商品list
			if($element.attr("completebox")){  //当存在时，说明已经处理过了
				return;
			}
			
			$element.css("position","relative");
			var page = elementData.page;
			var goodsDetailUrl = null;
			
			if(page==="taobao"){
				goodsDetailUrl = $element.find("a[id^='J_Itemlist_TLink_']").attr("href");
			}else if(page==="taobao_category"){
				goodsDetailUrl = $element.find(".J_TGoldData").attr("href");
			}else if(page==="taobao_shop_search"){
				goodsDetailUrl = $element.find(".J_TGoldData").attr("href");
			}
			else if(page==="ali_yao"){
				goodsDetailUrl = $element.find(".J_TGoldData").attr("href");
			}
			else if(page==="tmall_search"){
				goodsDetailUrl = $element.find("a.productImg").attr("href");
			}else if(page==="tmall_hk"){
				goodsDetailUrl = $element.find("a.productImg").attr("href");
			}else if(page==="tmall_chaoshi"){
				goodsDetailUrl = $element.find("div.product-img").find("a").attr("href");
			}else if(page==="tmall_category"){
				goodsDetailUrl = $element.find(".J_TGoldData").attr("href");
			}else if(page==="tmall_shop_01"){
				goodsDetailUrl = $element.find("dt.photo").find("a").attr("href");
			}else if(page==="tmall_shop_search"){
				goodsDetailUrl = $element.find(".J_TGoldData").attr("href");
			}
			else if(page==="jd"){
				goodsDetailUrl = $element.find("div.p-img").find("a").attr("href");
			}else if(page==="jd_hk"){
				goodsDetailUrl = $element.find("div.p-img").find("a").attr("href");
			}else if(page==="jd_yao"){
				goodsDetailUrl = $element.find("div.jPic").find("a").attr("href");
			}
			
			if(!goodsDetailUrl){
				return;
			}
			if(isDebug) console.log(goodsDetailUrl);
			
			var analysisData = null;
			if(page==="jd" || page==="jd_hk"){
				var jdId = commonFuncObject.getEndHtmlIdByUrl(goodsDetailUrl);
				if(!!jdId) analysisData = {"id":jdId, "platform":"jd"};
			}else{
				analysisData = this.analysisUrl(goodsDetailUrl, "id");
			}
			if(!!analysisData){ //一切数据校验完毕，此处查询是否有券
				let requestUrl = "http://j.jiayoushichang.com/api/ebusiness/coupon/exist/"+analysisData.platform+"?id="+analysisData.id;
				GM_xmlhttpRequest({
					url: requestUrl,
					method: "GET",
					headers: {"Content-Type": "application/x-www-form-urlencoded"},
					onload: function(response) {
						var status = response.status;
						if(status==200||status=='200'){
							var responseText = response.responseText;
							if(!!responseText){
								try{
									var serverResponseJson = JSON.parse(responseText);
									var tip = serverResponseJson.tip;
									if(!!tip){
										$element.append(tip);
									}
								}catch(e){}
							}
						}
					}
				});
			}
			$element.attr("completebox","true");
		};
		
		/**
		 * 开始查询
		 */
		this.startQueryList = function(){
			let selectorElementList = this.pickupSearchElements();
			let that = this;
			setInterval(()=>{
				if(that.intervalIsRunComplete){
					selectorElementList.forEach((elementData)=>{
						if(!!elementData){
							this.createAllElementHtml(elementData);
						}
					});
				}
			}, 400);
		};
	};
	try{
		(new searchPageObject()).startQueryList();
	}catch(e){}
	
	/**
	 * 详情页领券，查券功能
	 */
	function queryCoupon(){
		this.isRun = function(){
			var urls=["detail.tmall.com", "detail.tmall.hk", "item.taobao.com", "chaoshi.detail.tmall.com", "item.jd.com", "item.yiyaojd.com", "npcitem.jd.hk"];
			for(var i=0; i<urls.length;i++){
				if(window.location.host.indexOf(urls[i])!=-1){
					return true;
				}
			}
			return false;
		}
		this.getPlatform = function(){
			let host = window.location.host;
			let platform = "";
			if(host.indexOf("detail.tmall")!=-1){
				platform = "tmall";
			}else if(host.indexOf("item.taobao.com")!=-1){
				platform = "taobao";
			}else if(host.indexOf("item.jd.com")!=-1 || host.indexOf("item.yiyaojd.com")!=-1 || host.indexOf("npcitem.jd.hk")!=-1){
				platform = "jd";
			}
			return platform;
		};
		this.getGoodsData = function(platform){
			var goodsId = "";
			var goodsName = "";
			var href = window.location.href;
			if(platform=="taobao"){
				goodsId =  commonFuncObject.selectParamterQueryUrl(window.location.search, "id");
				goodsName=$(".tb-main-title").text();
	
			}else if(platform=="tmall"){
				goodsId =  commonFuncObject.selectParamterQueryUrl(window.location.search, "id");
				goodsName=$(".tb-detail-hd").text();
	
			}else if(platform=="jd"){
				goodsName=$("div.sku-name").text();
				goodsId = commonFuncObject.getEndHtmlIdByUrl(href);
	
			}
			var data={"goodsId":goodsId, "goodsName":commonFuncObject.filterStr(goodsName)}
			return data;
		};
		this.request = function(mothed, url, param){
			return new Promise(function(resolve, reject){
				GM_xmlhttpRequest({
					url: url,
					method: mothed,
					data:param,
					onload: function(response) {
						var status = response.status;
						var playurl = "";
						if(status==200||status=='200'){
							var responseText = response.responseText;
							resolve({"result":"success", "json":responseText});
						}else{
							reject({"result":"error", "json":null});
						}
					}
				});
			})
		};
		this.createCouponHtml = function(platform, goodsId, goodsName){
			if(!platform || !goodsId) return;
			
			var goodsCouponUrl = "http://j.jiayoushichang.com/api/ebusiness/find?n=7&v=1.0.2&p="+platform+"&i="+goodsId+"&q="+goodsName;
			var goodsPrivateUrl = "http://j.jiayoushichang.com/api/private/change/coupon?script=7&platform="+platform+"&id=";

			this.request("GET", goodsCouponUrl, null).then((resutData)=>{
				if(resutData.result==="success" && !!resutData.json){
					var data = JSON.parse(resutData.json).data;
					if(!data || data==="null" || !data.css || !data.html || !data.handler){
						return;
					}
					var cssText = data.css;
					var htmlText = data.html;
					var handler = data.handler;
					var templateId = data.templateId;
					$("body").prepend("<style>"+cssText+"</style>");
					
					console.log("handler",handler);
					console.log("htmlText",htmlText);

					var handlers = handler.split("@");
					for(var i=0; i<handlers.length; i++){
						var $handler = $(""+handlers[i]+"");
						if(platform=="taobao"){
							$handler.parent().after(htmlText);
						}else if(platform=="tmall"){
							$handler.parent().after(htmlText);
						}else if(platform=="jd"){
							$handler.after(htmlText);
						}
					}
					
					var $llkk = $("#"+templateId);
					if($llkk.length != 0){
						//可点击的情况下执行
						let couponElementA = $llkk.find("a[name='cpShUrl']");
						couponElementA.unbind("click").bind("click", ()=>{
							event.stopPropagation();
							event.preventDefault();
							let couponId = $llkk.data("id");
							if(!!couponId){
								this.request("GET", goodsPrivateUrl+couponId, null).then((resutData2)=>{
									if(resutData2.result==="success" && !!resutData2.json){
										let url = JSON.parse(resutData2.json).url;
										if(!!url) GM_openInTab(url, {active:true});
									}
								});
							}
						});
						
						setInterval(()=>{
							$llkk.find("*").each(function(){
								$(this).removeAttr("data-spm-anchor-id")
							});
						},100);
						
						//canvas画二维码
						var $canvasElement = $("#ca"+templateId);
						if($canvasElement.length != 0){
							let couponId = $llkk.data("id");
							this.request("GET", goodsPrivateUrl+couponId, null).then((resutData2)=>{
								if(resutData2.result==="success" && !!resutData2.json){
									let img = JSON.parse(resutData2.json).img;
									if(!!img){
										var canvasElement = document.getElementById("ca"+templateId);
										var cxt = canvasElement.getContext("2d");
										var imgData = new Image();
										imgData.src = img;
										imgData.onload=function(){
											cxt.drawImage(imgData, 0, 0, imgData.width, imgData.height);
										}
									}
								}
							});
						}
						
					}
				}
			});
		};
		this.start = function(){
			if(this.isRun()){				
				var platform = this.getPlatform();
				if(!!platform){
					var goodsData = this.getGoodsData(platform);
					this.createCouponHtml(platform, goodsData.goodsId, goodsData.goodsName);
				}
			}
		};
	}
	
	try{
		(new queryCoupon()).start();
	}catch(e){}
})(); 
}