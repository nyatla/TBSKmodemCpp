#pragma once
#include "../../../src/tbskmodem/tbskmodem.h"
#include <memory>
#include <vector>

using namespace TBSKmodemCPP;

class PointerHolder
{
private:
    std::vector<shared_ptr<void>> _buf;
public:
    //リストにインスタンスを加える。
    void* Add(const shared_ptr<void>&& v) {
        this->_buf.push_back(v);
        return &this->_buf[this->_buf.size() - 1];
    }

    //リストからインスタンスを取り除く
    void Remove(void* ptr) {
        for (auto i = 0;i < this->_buf.size();i++) {
            if (&this->_buf[i] == ptr) {
                this->_buf.erase(std::cbegin(this->_buf) + i);
                break;
            }
        }
        return;
    }
    //リストのサイズを返す
    int Size(){
        return this->_buf.size();
    }
};
