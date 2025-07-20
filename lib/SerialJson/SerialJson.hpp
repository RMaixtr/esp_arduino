#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

class SerialJson {
public:
    void begin(Stream &s, size_t buf = 256) {
        _s = &s;
        _buf.resize(buf);
        _cursor = 0;
    }

    // 注册同步方法：返回 bool 表示注册成功
    template<typename T>
    bool add(const char *name, T &&func) {
        if (_methodCnt >= MAX_METHODS) return false;
        _methods[_methodCnt].name = name;
        _methods[_methodCnt++].cb = [func](const JsonObjectConst &p, JsonVariant &r){
            r.set(func(p));               // 用户函数返回任意值
        };
        return true;
    }

    template <typename T>
    void onJson(T &&handler) { _handler = handler; }

    // 每 loop() 中调用
    void loop() {
        while (_s->available()) {
            char c = _s->read();
            if (_cursor >= _buf.size()) { _cursor = 0; continue; }
            _buf[_cursor++] = c;
            if (c == '\n') {           // 以换行结束一帧
                _buf[_cursor] = '\0';
                _process();
                _cursor = 0;
            }
        }
    }

private:
    static const int MAX_METHODS = 8;
    struct Entry {
        const char *name;
        std::function<void(const JsonObjectConst&, JsonVariant&)> cb;
    } _methods[MAX_METHODS];
    uint8_t _methodCnt = 0;
    std::function<void(const JsonDocument &)> _handler;

    Stream *_s;
    std::vector<char> _buf;
    size_t _cursor;

    void _process() {
        StaticJsonDocument<512> doc;
        DeserializationError err = deserializeJson(doc, _buf.data());
        if (err) return;                    // 非法 JSON，丢弃
        if (_handler) _handler(doc);
        JsonObjectConst root = doc.as<JsonObjectConst>();

        // 必须是 JSON-RPC 2.0 请求
        if (root["jsonrpc"] != "2.0" || !root.containsKey("method")) return;

        const char *method = root["method"];
        JsonVariantConst id = root["id"];   // 可为 null（通知）

        for (uint8_t i = 0; i < _methodCnt; ++i) {
            if (strcmp(method, _methods[i].name) == 0) {
                StaticJsonDocument<256> resp;
                resp["jsonrpc"] = "2.0";
                resp["id"] = id;
                JsonVariant result = resp.createNestedObject("result");
                _methods[i].cb(root["params"], result);   // 执行函数
                serializeJson(resp, *_s);
                _s->println();
                return;
            }
        }
        // 方法未找到
        StaticJsonDocument<128> resp;
        resp["jsonrpc"] = "2.0";
        resp["id"] = id;
        resp["error"]["code"] = -32601;
        resp["error"]["message"] = "Method not found";
        serializeJson(resp, *_s);
        _s->println();
    }
};