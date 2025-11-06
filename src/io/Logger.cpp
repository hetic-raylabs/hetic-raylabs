#include "io/Logger.hpp"

#include <atomic>
#include <cstdio>
#include <iostream>
#include <system_error>
#include <thread>

namespace io {

// ------------------------ ConsoleSink ----------------------------------------

static const char* ansi_color(LogLevel lvl) {
    switch (lvl) {
        case LogLevel::Trace:
            return "\033[90m";  // bright black
        case LogLevel::Debug:
            return "\033[36m";  // cyan
        case LogLevel::Info:
            return "\033[32m";  // green
        case LogLevel::Warn:
            return "\033[33m";  // yellow
        case LogLevel::Error:
            return "\033[31m";  // red
        case LogLevel::Critical:
            return "\033[41m";  // red bg
        default:
            return "";
    }
}
static constexpr const char* ansi_reset() {
    return "\033[0m";
}

void ConsoleSink::log(std::string_view line, LogLevel lvl, bool colorize) {
    FILE* stream = stdout;
    if (use_stderr_for_warn_ && static_cast<int>(lvl) >= static_cast<int>(LogLevel::Warn)) {
        stream = stderr;
    }
    if (colorize) {
        std::fputs(ansi_color(lvl), stream);
        std::fwrite(line.data(), 1, line.size(), stream);
        std::fputs(ansi_reset(), stream);
        std::fputc('\n', stream);
    } else {
        std::fwrite(line.data(), 1, line.size(), stream);
        std::fputc('\n', stream);
    }
    std::fflush(stream);
}

// ------------------------ FileSink -------------------------------------------

FileSink::FileSink(std::filesystem::path file_path, std::size_t rotate_bytes, int max_files)
    : file_path_(std::move(file_path)), rotate_bytes_(rotate_bytes), max_files_(max_files) {
    std::filesystem::create_directories(file_path_.parent_path());
    out_.open(file_path_, std::ios::out | std::ios::app | std::ios::binary);
    if (!out_) {
        throw std::runtime_error("FileSink: cannot open file: " + file_path_.string());
    }
    current_size_ = static_cast<std::size_t>(
        std::filesystem::exists(file_path_) ? std::filesystem::file_size(file_path_) : 0);
}

void FileSink::log(std::string_view line, LogLevel, bool) {
    const std::size_t add = line.size() + 1;
    if (rotate_bytes_ != 0) {
        roll_if_needed(add);
    }
    out_.write(line.data(), static_cast<std::streamsize>(line.size()));
    out_.put('\n');
    out_.flush();
    current_size_ += add;
}

void FileSink::roll_if_needed(std::size_t add_bytes) {
    if (rotate_bytes_ == 0)
        return;
    if (current_size_ + add_bytes <= rotate_bytes_)
        return;
    // Rotate
    out_.close();
    rotate_files();
    out_.open(file_path_, std::ios::out | std::ios::trunc | std::ios::binary);
    current_size_ = 0;
}

void FileSink::rotate_files() {
    if (max_files_ <= 0)
        return;
    // Remove oldest
    const auto oldest = file_path_.string() + "." + std::to_string(max_files_);
    std::error_code ec;
    std::filesystem::remove(oldest, ec);

    // Shift others
    for (int i = max_files_ - 1; i >= 1; --i) {
        const auto src = file_path_.string() + "." + std::to_string(i);
        const auto dst = file_path_.string() + "." + std::to_string(i + 1);
        if (std::filesystem::exists(src)) {
            std::filesystem::rename(src, dst, ec);
        }
    }
    // Current -> .1
    if (std::filesystem::exists(file_path_)) {
        std::filesystem::rename(file_path_, file_path_.string() + ".1", ec);
    }
}

// ------------------------ Logger ---------------------------------------------

static std::shared_ptr<Logger>& global_logger_singleton() {
    static std::shared_ptr<Logger> g;
    return g;
}

Logger::Logger(LoggerConfig cfg) : cfg_(std::move(cfg)) {}

void Logger::add_sink(std::shared_ptr<Sink> sink) {
    std::scoped_lock lock(mtx_);
    sinks_.push_back(std::move(sink));
}

void Logger::set_sinks(std::vector<std::shared_ptr<Sink>> sinks) {
    std::scoped_lock lock(mtx_);
    sinks_ = std::move(sinks);
}

std::shared_ptr<Logger> Logger::create_child(std::string name_suffix, const ChildOptions& opts) {
    LoggerConfig child_cfg = cfg_;
    child_cfg.name = cfg_.name.empty() ? std::move(name_suffix) : (cfg_.name + "." + name_suffix);
    if (opts.level)
        child_cfg.level = *opts.level;
    if (opts.pattern)
        child_cfg.pattern = *opts.pattern;
    if (opts.use_utc)
        child_cfg.use_utc = *opts.use_utc;
    if (opts.use_color)
        child_cfg.use_color = *opts.use_color;

    auto child = std::make_shared<Logger>(child_cfg);

    if (!opts.clear_inherited_sinks) {
        std::scoped_lock lock(mtx_);
        child->sinks_ = sinks_;
    }
    if (opts.file_path) {
        child->add_sink(
            std::make_shared<FileSink>(*opts.file_path, opts.rotate_bytes, opts.rotate_max_files));
    }
    return child;
}

void Logger::log(LogLevel lvl, std::string_view message, std::string_view file, int line,
                 std::string_view func) {
    if (static_cast<int>(lvl) < static_cast<int>(cfg_.level) || cfg_.level == LogLevel::Off) {
        return;
    }
    const auto line_str = format_line(lvl, message, file, line, func);

    std::scoped_lock lock(mtx_);
    if (sinks_.empty()) {
        // Fallback to stdout
        ConsoleSink fallback;
        fallback.log(line_str, lvl, cfg_.use_color);
        return;
    }
    for (auto& s : sinks_) {
        s->log(line_str, lvl, cfg_.use_color);
    }
}

std::shared_ptr<Logger> Logger::instance() {
    auto& g = global_logger_singleton();
    if (!g) {
        LoggerConfig cfg;
        cfg.name = "root";
        cfg.level = LogLevel::Info;
        g = std::make_shared<Logger>(cfg);
        g->add_sink(std::make_shared<ConsoleSink>(true));
    }
    return g;
}

void Logger::set_instance(std::shared_ptr<Logger> logger) {
    global_logger_singleton() = std::move(logger);
}

std::string Logger::format_line(LogLevel lvl, std::string_view message, std::string_view file,
                                int line, std::string_view func) const {
    // Time
    const auto now = std::chrono::system_clock::now();
    const auto now_c = std::chrono::system_clock::to_time_t(now);
    const auto tm = localtime(now_c);

    // Buffer date/time first for %Y-%m-%d %H:%M:%S
    std::ostringstream dt;
    dt << std::setfill('0') << std::setw(4) << (tm.tm_year + 1900) << '-' << std::setw(2)
       << (tm.tm_mon + 1) << '-' << std::setw(2) << tm.tm_mday << ' ' << std::setw(2) << tm.tm_hour
       << ':' << std::setw(2) << tm.tm_min << ':' << std::setw(2) << tm.tm_sec;

    // Pattern replacement (simple, linear scan)
    std::string out;
    out.reserve(cfg_.pattern.size() + message.size() + 64);
    for (std::size_t i = 0; i < cfg_.pattern.size(); ++i) {
        if (cfg_.pattern[i] == '%' && i + 1 < cfg_.pattern.size()) {
            const char c = cfg_.pattern[i + 1];
            switch (c) {
                case 'Y':
                    out += dt.str().substr(0, 4);
                    break;
                case 'm':
                    out += dt.str().substr(5, 2);
                    break;
                case 'd':
                    out += dt.str().substr(8, 2);
                    break;
                case 'H':
                    out += dt.str().substr(11, 2);
                    break;
                case 'M':
                    out += dt.str().substr(14, 2);
                    break;
                case 'S':
                    out += dt.str().substr(17, 2);
                    break;
                case 'l':
                    out += level_to_string(lvl);
                    break;
                case 'n':
                    out += cfg_.name;
                    break;
                case 't':
                    out += thread_id_string();
                    break;
                case 'g':
                    out += std::string(file);
                    break;
                case '#':
                    out += (line ? std::to_string(line) : "0");
                    break;
                case 'f':
                    out += std::string(func);
                    break;
                case 'v':
                    out += std::string(message);
                    break;
                case '%':
                    out.push_back('%');
                    break;
                default:
                    out.push_back('%');
                    out.push_back(c);
                    break;
            }
            ++i;  // skip specifier
        } else {
            out.push_back(cfg_.pattern[i]);
        }
    }
    return out;
}

std::string Logger::level_to_string(LogLevel lvl) {
    switch (lvl) {
        case LogLevel::Trace:
            return "TRACE";
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warn:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Critical:
            return "CRITICAL";
        default:
            return "OFF";
    }
}

std::string Logger::thread_id_string() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

std::tm Logger::localtime(std::time_t t) const {
    std::tm tm{};
#if defined(_WIN32)
    if (cfg_.use_utc) {
        gmtime_s(&tm, &t);
    } else {
        localtime_s(&tm, &t);
    }
#else
    if (cfg_.use_utc) {
        gmtime_r(&t, &tm);
    } else {
        localtime_r(&t, &tm);
    }
#endif
    return tm;
}

}  // namespace io
