#include "./recoverable.h"


namespace TBSKmodemCPP
{
    RecoverableStopIteration::RecoverableStopIteration() {};
    RecoverableStopIteration::RecoverableStopIteration(std::exception innerException) :PyStopIteration(innerException)
    {
    }

    template <typename T> RecoverableException<T>::RecoverableException(const shared_ptr<T>& recover_instance) : _recover_instance{recover_instance}
    {        
    }
    template <typename T> RecoverableException<T>::~RecoverableException() {
        this->Close();
    }
    template <typename T> const shared_ptr<T> RecoverableException<T>::Detach()
    {
        return move(this->_recover_instance);
    }
    template <typename T> void RecoverableException<T>::Close()
    {
        if (this->_recover_instance != NULL) {
            this->_recover_instance->Close();
            this->_recover_instance.reset();
        }
    }









}
