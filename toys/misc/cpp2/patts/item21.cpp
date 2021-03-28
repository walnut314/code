#include <iostream>
#include <memory>
#include <deque>

class Command {
public:
    virtual void execute() const = 0;
};

class Command1 : public Command {
public:
    virtual void execute() const {
        std::cout << "command 1" << std::endl;
    }
};

class Command2 : public Command {
public:
    virtual void execute() const {
        std::cout << "command 2" << std::endl;
    }
};

class Script {
private:
    std::deque<std::shared_ptr<Command>> m_commands;
public:
    void push_back(const std::shared_ptr<Command>& command) {
        m_commands.push_back(command);
    }
    void execute() {
        while (!m_commands.empty()) {
            m_commands.front()->execute();
            m_commands.pop_front();
        }
    }
};

int main()
{
    Script script;
    script.push_back(std::make_shared<Command1>());
    script.push_back(std::make_shared<Command2>());
    script.push_back(std::make_shared<Command2>());
    script.push_back(std::make_shared<Command2>());

    std::cout << "script 'execute()':" << std::endl;
    script.execute();
    return 0;
}

