#include "Chat.h"

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data + sizeof(int32_t);
    memcpy(tmp, &type, sizeof(int16_t));
    tmp += sizeof(int16_t);
    memcpy(tmp, (void*)nick, 8);
    tmp += 8;
    memcpy(tmp, (void*)message, 80);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data

    memcpy(&type, tmp,sizeof(int16_t));
    tmp += sizeof(int16_t);
    char* tmp = bobj + sizeof(int32_t);
    memcpy(nick,tmp,8);
    tmp += 8;
    memcpy(message,tmp, 80);
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
            ChatMessage cm(nick, msg);

            switch(cm.type)
            {
                case 0:
                    clients.push_back(cm);
                break;
                case 2:
                    clients.pop_back();
                break;
                case 1:
                    socket.send(cm, socket);
                break;
                default:
                break;
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

}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

