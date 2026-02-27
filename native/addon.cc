#include <napi.h>
#include "BCH.hpp"
#include <vector>
#include <cstring>

std::vector<unsigned char> bufferToVector(const Napi::Uint8Array &arr)
{
    std::vector<unsigned char> out(arr.ElementLength());
    std::memcpy(out.data(), arr.Data(), arr.ElementLength());
    return out;
}

Napi::Value GenerateSyndrome(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsTypedArray())
    {
        Napi::TypeError::New(env, "Expected Uint8Array or Buffer").ThrowAsJavaScriptException();
        return env.Null();
    }

    try
    {
        Napi::Uint8Array input = info[0].As<Napi::Uint8Array>();

        BCH bch(8);
        std::vector<unsigned char> w = bufferToVector(input);
        std::vector<int> s = bch.compute_syndrome(w);

        Napi::Array result = Napi::Array::New(env, s.size());
        for (size_t i = 0; i < s.size(); i++)
        {
            result[i] = Napi::Number::New(env, s[i]);
        }

        return result;
    }
    catch (...)
    {
        Napi::Error::New(env, "C++ Error in generateSyndrome").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value Recover(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsTypedArray() || !info[1].IsArray())
    {
        Napi::TypeError::New(env, "Expected (Buffer/Uint8Array, number[])").ThrowAsJavaScriptException();
        return env.Null();
    }

    try
    {
        Napi::Uint8Array input = info[0].As<Napi::Uint8Array>();
        Napi::Array synArray = info[1].As<Napi::Array>();

        BCH bch(8);
        std::vector<unsigned char> w_prime = bufferToVector(input);

        std::vector<int> saved_s;
        saved_s.reserve(synArray.Length());
        for (uint32_t i = 0; i < synArray.Length(); i++)
        {
            saved_s.push_back(synArray.Get(i).As<Napi::Number>().Int32Value());
        }

        std::vector<unsigned char> recovered = bch.recover(w_prime, saved_s);

        return Napi::Buffer<unsigned char>::Copy(env, recovered.data(), recovered.size());
    }
    catch (...)
    {
        Napi::Error::New(env, "Recovery Error").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set("generateSyndrome", Napi::Function::New(env, GenerateSyndrome));
    exports.Set("recover", Napi::Function::New(env, Recover));
    return exports;
}

NODE_API_MODULE(bchaddon, Init)