#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <random>
#include <chrono>
#include <condition_variable>
#include <atomic>

// Объявление глобальных переменных для синхронизации и обмена данными между потоками
std::mutex mtx;                                                                 // Мьютекс для защиты критических секций кода
std::condition_variable cv;                                                     // Условная переменная для сигнализации ожидающим потокам
std::vector<int> counters;                                                      // Вектор для счетчиков, подсчитывающих обработанные сообщения
std::queue<std::pair<int, std::chrono::steady_clock::time_point>> messages;     // Очередь для сообщений
std::atomic<bool> done{false};                                                  // Атомарная переменная для сигнала о завершении обработки
std::atomic<int> messageIdGenerator{0};                                         // Атомарная переменная для генерации уникальных номеров сообщений

// Функция обработки сообщений каждым обработчиком
void processMessages(int handlerId, int minProcessingTime, int maxProcessingTime) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> processingTime(minProcessingTime, maxProcessingTime);
    std::uniform_int_distribution<> counterDist(0, counters.size() - 1);

    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        // Пока не появится сообщение или не завершится работа
        cv.wait(lock, [] { return !messages.empty() || done.load(); });

        // Если очередь пуста и работа завершена, выходим из функции
        if (messages.empty() && done.load())
            return;

        // Если в очереди есть сообщение, извлекаем его
        if (!messages.empty()) {
            auto message = messages.front();
            messages.pop();
            lock.unlock();

            // Имитация времени обработки сообщения
            std::this_thread::sleep_for(std::chrono::milliseconds(processingTime(gen)));

            // Случайный выбор счетчика для увеличения
            int counterId = counterDist(gen);
            lock.lock();
            counters[counterId]++;
            lock.unlock();

            // Вывод информации о выполненной обработке
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - message.second).count();

            std::cout << "Message " << message.first << " processed by handler " << handlerId << " for "
                      << duration << " ms. " << "Counter " << counterId << " increased\n";
        }
    }
}

int main() {
    system("chcp 65001"); // Установка кодировки для вывода русских символов в консоли

    int n, m, k;
    int minMessageInterval, maxMessageInterval;
    int minProcessingTime, maxProcessingTime;

    std::cout << "=== PARAMETERS ===" << std::endl;

    std::cout << "Enter the number of input channels (n): ";
    std::cin >> n;
    std::cout << "Enter the number of handlers (m): ";
    std::cin >> m;
    std::cout << "Enter the number of counters (k): ";
    std::cin >> k;
    std::cout << "Enter the minimum message interval (ms): ";
    std::cin >> minMessageInterval;
    std::cout << "Enter the maximum message interval (ms): ";
    std::cin >> maxMessageInterval;
    std::cout << "Enter the minimum message processing time (ms): ";
    std::cin >> minProcessingTime;
    std::cout << "Enter the maximum message processing time (ms): ";
    std::cin >> maxProcessingTime;

    // Инициализация вектора счетчиков
    counters.resize(k);

    // Генерация случайных интервалов для появления сообщений
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> messageInterval(minMessageInterval, maxMessageInterval);

    // Создание и запуск потока-производителя сообщений
    std::thread producer([&]() {
        for (int i = 0; i < n; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(messageInterval(gen)));

            int messageId = messageIdGenerator++;

            {
                std::lock_guard<std::mutex> lock(mtx);
                messages.emplace(messageId, std::chrono::steady_clock::now());
            }

            cv.notify_one();
        }

        // Пометка о завершении работы
        done = true;
        cv.notify_all();
    });

    // Создание и запуск потоков-обработчиков
    std::vector<std::thread> handlers;
    handlers.reserve(m);

    std::cout << std::endl << "=== CALCULATIONS ===" << std::endl;

    for (int i = 0; i < m; ++i)
        handlers.emplace_back(processMessages, i, minProcessingTime, maxProcessingTime);

    // Ожидание завершения работы потока-производителя и потоков-обработчиков
    producer.join();

    for (auto& handler : handlers)
        handler.join();

    std::cout << std::endl << "=== RESULTS ===" << std::endl;

    int checksum = 0;

    for (int i = 0; i < k; ++i) {
        std::cout << "Counter " << i << ": " << counters[i] << std::endl;

        checksum += counters[i];
    }

    std::cout << "Checksum: " << checksum << "/" << n << std::endl;

    return 0;
}
