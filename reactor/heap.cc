/***********************************************************
 * 小根堆，合并到timer中
 * ********************************************************/
#include <bits/stdc++.h>

using namespace std;

class Heap {
public:
    int data;

public:
    void add(int);
    void pop();
    int top();
    Heap();

private:
    vector<int> _buffer;
    int _size;
};

Heap::Heap()
{
    _size = 0;
    _buffer.resize(10);
}

void Heap::add(int num)
{
    _buffer[++_size] = num;
    int parent;
    for (; _size > 0; _size = parent) {
        parent = (_size - 1) / 2;
        if (_buffer[parent] > _buffer[_size]) {
            break;
        }
        _buffer[_size] = _buffer[parent];
    }
}

void Heap::pop()
{
    _size--;
    int hole = 0;
    //_buffer.erase(_buffer.begin());
    _buffer[0] = _buffer[_size];
    int child;
    for (; (hole * 2 + 1) <= (_size - 1); hole = child) {
        child = hole * 2 + 1;
        if ((child < _size - 1) && (_buffer[child] > _buffer[child + 1])) {
            ++child;
        }
        if (_buffer[child] < _buffer[hole]) {
            _buffer[hole] = _buffer[child];
        } else {
            break;
        }
    }
    _buffer[hole] = _buffer[_size];
}