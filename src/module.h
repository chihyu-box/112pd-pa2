#ifndef MODULE_H
#define MODULE_H

#include <vector>
#include <string>
#include <limits>
using namespace std;

class Node
{
public:
    // Constructor and destructor
    Node(const int& id) :
        _id(id), _parent(NULL), _left(NULL), _right(NULL) { }
    // Constructor and destructor
    Node(const int& id, Node *parent, Node *left, Node *right) :
        _id(id), _parent(parent), _left(left), _right(right) { }
    ~Node() { }

    // Basic access methods
    int getId() const       { return _id; }
    Node* getParent() const   { return _parent; }
    Node* getLeft() const   { return _left; }
    Node* getRight() const   { return _right; }

    // Set functions
    void setId(const int& id) { _id = id; }
    void setParent(Node* parent)  { _parent = parent; }
    void setLeft(Node* left)  { _left = left; }
    void setRight(Node* right)  { _right = right; }

    // save and recover
    void save() {
        _oldParent = _parent;
        _oldLeft = _left;
        _oldRight = _right;
    }

    void recover() {
        _parent = _oldParent;
        _left = _oldLeft;
        _right = _oldRight;
    }

    void saveBest() {
        _bestParent = _parent;
        _bestLeft = _left;
        _bestRight = _right;
    }

    void setBest() {
        _parent = _bestParent;
        _left = _bestLeft;
        _right = _bestRight;
    }

private:
    int         _id;    // id of the node (indicating the cell)
    Node*       _parent;  // pointer to the parent node
    Node*       _left;  // pointer to the left node
    Node*       _right;  // pointer to the left node
    Node*       _oldParent;
    Node*       _oldLeft;
    Node*       _oldRight;
    Node*       _bestParent;
    Node*       _bestLeft;
    Node*       _bestRight;    
};

class Terminal
{
public:
    // constructor and destructor
    Terminal(string& name, size_t x, size_t y) :
        _name(name), _x1(x), _y1(y), _x2(x), _y2(y) { }
    ~Terminal()  { }

    // basic access methods
    const string getName()  { return _name; }
    const size_t getX1()    { return _x1; }
    const size_t getX2()    { return _x2; }
    const size_t getY1()    { return _y1; }
    const size_t getY2()    { return _y2; }
    const size_t getCenterX() { return (_x1+_x2) / 2; }
    const size_t getCenterY() { return (_y1+_y2) / 2; }

    // set functions
    void setName(string& name) { _name = name; }
    void setPos(size_t x1, size_t y1, size_t x2, size_t y2) {
        _x1 = x1;   _y1 = y1;
        _x2 = x2;   _y2 = y2;
    }

    // recover
    void saveCoordinate() {
        _oldX1 = _x1;
        _oldX2 = _x2;
        _oldY1 = _y1;
        _oldY2 = _y2;
    }
    void recoverCoordinate() {
        _x1 = _oldX1;
        _x2 = _oldX2;
        _y1 = _oldY1;
        _y2 = _oldY2;
    }

    void saveBest() {
        _bestX1 = _x1;
        _bestX2 = _x2;
        _bestY1 = _y1;
        _bestY2 = _y2;
    }
    void setBest() {
        _x1 = _bestX1;
        _x2 = _bestX2;
        _y1 = _bestY1;
        _y2 = _bestY2;
    }

protected:
    string      _name;      // module name
    size_t      _x1;        // min x coordinate of the terminal
    size_t      _y1;        // min y coordinate of the terminal
    size_t      _x2;        // max x coordinate of the terminal
    size_t      _y2;        // max y coordinate of the terminal
    size_t      _oldX1;
    size_t      _oldY1;
    size_t      _oldX2;
    size_t      _oldY2;
    size_t      _bestX1;
    size_t      _bestY1;
    size_t      _bestX2;
    size_t      _bestY2;
};


class Block : public Terminal
{
public:
    // constructor and destructor
    Block(string& name, size_t w, size_t h, int id) :
        Terminal(name, 0, 0), _w(w), _h(h) {
            _node = new Node(id);
        }
    ~Block() { }

    // basic access methods
    const size_t getWidth()  { return _rotate? _h: _w; }
    const size_t getHeight() { return _rotate? _w: _h; }
    const size_t getArea()  { return _h * _w; }
    static size_t getMaxX() { return _maxX; }
    static size_t getMaxY() { return _maxY; }
    Node*         getNode() { return _node; }
    const bool    getRotate() { return _rotate; }

    // set functions
    void setWidth(size_t w)         { _w = w; }
    void setHeight(size_t h)        { _h = h; }
    static void setMaxX(size_t x)   { _maxX = x; }
    static void setMaxY(size_t y)   { _maxY = y; }
    void setRotate(bool rotate)     { _rotate = rotate; }

    // save
    void saveRotate() {
        _oldRotate = _rotate;
    }
    void recoverRotate() {
        _rotate = _oldRotate;
    }

    void saveBestRotate() {
        _bestRotate = _rotate;
    }
    void setBestRotate() {
        _rotate = _bestRotate;
    }

private:
    size_t          _w;         // width of the block
    size_t          _h;         // height of the block
    static size_t   _maxX;      // maximum x coordinate for all blocks
    static size_t   _maxY;      // maximum y coordinate for all blocks
    Node *          _node;
    bool            _rotate;
    bool            _oldRotate;
    bool            _bestRotate;
};


class Net
{
public:
    // constructor and destructor
    Net()   { }
    ~Net()  { }

    // basic access methods
    const vector<Terminal*> getTermList()   { return _termList; }

    // modify methods
    void addTerm(Terminal* term) { _termList.push_back(term); }

    // other member functions
    double calcHPWL() {
        size_t top = 0, right = 0, left = INT_MAX, down = INT_MAX;
        for(auto ter : _termList) {
            double x = (ter->getX1() + ter->getX2()) / 2;
            double y = (ter->getY1() + ter->getY2()) / 2;
            if(y > top)     top = y;
            if(y < down)    down = y;
            if(x > right)   right = x;
            if(x < left)    left = x;
        }
        return top - down + right - left;
    }

private:
    vector<Terminal*>   _termList;  // list of terminals the net is connected to
};

#endif  // MODULE_H
