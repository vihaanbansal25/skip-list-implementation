#include<iostream>
#include<random>
using namespace std;
const int MAX_LEVEL = 16; // maximum permissible levels in the skip list

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
        bool deleteValue(int value){
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
                return true;
            }
            else return false;
        }

        // display contents level by level
        void display(){
            for(int i=currentMaxHeight-1;i>=0;i--){
                Node *current = senitel->next[i];
                cout << "Level " << i+1 << ": ";
                while(current != nullptr){
                    cout << current->value << " ";
                    current = current->next[i];
                }
                cout << "\n";
            }
        }
};

//Driver code
int main(){
    SkipList skiplist;
    cout<<"Skiplist Created.\n";
    int n;
    while(true){
        cout<<"Choose an operation:\n";
        cout << "1. Insert\n2. Search\n3. Delete\n4. Display\n5. Exit\n";
        cin >> n;
        if(n == 1){
            int value;
            cout << "Enter value to insert: ";
            cin >> value;
            skiplist.insert(value);
            cout << "Inserted " << value << "\n";
        }
        else if(n == 2){
            int value;
            cout << "Enter value to search: ";
            cin >> value;
            if(skiplist.search(value)){
                cout << value << " found in skip list.\n";
            }
            else{
                cout << value << " not found in skip list.\n";
            }
        }
        else if(n == 3){
            int value;
            cout << "Enter value to delete: ";
            cin >> value;
            if(skiplist.deleteValue(value)){
                cout << "Deleted " << value << "\n";
            }
            else{
                cout << value << " not found in skip list.\n";
            }
        }
        else if(n==4){
            cout << "Skip List contents:\n";
            skiplist.display();
        }
        else if(n == 5){
            cout << "Exiting program.\n";
            break;
        }
        else{
            cout << "Invalid option. Please try again.\n";
        }
    }
}