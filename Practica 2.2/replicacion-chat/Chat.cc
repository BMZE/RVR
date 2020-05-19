#include "Chat.h"

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data;
    memcpy(tmp, &type, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    nick[nick.size()+1]='\0';
    memcpy(tmp, nick.c_str(), sizeof(char) * 8);
    tmp += sizeof(char) * 8;
    message[message.size()+1]='\0';
    memcpy(tmp, message.c_str(), sizeof(char) * 80);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* tmp = _data;
    memcpy(&type, tmp,sizeof(int8_t));
    tmp += sizeof(int8_t);
    nick.resize(sizeof(char) * 8);
    memcpy(&nick[0], tmp, sizeof(char) * 8);
    tmp += sizeof(char) * 8;
    message.resize(sizeof(char) * 80);
    memcpy(&message[0], tmp, sizeof(char) * 80);
    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        while (true)
        {

            Socket* client;
            ChatMessage msg;
            socket.recv(msg, client);


            switch(msg.type)
            {
                case ChatMessage::LOGIN:
                    clients.push_back(client);
                    std::cout << msg.nick.c_str() << " connected" << std::endl;
                break;
                case  ChatMessage::LOGOUT:
                    for(int i = 0; i < clients.size(); i++)
                    {
                         if(!(*clients[i] == *client))
                          {
                            clients.erase(clients.begin() + i);
                            break;
                          }
                    }
                    std::cout << msg.nick.c_str() << " disconnected" << std::endl;

                break;
                case ChatMessage::MESSAGE:
                    for(int i = 0; i < clients.size(); i++)
                    {
                        if(!(*clients[i] == *client))
                            socket.send(msg, *clients[i]);
                    }
                    std::cout << msg.nick.c_str() << " sent a message" << std::endl;
                break;
                default:
                break;
            }

        }
    }
}

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;
    
    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{

    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket

        std::string msg;
        std::getline(std::cin, msg);

        ChatMessage em (nick, msg);
        em.type = ChatMessage::MESSAGE;
        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"

        ChatMessage em;

        socket.recv(em);

        if(em.nick != nick)
            std::cout << "Nick: " << em.nick << " Message: " << em.message << std::endl;
        
    }
}

