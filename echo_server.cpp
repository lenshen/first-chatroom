#include<iostream>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>//ip 轉換器
#include<sys/socket.h>

using namespace std;

constexpr int PROT = 8088; 
constexpr int BUFFER_SIZE = 1024; // 一次最多聽 1024 字元


int main(){
    // build socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        cerr << "socket 失敗\n";
        return 1;
    }

    //addres location strctrue
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PROT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0){
        cerr << "bind 失敗了 \n";
        return 1;
    }

    if(listen(server_fd, 5) < 0){
        cerr << "listen 失敗 \n";
        return 1;
    }

    cout << "聊天室伺服器已在端口 " << PROT << " 開門了 \n";
   
    //接受客戶端連線
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    if(client_fd < 0){
        cerr << "accept 失敗 \n";
        return 1;
    }

    //顯示誰連進來
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    cout<< "有人從 " << client_ip <<" 溜近來! \n";


    char buffer[BUFFER_SIZE];
    while(true){
        // 清空緩存區
        memset(buffer, 0, BUFFER_SIZE);
        
        //讀取客戶端訊息
        ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        //如果讀取到 0 或是錯誤
        if(bytes_read <=0){
            cout << "對方悄悄離開了 ....\n";
            break;
        }

        //在螢幕顯示你收到甚麼
        cout << "收到: "<<buffer;

        //原封不動回傳(迴音)
        send(client_fd, buffer, bytes_read, 0);

    }

    //關門
    close(client_fd);
    close(server_fd);
    cout << "聊天室到此結束......\n";



    return 0;
}

