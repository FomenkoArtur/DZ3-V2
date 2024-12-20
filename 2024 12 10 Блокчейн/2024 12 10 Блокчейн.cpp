#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <ctime>

using namespace std;

class Block {
public:
    int index;
    string previousHash;
    string data;
    string hash;
    struct tm timestamp;
    int nonce;
    int difficulty;

    Block(int idx, const string& d, const string& prevHash, int diff) : index(idx), data(d), previousHash(prevHash), difficulty(diff), nonce(0) {
        time_t now = time(nullptr);
        localtime_s(&timestamp, &now);
        hash = calculateHash();
        mineBlock();
    }

    string calculateHash() const {
        string toHash = to_string(index) + previousHash + data +
            to_string(mktime(const_cast<tm*>(&timestamp))) + to_string(nonce);
        return sha256(toHash);
    }

    void mineBlock() {
        string target(difficulty, '0');
        do {
            hash = calculateHash();
            nonce++;
        } while (hash.substr(0, difficulty) != target);
        cout << "Майнинг блока #" << index << " завершен. Нонс: " << nonce << "\n";
    }

    static string sha256(const string& input) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);
        std::ostringstream oss;
        for (unsigned char c : hash) {
            oss << hex << std::setw(2) << setfill('0') << static_cast<int>(c);
        }
        return oss.str();
    }
};

class Blockchain {
private:
    std::vector<Block> chain;
    int difficulty;

    Block createGenesisBlock() {
        return Block(0, "Genesis Block", "0", difficulty);
    }

public:
    Blockchain(int diff) : difficulty(diff) {
        if (difficulty <= 0) {
            throw invalid_argument("Difficulty must be greater than 0");
        }
        chain.push_back(createGenesisBlock());
    }

    void addBlock(const string& data) {
        const Block& lastBlock = chain.back();
        chain.emplace_back(chain.size(), data, lastBlock.hash, difficulty);
        cout << "Блок успешно добавлен.\n";
        cout << "Хеш: " << chain.back().hash << "\n";
    }

    bool isChainValid() const {
        if (chain.size() < 2) {
            return true;
        }

        for (size_t i = 1; i < chain.size(); ++i) {
            const Block& current = chain[i];
            const Block& previous = chain[i - 1];
            if (current.hash != current.calculateHash() || current.previousHash != previous.hash) {
                return false;
            }
        }
        return true;
    }

    void printChain() const {
        for (const Block& block : chain) {
            cout << "Блок " << block.index << ":\n";
            cout << "Дата: " << std::put_time(&block.timestamp, "%Y-%m-%d %H:%M:%S") << "\n";
            cout << "Данные: " << block.data << "\n";
            cout << "Хеш: " << block.hash << "\n";
            cout << "Предыдущий хеш: " << block.previousHash << "\n";
            cout << "Нонс: " << block.nonce << "\n";
            cout << "-----------------------\n";
        }
    }
};

int main() {
    Blockchain myBlockchain(4);
    setlocale(LC_ALL, "ru");
    while (true) {
        cout << "Выберите действие:\n";
        cout << "1. Добавить блок\n";
        cout << "2. Проверить цепочку\n";
        cout << "3. Вывести цепочку\n";
        cout << "4. Выйти\n";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            string data;
            cout << "Введите данные транзакции: ";
            cin.ignore();
            getline(cin, data);
            myBlockchain.addBlock(data);
            break;
        }
        case 2:
            if (myBlockchain.isChainValid()) {
                cout << "Цепочка блоков корректна!\n";
            }
            else {
                cout << "Цепочка блоков повреждена!\n";
            }
            break;
        case 3:
            myBlockchain.printChain();
            break;
        case 4:
            cout << "Выход из программы...\n";
            return 0;
        default:
            cout << "Некорректный выбор. Попробуйте снова.\n";
        }
    }
    return 0;
}