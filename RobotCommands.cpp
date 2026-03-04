#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <cctype>

class AEngine {
public:
    virtual ~AEngine() = default;
    virtual void forward(int time_ms) = 0;
    virtual void right(int time_ms) = 0;
    virtual void left(int time_ms) = 0;
    virtual void stop() = 0;
};

class FooEngine : public AEngine {
private:
    static void wait_ms(int time_ms) {
        if (time_ms < 0) time_ms = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
    }

public:
    void forward(int time_ms) override {
        wait_ms(time_ms);
        std::cout << "[Engine] forward for " << time_ms << " ms\n";
    }

    void right(int time_ms) override {
        wait_ms(time_ms);
        std::cout << "[Engine] right for " << time_ms << " ms\n";
    }

    void left(int time_ms) override {
        wait_ms(time_ms);
        std::cout << "[Engine] left for " << time_ms << " ms\n";
    }

    void stop() override {
        std::cout << "[Engine] stop\n";
    }
};

class ACmdReceiver {
public:
    virtual ~ACmdReceiver() = default;
    virtual std::string receive() = 0;
};

class FooCmdReceiver : public ACmdReceiver {
public:
    std::string receive() override {
        std::cout << "Enter command (forward N / right N / left N / stop / exit): ";
        std::string line;
        std::getline(std::cin, line);
        return line;
    }
};

class ControlSystem {
public:
    ControlSystem(ACmdReceiver& receiver, AEngine& engine)
        : receiver_(receiver), engine_(engine) {
    }

    void run() {
        while (true) {
            std::string msg = receiver_.receive();
            trim(msg);

            if (msg.empty()) continue;

            if (toLowerCopy(msg) == "exit") {
                std::cout << "Bye!\n";
                break;
            }

            if (!parseAndExecute(msg)) {
                std::cout << "Unknown/invalid command. Examples: forward 1000, right 500, left 700, stop\n";
            }
        }
    }

private:
    ACmdReceiver& receiver_;
    AEngine& engine_;

    static void trim(std::string& s) {
        auto notSpace = [](unsigned char c) { return !std::isspace(c); };
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
        s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    }

    static std::string toLowerCopy(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
        return s;
    }

    bool parseAndExecute(std::string msg) {
        std::istringstream iss(msg);
        std::string cmd;
        iss >> cmd;
        cmd = toLowerCopy(cmd);

        if (cmd == "stop") {
            engine_.stop();
            return true;
        }

        int time_ms = 0;
        if (!(iss >> time_ms)) return false;

        if (cmd == "forward") { engine_.forward(time_ms); engine_.stop(); return true; }
        if (cmd == "right") { engine_.right(time_ms);   engine_.stop(); return true; }
        if (cmd == "left") { engine_.left(time_ms);    engine_.stop(); return true; }

        return false;
    }
};

int main() {
    FooEngine engine;
    FooCmdReceiver receiver;
    ControlSystem system(receiver, engine);
    system.run();
    return 0;
}
