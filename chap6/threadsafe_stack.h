#include <exception>
#include <stack>
#include <mutex>
#include <memory>

struct empty_stack : std::exception {
    [[nodiscard]] const char *what() const noexcept;
};

template <typename T>
class threadsafe_stack {
public:
    threadsafe_stack() {}
    
    threadsafe_stack(const threadsafe_stack& other) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = other.data_;
    }

    threadsafe_stack& operator=(const threadsafe_stack& other) = delete;

    void push(T new_value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push(std::move(new_value));
    }

    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (data_.empty()) {
            throw empty_stack();
        }
        auto res = std::make_shared<T>(std::move(data_.top()));
        data_.pop();
        return res;
    }

    void pop(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (data_.empty()) {
            throw empty_stack();
        }
        value = std::move(data_.top());
        data_.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.empty();
    }

private:
    std::stack<T> data_;
    mutable std::mutex mutex_;
};
