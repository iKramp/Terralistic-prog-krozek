#include <iostream>
#include "serverNetworking.hpp"
#include "print.hpp"
#include "compress.hpp"
#include "graphics.hpp"

void Connection::send(sf::Packet& packet) {
    if(!socket->isBlocking())
        socket->setBlocking(true);
    socket->send(packet);
}

sf::Socket::Status Connection::receive(sf::Packet& packet) {
    if(socket->isBlocking())
        socket->setBlocking(false);
    return socket->receive(packet);
}

bool Connection::hasBeenGreeted() {
    return greeted;
}

void Connection::greet() {
    greeted = true;
}

std::string Connection::getIpAddress() {
    return socket->getRemoteAddress().toString();
}

Connection::~Connection() {
    delete socket;
}

void Connection::send(std::vector<char>& data) {
    if(!socket->isBlocking())
        socket->setBlocking(true);
    
    sf::Packet packet;
    packet << (int)data.size();
    send(packet);
    
    size_t sent;
    int bytes_sent = 0;
    while(bytes_sent < data.size()) {
        socket->send(&data[bytes_sent], (int)data.size() - bytes_sent, sent);
        bytes_sent += sent;
    }
}

void Connection::pushPacket(sf::Packet& packet, ClientPacketType type) {
    packet_buffer.push({packet, type});
}

bool Connection::hasPacketInBuffer() {
    return !packet_buffer.empty();
}

std::pair<sf::Packet, ClientPacketType> Connection::getPacket() {
    auto result = packet_buffer.front();
    packet_buffer.pop();
    return result;
}

void ServerNetworking::init() {
    listener.listen(port);
    listener.setBlocking(false);
}

void ServerNetworking::sendToEveryone(sf::Packet& packet) {
    for(Connection* connection : connections)
        connection->send(packet);
}

void ServerNetworking::update(float frame_length) {
    static sf::TcpSocket *socket = new sf::TcpSocket;
    while(listener.accept(*socket) != sf::Socket::NotReady)
        if(!is_private || socket->getRemoteAddress().toString() == "127.0.0.1") {
            Connection* connection = new Connection(socket);
            connections.push_back(connection);
            socket = new sf::TcpSocket;
        }
    
    sf::Packet packet;
    for(int i = 0; i < connections.size(); i++) {
        if(connections[i]->hasBeenGreeted())
            while(true) {
                sf::Socket::Status status = connections[i]->receive(packet);
                
                if(status == sf::Socket::NotReady)
                    break;
                else if(status == sf::Socket::Disconnected) {
                    removeConnection(connections[i]);
                    
                    break;
                } else if(status == sf::Socket::Done) {
                    unsigned char packet_type;
                    packet >> packet_type;
                    
                    connections[i]->pushPacket(packet, (ClientPacketType)packet_type);
                }
            }
        else if(connections[i]->receive(packet) != sf::Socket::NotReady) {
            sf::Packet time_packet;
            time_packet << WelcomePacketType::TIME << gfx::getTicks();
            connections[i]->send(time_packet);
            
            ServerConnectionWelcomeEvent event(connections[i], packet);
            connection_welcome_event.call(event);
            
            sf::Packet welcome_packet;
            welcome_packet << WelcomePacketType::WELCOME;
            connections[i]->send(welcome_packet);
            connections[i]->greet();
            
            ServerNewConnectionEvent event2(connections[i]);
            new_connection_event.call(event2);
        }
    }
}

void ServerNetworking::stop() {
    if(!is_private)
        for(Connection* connection : connections)
            kickConnection(connection, "Server stopped!");
    
    listener.close();
}

void ServerNetworking::kickConnection(Connection* connection, const std::string& reason) {
    sf::Packet kick_packet;
    kick_packet << ServerPacketType::KICK << reason;
    connection->send(kick_packet);
    
    removeConnection(connection);
}

void ServerNetworking::removeConnection(Connection* connection) {
    ServerDisconnectEvent event(connection);
    disconnect_event.call(event);
    
    print::info(connection->getIpAddress() + " disconnected (" + std::to_string(connections.size()) + " players online)");
    
    delete connection;
    connections.erase(std::find(connections.begin(), connections.end(), connection));
}
