#include <napi.h>
#include "BCH.hpp"

Napi::Value GenerateSyndrome(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Expected hex string").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string hexStr = info[0].As<Napi::String>();

    // hex → byte vector 변환
    std::vector<unsigned char> data;
    for (size_t i = 0; i < hexStr.length(); i += 2)
    {
        std::string byteString = hexStr.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byteString.c_str(), nullptr, 16);
        data.push_back(byte);
    }

    BCH bch(10); // t=10 예시

    std::vector<int> syndrome = bch.compute_syndrome(data);

    Napi::Array result = Napi::Array::New(env, syndrome.size());

    for (size_t i = 0; i < syndrome.size(); i++)
    {
        result[i] = Napi::Number::New(env, syndrome[i]);
    }

    return result;
}

Napi::Value Recover(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    std::string hexStr = info[0].As<Napi::String>();
    Napi::Array jsSyndrome = info[1].As<Napi::Array>();

    std::vector<unsigned char> data;
    for (size_t i = 0; i < hexStr.length(); i += 2)
    {
        std::string byteString = hexStr.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byteString.c_str(), nullptr, 16);
        data.push_back(byte);
    }

    std::vector<int> syndrome;
    for (uint32_t i = 0; i < jsSyndrome.Length(); i++)
    {
        syndrome.push_back(jsSyndrome.Get(i).As<Napi::Number>().Int32Value());
    }

    BCH bch(8);

    std::vector<unsigned char> recovered = bch.recover(data, syndrome);

    std::string resultHex;
    char buf[3];
    for (unsigned char byte : recovered)
    {
        sprintf(buf, "%02x", byte);
        resultHex += buf;
    }

    return Napi::String::New(env, resultHex);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set("generateSyndrome", Napi::Function::New(env, GenerateSyndrome));
    exports.Set("recover", Napi::Function::New(env, Recover));
    return exports;
}

NODE_API_MODULE(bchaddon, Init)