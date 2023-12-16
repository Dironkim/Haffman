#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iomanip>

class Node {
public:
    Node(double prob, int branch, Node* next) : p(prob), branch(branch), next(next), isMerged(false) {}

    double p;
    int branch;
    bool isMerged;
    Node* next;
};
using namespace std;

bool IsSumEqualToOne(const vector<double>& numbers) {
    double sum = 0.0;
    for (double number : numbers) {
        sum += number;
    }
    return abs(sum - 1.0) < numeric_limits<double>::epsilon();
}

vector<double> SortDescending(const vector<double>& numbers) {
    vector<double> sortedNumbers = numbers;
    sort(sortedNumbers.rbegin(), sortedNumbers.rend());
    return sortedNumbers;
}


vector<double> Input(int& normalBranches) {
    vector<double> result;
    normalBranches = 0;

    cout << "Enter probabilities separated by space: ";
    string input;
    getline(cin,input);
    istringstream iss(input);
    copy(istream_iterator<double>(iss),istream_iterator<double>(),back_inserter(result));
    if (!IsSumEqualToOne(result)) {
        cout << "Probabilities don't add up to 1" << endl;
        return {};
    }

    cout << "Enter number of different letters: ";
    if (!(cin >> normalBranches)) {
        cout << "Invalid input" << endl;
        return {};
    }

    return SortDescending(result);
}



int SpecialNodeBranches(int overallLetters, int normalNodeBranches) {
    int temp = (overallLetters - 1) % (normalNodeBranches - 1);
    return (temp == 0) ? normalNodeBranches : temp + 1;
}

vector<Node*> NextColumn(const vector<Node*>& prevColumn, int maxBranches, int currentBranches) {
    vector<Node*> newCol;
    int mergingBranches = currentBranches % maxBranches;
    int straightsEnd = prevColumn.size() - currentBranches;

    for (int i = 0; i < straightsEnd; i++) {
        Node* temp = new Node(prevColumn[i]->p, -1, nullptr);
        newCol.push_back(temp);
        prevColumn[i]->next = temp;
    }

    Node* merged = new Node(0, -1, nullptr);
    merged->isMerged = true;

    for (int i = 0; i < currentBranches; i++) {
        prevColumn[straightsEnd + i]->next = merged;
        prevColumn[straightsEnd + i]->branch = i;
        merged->p += prevColumn[straightsEnd + i]->p;
    }

    newCol.push_back(merged);
    sort(newCol.rbegin(), newCol.rend(), [](Node* a, Node* b) { return b->p > a->p; });
    return newCol;
}

void DisplayTree(const vector<vector<Node*>>& tree) {
    for (int i = 0; i < tree[0].size(); i++) {
        for (int j = 0; j < tree.size() && tree[j].size() > i; j++) {
            if (tree[j][i]->isMerged) {
                cout << ">";
            }
            cout << tree[j][i]->p;
            if (tree[j][i]->branch > -1) {
                cout << "-" << tree[j][i]->branch;
            }
            cout << "\t";
        }
        cout << endl;
    }
}

void DisplayCodes(const vector<vector<Node*>>& tree) {
    vector<string> codes = {};
    for (int i = 0; i < tree[0].size(); i++) {
        string word;
        for (Node* j = tree[0][i]; j->next != nullptr; j = j->next) {
            if (j->branch > -1) {
                word += to_string(j->branch);
            }
        }
        reverse(word.begin(), word.end());
        cout << word << endl;
        codes.push_back(word);
    }
    double avg_len = 0;
    for (int i=0;i<tree[0].size();i++)
    {
        avg_len += tree[0][i]->p*codes[i].size();
    }
    cout <<"Average length: " << fixed << setprecision(3) << avg_len << endl;
}

int main() {
    int normalBranches;
    vector<double> probabilities = Input(normalBranches);
    if (probabilities.empty()) return 0;

    vector<Node*> initialNodes;
    for (double probability : probabilities) {
        initialNodes.push_back(new Node(probability, -1, nullptr));
    }

    vector<vector<Node*>> tree = { initialNodes };
    vector<Node*> tempNodes = initialNodes;

    tempNodes = NextColumn(tempNodes, normalBranches, SpecialNodeBranches(probabilities.size(), normalBranches));
    tree.push_back(tempNodes);

    while (tempNodes.size() > 1) {
        tempNodes = NextColumn(tempNodes, normalBranches, normalBranches);
        tree.push_back(tempNodes);
    }

    DisplayTree(tree);
    cout << endl;
    DisplayCodes(tree);

    // Освобождаем память
    for (vector<Node*>& nodes : tree) {
        for (Node* node : nodes) {
            delete node;
        }
    }

    return 0;
}
