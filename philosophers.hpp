#pragma once

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <memory>
#include <algorithm>
#include <thread>
#include <functional>
#include <random>


class EatTimeEngine
{
private:
    std::mt19937 m_generator;
    std::uniform_int_distribution<int> m_distribution;
    static EatTimeEngine* instanec_;
    static std::mutex mutex_;
protected:
    EatTimeEngine(std::random_device::result_type seed) : m_generator(seed), m_distribution(1, 100) {}
    ~EatTimeEngine();
public:
    int pickTime() {
            std::lock_guard<std::mutex> lock(mutex_);
            return m_distribution(m_generator);
    }
    static EatTimeEngine* GetInstance();
    EatTimeEngine ( const EatTimeEngine &other) = delete;
    EatTimeEngine& operator=(const EatTimeEngine &other) = delete;
}

enum class Status {
    Eating, 
    Thinking,
    Done
};

class Table;
class Fork;

class Philosopher {
private:
    const int id_;
    const int quota = 6;
    EatTimeEngine* loterry_;
    std::thread m_t;
    mutable std::mutex mtx_;
    Status status_;
    Table* p_table;
    std::shared_ptr<Fork> right_;
    std::shared_ptr<Fork> left_;

public:
    Philosopher(int id, Table* t, std::shared_ptr<Fork>left, std::shared_ptr<Fork>rigth)
        : id_(id), status_(Status::Thinking), p_table(t), left_(left), right_(right) {
            loterry_ = EatTimeEngine::GetInstance();
        }
    ~Philosopher() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (m_t.joinable()) {
            m_t.join();
        }
    }
    Philosopher(const Philosopher&) = delete;
    Philosopher& operator= (const Philosopher& other) = delete;

    Philosopher(Philosopher&&) = default;
    Philosopher& operator=(Philosopher&&) = default;

    std::string readStatus();
    Status getStatus();
    void threadStartTheFeast();

    void setThread(std::thread &&t) {
        m_t = std::move(t);
    }

    void endThread() {
        std::lock_guard<std::mutex> (lock(mtx_));
        if (m_t.joinable()) {
            m_t.join();
        }
    }

    const int getId() const { return id_; }
};