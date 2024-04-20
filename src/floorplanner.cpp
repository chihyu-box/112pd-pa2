#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <deque>
#include <algorithm>
#include <stack>
#include <cmath>
#include <iomanip>
#include <chrono>
#include "floorplanner.h"
#include "module.h"

using namespace std;
using namespace chrono;

void Floorplanner::ParseBlk(fstream &input_blk) {
    input_blk.seekg(0, ios::end);
    size_t fileSize = input_blk.tellg();
    input_blk.seekg(0, ios::beg);

    string fileContent(fileSize, '\0');
    input_blk.read(&fileContent[0], fileSize);

    stringstream fileSS(fileContent);
    string word;

    // parsing
    fileSS >> word;
    for(int i=0; i<=1; ++i) { 
        fileSS >> word;
        _outline[i] = stod(word);
    }
    fileSS >> word >> word;
    _numBlock = stoi(word);
    fileSS >> word >> word;
    _numTerminals = stoi(word);
    
    // block & terminal
    string name, x, y;
    int terIndex = 0;
    int blockIndex = 0;
    while (fileSS >> name) {
        bool isTerminal = false;
        fileSS >> x;
        if(x == "terminal") {
            isTerminal = true;
            fileSS >> x;
        }
        fileSS >> y;
        if(isTerminal) {
            Terminal *terminal = new Terminal(name, stoi(x), stoi(y));
            _terName2Id.emplace(make_pair(name, terIndex++));
            addTerminal(terminal);
        }
        else {
            Block *block = new Block(name, stoi(x), stoi(y), blockIndex++);
            addTerminal(block);
            addBlock(block);
            _terName2Id.emplace(make_pair(name, terIndex++));
        }
    }
    return;
}

void Floorplanner::ParseNet(fstream &input_net) {
    input_net.seekg(0, ios::end);
    size_t fileSize = input_net.tellg();
    input_net.seekg(0, ios::beg);

    string fileContent(fileSize, '\0');
    input_net.read(&fileContent[0], fileSize);

    stringstream fileSS(fileContent);
    string word;
    
    // parsing
    fileSS >> word >> word;
    _numNets = stoi(word);

    // net
    while (fileSS >> word) {
        fileSS >> word;
        int degree = stoi(word);
        Net *net = new Net();
        while(degree > 0) {
            fileSS >> word;
            net->addTerm(_terminalList[_terName2Id.at(word)]);
            degree--;
        }
        _netList.emplace_back(net);
    }
    return;
}

void Floorplanner::printParser() {
    cout << "_terminalList size " << _terminalList.size() << endl;
    cout << "_terName2Id size " << _terName2Id.size() << endl;
    cout << "netlist size " << _netList.size() << endl;
    cout << "block size " << _blockList.size() << endl;
    for(auto block : _blockList) {
        cout << block->getName() << " ";
    } cout << endl;
    return;
}

void Floorplanner::printTree() {
    deque<Node*> dq;
    int i = 0;
    int iter = 0;
    if(head->getLeft() != nullptr) {
        dq.emplace_back(head->getLeft());
        dq.emplace_back(new Node(-1));
    }
    while(dq.size() != 0) {
        auto node = dq.front();
        dq.pop_front();
        if(node->getId() == -1) {
            cout << endl;
            if(dq.size() != 0) dq.emplace_back(new Node(-1));
            continue;
        }
        auto left = node->getLeft();
        auto right = node->getRight();
        if(left != nullptr) dq.emplace_back(node->getLeft());
        if(right != nullptr) dq.emplace_back(node->getRight());
        cout << setw(7) << _blockList[node->getId()]->getName() << " ";
    }
    return;
}

void Floorplanner::printNode() {
    for(auto blk : _blockList) {
        cout << "node " << blk->getName() << endl;
        auto left = blk->getNode()->getLeft();
        auto right = blk->getNode()->getRight();
        auto parent = blk->getNode()->getParent();
        if(parent->getId() != -1) cout << "parent " << _blockList[parent->getId()]->getName() << endl;
        else cout << "parent head" << endl;
        if(left) cout << "left " << _blockList[left->getId()]->getName() << endl;
        else cout << "left NULL" << endl;
        if(right) cout << "right " << _blockList[right->getId()]->getName() << endl;
        else cout << "right NULL" << endl;
    }
    return;
}

void Floorplanner::printNodeInfo(Node *node) {
    // print node info
    if(node == nullptr) cout << "nullptr" << " , ";
    else if (node->getId() == -1) cout << "head" << " , ";
    else cout << _blockList[node->getId()]->getName() << " , ";    
}

void Floorplanner::printContourLine() {
    cout << "print out contour line" << endl;
    for(auto itr : _contourLine) {
        cout << itr.first << " " << itr.second << endl;
    }
    return;
}

void Floorplanner::plot()
{
    fstream outgraph("./floorplan.gp", ios::out);
    outgraph << "reset\n";
    outgraph << "set terminal png\n";
    outgraph << "set output \"floorplan.png\"\n";
    outgraph << "set tics\n";
    outgraph << "unset key\n";
    outgraph << "set title \"The result of FloorPlanning\"\n";
    outgraph << "set size noratio\n";

    int screen_left = -_outline[0] * 0.1;
    int screen_right = _outline[0] * 1.1;
    int screen_down = -_outline[1] * 0.1;
    int screen_up = _outline[1] * 1.1;
    int index = 1;

    outgraph << "set object " << index++ << " rect from "
             << screen_left << "," << screen_down << " to "
             << screen_right << "," << screen_up << " fc rgb 'black'\n";
    for (vector<Block *>::iterator it = _blockList.begin(); it != _blockList.end(); ++it)
    {
        Block *blk = *it;
        int x0 = blk->getX1(), y0 = blk->getY1();
        int x1 = blk->getX2(), y1 = blk->getY2();
        int midX = blk->getCenterX(), midY = blk->getCenterY();
        outgraph << "set object " << index++ << " rect from "
                 << x0 << "," << y0 << " to " << x1 << "," << y1
                 << " fs empty border fc rgb 'green'\n"
                 << "set label "
                 << "\"" << blk->getName() << "\""
                 << " at " << midX << "," << midY << " center "
                 << "font \",8\" tc rgb \"white\"\n";
        // draw B* tree
        if (blk->getNode()->getLeft() != NULL)
        {
            outgraph << "set arrow " << index++ << " from "
                     << midX << "," << midY << " to " << _blockList[blk->getNode()->getLeft()->getId()]->getCenterX()
                     << "," << _blockList[blk->getNode()->getLeft()->getId()]->getCenterY() << " nohead lc rgb \'red\'\n";
        }
        if (blk->getNode()->getRight() != NULL)
        {
            outgraph << "set arrow " << index++ << " from "
                     << midX << "," << midY << " to " << _blockList[blk->getNode()->getRight()->getId()]->getCenterX()
                     << "," << _blockList[blk->getNode()->getRight()->getId()]->getCenterY() << " nohead lc rgb \'red\'\n";
        }
    }
    // draw outline
    fstream outline("line", ios::out);
    outgraph << "set arrow " << index++ << " from "
             << 0 << "," << 0 << " to " << _outline[0] << "," << 0 << " nohead lc rgb \'yellow\'\n"
             << "set arrow " << index++ << " from "
             << _outline[0] << "," << 0 << " to " << _outline[0] << "," << _outline[1] << " nohead lc rgb \'yellow\'\n"
             << "set arrow " << index++ << " from "
             << _outline[0] << "," << _outline[1] << " to " << 0 << "," << _outline[1] << " nohead lc rgb \'yellow\'\n"
             << "set arrow " << index++ << " from "
             << 0 << "," << _outline[1] << " to " << 0 << "," << 0 << " nohead lc rgb \'yellow\'\n";
    outgraph << "set style line 1 lc rgb \"red\" lw 3\n";
    outgraph << "set border ls 1\n";
    outgraph << "plot [" << screen_left << ":" << screen_right << "]["
             << screen_down << ":" << screen_up << "]\'line\' w l lt 2 lw 1\n";
    outgraph << "set terminal x11 persist\n";
    // outgraph << "replot\n";
    outgraph << "exit";
    outgraph.close();
    int gnuplot = system("gnuplot floorplan.gp");
    return;
}

void Floorplanner::floorplan() {
    initTree();
    preSA();
    SA();
    setBestResult();
    plot();
    return;
}

void Floorplanner::initTree() {    
    // connection between head node & root
    head->setLeft(_blockList[0]->getNode());
    head->setRight(_blockList[0]->getNode());
    _blockList[0]->getNode()->setParent(head);

    // parent index = (child index + 1) / 2 - 1
    for(int i=1; i<_blockList.size(); ++i) {
        auto parentNode = _blockList[(i + 1) / 2 - 1]->getNode();
        auto childNode = _blockList[i]->getNode();
        // left child
        if(i % 2 == 1) {
            parentNode->setLeft(childNode);
            childNode->setParent(parentNode);
        } 
        // right child
        else {
            parentNode->setRight(childNode);
            childNode->setParent(parentNode);
        }
    }
    return;
}

void Floorplanner::swapNode(int a, int b) {
    auto nodeA = _blockList[a]->getNode();
    auto nodeB = _blockList[b]->getNode();
    
    // store node of A
    auto AParent = nodeA->getParent();
    auto ALeft   = nodeA->getLeft();
    auto ARight  = nodeA->getRight();
    // store node of B
    auto BParent = nodeB->getParent();
    auto BLeft   = nodeB->getLeft();
    auto BRight  = nodeB->getRight();

    // store relationship, return true if node is left child
    bool AOnLeft = leftRight(nodeA);
    bool BOnLeft = leftRight(nodeB);

    // node A is the parent of node B (A -> B)
    if(nodeA == BParent) {
        // cout << "case 1" << endl;
        if(BOnLeft) {
            nodeB->setLeft(nodeA);
            nodeB->setRight(ARight);
            if(ARight) ARight->setParent(nodeB);
        }
        else {
            nodeB->setRight(nodeA);
            nodeB->setLeft(ALeft);
            if(ALeft) ALeft->setParent(nodeB);
        }
        if(AOnLeft) AParent->setLeft(nodeB);
        else AParent->setRight(nodeB);
        nodeB->setParent(AParent);
        nodeA->setParent(nodeB);
        nodeA->setLeft(BLeft);
        nodeA->setRight(BRight);
        if(BLeft) BLeft->setParent(nodeA);
        if(BRight) BRight->setParent(nodeA);
        return;
    } 
    // node B is the parent of node A (B -> A)
    if(nodeB == AParent) {
        // cout << "case 2" << endl;
        if(AOnLeft) {
            nodeA->setLeft(nodeB);
            nodeA->setRight(BRight);
            if(BRight) BRight->setParent(nodeA);
        }
        else {
            nodeA->setRight(nodeB);
            nodeA->setLeft(BLeft);
            if(BLeft) BLeft->setParent(nodeA);
        }
        if(BOnLeft) BParent->setLeft(nodeA);
        else BParent->setRight(nodeA);
        nodeA->setParent(BParent);
        nodeB->setParent(nodeA);
        nodeB->setLeft(ALeft);
        nodeB->setRight(ARight);
        if(ALeft) ALeft->setParent(nodeB);
        if(ARight) ARight->setParent(nodeB);
        return;
    }
    // otherwise
    // cout << "case 3" << endl;
    if(AOnLeft) AParent->setLeft(nodeB);
    else AParent->setRight(nodeB);
    if(BOnLeft) BParent->setLeft(nodeA);
    else BParent->setRight(nodeA);
    nodeA->setParent(BParent);
    nodeB->setParent(AParent);
    nodeA->setLeft(BLeft);
    nodeB->setLeft(ALeft);
    nodeA->setRight(BRight);
    nodeB->setRight(ARight);
    if(ALeft) ALeft->setParent(nodeB);
    if(ARight) ARight->setParent(nodeB);
    if(BLeft) BLeft->setParent(nodeA);
    if(BRight) BRight->setParent(nodeA);
    return;
}

void Floorplanner::removeNode(int a) {
    // random divice
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> side(0, 1); // 0 to insert left, 1 to insert right

    auto node = _blockList[a]->getNode();
    auto parentNode = node->getParent();
    auto leftNode = node->getLeft();
    auto rightNode = node->getRight();
    bool onLeft = leftRight(node);
    // two children
    if(leftNode && rightNode) {
        if(side(gen)) {
            swapNode(a, leftNode->getId());
            removeNode(a);
        }
        else {
            swapNode(a, rightNode->getId());
            removeNode(a);
        }
        return;
    }
    // remove connection of node
    node->setParent(nullptr);
    node->setLeft(nullptr);
    node->setRight(nullptr);

    // one child
    if(leftNode != nullptr || rightNode != nullptr) {
        auto child = leftNode ? leftNode : rightNode;
        if(onLeft) parentNode->setLeft(child);
        else parentNode->setRight(child);
        child->setParent(parentNode);
        return;
    }

    // no child
    if(onLeft) parentNode->setLeft(nullptr);
    else parentNode->setRight(nullptr);
    return;
}

void Floorplanner::insertNode(int a) {
    // random divice
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> pick(0, _blockList.size()-1);
    uniform_int_distribution<> side(0, 1); // 1 to insert left, 0 to insert right

    auto node = _blockList[a]->getNode(); 

    // inserted location
    int b = 0;
    do {
        b = pick(gen);
    } while(a == b);
    // } while(a == b || (insNode->getLeft() != nullptr && insNode->getRight() != nullptr));
// cout << "a " << a << " " << _blockList[a]->getName() << endl;
// cout << "b " << b << " " << _blockList[b]->getName() << endl;
    Node *insNode = _blockList[b]->getNode();

    auto leftNode = insNode->getLeft();
    auto rightNode = insNode->getRight();
    int flag = -1;
    // two child
    if(leftNode != nullptr && rightNode != nullptr) {
        int m = side(gen); // node insert to target node left(right) 1(0)
        int n = side(gen); // child node insert to node left(right) 1(0)
// cout << "parent " << m << endl;
// cout << "child " << n << endl;
        if(m) {
            node->setParent(insNode);
            insNode->setLeft(node);
            leftNode->setParent(node);
            if(n)
                node->setLeft(leftNode); 
            else
                node->setRight(leftNode);
        }
        else {
            node->setParent(insNode);
            insNode->setRight(node);
            rightNode->setParent(node);
            if(n)
                node->setLeft(rightNode); 
            else
                node->setRight(rightNode);
        }
        return;
    }
    // no child
    if(leftNode == nullptr && rightNode == nullptr) {
        flag = side(gen);
    }
    // one child
    if(leftNode == nullptr || flag == 0) {
        insNode->setLeft(node);
        node->setParent(insNode);
        return;
    }
    if(rightNode == nullptr || flag == 1) {
        insNode->setRight(node);
        node->setParent(insNode);
        return;
    }
    return;
}

void Floorplanner::rotateBlock(int a) {
    _blockList[a]->setRotate(!_blockList[a]->getRotate());
    return;
}

void Floorplanner::saveNode(int a) {
    auto node = _blockList[a]->getNode();
    node->save();
    node->getParent()->save();
    if(node->getLeft()) node->getLeft()->save();
    if(node->getRight()) node->getRight()->save();
    return;
}

void Floorplanner::recoverNode(int a) {
    auto node = _blockList[a]->getNode();
    node->recover();
    node->getParent()->recover();
    if(node->getLeft()) node->getLeft()->recover();
    if(node->getRight()) node->getRight()->recover();
    return;
}

void Floorplanner::saveBlock() {
    oldRoot = head->getLeft();
    for(auto blk : _blockList) {
        blk->saveRotate();
        blk->getNode()->save();
    }
    return;
}

void Floorplanner::recoverBlock() {
    head->setLeft(oldRoot);
    head->setRight(oldRoot);
    for(auto blk : _blockList) {
        blk->recoverRotate();
        blk->getNode()->recover();
    }
    return;
}

void Floorplanner::buildContourLine() {
    resetContourLine();
    // Preorder traversal
    stack<Node*> container;
    // root node    
    auto root = head->getLeft();
    auto rootBlock = _blockList[root->getId()];
    rootBlock->setPos(0, 0, rootBlock->getWidth(), rootBlock->getHeight());
    container.push(root);
    
    // Preorder
    while(!container.empty()) {
        auto node = container.top(); container.pop();
        auto block = _blockList[node->getId()];
        int x1 = 0, x2 = 0, y1 = 0, y2 = 0, maxY = 0;
        if(node->getParent()->getId() == -1)
            x1 = 0;
        else if(leftRight(node))
            x1 = _blockList[node->getParent()->getId()]->getX2();
        else
            x1 = _blockList[node->getParent()->getId()]->getX1();
        x2 = x1 + block->getWidth();

        auto itr1 = _contourLine.begin();
        // find the previous corner of ^[a, b]
        while(next(itr1)->first < x1) {
            advance(itr1, 1);
        }
        auto itr2 = itr1;
        // find the next corner of [a, b]^
        while(itr2->first <= x2) {
            advance(itr2, 1);
        }
        // find max Y
        for(auto it = itr1; it != itr2; ++it) {
            // segment
            if(it->second == next(it)->second) {
                if(it->first != x2 && next(it)->first != x1)
                    maxY = max(maxY, it->second);
            }
        }
        // erase contour line
        _contourLine.erase(next(itr1), itr2);
        
        y1 = maxY;
        y2 = maxY + block->getHeight();
        block->setPos(x1, y1, x2, y2);
        
        // update contour line
        if(itr1->second != y2) {
            _contourLine.insert(itr2, make_pair(x1, itr1->second));
            _contourLine.insert(itr2, make_pair(x1, y2));            
        }
        if(itr2->second != y2) {
            _contourLine.insert(itr2, make_pair(x2, y2));            
            _contourLine.insert(itr2, make_pair(x2, itr2->second));
        }
        if(node->getRight()) container.push(node->getRight());
        if(node->getLeft()) container.push(node->getLeft());   
        if(y2 > maxHeight) maxHeight = y2;
    }
    maxWidth = prev(_contourLine.end(), 2)->first;
    return;
}

void Floorplanner::preSA() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> method(0, 2);
    uniform_int_distribution<> pick(0, _blockList.size()-1);

    int a = 0, b = 0, N=100;
    double oldCost = 0, newCost = 0;
    for(int i=0; i<N; ++i) {
        int step = method(gen);
        if(step == 0) {
            a = pick(gen);
            b = pick(gen);
            swapNode(a, b);
        }
        else if(step == 1) {
            a = pick(gen);
            rotateBlock(a);
        }
        else if(step == 2) {
            a = pick(gen);
            removeNode(a);
            insertNode(a);
        }
        buildContourLine();
        Area = reportArea();
        HPWL = reportHPWL();
        avgArea += Area;
        avgHPWL += HPWL;
        newCost = reportCost();
        avgCostDiff = (avgCostDiff + abs(newCost-oldCost)/N);
        oldCost = reportCost();
    }
    avgArea /= N;
    avgHPWL /= N;
    return;
}

void Floorplanner::SA() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> pick(0, _blockList.size()-1);
    uniform_int_distribution<> method(0, 2);
    uniform_real_distribution<> prob(0.0, 1.0);

    double Ts = 0.1;
    T = Ts;
    double freeze = 0.00001;
    double lamda = 0.99;
    int N = 500;

    double oriCost = 0;
    double newCost = 0;
    double randNum = 0;

    buildContourLine();
    Area = reportArea();
    HPWL = reportHPWL();
    oriCost = reportCost();

    bool legal = false;
    while(legal == false) {
        T = Ts;
        while(T > freeze) {
            for(int i=0; i<N; ++i) {
                int step = method(gen);
                int a = 0, b = 0;
                saveBlock();
                if(step == 0) {
                    do {
                        a = pick(gen);
                        b = pick(gen);
                    } while (a == b);
                    swapNode(a, b);
                }
                else if(step == 1) {
                    do {
                        a = pick(gen);
                    } while (_blockList[a]->getHeight() == _blockList[a]->getWidth());
                    rotateBlock(a);
                }
                else if(step == 2) {
                    a = pick(gen);
                    removeNode(a);
                    insertNode(a); 
                }
                // auto start = high_resolution_clock::now();
                buildContourLine();
                // auto end = high_resolution_clock::now();
                // auto duration = duration_cast<microseconds>(end - start);
                // cout << "runtime  " << duration.count() << " us" << endl;
                Area = reportArea();
                HPWL = reportHPWL();
                newCost = reportCost();
                randNum = prob(gen);
// cout << T << setw(15) << oriCost << setw(15) << (newCost-oriCost) << setw(15) << exp(-(newCost-oriCost)/T) << endl;
               if(exp(-(newCost-oriCost)/T) > randNum) {
                    oriCost = newCost;
                    // if(newCost < bestCost) {
                    if(newCost < bestCost && maxWidth <= _outline[0] && maxHeight <= _outline[1]) {
                        legal = true;
                        bestCost = newCost;
                        bestArea = maxWidth * maxHeight;
                        bestHPWL = HPWL;
                        bestOutline[0] = maxWidth;
                        bestOutline[1] = maxHeight;
                        saveBestResult();
                    }
                }
                else {
                    recoverBlock();
                }
            }
            T = T * lamda;
        }
        if(!legal) cout << "Ilegal" << endl;
    }
    return;
}

void Floorplanner::saveBestResult() {
    for(auto blk : _blockList) {
        blk->saveBest();
        blk->saveBestRotate();
        blk->getNode()->saveBest();
    }
    return;
}

void Floorplanner::setBestResult() {
    for(auto blk : _blockList) {
        blk->setBest();
        blk->setBestRotate();
        blk->getNode()->setBest();
    }
    return;
}

inline double Floorplanner::reportCost() {
    // cout << setw(15) << alpha*(Area/avgArea) << setw(15) << pow(_outline[1]/_outline[0] - maxHeight/maxWidth, 2) << endl;
    // return alpha*(Area/avgArea);
    // return alpha*(Area/avgArea) + (1-alpha)*(HPWL/avgHPWL);    
    // return 2*alpha*(Area/avgArea) + pow(_outline[1]/_outline[0] - maxHeight/maxWidth, 2);
    // return 3*(1-alpha)*(HPWL/avgHPWL) + pow(_outline[1]/_outline[0] - maxHeight/maxWidth, 2);
    // if(bestCost == INT_MAX) return (Area/avgArea) + pow(_outline[1]/_outline[0] - maxHeight/maxWidth, 2);
    // return alpha*(Area/avgArea) + (1-alpha)*(HPWL/avgHPWL) + pow(_outline[1]/_outline[0] - maxHeight/maxWidth, 2);
    
// cout << "Area " << Area << endl;
// cout << "avgArea " << avgArea << endl;
    // return alpha*(Area/avgArea) + pow(_outline[1]/_outline[0] - maxHeight/maxWidth, 2);

    return alpha*(Area/avgArea) + (1-alpha)*(HPWL/avgHPWL) + pow(_outline[1]/_outline[0] - maxHeight/maxWidth, 2);
}

inline double Floorplanner::reportHPWL() {
    double hpwl = 0;
    for(auto net : _netList) {
        hpwl += net->calcHPWL();
    }
    return hpwl;
}

inline double Floorplanner::reportArea() {
    return maxHeight * maxWidth;
}

void Floorplanner::reportFile(std::chrono::seconds t, fstream &file) {
    file << std::fixed << alpha * bestArea + (1 - alpha) * bestHPWL << endl;
    file << std::fixed << bestHPWL << endl;
    file << std::fixed << bestOutline[0] * bestOutline[1] << endl;
    file << std::fixed << bestOutline[0] << " " << bestOutline[1] << endl;
    file << std::fixed << t.count() << endl;
    for (auto blk : _blockList)
        file << blk->getName() << " " << blk->getX1() << " " << blk->getY1() << " " << blk->getX2() << " " << blk->getY2() << endl;
}

void Floorplanner::resetContourLine() {
    maxHeight = 0;
    maxWidth = 0;
    _contourLine.clear();
    _contourLine.emplace_front(make_pair(-INT_MAX, 0));
    _contourLine.emplace_back(make_pair(INT_MAX, 0));
    return;
}

inline bool Floorplanner::leftRight(Node *A) {
    // return true if A is the left child node of A'parent 
    return A->getParent()->getLeft() == A;
}

