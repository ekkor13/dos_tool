#ifndef CORE_ATTACKER_HPP
#define CORE_ATTACKER_HPP

#include "../cli/cli.hpp"
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>


class CoreAttacker {
public:
    CoreAttacker(const TargetInfo& target, int threads_count);

    void start_flood();

private:
    const TargetInfo target_;
    const int threads_count_;
    int raw_socket_fd_ = -1;
    const std::string interface_name_;
    
    std::atomic<long long> packet_count_ = 0;
    std::mutex log_mutex_;
    
    void attack_thread_loop();
    void join_threads(std::vector<std::thread>& threads);
    void bind_to_interface(int socket_fd, const std::string& interface_name); 
};

#endif
