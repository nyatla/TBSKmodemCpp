#pragma once
namespace TBSKmodemCPP
{
    class NoneCopyConstructor_class
    {
    public:
        NoneCopyConstructor_class() {};
        virtual ~NoneCopyConstructor_class() {};
    public:
        // コピー禁止 (C++11)
        NoneCopyConstructor_class(const NoneCopyConstructor_class&) = delete;
        NoneCopyConstructor_class& operator=(const NoneCopyConstructor_class&) = delete;
    };
}
