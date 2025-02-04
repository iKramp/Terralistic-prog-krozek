#include <cstring>
#include "networking.hpp"
#include "exception.hpp"
#include "graphics.hpp"

#ifdef WIN32
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#endif

void _socketDisableBlocking(int socket_handle) {
#ifdef _WIN32
    unsigned long mode = 1;
    if(ioctlsocket(socket_handle, FIONBIO, &mode) != 0) throw SocketError("Could not set socket to blocking");
#else
    int status = fcntl(socket_handle, F_GETFL);
    if(fcntl(socket_handle, F_SETFL, status | O_NONBLOCK) == -1) throw SocketError("Failed to set file status flags: " + std::to_string(errno));
#endif
}

#ifdef WIN32
bool TcpSocket::handleError() {
    switch (WSAGetLastError()) {
        case WSAEWOULDBLOCK: return true;
        case WSAEISCONN:
        case WSAEALREADY:
        case WSAECONNREFUSED:
        case WSAECONNABORTED:
        case WSAECONNRESET:
        case WSAETIMEDOUT:
        case WSAENETRESET:
        case WSAENOTCONN: disconnect(); return false;
        default: {
            char message[256] = {0};
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, WSAGetLastError(), 0, message, 256, 0);
            throw SocketError("Socket error: " + (std::string)message);
        }
    }
}
#else
bool TcpSocket::handleError() {
    switch(errno) {
        case EWOULDBLOCK:
            return true;
        case EINPROGRESS:
        case ECONNREFUSED:
        case ECONNABORTED:
        case ECONNRESET:
        case ETIMEDOUT:
        case ENETRESET:
        case ENOTCONN:
        case EPIPE:
            disconnect();
            return false;
        default:
            throw SocketError("Socket error: " + (std::string)strerror(errno));
    }
}
#endif

void TcpSocket::send(const void* obj, unsigned int size) {
    unsigned int sent = 0;
    while(sent < size) {
#ifdef _WIN32
        int flags = 0;
#else
        int flags = MSG_NOSIGNAL;
#endif

        int curr_sent = (int)::send(socket_handle, (const char*)((unsigned long)(intptr_t)obj + sent), size - sent, flags);

        if(curr_sent != -1)
            sent += curr_sent;
        
        if(curr_sent < 0 && !handleError()) break;
    }
}

void TcpSocket::send(Packet& packet) {
    if(!connected)
        throw NotConnectedError("Not connected!");
    
    unsigned int size = (unsigned int)packet.getDataSize();
    unsigned int prev_buffer_size = (unsigned int)packet_buffer_out.size();
    packet_buffer_out.resize(packet_buffer_out.size() + sizeof(unsigned int) + size);
    std::memcpy(&packet_buffer_out[prev_buffer_size], &size, sizeof(unsigned int));
    std::memcpy(&packet_buffer_out[prev_buffer_size + sizeof(unsigned int)], packet.getData(), size);

    if(packet_buffer_out.size() > 65536)
        flushPacketBuffer();
}

bool TcpSocket::receive(void* obj, unsigned int size, bool expect_data) {
    if(!connected)
        throw NotConnectedError("Not connected!");
    
    unsigned int received = 0;
    while(received < size) {
#ifdef WIN32
        int curr_received = (int)::recv(socket_handle, (char*)((unsigned long)(intptr_t)obj + received), size - received, 0);
#else
        int curr_received = (int)::recv(socket_handle, (char*)((unsigned long)obj + received), size - received, MSG_DONTWAIT);
#endif
        if(curr_received != -1)
            received += curr_received;
        
        
        if(curr_received == 0) {
            connected = false;
            return false;
        } else if(curr_received < 0) {
            if(!handleError() || !expect_data)
                return false;
            else
                gfx::sleep(0.1);
        }
    }
    
    return true;
}

bool TcpSocket::receive(Packet& packet) {
    while(receivePacket());
    
    if(packet_buffer_in.empty())
        return false;
    
    packet = packet_buffer_in.front();
    packet_buffer_in.pop();
    
    return true;
}

bool TcpSocket::receivePacket() {
    unsigned int size;
    if(!receive(&size, sizeof(unsigned int), false)) return false;
    
    std::vector<unsigned char> obj(size);
    if(!receive(&obj[0], size, true)) return false;
    
    Packet packet;
    packet.append(&obj[0], size);
    
    packet_buffer_in.push(packet);
    
    return true;
}

bool TcpSocket::connect(const std::string& ip, unsigned short port) {
    if(connected)
        throw AlreadyConnectedError("Already connected!");

    create(socket(AF_INET, SOCK_STREAM, 0), ip);
 
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) == 0)
        throw AddressFormatError(ip + " is not a valid formatted address");
 
    if(::connect(socket_handle, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        if(!handleError())
            return false;
    
    _socketDisableBlocking(socket_handle);
    
    return true;
}

const std::string& TcpSocket::getIpAddress() const {
    if(!connected)
        throw NotConnectedError("Not connected!");
    return ip_address;
}

void TcpSocket::disconnect() {
    if(!connected)
        throw NotConnectedError("Not connected!");
    
#ifdef WIN32
    closesocket(socket_handle);
#else
    close(socket_handle);
#endif
    connected = false;
    ip_address.clear();
}

bool TcpSocket::isConnected() const {
    return connected;
}

void TcpSocket::flushPacketBuffer() {
    if(!connected)
        throw NotConnectedError("Not connected!");
    
    if(!packet_buffer_out.empty()) {
        send(packet_buffer_out.data(), (unsigned int)packet_buffer_out.size());
        packet_buffer_out.clear();
    }
}

void TcpSocket::create(int handle, const std::string& address) {
    if(connected)
        throw AlreadyConnectedError("Already connected!");
    
    socket_handle = handle;
    ip_address = address;

    int opt = 1;
    if(setsockopt(socket_handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt))) throw SocketError("Setsockopt failed");
    
    if(setsockopt(socket_handle, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt))) throw SocketError("Setsockopt failed");
    
    connected = true;
}

#ifdef WIN32
struct SocketInitializer {
    SocketInitializer() {
        WSADATA init;
        WSAStartup(MAKEWORD(2, 2), &init);
    }

    ~SocketInitializer() {
        WSACleanup();
    }
};

SocketInitializer socket_initializer;
#endif
