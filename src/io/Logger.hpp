#pragma once

#include <chrono>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace io {

enum class LogLevel : int {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Critical = 5,
    Off = 6
};

struct LoggerConfig {
    std::string name = "root";
    LogLevel level = LogLevel::Info;
    std::string pattern = "%Y-%m-%d %H:%M:%S [%l] %n (%t) %g:%# %f - %v";
    bool use_utc = false;
    bool use_color = true;
};

class Sink {
   public:
    virtual ~Sink() = default;
    virtual void log(std::string_view line, LogLevel lvl, bool colorize) = 0;
};

class ConsoleSink final : public Sink {
   public:
    // If use_stderr_for_warn = true, Warn+ goes to stderr, others to stdout.
    explicit ConsoleSink(bool use_stderr_for_warn = true) noexcept
        : use_stderr_for_warn_(use_stderr_for_warn) {}
    void log(std::string_view line, LogLevel lvl, bool colorize) override;

   private:
    bool use_stderr_for_warn_;
};

class FileSink final : public Sink {
   public:
    // Simple size-based rotation. If rotate_bytes == 0, rotation disabled.
    FileSink(std::filesystem::path file_path, std::size_t rotate_bytes = 0, int max_files = 3);

    void log(std::string_view line, LogLevel lvl, bool /*colorize*/) override;

    const std::filesystem::path& path() const noexcept { return file_path_; }

   private:
    void roll_if_needed(std::size_t add_bytes);
    void rotate_files();

    std::filesystem::path file_path_;
    std::ofstream out_;
    const std::size_t rotate_bytes_;
    const int max_files_;
    std::size_t current_size_ = 0;
};

// Options to customize child logger creation.
struct ChildOptions {
    std::optional<LogLevel> level;
    std::optional<std::string> pattern;
    std::optional<bool> use_utc;
    std::optional<bool> use_color;
    // If provided, a dedicated FileSink is added for the child.
    std::optional<std::filesystem::path> file_path;
    // If true, child starts with no sinks and only uses what options add.
    bool clear_inherited_sinks = false;
    // Optional rotation for child file sink
    std::size_t rotate_bytes = 0;
    int rotate_max_files = 3;
};

class Logger : public std::enable_shared_from_this<Logger> {
   public:
    explicit Logger(LoggerConfig cfg);

    // Attach sinks
    void add_sink(std::shared_ptr<Sink> sink);
    // Replace all sinks
    void set_sinks(std::vector<std::shared_ptr<Sink>> sinks);

    // Create a child logger: inherits sinks and config, applies overrides.
    std::shared_ptr<Logger> create_child(std::string name_suffix, const ChildOptions& opts = {});

    // Level and config
    void set_level(LogLevel lvl) noexcept { cfg_.level = lvl; }
    LogLevel level() const noexcept { return cfg_.level; }
    const std::string& name() const noexcept { return cfg_.name; }

    // Generic log API
    void log(LogLevel lvl, std::string_view message, std::string_view file = {}, int line = 0,
             std::string_view func = {});

    // Global singleton helpers (compatible with previous usage)
    static std::shared_ptr<Logger> instance();
    static void set_instance(std::shared_ptr<Logger> logger);

    // Static convenience (no file/line/function info)
    template <typename... Args>
    static void trace(Args&&... a) {
        instance()->log(LogLevel::Trace, concat(std::forward<Args>(a)...));
    }
    template <typename... Args>
    static void debug(Args&&... a) {
        instance()->log(LogLevel::Debug, concat(std::forward<Args>(a)...));
    }
    template <typename... Args>
    static void info(Args&&... a) {
        instance()->log(LogLevel::Info, concat(std::forward<Args>(a)...));
    }
    template <typename... Args>
    static void warn(Args&&... a) {
        instance()->log(LogLevel::Warn, concat(std::forward<Args>(a)...));
    }
    template <typename... Args>
    static void error(Args&&... a) {
        instance()->log(LogLevel::Error, concat(std::forward<Args>(a)...));
    }
    template <typename... Args>
    static void critical(Args&&... a) {
        instance()->log(LogLevel::Critical, concat(std::forward<Args>(a)...));
    }

    // Helper to stringify multiple arguments using iostreams.
    template <typename... Args>
    static std::string concat(Args&&... args) {
        std::ostringstream oss;
        (oss << ... << std::forward<Args>(args));
        return oss.str();
    }

   private:
    std::string format_line(LogLevel lvl, std::string_view message, std::string_view file, int line,
                            std::string_view func) const;

    static std::string level_to_string(LogLevel lvl);
    static std::string thread_id_string();
    std::tm localtime(std::time_t t) const;

   private:
    LoggerConfig cfg_;
    std::vector<std::shared_ptr<Sink>> sinks_;
    mutable std::mutex mtx_;
};

// Helpful macros capturing file/line/function.
#define IO_LOG_TRACE(logger, ...)                                                               \
    (logger)->log(::io::LogLevel::Trace, ::io::Logger::concat(__VA_ARGS__), __FILE__, __LINE__, \
                  __func__)
#define IO_LOG_DEBUG(logger, ...)                                                               \
    (logger)->log(::io::LogLevel::Debug, ::io::Logger::concat(__VA_ARGS__), __FILE__, __LINE__, \
                  __func__)
#define IO_LOG_INFO(logger, ...)                                                               \
    (logger)->log(::io::LogLevel::Info, ::io::Logger::concat(__VA_ARGS__), __FILE__, __LINE__, \
                  __func__)
#define IO_LOG_WARN(logger, ...)                                                               \
    (logger)->log(::io::LogLevel::Warn, ::io::Logger::concat(__VA_ARGS__), __FILE__, __LINE__, \
                  __func__)
#define IO_LOG_ERROR(logger, ...)                                                               \
    (logger)->log(::io::LogLevel::Error, ::io::Logger::concat(__VA_ARGS__), __FILE__, __LINE__, \
                  __func__)
#define IO_LOG_CRIT(logger, ...)                                                                   \
    (logger)->log(::io::LogLevel::Critical, ::io::Logger::concat(__VA_ARGS__), __FILE__, __LINE__, \
                  __func__)

}  // namespace io
