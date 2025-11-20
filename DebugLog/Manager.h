#pragma once
#ifndef DEBUGLOG_MANAGER_H
#define DEBUGLOG_MANAGER_H

#include "Types.h"

#ifdef DEBUGLOG_USE_COLORS
#include "Colors.h"
#endif

namespace arx {
namespace debug {

    class Manager {
        LogLevel log_lvl {DEBUGLOG_DEFAULT_LOG_LEVEL};
        LogBase log_base {LogBase::DEC};
        string_t delim {" "};
        bool b_base_reset {true};

#ifdef ARDUINO
        Stream* primary_stream {&Serial};
        Stream* file_stream {nullptr};
        LogLevel file_lvl {DEBUGLOG_DEFAULT_FILE_LEVEL};
        bool b_auto_save {false};
        LogPrecision log_precision {LogPrecision::TWO};
#endif

        // singleton
        Manager() {}
    public:
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;


        static Manager& get() {
            static Manager m;
            return m;
        }

        LogLevel log_level() const {
            return log_lvl;
        }

        void log_level(const LogLevel l) {
            log_lvl = l;
        }

        void delimiter(const string_t& del) {
            delim = del;
        }

        void base_reset(const bool b) {
            b_base_reset = b;
        }

#ifdef ARDUINO


        void attach_primary_stream(Stream& s) {
            primary_stream = &s;
        }

        void attach_file_stream(Stream& s, const bool auto_save) {
            file_stream = &s;
            b_auto_save = auto_save;
        }

        void assertion(const bool b, const char* file, const int line, const char* func, const char* expr, const String& msg = "") const {
            if (!b) {
                string_t str = string_t("[ASSERT] ") + file + string_t(" ") + line + string_t(" ") + func + string_t(" : ") + expr;
                if (msg.length()) str += string_t(" => ") + msg;
                primary_stream->println(str);
                if (file_stream) {
                    file_stream->println(str);
                    file_stream->flush();
                }
                while (true){yield();}
            }
        }



        LogLevel file_level() const {
            return file_lvl;
        }

        void file_level(const LogLevel l) {
            file_lvl = l;
        }

#endif  // ARDUINO

        template <typename... Args>
        void log(const LogLevel level, Args&&... args) {
            bool b_ignore = (log_lvl == LogLevel::LVL_NONE);
#ifdef ARDUINO
            b_ignore &= (file_lvl == LogLevel::LVL_NONE);
#endif
            b_ignore |= (level == LogLevel::LVL_NONE);
            if (b_ignore) return;

            const char* header = generate_header(level);
            if (static_cast<uint8_t>(level) <= static_cast<uint8_t>(log_lvl)) {
#ifdef DEBUGLOG_USE_COLORS
                print(generate_color_tag(level));
#endif
                print(header);  // to avoid delimiter after header
#ifdef DEBUGLOG_USE_COLORS
                print(std::forward<Args>(args)...);
                println(DEBUGLOG_CLEAR_COLOR_TAG);
#else
                println(std::forward<Args>(args)...);
#endif
            }
#ifdef ARDUINO
            if (!file_stream) return;
            if (static_cast<uint8_t>(level) <= static_cast<uint8_t>(file_lvl)) {
                print_file(header);  // to avoid delimiter after header
                println_file(std::forward<Args>(args)...);
            }
#endif
        }

        // ===== print / println =====

        void print() {
            if (b_base_reset) log_base = LogBase::DEC;
        }

        template <typename Head, typename... Tail>
        void print(const Head& head, Tail&&... tail) {
#ifdef ARDUINO
            print_one(head, primary_stream);
            if (sizeof...(tail) != 0)
                print_one(delim, primary_stream);
#else
            print_one(head);
            if (sizeof...(tail) != 0)
                print_one(delim);
#endif
            print(std::forward<Tail>(tail)...);
        }

        void println() {
#ifdef ARDUINO
            print_one("\n", primary_stream);
#else
            print_one("\n");
#endif
            if (b_base_reset) log_base = LogBase::DEC;
        }

        template <typename Head, typename... Tail>
        void println(const Head& head, Tail&&... tail) {
#ifdef ARDUINO
            print_one(head, primary_stream);
            if (sizeof...(tail) != 0)
                print_one(delim, primary_stream);
#else
            print_one(head);
            if (sizeof...(tail) != 0)
                print_one(delim);
#endif
            println(std::forward<Tail>(tail)...);
        }

#ifdef ARDUINO
        void print_file() {
            if (!file_stream) return;
            if (b_auto_save) file_stream->flush();
            print();
        }

        template <typename Head, typename... Tail>
        void print_file(const Head& head, Tail&&... tail) {
            if (!file_stream) return;
            print_one(head, file_stream);
            if (sizeof...(tail) != 0)
                print_one(delim, file_stream);
            print_file(std::forward<Tail>(tail)...);
        }

        void println_file() {
            if (!file_stream) return;
            print_one("\n", file_stream);
            print_file();
        }

        template <typename Head, typename... Tail>
        void println_file(const Head& head, Tail&&... tail) {
            print_one(head, file_stream);
            if (sizeof...(tail) != 0)
                print_one(delim, file_stream);
            println_file(std::forward<Tail>(tail)...);
        }
#endif

    private:
#ifdef ARDUINO

        // print without base and precision
        template <typename Head, typename S>
        void print_one(const Head& head, S* s) { s->print(head); }

        // print with base
        template <typename S>
        void print_one(const signed char head, S* s) { s->print(head, static_cast<uint8_t>(log_base)); }
        template <typename S>
        void print_one(const unsigned char head, S* s) { s->print(head, static_cast<int>(log_base)); }
        template <typename S>
        void print_one(const short head, S* s) { s->print(head, static_cast<int>(log_base)); }
        template <typename S>
        void print_one(const unsigned short head, S* s) { s->print(head, static_cast<int>(log_base)); }
        template <typename S>
        void print_one(const int head, S* s) { s->print(head, static_cast<int>(log_base)); }
        template <typename S>
        void print_one(const unsigned int head, S* s) { s->print(head, static_cast<int>(log_base)); }
        template <typename S>
        void print_one(const long head, S* s) { s->print(head, static_cast<int>(log_base)); }
        template <typename S>
        void print_one(const unsigned long head, S* s) { s->print(head, static_cast<int>(log_base)); }
        template <typename S>
        void print_one(const LogBase& head, S*) {
            log_base = head;
        }

        // print with precision
        template <typename S>
        void print_one(const float head, S* s) { s->print(head, static_cast<int>(log_precision)); }
        template <typename S>
        void print_one(const double head, S* s) { s->print(head, static_cast<int>(log_precision)); }
        template <typename S>
        void print_one(const LogPrecision& head, S*) {
            log_precision = head;
        }

        template <typename S, typename T>
        void print_one(const Array<T>& head, S* s) {
            print_array(head, s);
        }

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11

        template <typename S, typename T>
        void print_one(const vec_t<T>& head, S* s) {
            print_array(head, s);
        }

        template <typename S, typename T>
        void print_one(const deq_t<T>& head, S* s) {
            print_array(head, s);
        }

        template <typename S, typename K, typename V>
        void print_one(const map_t<K, V>& head, S* s) {
            print_map(head, s);
        }

#else  // Do not have libstdc++11

        template <typename S, typename T, size_t N>
        void print_one(const vec_t<T, N>& head, S* s) {
            print_array(head, s);
        }

        template <typename S, typename T, size_t N>
        void print_one(const deq_t<T, N>& head, S* s) {
            print_array(head, s);
        }

        template <typename S, typename K, typename V, size_t N>
        void print_one(const map_t<K, V, N>& head, S* s) {
            print_map(head, s);
        }

#endif  // Do not have libstdc++11

        // print one helper
        template <typename S, typename T>
        void print_array(const T& head, S* s) {
            bool b_base_reset_restore = b_base_reset;
            b_base_reset = false;
            print_one("[", s);
            for (size_t i = 0; i < head.size(); ++i) {
                print_one(head[i], s);
                if (i + 1 != head.size())
                    print_one(", ", s);
            }
            print_one("]", s);
            b_base_reset = b_base_reset_restore;
            if (b_base_reset) log_base = LogBase::DEC;
        }

        template <typename S, typename T>
        void print_map(const T& head, S* s) {
            bool b_base_reset_restore = b_base_reset;
            print_one("{", s);
            const size_t size = head.size();
            size_t i = 0;
            for (const auto& kv : head) {
                print_one(kv.first, s);
                print_one(":", s);
                print_one(kv.second, s);
                if (++i != size)
                    print_one(", ", s);
            }
            print_one("}", s);
            b_base_reset = b_base_reset_restore;
            if (b_base_reset) log_base = LogBase::DEC;
        }

#else

        template <typename Head>
        void print_one(const Head& head) {
            switch (log_base) {
                case LogBase::DEC: std::cout << std::dec; break;
                case LogBase::HEX: std::cout << std::hex; break;
                case LogBase::OCT: std::cout << std::oct; break;
            }
            std::cout << head;
        }

        template <typename T>
        void print_one(const Array<T>& head) {
            print_array(head);
        }

        template <typename T>
        void print_one(const vec_t<T>& head) {
            print_array(head);
        }

        template <typename T>
        void print_one(const deq_t<T>& head) {
            print_array(head);
        }

        template <typename K, typename V>
        void print_one(const map_t<K, V>& head) {
            print_map(head);
        }

        // print one helper
        template <typename T>
        void print_array(const T& head) {
            print_one("[");
            for (size_t i = 0; i < head.size(); ++i) {
                print_one(head[i]);
                if (i + 1 != head.size())
                    print_one(", ");
            }
            print_one("]");
        }

        template <typename T>
        void print_map(const T& head) {
            print_one("{");
            const size_t size = head.size();
            size_t i = 0;
            for (const auto& kv : head) {
                print_one(kv.first);
                print_one(":");
                print_one(kv.second);
                if (++i != size)
                    print_one(", ");
            }
            print_one("}");
        }

#endif

        // ===== other utilities =====

        #ifndef DEBUGLOG_ERROR_HEADER
        #define DEBUGLOG_ERROR_HEADER "[ERROR] "
        #endif
        #ifndef DEBUGLOG_WARN_HEADER
        #define DEBUGLOG_WARN_HEADER "[WARN] "
        #endif
        #ifndef DEBUGLOG_INFO_HEADER
        #define DEBUGLOG_INFO_HEADER "[INFO] "
        #endif
        #ifndef DEBUGLOG_DEBUG_HEADER
        #define DEBUGLOG_DEBUG_HEADER "[DEBUG] "
        #endif
        #ifndef DEBUGLOG_TRACE_HEADER
        #define DEBUGLOG_TRACE_HEADER "[TRACE] "
        #endif
        #ifndef DEBUGLOG_NONE_HEADER
        #define DEBUGLOG_NONE_HEADER "[NONE] "
        #endif

        static const char* generate_header(const LogLevel lvl) {
            switch (lvl) {
            case LogLevel::LVL_ERROR: return DEBUGLOG_ERROR_HEADER;
            case LogLevel::LVL_WARN:  return DEBUGLOG_WARN_HEADER;
            case LogLevel::LVL_INFO:  return DEBUGLOG_INFO_HEADER;
            case LogLevel::LVL_DEBUG: return DEBUGLOG_DEBUG_HEADER;
            case LogLevel::LVL_TRACE: return DEBUGLOG_TRACE_HEADER;
            case LogLevel::LVL_NONE:  return DEBUGLOG_NONE_HEADER;
            }
            return "";
        }
    };

}  // namespace debug
}  // namespace arx

#endif  // DEBUGLOG_MANAGER_H
