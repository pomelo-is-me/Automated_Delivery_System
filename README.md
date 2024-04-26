# Automated_Delivery_System

## 簡介：
高二自主學習的主題是「自動化外送系統」，目前完成的部分如下:

### auto_test：
這裡放置自動測試的程式碼，測試時，分別為：(以下程式碼皆分別上傳至ESP32)
#### auto_test_car：
負責控制自走車與控制電梯按鈕裝置。
#### auto_test_ele：
負責接收自走車資料，並控制伺服馬達按電梯按鈕。
#### see_mac：
測試前先上傳至電梯按鈕裝置的ESP32上，取得MAC後記錄於auto_test_car程式碼中。

### web_to_car_ESP32：
負責處理網站資料，並發送資料給自走車。
  
### web_code_formatter_html：
#### formatter：
可以先將原本在 VScode 中編輯的網站代碼透過網路上的 HTML Minifier 壓縮並整理後，丟進formatter進行排版，以符合語法

#### web_code_html：

##### home.html
主頁
##### sel_b.html
選擇樓
##### deli_2.html
選擇班級
##### rec.html
顯示已登記班級
##### reload.html
跳轉業面
##### undone.html
未完成功能或需ESP32處理資料

### 未完成：
自走車接收資料並拿取外送運行指定路線。
