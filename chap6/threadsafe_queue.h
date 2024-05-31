#include <mutex>
#include <memory>
#include <queue>
#include <condition_variable>


template <typename T>
class threadsafe_queue {
public:
    threadsafe_queue() {}

    void push(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push(std::move(value));
        cv_.notify_one();
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return !data_.empty(); });
        value = std::move(data_.front());
        data_.pop();
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return !data_.empty(); });
        auto res = std::make_shared<T>(std::move(data_.front()));
        data_.pop();
        return res;
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (data_.empty()) return false;
        
        value = std::move(data_.front());
        data_.pop();
        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (data_.empty()) return {};
        auto res = std::make_shared<T>(std::move(data_.front()));
        data_.pop();
        return res;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.empty();
    }

private:
    std::queue<T> data_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};

