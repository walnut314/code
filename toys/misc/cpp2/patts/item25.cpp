#include <iostream>
#include <list>
#include <string>

class Interface {
public:
    virtual void message_receive(const std::string& message) = 0;
    virtual void message_send(const std::string& message) = 0;
};

class Client : public Interface {
private:
    Interface* m_mediator = 0;
    std::string m_numero;
public:
    Client(Interface* mediator, const std::string& numero)
        : m_mediator(mediator), m_numero(numero) {}
    void message_receive(const std::string& message) {
        std::cout << "Client #" << m_numero << " update : "
            << message << std::endl;
    }
    void message_send(const std::string& message) {
        std::string msg = "fron client ";
        msg += m_numero;
        msg += ": ";
        msg += message;
        m_mediator->message_receive(msg);
    }
};

class Mediator : public Interface {
private:
    std::list<Interface*> m_client;
public:
    Mediator(){}
    void attach(Interface* client) {
        m_client.push_back(client);
    }
    void detach(Interface* client) {
        m_client.remove(client);
    }
    void message_receive(const std::string& message) {
        message_send(message);
    }
    void message_send(const std::string& message) {
        std::string msg = "from mediator: ";
        msg += message;
        for (auto& client : m_client)
            client->message_receive(msg);
    }
};

int main()
{
    Mediator mediator;

    Client* client_1(new Client(&mediator, "1"));
    Client* client_2(new Client(&mediator, "2"));
    Client* client_3(new Client(&mediator, "3"));

    mediator.attach(client_1);
    mediator.attach(client_2);
    mediator.attach(client_3);

    client_1->message_send("tweet1");
    client_3->message_send("tweet3");

    return 0;
}

