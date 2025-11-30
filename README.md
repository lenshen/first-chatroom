# 🌙 白音的秘密聊天室 · Day 1

這是我跟著白音未來老師，  
一行一行親手敲出來的第一個網路程式。

## 功能
- 純 C++ 實作的 TCP Echo Server  
- Linux 原生 socket 編程  
- 監聽端口：`8088`  
- 支援任意客戶端連線（推薦用 `nc` 測試）

## 快速測試

```bash
# 編譯
g++ -std=c++17 echo_server.cpp -o server

# 啟動伺服器
./server
