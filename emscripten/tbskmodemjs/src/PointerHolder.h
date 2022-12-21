#pragma once
#include "../../../src/tbskmodem/tbskmodem.h"
#include <memory>
#include <array>

using namespace TBSKmodemCPP;

class PointerHolder
{
private:
    std::array<shared_ptr<void>,100> _buf;
public:
    //���X�g�ɃC���X�^���X��������B
    // @return ���݂̎Q�Ɛ�
    void* Add(const shared_ptr<void>& v) {
        for (auto i = 0;i < this->_buf.size();i++) {
            if (!this->_buf[i]) {
                this->_buf[i]=v;
                return &this->_buf[i];
            }
        }
        throw std::overflow_error("Pointer holder full.");
    }

    //���X�g����C���X�^���X����菜��
    void Remove(void* ptr) {
        for (auto i = 0;i < this->_buf.size();i++) {
            if (&this->_buf[i] == ptr) {
                this->_buf[i].reset();
                return;
            }
        }
        throw std::runtime_error("Invalid ptr.");
    }
    //���蓖�čς̐���Ԃ��B
    int Size(){
        auto s = 0;
        for (auto i = 0;i < this->_buf.size();i++) {
            if (this->_buf[i]) {
                s = s + 1;
            }
        }
        return s;
    }
};
