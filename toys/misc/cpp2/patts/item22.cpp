#include <iostream>

class Client {
public:
    int m_number;
    int m_result = -1;
    Client(int number) : m_number(number) {}
};

class Handler {
public:
    virtual void add(Handler* handler) = 0;
    virtual Client& apply(Client& client) const = 0;
};

class Handler_Imp : public Handler {
private:
    Handler *m_handler = 0;
    int m_numero;
public:
    Handler_Imp(int numero) : m_numero(numero) {}
    void add(Handler* handler) {
        if (m_handler)
            m_handler->add(handler);
        else
            m_handler = handler;
    }
    Client& apply(Client& client) const {
        if (client.m_number%m_numero == 0) {
            client.m_result = 10 * m_numero;
            return client;
        } else
        if (m_handler) 
            return m_handler->apply(client);
        return client;
    }
};

int main()
{
    Handler_Imp handler2(2);
    Handler_Imp handler3(3);
    Handler_Imp handler5(5);

    handler2.add(&handler3);
    handler2.add(&handler5);

    auto Result = [](const Handler& handler, int num) {
        Client client(num);
        std::cout << "Result for Client(" << num << "): "
            << handler.apply(client).m_result << std::endl;
    };

    Result(handler2, 81);
    Result(handler2, 80);
    Result(handler2, 17);
    Result(handler2, 25);

    return 0;
}

