#include "Chat.h"

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data + sizeof(int32_t);
    memcpy(tmp, &type, sizeof(int16_t));
    tmp += sizeof(int16_t);
    memcpy(tmp, nick.c_str(), 8);
    tmp += 8;
    memcpy(tmp, message.c_str(), 80);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* tmp = _data + sizeof(int32_t);
    memcpy(&type, tmp,sizeof(int16_t));
    tmp += sizeof(int16_t);
    tmp = bobj + sizeof(int32_t);
    memcpy(&nick, tmp, 8);
    tmp += 8;
    memcpy(&message, tmp, 80);
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

