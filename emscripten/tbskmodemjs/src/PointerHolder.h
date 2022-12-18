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
    //���X�g�ɃC���X�^���X��������B
    void* Add(const shared_ptr<void>&& v) {
        this->_buf.push_back(v);
        return &this->_buf[this->_buf.size() - 1];
    }

    //���X�g����C���X�^���X����菜��
    void Remove(void* ptr) {
        for (auto i = 0;i < this->_buf.size();i++) {
            if (&this->_buf[i] == ptr) {
                this->_buf.erase(std::cbegin(this->_buf) + i);
                break;
            }
        }
        return;
    }
    //���X�g�̃T�C�Y��Ԃ�
    int Size(){
        return this->_buf.size();
    }
};
