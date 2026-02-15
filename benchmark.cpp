#include <iostream>
#include <chrono>
#include <random> // For random numbers
#include <algorithm> // For shuffling
#include <fstream>
using namespace std;
using namespace std::chrono;
const int MAX_LEVEL = 16; // maximum permissible levels in the skip list
std::ofstream myFile("skiplist_results.csv");
// Simulate a coin flip: returns true ~50% of the time
bool flipCoin() {
    // single shared random engine seeded once
    static std::random_device rd;
    static std::mt19937 gen(rd());
    // 50/50 Bernoulli distribution
    static std::bernoulli_distribution coin(0.5);
    return coin(gen); // true ~50% of the time
}

// Node in the skip list: stores the key, its level (height), and forward pointers.
struct Node{
    int value;      // stored value/key for this node
    int height;     // number of levels this node spans (1 .. MAX_LEVEL)
    Node *next[];   // flexible array of forward pointers; allocated with size >= height
};

class SkipList{
    private:
        Node *senitel; // sentinel
        int currentMaxHeight=0; // current highest non-empty level
    public:
        SkipList(){
            // allocate sentinel with MAX_LEVEL forward pointers
            senitel = (Node*)malloc(sizeof(Node) + MAX_LEVEL * sizeof(Node*));
            senitel->height = MAX_LEVEL;
            for(int i=0;i<MAX_LEVEL;i++){
                senitel->next[i] = nullptr;
            }
        }

        // determine node height by repeated coin flips
        int randomHeight(){
            int height = 1;
            while(height < MAX_LEVEL && flipCoin()){
                height++;
            }
            return height;
        }

        // insert value into skip list
        void insert(int value){
            Node *newNode = (Node*)malloc(sizeof(Node) + MAX_LEVEL * sizeof(Node*));
            newNode->value = value;
            int nodeHeight = randomHeight();
            newNode->height = nodeHeight;
            if(nodeHeight > currentMaxHeight){
                currentMaxHeight = nodeHeight; // update overall height
            }
            Node *current = senitel;
            Node *update[currentMaxHeight]; // to track predecessors
            for(int i=currentMaxHeight-1;i>=0;i--){
                while(current->next[i] != nullptr && current->next[i]->value < value){
                    current = current->next[i];
                }
                update[i] = current;
            }
            for(int i=0;i<nodeHeight;i++){
                newNode->next[i] = update[i]->next[i];
                update[i]->next[i] = newNode;
            }
        }

        // search for a value, return true if found
        bool search(int value){
            Node *current = senitel;
            for(int i=currentMaxHeight-1;i>=0;i--){
                while(current->next[i] != nullptr && current->next[i]->value < value){
                    current = current->next[i];
                }
            }
            current = current->next[0];
            return current != nullptr && current->value == value;
        }

        // delete a value, return true if deleted
        void deleteValue(int value){
            Node *current = senitel;
            Node *update[MAX_LEVEL];
            for(int i=currentMaxHeight-1;i>=0;i--){
                while(current->next[i] != nullptr && current->next[i]->value < value){
                    current = current->next[i];
                }
                update[i] = current;
            }
            current = current->next[0];
            if(current != nullptr && current->value == value){
                for(int i=0;i<current->height;i++){
                    if(update[i]->next[i] != current){
                        break;
                    }
                    update[i]->next[i] = current->next[i];
                }
                free(current);
                // shrink currentMaxHeight if top levels become empty
                while(currentMaxHeight > 0 && senitel->next[currentMaxHeight-1] == nullptr){
                    currentMaxHeight--;
                }
            }
        }
};
void runBenchmark(int numOperations) {
    cout<<numOperations<<", ";
    int maxValue = numOperations; // Range of values to insert
    SkipList skiplist;
    int arr[numOperations];
    for (int i = 0; i < numOperations; i++) {
        arr[i] = i;
    }
    // Shuffle the array to get random values
    shuffle(arr, arr+numOperations, default_random_engine(0));
    auto start = high_resolution_clock::now();
    for (int i = 0; i < numOperations; i++) {
        skiplist.insert(arr[i]);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    myFile << duration << ",";
    cout << duration << " ms, ";
    int searchData[numOperations];
    for (int i = 0; i < numOperations; i++) {
        searchData[i] = rand() % maxValue;
    }
    start = high_resolution_clock::now();
    for (int i = 0; i < numOperations; i++) {
        skiplist.search(searchData[i]);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start).count();
    cout << duration << " ms, ";
    myFile << duration << ",";
    // Shuffle the array to get random values
    shuffle(arr, arr+numOperations, default_random_engine(0));
    start = high_resolution_clock::now();
    for (int i = 0; i < numOperations; i++) {
        skiplist.deleteValue(arr[i]);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start).count();
    myFile << duration << "\n";
    cout << duration << " ms\n";
}
//Driver code
int main() {
    myFile << "Operations, Insertion Time (ms), Search Time (ms), Deletion Time (ms)\n";
    cout << "Operations, Insertion Time, Search Time, Deletion Time\n";
    for (int ops = 1000; ops <= 100000; ops += 1000) {
        myFile << ops << ",";
        runBenchmark(ops);
    }
    myFile.close();
    return 0;
}