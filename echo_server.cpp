#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <vector>
#include <algorithm>

constexpr int PORT = 8088;
constexpr int MAX_CLIENTS = 64;
constexpr int BUFFER_SIZE = 1024;

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { std::cerr << "socket 失敗\n"; return 1; }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "bind 失敗\n"; return 1;
    }
    if (listen(server_fd, 10) < 0) {
        std::cerr << "listen 失敗\n"; return 1;
    }

    std::cout << "多人聊天室已在端口 " << PORT << " 開門了（最多 " << MAX_CLIENTS << " 人）\n";

    std::vector<int> clients;
    fd_set readfds;
    int max_fd = server_fd;
    char buffer[BUFFER_SIZE];

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);

        for (int fd : clients) FD_SET(fd, &readfds);
        for (int fd : clients) if (fd > max_fd) max_fd = fd;

        int activity = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);
        if (activity < 0) { std::cerr << "select 錯誤\n"; break; }

        // 新客戶端連進來
        if (FD_ISSET(server_fd, &readfds)) {
            sockaddr_in client_addr{};
            socklen_t len = sizeof(client_addr);
            int client_fd = accept(server_fd, (sockaddr*)&client_addr, &len);
            if (client_fd >= 0) {
                if (clients.size() >= MAX_CLIENTS) {
                    const char* msg = "聊天室已滿！\n";
                    send(client_fd, msg, strlen(msg), 0);
                    close(client_fd);
                } else {
                    clients.push_back(client_fd);
                    char ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
                    std::cout << "歡迎 " << ip << " 加入！（" << clients.size() << " 人）\n";
                    std::string welcome = "歡迎來到白音的秘密聊天室！\n";
                    send(client_fd, welcome.c_str(), welcome.size(), 0);
                }
            }
        }

        // 檢查誰說話了
        for (auto it = clients.begin(); it != clients.end(); ) {
            int fd = *it;
            if (FD_ISSET(fd, &readfds)) {
                std::memset(buffer, 0, BUFFER_SIZE);
                ssize_t bytes = recv(fd, buffer, BUFFER_SIZE - 1, 0);
                if (bytes <= 0) {
                    close(fd);
                    it = clients.erase(it);
                    std::cout << "有人離開了（剩 " << clients.size() << " 人）\n";
                } else {
                    std::string msg = "[有人說] " + std::string(buffer, bytes);
                    std::cout << msg;
                    for (int target : clients) {
                        send(target, msg.c_str(), msg.size(), 0);
                    }
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }

    for (int fd : clients) close(fd);
    close(server_fd);
    return 0;
}
