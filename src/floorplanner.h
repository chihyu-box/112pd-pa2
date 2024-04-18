#ifndef FLOORPLANNER_H
#define FLOORPLANNER_H

#include <fstream>
#include <unordered_map>
#include <map>
#include <list>
#include <stack>
#include <climits>
#include <chrono>
#include "module.h"

using namespace std;
using namespace chrono;

class Floorplanner {
private:
    double _outline[2];
    int _numBlock;
    int _numTerminals;
    int _numNets;
    string filename;
    double alpha;
    map<string, int> _terName2Id;
    vector<Terminal*> _terminalList; 
    vector<Block*> _blockList;
    vector<Net*> _netList;
    list<pair<int, int>> _contourLine;
public:
    Floorplanner(string filename, fstream& input_blk, fstream& input_net, double alpha) 
    : filename(filename), _numBlock(0), _numTerminals(0), _numNets(0), alpha(alpha) 
    {
        _outline[0] = 0;
        _outline[1] = 0;
        ParseBlk(input_blk);
        ParseNet(input_net);
        head = new Node(-1);
        _contourLine.push_front(make_pair(-INT_MAX, 0));
        _contourLine.push_back(make_pair(INT_MAX, 0));
        maxHeight = 0;
        maxWidth = 0;
        T = 0;
        Area = 0; 
        HPWL = 0; 
        avgArea = 0; 
        avgHPWL = 0; 
        avgCostDiff = 0;
        bestCost = INT_MAX;
        bestHPWL = INT_MAX;
        bestArea = INT_MAX;
        bestOutline[0] = INT_MAX;
        bestOutline[1] = INT_MAX;
    }
    void floorplan();
    // parser
    void ParseBlk(fstream& input_blk);
    void ParseNet(fstream& input_net);

    // modify method
    void addTerminal(Terminal* terminal) { _terminalList.emplace_back(terminal); }
    void addBlock(Block* block) { _blockList.emplace_back(block); }

    // print state
    void printParser();

    // report
    void reportFile(std::chrono::seconds, fstream&);

private:
    // B* tree variable
    Node *head;
    Node *oldRoot;
    double maxHeight;
    double maxWidth;

    // SA variable
    double T;
    double Area;
    double HPWL;
    double avgArea;
    double avgHPWL;
    double avgCostDiff;
    double bestCost;
    double bestHPWL;
    double bestArea;
    double bestOutline[2];

    // B* tree method
    void initTree();
    void buildContourLine();
    void preSA();
    void SA();
    void resetContourLine();

    // B* tree SA move
    void swapNode(int, int);
    void removeNode(int);
    void insertNode(int);
    void rotateBlock(int);
    void recoverNode(int);
    void saveNode(int);
    void recoverBlock();
    void saveBlock();
    void saveBestResult();
    void setBestResult();

    // B* tree report function
    inline double reportHPWL();
    inline double reportArea();
    inline double reportCost();

    // B* tree support function
    tuple<list<pair<int, int>>::iterator, list<pair<int, int>>::iterator, int> eraseRangeWithMax(int, int);
    inline bool leftRight(Node *A);

    // B* tree print state
    void printNodeInfo(Node *);
    void printTree();
    void printNode();
    void printContourLine();
    void plot();
};

#endif