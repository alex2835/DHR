#pragma  once

#ifdef DHR_EXPORT_SYMBOL
#define DYNALO_EXPORT_SYMBOLS
#include "dynalo/symbol_helper.hpp"

#define DHR_EXPORT DYNALO_EXPORT
#define DHR_CALL DYNALO_CALL
#else

#include "dynalo/dynalo.hpp"

#include <unordered_map>
#include <type_traits>
#include <filesystem>
#include <thread>
namespace fs = std::filesystem;
using namespace std::chrono_literals;

const char* CACHED_DLL_DIR = "./HotReloadedDLL/";


struct DLLHotReloader
{
    dynalo::library* mLibrary;
    std::string mInputPath;
    std::string mOutputPath;
    fs::file_time_type mLastUpdateTime;

    typedef void (*FunctionPointer)(void);
    mutable std::unordered_map<std::string, FunctionPointer> mFunctionCache;

    /*
    *  Name without extension
    *  @throw If dll can't be loaded
    */ 
    inline DLLHotReloader(const std::string& path)
        : mLibrary(nullptr)
    {
        // Directory where dll copies are stored
        if (!std::filesystem::exists(CACHED_DLL_DIR))
        {
            fs::create_directory(CACHED_DLL_DIR);
        }

        std::string name;
        std::string input_dir;
        int pos = path.find_last_of('/') + 1;
        if (pos != std::string::npos)
        {
            name = dynalo::to_native_name(path.substr(pos));
            input_dir = path.substr(0, pos);
        }
        else {
            name = dynalo::to_native_name(path);
        }

        mInputPath  = input_dir + name;
        mOutputPath = CACHED_DLL_DIR + name;
        CheckForUpdate();
    }

    ~DLLHotReloader()
    {
        delete mLibrary;
    }

    /*
    *  @throw If dll can't be reloaded
    */
    inline void CheckForUpdate()
    {
        fs::file_time_type lib_update_time = mLastUpdateTime;
        try
        {
            lib_update_time = fs::last_write_time(dynalo::to_native_name("../ui/UI").c_str());
        }
        catch (const std::exception& e)
        {}

        if (mLastUpdateTime != lib_update_time || mLibrary == nullptr)
        {
            std::this_thread::sleep_for(1ms);

            delete mLibrary;
            mLibrary = nullptr;
            fs::remove(mOutputPath);
            fs::copy(mInputPath, mOutputPath);
            mLibrary = new dynalo::library(mOutputPath);
            
            mLastUpdateTime = lib_update_time;
            mFunctionCache.clear();
        }

    }

    /*
    *  @throw If no function with such signature
    */
    template <typename FunctionSignature, typename ...Args>
    auto Call(const std::string& name, Args... args)
    {
        if (mFunctionCache.count(name))
        {
            auto func = reinterpret_cast<FunctionSignature*>(mFunctionCache[name]);
            return func(args...);
        }
        else {
            auto func = mLibrary->get_function<FunctionSignature>(name);
            mFunctionCache[name] = reinterpret_cast<FunctionPointer>(func);
            return func(args...);
        }
    }

    /*
    *  After hot reloading function pointers may be invalid
    *  @throw If no function with such signature
    */
    template <typename FunctionSignature>
    FunctionSignature* Get(const std::string& name)
    {
        return mLibrary->get_function<FunctionSignature>(name);
    }

};

#endif